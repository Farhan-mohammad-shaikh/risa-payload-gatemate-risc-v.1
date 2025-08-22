/*
 * Copyright (c) 2017, Muhammad Bassam
 * Copyright (c) 2017-2018, Fabian Greif
 * Copyright (c) 2019-2021, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_COMM_RMAP_PACKET_H_
#define OUTPOST_COMM_RMAP_PACKET_H_

#include <outpost/base/slice.h>
#include <outpost/comm/rmap/rmap_node.h>
#include <outpost/comm/rmap/rmap_status.h>
#include <outpost/storage/bit_access.h>
#include <outpost/storage/serialize.h>

#include <string>

namespace outpost
{
namespace comm
{
/**
 * RMAP packet.
 *
 * Provides RMAP packet definitions and methods to setup the send/receive packet
 * for RMAP transaction.
 *
 * \author  Muhammad Bassam
 */
class RmapPacket
{
    friend class TestingRmap;

public:
    enum class ExtractionResult
    {
        success,
        crcError,
        invalid,
        incorrectAddress
    };

    /**
     * RMAP packet instruction field, for reference
     * (see ECSS-E-ST-50-52C pg. 17)
     *
     * */
    struct InstructionField
    {
        static constexpr int incrementBit = 2;
        static constexpr int replyBit = 3;
        static constexpr int verifyBit = 4;
        static constexpr int operationBit = 5;

        enum PacketType : uint8_t
        {
            replyPacket = 0,
            commandPacket = 1
        };

        enum Operation : uint8_t
        {
            read = 0,
            write = 1
        };

        enum ReplyAddressLength : uint8_t
        {
            zeroBytes = 0,
            fourBytes = 1,
            eightBytes = 2,
            eigthBytes
            [[deprecated("Use 'eightBytes' instead")]] = eightBytes,  // kept for legacy (for now)
            twelveBytes = 3
        };

        constexpr InstructionField() : mField(0)
        {
        }

        InstructionField(const InstructionField&) = delete;

        InstructionField&
        operator=(const InstructionField&) = delete;

        ~InstructionField()
        {
        }

        inline void
        setOperation(const Operation& op)
        {
            // bit5 operation
            outpost::BitAccess::set<uint8_t, operationBit>(mField, op);
        }

        inline Operation
        getOperation() const
        {
            // bit5 operation
            return static_cast<Operation>(outpost::BitAccess::get<uint8_t, operationBit>(mField));
        }

        inline void
        setPacketType(PacketType type)
        {
            // bit7 & bit6 packet type
            outpost::BitAccess::set<uint8_t, 7, 6>(mField, type);
        }

        inline PacketType
        getPacketType() const
        {
            // bit7 & bit6 packet type
            return static_cast<PacketType>(outpost::BitAccess::get<uint8_t, 7, 6>(mField));
        }

        inline void
        setVerify(bool enable)
        {
            outpost::BitAccess::set<uint8_t, verifyBit>(mField, enable);
        }

        inline void
        setReply(bool enable)
        {
            outpost::BitAccess::set<uint8_t, replyBit>(mField, enable);
        }

        inline void
        setIncrement(bool enable)
        {
            outpost::BitAccess::set<uint8_t, incrementBit>(mField, enable);
        }

        inline bool
        isVerifyEnabled() const
        {
            // bit4 verify
            return outpost::BitAccess::get<uint8_t, verifyBit>(mField);
        }

        inline bool
        isReplyEnabled() const
        {
            return outpost::BitAccess::get<uint8_t, replyBit>(mField);
        }

        inline bool
        isIncrementEnabled() const
        {
            return outpost::BitAccess::get<uint8_t, incrementBit>(mField);
        }

        inline void
        setReplyAddressLength(ReplyAddressLength len)
        {
            // bit 1 & bit 0 reply length
            outpost::BitAccess::set<uint8_t, 1, 0>(mField, len);
        }

        inline ReplyAddressLength
        getReplyAddressLength() const
        {
            return static_cast<ReplyAddressLength>(outpost::BitAccess::get<uint8_t, 1, 0>(mField));
        }

        inline void
        setAllRaw(uint8_t inst)
        {
            mField = inst;
        }

        inline uint8_t
        getRaw() const
        {
            return mField;
        }

        inline void
        reset()
        {
            mField = 0;
        }

    private:
        uint8_t mField;
    };

