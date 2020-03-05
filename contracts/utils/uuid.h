#pragma ones

#include <string>
#include "../types.h"

namespace uuid {
static constexpr uint64_t nullID = UINT64_MAX;
inline static types::uuidType toUUID(std::string data) {
    return std::hash<std::string>{}(data);
}
};  // namespace uuid