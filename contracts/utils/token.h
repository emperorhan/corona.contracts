#pragma ones

#include <eosio/asset.hpp>

namespace token {
static const eosio::symbol S_LED("LED", 4);
static const eosio::symbol S_CORONA("CORONA", 0);  // nft token
// TABLE nft {
//     id_type token_id;  // Unique 64 bit identifier,
//     name owner;        // token owner
//     name tokenName;    // token name
//     name spender;      // token spender

//     id_type primary_key() const { return token_id; }
//     uint64_t get_owner() const { return owner.value; }
// };
};  // namespace token