#pragma ones

#include <libc/bits/stdint.h>
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

namespace time {
static constexpr uint32_t secondsPerYear = 52 * 7 * 24 * 3600;
static constexpr uint32_t secondsPerMonth = 4 * 7 * 24 * 3600;  // secondsPerYear / 12
static constexpr uint32_t secondsPerWeek = 24 * 3600 * 7;
static constexpr uint32_t secondsPerDay = 24 * 3600;
static constexpr uint32_t secondsPerHour = 3600;
static constexpr uint32_t secondsPerMinute = 60;
static constexpr int64_t usecondsPerYear = int64_t(secondsPerYear) * 1000000ll;
static constexpr int64_t usecondsPerMonth = int64_t(secondsPerMonth) * 1000000ll;
static constexpr int64_t usecondsPerWeek = int64_t(secondsPerWeek) * 1000000ll;
static constexpr int64_t usecondsPerDay = int64_t(secondsPerDay) * 1000000ll;
static constexpr int64_t usecondsPerHour = int64_t(secondsPerHour) * 1000000ll;
static constexpr int64_t usecondsPerMinute = int64_t(secondsPerMinute) * 1000000ll;
static constexpr uint32_t blocksPerYear = 2 * secondsPerYear;    // half seconds per year
static constexpr uint32_t blocksPerMonth = 2 * secondsPerMonth;  // half seconds per month
static constexpr uint32_t blocksPerWeek = 2 * secondsPerWeek;    // half seconds per week
static constexpr uint32_t blocksPerDay = 2 * secondsPerDay;      // half seconds per day

// 한국시간은 협정 세계시 +9:00
eosio::time_point currentTimePoint() {
    return eosio::current_time_point() += eosio::microseconds(usecondsPerHour * 9);
}
};  // namespace time
