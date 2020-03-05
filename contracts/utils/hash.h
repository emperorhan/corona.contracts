#pragma ones

#include <libc/bits/stdint.h>

namespace hash {
static constexpr uint32_t constHash(const char* p) {
    return *p ? static_cast<uint32_t>(*p) + 33 * constHash(p + 1) : 5381;
}
};  // namespace hash
