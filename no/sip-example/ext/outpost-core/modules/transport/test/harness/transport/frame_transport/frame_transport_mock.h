

#ifndef OUTPOST_TRANSPORT_FRAME_TRANSPORT_MOCK_H
#define OUTPOST_TRANSPORT_FRAME_TRANSPORT_MOCK_H

#include <outpost/base/slice.h>
#include <outpost/transport/frame_transport/frame_transport.h>
#include <outpost/transport/operation_result.h>
#include <outpost/utils/expected.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/base/slice.h>

namespace harness
{
namespace transport
{

class FrameTransportTxMock : public virtual outpost::transport::FrameTransportTx
{
public:
    MOCK_METHOD((outpost::Expected<size_t, outpost::transport::OperationResult>),
                transmit,
                (const outpost::Slice<const uint8_t>& inputBytes),
                (override));

    // "Spy" on what the mock transmitted
    static inline auto
    TransmitTo(outpost::Slice<uint8_t> transmitBuffer)
    {
        return ::testing::DoAll(unittest::base::SliceCopyInto<0>(transmitBuffer),
                                unittest::base::SliceGetNumberOfElements<0>());
    }
};

class FrameTransportRxMock : public virtual outpost::transport::FrameTransportRx
{
public:
    MOCK_METHOD((outpost::Expected<outpost::Slice<uint8_t>, outpost::transport::OperationResult>),
                receive,
                (outpost::Slice<uint8_t> const& outputBuffer, outpost::time::Duration timeout),
                (override));

    // Insert data that the mock receives
    static inline auto
    ReceiveFrom(outpost::Slice<uint8_t> dataToReceive)
    {
        return ::testing::DoAll(unittest::base::SliceCopyFrom<0>(dataToReceive),
                                unittest::base::SliceFirst<0>(dataToReceive.getNumberOfElements()));
    }
};

class FrameTransportMock : public FrameTransportTxMock,
                           public FrameTransportRxMock,
                           public outpost::transport::FrameTransport
{
public:
    using FrameTransportRxMock::receive;
    using FrameTransportRxMock::ReceiveFrom;
    using FrameTransportTxMock::transmit;
    using FrameTransportTxMock::TransmitTo;
};

}  // namespace transport
}  // namespace harness

#endif
