/*
 * Copyright (c) 2020, Moyano, Gabriel
 * Copyright (c) 2021, 2024, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2023-2024, Pieper, Pascal
 * Copyright (c) 2023, Mess, Jan-Gerd
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Bleeke, Kai
 * Copyright (c) 2024, Christof Efkemann
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "termios_rawport.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#ifdef __linux__
#include <asm/termbits.h>
#else
#include <termios.h>
#endif
#ifdef __rtems__
#include <rtems/termiostypes.h>
#endif

#include <outpost/time/timeout.h>

#include <limits>

#ifdef __linux__
// We support non-standard baudrates on Linux by using struct termios2 and the
// TCGETS2 ioctl. We provide macro replacements for the normal termios functions
// here in order to avoid lots if ifdefs in the TermiosRawPort code below.
typedef struct termios2 termios_t;
#define tcgetattr(fd, tio) ioctl((fd), TCGETS2, (tio))
#define tcsetattr(fd, actions, tio) ioctl((fd), TCSETS2, (tio))
#define tcflush(fd, queue) ioctl((fd), TCFLSH, (queue))
#define tcdrain(fd) ioctl((fd), TCSBRK, 1)
#define cfmakeraw(tio)                                                                         \
    do                                                                                         \
    {                                                                                          \
        (tio)->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON); \
        (tio)->c_oflag &= ~OPOST;                                                              \
        (tio)->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);                           \
        (tio)->c_cflag &= ~(CSIZE | PARENB);                                                   \
        (tio)->c_cflag |= CS8;                                                                 \
    } while (0)
#define cfsetospeed(tio, speed)    \
    ({                             \
        (tio)->c_cflag &= ~CBAUD;  \
        (tio)->c_cflag |= (speed); \
        0;                         \
    })
#define cfsetispeed(tio, speed)                \
    ({                                         \
        (tio)->c_cflag &= ~(CBAUD << IBSHIFT); \
        (tio)->c_cflag |= (speed) << IBSHIFT;  \
        0;                                     \
    })
#else
// On all other platforms we use the normal struct termios and functions.
typedef struct termios termios_t;
#endif

static outpost::Expected<speed_t, bool>
translateBaudrateToPosixSpeed(uint32_t baudrate);

outpost::posix::TermiosRawPort::TermiosRawPort(outpost::time::Clock& clock) :
    mIsOpen(false), mReadTimeout(outpost::time::Duration::maximum()), mClock(clock), mFd(-1)
{
}

outpost::posix::TermiosRawPort::~TermiosRawPort()
{
    close();
}

outpost::ErrorCode
outpost::posix::TermiosRawPort::open(const outpost::Slice<const char>& deviceName,
                                     uint32_t baudrate,
                                     Parity parity)
{
    if (deviceName.begin() == nullptr)
    {
        return SerialError::inputInvalid();
    }

    if (mIsOpen == false)
    {
        // The port hasn't been opened

        // FIXME: we can't be sure that `deviceName` contains a zero terminated string.
        // (and we don't check)
        // Use a `outpost::BoundedString` instead.
        mFd = ::open(deviceName.begin(), O_RDWR);
        if (mFd < 0)
        {
            if (mFd == -EAGAIN)
            {
                return SerialError::temporary();
            }
            return SerialError::error(mFd);
        }

        termios_t mTerm;
        int ret;
        // Get the termios structure with the current settings
        ret = tcgetattr(mFd, &mTerm);
        if (ret < 0)
        {
            return SerialError::error(ret);
        }

        // Set the mTermios driver to raw mode
        cfmakeraw(&mTerm);

        // Set the baud rate
        const auto br = translateBaudrateToPosixSpeed(baudrate);
        if (!br)
        {
            // If we're on RTEMS or Linux and the BOTHER flag is
            // defined (in rtems/termiostypes.h or asm/termbits.h)
            // we can use this flag to set non-standard baudrates:
#ifdef BOTHER
#ifdef __linux__
            // On Linux the standard baudrates are set in the
            // c_cflag field. We must clear it before setting BOTHER.
            mTerm.c_cflag &= ~CBAUD;
#endif
            mTerm.c_cflag |= BOTHER;
            mTerm.c_ispeed = mTerm.c_ospeed = baudrate;
#else
            return SerialError::error(-EINVAL);
#endif
        }
        else
        {
            ret = cfsetospeed(&mTerm, *br) == 0;

            if (ret)
            {
                ret = cfsetispeed(&mTerm, *br) == 0;
            }

            if (!ret)
            {
                return SerialError::error(ret);
            }
        };

        // Set "blocking read" by default
        mTerm.c_cc[VMIN] = 1;
        mTerm.c_cc[VTIME] = 0;

        // Set parity
        switch (parity)
        {
            case Parity::none:
                mTerm.c_cflag &= ~PARENB;
                mTerm.c_iflag &= ~INPCK;  // Disable input parity checking
                break;

            case Parity::odd:
                mTerm.c_cflag |= PARENB;
                mTerm.c_cflag |= PARODD;
                mTerm.c_iflag |= INPCK;   // Enable input parity checking
                mTerm.c_iflag |= IGNPAR;  // Ignore any byte with framing or parity error
                break;

            case Parity::even:
                mTerm.c_cflag |= PARENB;
                mTerm.c_cflag &= ~PARODD;
                mTerm.c_iflag |= INPCK;   // Enable input parity checking
                mTerm.c_iflag |= IGNPAR;  // Ignore any byte with framing or parity error
                break;

            default:
                // Same as Parity::none
                mTerm.c_cflag &= ~PARENB;
                mTerm.c_iflag &= ~INPCK;  // Disable input parity checking
                break;
        }

        // Set the changes
        ret = tcsetattr(mFd, TCSANOW, &mTerm);
        if (ret < 0)
        {
            return SerialError::error(ret);
        }

        // Save if the device was configured correctly
        mIsOpen = true;
    }

    return SerialError::success();
}

void
outpost::posix::TermiosRawPort::close()
{
    if (mIsOpen)
    {
        ::close(mFd);
        mIsOpen = false;
    }
}

bool
outpost::posix::TermiosRawPort::isAvailable()
{
    bool result = false;

    if (mIsOpen)
    {
        result = (getNumberOfBytesAvailable() > 0);
    }

    return result;
}

size_t
outpost::posix::TermiosRawPort::getNumberOfBytesAvailable()
{
    int numberOfBytes = 0;

    if (mIsOpen)
    {
        int rv = ioctl(mFd, FIONREAD, &numberOfBytes);
        if (rv == -1)
        {
            numberOfBytes = 0;
        }
    }

    return numberOfBytes;
}

outpost::hal::Serial::MaybeSize
outpost::posix::TermiosRawPort::read(const outpost::Slice<uint8_t>& data,
                                     const outpost::time::Duration& timeout)
{
    if (data.getNumberOfElements() == 0 || !mIsOpen)
    {
        return outpost::unexpected(SerialError::inputInvalid());
    }
    if (timeout != mReadTimeout)
    {
        updateReadTimeout(timeout);
    }

    ssize_t readResult = ::read(mFd, data.begin(), data.getNumberOfElements());
    if (readResult >= 0)
    {
        // No error happened, perhaps timeout
        return static_cast<size_t>(readResult);
    }
    else
    {
        if (readResult == -EAGAIN)
        {
            return outpost::unexpected(SerialError::temporary());
        }
        return outpost::unexpected(SerialError::error(readResult));
    }
}

outpost::hal::Serial::MaybeSize
outpost::posix::TermiosRawPort::write(const outpost::Slice<const uint8_t>& data,
                                      const outpost::time::Duration& timeout)
{
    ssize_t writeResult = 0;
    size_t bytesWritten = 0;

    if (data.getNumberOfElements() > 0 && mIsOpen)
    {
        if (timeout == outpost::time::Duration::myriad())
        {
            // Blocking write
            writeResult = ::write(mFd, data.begin(), data.getNumberOfElements());

            if (writeResult > 0)
            {
                // No error happened
                bytesWritten = static_cast<size_t>(writeResult);
            }
        }
        else
        {
            // Bytes are written one by one while the timeout is not expired
            outpost::time::Timeout writeTimeout(mClock, timeout);

            do
            {
                writeResult = ::write(mFd, data.begin() + bytesWritten, 1);  // Send one byte

                if (writeResult > 0)
                {
                    // No error happened
                    bytesWritten++;
                }

            } while ((writeTimeout.getState() != outpost::time::Timeout::State::expired)
                     && (bytesWritten < data.getNumberOfElements()) && (writeResult > 0));
        }
    }

    return bytesWritten;
}

void
outpost::posix::TermiosRawPort::flushReceiver()
{
    if (mIsOpen)
    {
        tcflush(mFd, TCIFLUSH);
    }
}

void
outpost::posix::TermiosRawPort::flushTransmitter()
{
    if (mIsOpen)
    {
        tcdrain(mFd);
    }
}

void
outpost::posix::TermiosRawPort::updateReadTimeout(outpost::time::Duration timeout)
{
    termios_t mTerm;
    // Get the termios structure with the current settings
    tcgetattr(mFd, &mTerm);

    mReadTimeout = timeout;

    uint64_t timeoutTenthSecond =
            mReadTimeout.milliseconds() / 100;  // Conversion to tenths of seconds
    if (timeoutTenthSecond > std::numeric_limits<unsigned int>::max())
    {
        // Set "blocking read"
        mTerm.c_cc[VMIN] = 1;
        mTerm.c_cc[VTIME] = 0;
    }
    else
    {
        // Set "read with timeout"
        mTerm.c_cc[VMIN] = 0;
        mTerm.c_cc[VTIME] = static_cast<unsigned int>(timeoutTenthSecond);
    }

    // Set the changes
    tcsetattr(mFd, TCSANOW, &mTerm);
}

static outpost::Expected<speed_t, bool>
translateBaudrateToPosixSpeed(uint32_t baudrate)
{
    switch (baudrate)
    {
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        // others ...
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400:
            return B38400;
            /* Extra output baud rates (not in POSIX).  */
#ifdef B57600
        case 57600: return B57600;
#endif
#ifdef B115200
        case 115200: return B115200;
#endif
#ifdef B230400
        case 230400: return B230400;
#endif
#ifdef B460800
        case 460800: return B460800;
#endif
#ifdef B500000
        case 500000: return B500000;
#endif
#ifdef B576000
        case 576000: return B576000;
#endif
#ifdef B921600
        case 921600: return B921600;
#endif
#ifdef B1000000
        case 1000000: return B1000000;
#endif
#ifdef B1152000
        case 1152000: return B1152000;
#endif
#ifdef B1500000
        case 1500000: return B1500000;
#endif
#ifdef B2000000
        case 2000000: return B2000000;
#endif
#ifdef B2500000
        case 2500000: return B2500000;
#endif
#ifdef B3000000
        case 3000000: return B3000000;
#endif
#ifdef B3500000
        case 3500000: return B3500000;
#endif
#ifdef B4000000
        case 4000000: return B4000000;
#endif
        // others...
        default: return outpost::unexpected(false);
    }
}