    //--------------------------------------------------------------------------
    RmapPacket();
    RmapPacket(outpost::Slice<uint8_t> spwTargets,
               uint8_t targetLogicalAddress,
               InstructionField::ReplyAddressLength rplyAddrLen,
               uint8_t key,
               uint8_t* replyAddress,
               uint8_t initiatorLogicalAddress,
               uint32_t address,
               uint32_t dataLength);

    RmapPacket(uint8_t targetLogicalAddress,
               uint8_t key,
               uint8_t initiatorLogicalAddress,
               uint32_t address,
               uint32_t dataLength);
    ~RmapPacket();

    /**
     * Reset or clear the contents of the packet. This method is being
     * used by the RMAP initiator thread for clearing the received old packet
     * contents for next incoming packets.
     *
     * */
    void
    reset();

    /**
     * Construct the RMAP packet according to the given standard. After filling
     * the packet content corresponding CRC will be calculated and inserted into
     * the packet buffer making it ready to send.
     *
     * \param buffer
     *      SpW buffer provided by the RMAP initiator, size will be set as well
     *
     *
     * \return
     *      True for successful integration of packet into the buffer, false for
     *      insufficient memory space in the buffer
     *
     * */
    bool
    constructPacket(outpost::Slice<uint8_t>& buffer);

    /**
     * Extract the received RMAP packet according to the given standard by
     * checking it's content and verifying particular CRC's for packet data
     * validity checks.
     *
     * \param data
     *      Reference to the raw received data buffer from SpW
     *
     * \param expectedInitiatorLogicalAddress
     *      Used to validate whether the packet is intended for the provided packet
     *      initiator
     *
     * \return
     *	  \c ExtractionResult::success	successful extracted
     *	  \c ExtractionResult::crcError	a crc is incorrect
     *	  \c ExtractionResult::invalid	format is invalid
     *	  \c ExtractionResult::incorrectAddress the received address does not match
     *
     * */
    ExtractionResult
    extractReplyPacket(const outpost::Slice<const uint8_t>& data,
                       const uint8_t expectedInitiatorLogicalAddress);

    /**
     * Setting the RMAP target specific information into the packet, will be
     * done before initiating the transaction for sending commands.
     *
     * \param rmapTargetNode
     *      Reference to the RMAP target node
     *
     * */
    void
    setTargetInformation(RmapTargetNode& rmapTargetNode);

    RmapPacket&
    operator=(const RmapPacket& rhs);

    // Disabling copy constructor
    RmapPacket(const RmapPacket&) = delete;

    //--------------------------------------------------------------------------

    inline void
    setTargetSpaceWireAddress(outpost::Slice<uint8_t> targetSpaceWireAddress)
    {
        if (targetSpaceWireAddress.getNumberOfElements() > 0)
        {
            memcpy(mSpwTargets,
                   targetSpaceWireAddress.getDataPointer(),
                   targetSpaceWireAddress.getNumberOfElements());
        }
        mSpwTargetAddressLength = targetSpaceWireAddress.getNumberOfElements();
    }

    inline outpost::Slice<const uint8_t>
    getTargetSpaceWireAddress() const
    {
        return outpost::Slice<const uint8_t>::unsafe(mSpwTargets, mSpwTargetAddressLength);
    }

    outpost::Slice<const uint8_t>
    getReplyAddress() const
    {
        return outpost::Slice<const uint8_t>::unsafe(
                reinterpret_cast<const uint8_t*>(mReplyAddress),
                mInstruction.getReplyAddressLength());
    }

    inline void
    setReplyPathAddressLength(InstructionField::ReplyAddressLength pathAddressLength)
    {
        mInstruction.setReplyAddressLength(pathAddressLength);
    }

    inline uint8_t
    getReplyPathAddressLength() const
    {
        return mInstruction.getReplyAddressLength();
    }

    inline void
    setTargetLogicalAddress(uint8_t targetLogicalAddress)
    {
        mTargetLogicalAddress = targetLogicalAddress;
    }

    inline uint8_t
    getTargetLogicalAddress() const
    {
        return mTargetLogicalAddress;
    }

    inline void
    setInitiatorLogicalAddress(uint8_t initiatorLogicalAddress)
    {
        mInitiatorLogicalAddress = initiatorLogicalAddress;
    }

