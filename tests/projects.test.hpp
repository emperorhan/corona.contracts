#include <boost/test/unit_test.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/testing/tester.hpp>
#include "Runtime/Runtime.h"
#include "contracts.hpp"
#include "test_symbol.hpp"

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace crypto;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

class project_tester : public tester {
   public:
    project_tester() {
        produce_blocks(2);

        create_accounts({N(alice), N(bob), N(carol), N(lily),
                         N(yeop), N(pepp), N(project), N(ibct), N(led.token)});
        produce_blocks(2);

        set_code(N(led.token), contracts::token_wasm());
        set_abi(N(led.token), contracts::token_abi().data());

        produce_blocks();

        const auto& token_accnt = control->db().get<account_object, by_name>(N(led.token));
        abi_def token_abi;
        BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(token_accnt.abi, token_abi), true);
        token_abi_ser.set_abi(token_abi, abi_serializer_max_time);

        set_code(N(project), contracts::project_wasm());
        set_abi(N(project), contracts::project_abi().data());

        produce_blocks();

        const auto& project_accnt = control->db().get<account_object, by_name>(N(project));
        abi_def project_abi;
        BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(project_accnt.abi, project_abi), true);
        project_abi_ser.set_abi(project_abi, abi_serializer_max_time);

        produce_blocks(2);

        create(N(project), asset::from_string("100000000.0000 ED"));
        issue(N(project), N(project), asset::from_string("100000000.0000 ED"), "issue");

        produce_blocks(10);
    }

    asset get_balance(const name& act) {
        const auto& db = control->db();
        const auto* tbl = db.find<table_id_object, by_code_scope_table>(boost::make_tuple(N(led.token), act, N(accounts)));
        share_type result = 0;

        // the balance is implied to be 0 if either the table or row does not exist
        if (tbl) {
            const auto* obj = db.find<key_value_object, by_scope_primary>(boost::make_tuple(tbl->id, symbol(PROJECT_SYM).to_symbol_code()));
            if (obj) {
                // balance is the first field in the serialization
                fc::datastream<const char*> ds(obj->value.data(), obj->value.size());
                fc::raw::unpack(ds, result);
            }
        }
        return asset(result, symbol(PROJECT_SYM));
    }

    string uint64_to_string(uint64_t value) {
        string result;
        result.reserve(20);  // max. 20 digits possible
        uint64_t q = value;
        do {
            result += "0123456789"[q % 10];
            q /= 10;
        } while (q);
        std::reverse(result.begin(), result.end());
        return result;
    }

    fc::variant get_creator(const name& act) {
        vector<char> data = get_row_by_account(N(project), N(project), N(creators), act);
        return data.empty() ? fc::variant() : project_abi_ser.binary_to_variant("CreatorInfo", data, abi_serializer_max_time);
    }

    fc::variant get_test() {
        vector<char> data = get_row_by_account(N(project), N(project), N(test), 1);
        return data.empty() ? fc::variant() : project_abi_ser.binary_to_variant("Test", data, abi_serializer_max_time);
    }

    fc::variant get_product(const uint64_t& id) {
        vector<char> data = get_row_by_account(N(project), N(project), N(products), id);
        return data.empty() ? fc::variant() : project_abi_ser.binary_to_variant("ProductInfo", data, abi_serializer_max_time);
    }

    fc::variant get_config_state() {
        vector<char> data = get_row_by_account(N(project), N(project), N(config), N(config));
        if (data.empty())
            std::cout << "\nData is empty\n"
                      << std::endl;
        return data.empty() ? fc::variant() : project_abi_ser.binary_to_variant("ConfigInfo", data, abi_serializer_max_time);
    }

    action_result push_action(const name& signer, const action_name& name, const variant_object& data) {
        string action_type_name = project_abi_ser.get_action_type(name);

        action act;
        act.account = N(project);
        act.name = name;
        act.data = project_abi_ser.variant_to_binary(action_type_name, data, abi_serializer_max_time);

        return base_tester::push_action(std::move(act), uint64_t(signer));
    }

    transaction_trace_ptr push_multsign_action(const vector<name>& signer, const action_name& name, const variant_object& data) {
        string action_type_name = project_abi_ser.get_action_type(name);
        action act;
        act.account = N(project);
        act.name = name;
        act.data = project_abi_ser.variant_to_binary(action_type_name, data, abi_serializer_max_time);

        vector<permission_level> permission;
        for (const auto& sign : signer) {
            permission_level p = {sign, N(active)};
            permission.emplace_back(p);
        }
        act.authorization = permission;

        signed_transaction trx;
        // expire time test
        trx.actions.emplace_back(std::move(act));
        set_transaction_headers(trx);  //, 86000 * 2, 0);
        for (const auto authorizer : signer)
            trx.sign(get_private_key(authorizer, "active"), control->get_chain_id());

        // produce_block(fc::seconds(85000 * 2));
        // produce_block();

        return base_tester::push_transaction(trx);
    }

    action_result push_action_token(const name& signer, const action_name& name, const variant_object& data) {
        string action_type_name = token_abi_ser.get_action_type(name);

        action act;
        act.account = N(led.token);
        act.name = name;
        act.data = token_abi_ser.variant_to_binary(action_type_name, data, abi_serializer_max_time);

        return base_tester::push_action(std::move(act), uint64_t(signer));
    }

    action_result create(name issuer,
                         asset maximum_supply) {
        return push_action_token(N(ibct), N(create), mvo()("issuer", issuer)("maximum_supply", maximum_supply));
    }

    action_result issue(name issuer, name to, asset quantity, string memo) {
        return push_action_token(issuer, N(issue), mvo()("to", to)("quantity", quantity)("memo", memo));
    }

    action_result transfer(name from,
                           name to,
                           asset quantity,
                           string memo) {
        return push_action_token(from, N(transfer), mvo()("from", from)("to", to)("quantity", quantity)("memo", memo));
    }

    action_result setcreator(const name& creator) {
        return push_action(N(project), N(setcreator), mvo()("owner", creator));
    }

    action_result setcustomer(const name& customer) {
        return push_action(N(project), N(setcustomer), mvo()("owner", customer));
    }

    action_result setpubkey(const public_key& projectPubKey) {
        return push_action(N(project), N(setpubkey), mvo()("projectPubKey", projectPubKey));
    }

    action_result editstatus(const uint64_t& productId, const uint8_t status) {
        return push_action(N(project), N(editstatus), mvo()("productId", productId)("status", status));
    }

    action_result addproduct(const uint64_t& productId, const name& creator, const asset& price, const string& productName, const string& description, const uint64_t& remixId, const uint64_t& ratio) {
        if (remixId == UINT64_MAX) {
            return push_action(N(project), N(addproduct), mvo()("productId", productId)("creator", creator)("price", price)("productName", productName)("description", description)("remixId", nullptr)("ratio", nullptr));
        } else {
            return push_action(N(project), N(addproduct), mvo()("productId", productId)("creator", creator)("price", price)("productName", productName)("description", description)("remixId", remixId)("ratio", ratio));
        }
    }

    action_result postreview(const name& owner, const uint64_t& product, const uint64_t& reviewId, const uint64_t& rating, const string& contents) {
        return push_action(N(project), N(postreview), mvo()("reviewer", owner)("product", product)("reviewId", reviewId)("rating", rating)("contents", contents));
    }

    action_result givecoupon() {
        return push_action(N(project), N(givecoupon), mvo());
    }

    action_result giverewards() {
        return push_action(N(project), N(giverewards), mvo());
    }

    abi_serializer token_abi_ser;
    abi_serializer project_abi_ser;
};