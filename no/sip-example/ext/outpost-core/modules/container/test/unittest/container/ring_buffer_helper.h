#ifndef MODULES_UTILS_TEST_UNITTEST_CONTAINER_RING_BUFFER_HELPER_H
#define MODULES_UTILS_TEST_UNITTEST_CONTAINER_RING_BUFFER_HELPER_H

#include <outpost/container/ring_buffer.h>

#include <string>

std::string
to_string(const outpost::container::RingBufferBase::OperationResult& res);

namespace outpost::container
{
// For gtest printing of values
void
PrintTo(const RingBufferBase::OperationResult& res, std::ostream* os);

}  // namespace outpost::container

#endif /* MODULES_UTILS_TEST_UNITTEST_CONTAINER_RING_BUFFER_HELPER_H */
