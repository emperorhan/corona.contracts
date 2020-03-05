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
#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/crypto.hpp>
#include <eosio/singleton.hpp>

#include "../../../types.h"
#include "../../../utils/hash.h"
#include "../../../utils/table.h"
#include "../../../utils/time.h"

#define TEST

namespace project_contracts {
struct [[eosio::table("config"), eosio::contract("project_contracts")]] ConfigInfo {
    eosio::public_key PubKey;
    eosio::time_point lastUpdate;
    EOSLIB_SERIALIZE(ConfigInfo, (PubKey)(lastUpdate));
};

typedef eosio::singleton<name("config"), ConfigInfo> configSingleton;

class [[eosio::contract("project_contracts")]] project_contracts : public eosio::contract {
   private:
    configSingleton _config;
    ConfigInfo _cstate;
    ConfigInfo getDefaultConfig() {
        return ConfigInfo{
            eosio::public_key(),
            time::currentTimePoint()};
    };
    // receive_recipient에 따른 핸들러(ex. transfer)
    template <typename T>
    void transferEventHandler(T func);
    // TODO: 핸들러가 실행할 내부 function 정의

    // TODO: 내부 private method 정의
   public:
    project_contracts(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds)
        : contract(receiver, code, ds), _config(receiver, receiver.value) {
        _cstate = _config.exists() ? _config.get() : getDefaultConfig();
    }
    ~project_contracts() {
        _config.set(_cstate, get_self());
    }
    [[eosio::action]] void clean();

    [[eosio::action]] void temp();

    [[eosio::on_notify("led.token::transfer")]] void transferevnt();
};
};  // namespace project_contracts
