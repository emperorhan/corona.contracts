#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <string>
#include <vector>

namespace types {
// suspected cases 의심환자
// quarantined cases 격리환자
// confirmed cases 확진환자
// proven cases 증명된 환자
// donated cases 지원받은 환자
enum status : uint8_t {
    STATUS_LOCATION_AGREEMENT,
    STATUS_SUSPECTED_CASES,
    STATUS_QUARANTINED_CASES,
    STATUS_CONFIRMED_CASES,
    STATUS_PROVEN_CASES,
    STATUS_DONATED_CASES
};

enum tiers : uint8_t {
    IRON = 0,
    BRONZE = 5,
    SILVER = 10,
    GOLD = 15,
    PLATINUM = 20,
    DIAMOND = 25
};

typedef uint64_t uuidType;
typedef uint64_t idType;

struct transferArgs {
    eosio::name from;
    eosio::name to;
    eosio::asset quantity;
    std::string memo;
};

struct eventArgs {
    eosio::name from;
    eosio::asset quantity;
    std::string action;
    std::vector<std::string> param;
};

};  // namespace types