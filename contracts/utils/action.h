#pragma ones

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/action.hpp>
#include <string>

namespace action {
void transferToken(const eosio::name& from, const eosio::name& to, const eosio::asset& quantity, const std::string& memo) {
    eosio::action(eosio::permission_level{from, eosio::name("active")},
                  eosio::name("led.token"),
                  eosio::name("transfer"),
                  make_tuple(from, to, quantity, memo))
        .send();
}
};  // namespace action