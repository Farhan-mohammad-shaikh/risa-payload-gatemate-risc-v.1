#include "ring_buffer_helper.h"

std::string
to_string(const outpost::container::RingBufferBase::OperationResult& res)
{
    using OperationResult = outpost::container::RingBufferBase::OperationResult;
    switch (res)
    {
        case OperationResult::success: return "OperationResult::success";
        case OperationResult::notEnoughSpace: return "OperationResult::notEnoughSpace";
        case OperationResult::readWriteTooBig: return "OperationResult::readWriteTooBig";
        case OperationResult::readWriteOverBoundary:
            return "OperationResult::readWriteOverBoundary";
        default: return "INVALID OperationResult";
    }
}

void
outpost::container::PrintTo(const outpost::container::RingBufferBase::OperationResult& res,
                            std::ostream* os)
{
    *os << to_string(res);
}
