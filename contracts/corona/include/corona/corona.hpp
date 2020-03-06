#pragma once

/*
 * Copyright 2020 IBCT
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/singleton.hpp>

#include "../../../types.h"
#include "../../../utils/token.h"
#include "../../../utils/time.h"

#define TEST
#define BIT(x) (1 << (x - 1))

namespace corona {

struct [[eosio::table("config"), eosio::contract("corona")]] ConfigInfo {
    uint64_t totalUserCount;
    eosio::symbol donateSymbol;
    EOSLIB_SERIALIZE(ConfigInfo, (totalUserCount)(donateSymbol));
};

struct [[eosio::table, eosio::contract("corona")]] UserInfo {
    // scope: code, ram payer: corona
    eosio::name name;
    uint64_t order;
    double score = 0;

    // STATUS_LOCATION_AGREEMENT    위치서비스 제공 동의
    // STATUS_SUSPECTED_CASES       의심환자
    // STATUS_QUARANTINED_CASES     격리환자
    // STATUS_CONFIRMED_CASES       확진환자
    // STATUS_PROVEN_CASES          증명환자
    // STATUS_DONATED_CASES         기부받은환자
    __int8_t status;

    eosio::time_point lastLocationServiceAgreementTime;

    eosio::time_point setSuspectTime;
    eosio::time_point setSelfIsolationTime;

    eosio::time_point lastSuspectUpdate;
    eosio::time_point lastSelfIsolationUpdate;

    uint64_t primary_key() const { return name.value; }
    uint64_t by_orders() const { return order; }
    double by_scores() const { return -score; }
    double by_confirmed_scores() const { return status & BIT(types::STATUS_PROVEN_CASES) ? -score : score; }

    EOSLIB_SERIALIZE(UserInfo, (name)(order)(score)(status)(lastLocationServiceAgreementTime)(setSuspectTime)(setSelfIsolationTime)(lastSuspectUpdate)(lastSelfIsolationUpdate));
};

struct [[eosio::table, eosio::contract("corona")]] DonateInfo {
    // scope: code, ram payer: corona
    types::idType tokenId;
    eosio::name tokenName;
    std::string description;
    eosio::name owner;
    eosio::time_point receivedTime;
    uint64_t primary_key() const { return tokenId; }
    uint64_t by_owners() const { return owner.value; }

    EOSLIB_SERIALIZE(DonateInfo, (tokenId)(tokenName)(description)(owner)(receivedTime));
};

typedef eosio::singleton<name("config"), ConfigInfo> configSingleton;

typedef eosio::multi_index<name("users"), UserInfo,
                           eosio::indexed_by<name("byscores"), eosio::const_mem_fun<UserInfo, double, &UserInfo::by_scores> >,
                           eosio::indexed_by<name("byconfirms"), eosio::const_mem_fun<UserInfo, double, &UserInfo::by_confirmed_scores> >,
                           eosio::indexed_by<name("byorders"), eosio::const_mem_fun<UserInfo, uint64_t, &UserInfo::by_orders> > >
    usersTable;

typedef eosio::multi_index<name("donates"), DonateInfo,
                           eosio::indexed_by<name("byowners"), eosio::const_mem_fun<DonateInfo, uint64_t, &DonateInfo::by_owners> > >
    donatesTable;

class [[eosio::contract("corona")]] corona : public eosio::contract {
   private:
    configSingleton _config;
    ConfigInfo _cstate;

    ConfigInfo getDefaultConfig() {
        return ConfigInfo{
            0,
            token::S_CORONA};
    };

   public:
    corona(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds)
        : eosio::contract(receiver, code, ds), _config(receiver, receiver.value) {
        _cstate = _config.exists() ? _config.get() : getDefaultConfig();
    }
    ~corona() {
        _config.set(_cstate, get_self());
    }

    [[eosio::action]] void setuser(const eosio::name& name);

    [[eosio::action]] void regsuspect(const eosio::name& name);
    [[eosio::action]] void regisolate(const eosio::name& name);
    [[eosio::action]] void unregsuspect(const eosio::name& name);
    [[eosio::action]] void unregisolate(const eosio::name& name);
    [[eosio::action]] void locationserv(const eosio::name& name, const bool& agreement);

    [[eosio::action]] void recvdotate(const eosio::name& name, const types::idType& tokenId);
};

}  // namespace corona