    inline uint8_t
    getInitiatorLogicalAddress() const
    {
        return mInitiatorLogicalAddress;
    }

    inline uint8_t
    getInstruction() const
    {
        return mInstruction.getRaw();
    }

    inline bool
    isCommandPacket() const
    {
        return (mInstruction.getPacketType() == InstructionField::commandPacket);
    }

    inline void
    setCommand()
    {
        mInstruction.setPacketType(InstructionField::commandPacket);
    }

    inline void
    setReply()
    {
        mInstruction.setPacketType(InstructionField::replyPacket);
    }

    inline bool
    isReplyPacket() const
    {
        return (mInstruction.getPacketType() == InstructionField::replyPacket);
    }

    inline bool
    isWrite() const
    {
        return (mInstruction.getOperation() == InstructionField::write);
    }

    inline void
    setWrite()
    {
        mInstruction.setOperation(InstructionField::write);
    }

    inline bool
    isRead() const
    {
        return (mInstruction.getOperation() == InstructionField::read);
    }

    inline void
    setRead()
    {
        mInstruction.setOperation(InstructionField::read);
    }

    inline bool
    isVerifyFlagSet() const
    {
        return mInstruction.isVerifyEnabled();
    }

    inline void
    setVerifyFlag(bool enable)
    {
        mInstruction.setVerify(enable);
    }

    inline bool
    isReplyFlagSet() const
    {
        return mInstruction.isReplyEnabled();
    }

    inline void
    setReplyFlag(bool enable)
    {
        mInstruction.setReply(enable);
    }

    inline bool
    isIncrementFlagSet() const
    {
        return mInstruction.isIncrementEnabled();
    }

    inline void
    setIncrementFlag(bool enable)
    {
        mInstruction.setIncrement(enable);
    }

    inline void
    setStatus(uint8_t status)
    {
        mStatus = status;
    }

    inline uint8_t
    getStatus() const
    {
        return mStatus;
    }

    inline void
    setTransactionID(uint16_t transactionID)
    {
        mTransactionIdentifier = transactionID;
    }

    inline uint16_t
    getTransactionID() const
    {
        return mTransactionIdentifier;
    }

    inline void
    setAddress(uint32_t address)
    {
        mAddress = address;
    }

    inline uint32_t
    getAddress() const
    {
        return mAddress;
    }

    inline void
    setExtendedAddress(uint8_t extendedAddress)
    {
        mExtendedAddress = extendedAddress;
    }

    inline uint8_t
    getExtendedAddress() const
    {
        return mExtendedAddress;
    }

    inline void
    setKey(uint8_t key)
    {
        mDestKey = key;
    }

    inline uint8_t
    getKey() const
    {
        return mDestKey;
    }

    inline uint32_t
    getHeaderLength() const
    {
        return mHeaderLength;
    }

    inline uint8_t
    getHeaderCRC() const
    {
        return mHeaderCRC;
    }

    inline outpost::Slice<const uint8_t>
    getData() const
    {
        return mData;
    }

    void
    setData(const outpost::Slice<const uint8_t>& data);

    inline uint8_t
    getDataCRC() const
    {
        return mDataCRC;
    }

    inline void
    setDataLength(uint32_t len)
    {
        mDataLength = len;
    }

    inline uint32_t
    getDataLength() const
    {
        return mDataLength;
    }

private:
    /**
     * Construct RMAP packet header.
     *
     * \param stream
     *      Reference to the serialize stream of SpW buffer
     *
     * */
    void
    constructHeader(outpost::Serialize& stream);

    //--------------------------------------------------------------------------
    uint8_t mSpwTargetAddressLength;
    uint8_t mSpwTargets[rmap::maxPhysicalRouterOutputPorts];
    uint8_t mTargetLogicalAddress;
    InstructionField mInstruction;
    uint8_t mDestKey;
    uint32_t mReplyAddress[rmap::maxNumberOfTargetNodes / 4];
    uint8_t mInitiatorLogicalAddress;
    uint8_t mExtendedAddress;
    uint16_t mTransactionIdentifier;
    uint32_t mAddress;
    uint32_t mDataLength;
    uint8_t mStatus;
    uint32_t mHeaderLength;
    outpost::Slice<const uint8_t> mData;

    uint8_t mHeaderCRC;
    uint8_t mDataCRC;
};
}  // namespace comm
}  // namespace outpost

#endif
