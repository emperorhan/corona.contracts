#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <string>
#include <vector>

namespace types {

enum tiers : uint8_t {
    IRON = 0,
    BRONZE = 5,
    SILVER = 10,
    GOLD = 15,
    PLATINUM = 20,
    DIAMOND = 25
};

typedef uint64_t uuidType;

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