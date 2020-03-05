#pragma ones

#include <eosio/eosio.hpp>

namespace table {
template <typename T>
void cleanTable(eosio::name self, uint64_t scope = 0) {
    uint64_t s = scope ? scope : self.value;
    T db(self, s);
    while (db.begin() != db.end()) {
        auto itr = --db.end();
        db.erase(itr);
    }
}
};  // namespace table