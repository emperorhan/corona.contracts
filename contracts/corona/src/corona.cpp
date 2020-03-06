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

#include "../include/corona/corona.hpp"

namespace corona {
void corona::setuser(const eosio::name& name) {
    require_auth(get_self());
    is_account(name);

    usersTable usertable(get_self(), get_self().value);
    auto uitr = usertable.find(name.value);
    eosio::check(uitr == usertable.end(), "이미 등록된 회원입니다.");

    usertable.emplace(get_self(), [&](UserInfo& u) {
        u.name = name;
        u.order = _cstate.totalUserCount++;
        u.score = 0;
        u.status = 0;
    });
}

void corona::regsuspect(const eosio::name& name) {
    require_auth(name);

    usersTable usertable(get_self(), get_self().value);
    auto uitr = usertable.find(name.value);
    eosio::check(uitr != usertable.end(), "등록된 회원이 아닙니다.");

    const auto ct = time::currentTimePoint();
    if (uitr->status & BIT(types::STATUS_SUSPECTED_CASES)) {
        uint32_t passedDay = ((ct.sec_since_epoch() / time::secondsPerDay) - (uitr->lastSuspectUpdate.sec_since_epoch() / time::secondsPerDay));
        eosio::check(passedDay > 2, "의심증상자로 (재)등록한지 3일이 지나야 합니다.");
        eosio::check(passedDay < 4, "의심증상자로 재등록할 수 있는 기간이 지났습니다.");

        uint32_t totalPassedDay = ((ct.sec_since_epoch() / time::secondsPerDay) - (uitr->setSuspectTime.sec_since_epoch() / time::secondsPerDay));
        eosio::check(totalPassedDay < 11, "더이상 의심증상자로 등록할 수 없습니다.");

        usertable.modify(uitr, get_self(), [&](UserInfo& u) {
            u.score += 5;
            u.lastSuspectUpdate = ct;
        });
    } else {
        eosio::check(!uitr->setSuspectTime.sec_since_epoch(), "의심증상자를 해지했을 경우 다시 등록이 불가능합니다.");

        usertable.modify(uitr, get_self(), [&](UserInfo& u) {
            u.status |= BIT(types::STATUS_SUSPECTED_CASES);
            u.score = 10;
            u.setSuspectTime = ct;
            u.lastSuspectUpdate = ct;
        });
    }
}

void corona::regisolate(const eosio::name& name) {
    require_auth(name);

    usersTable usertable(get_self(), get_self().value);
    auto uitr = usertable.find(name.value);
    eosio::check(uitr != usertable.end(), "등록된 회원이 아닙니다.");

    const auto ct = time::currentTimePoint();
    if (uitr->status & BIT(types::STATUS_QUARANTINED_CASES)) {
        uint32_t passedDay = ((ct.sec_since_epoch() / time::secondsPerDay) - (uitr->lastSuspectUpdate.sec_since_epoch() / time::secondsPerDay));
        eosio::check(passedDay > 2, "자가격리자로 (재)등록한지 3일이 지나야 합니다.");
        eosio::check(passedDay < 4, "자가격리자로 재등록할 수 있는 기간이 지났습니다.");

        uint32_t totalPassedDay = ((ct.sec_since_epoch() / time::secondsPerDay) - (uitr->setSuspectTime.sec_since_epoch() / time::secondsPerDay));
        eosio::check(totalPassedDay < 8, "더이상 자가격리자로 등록할 수 없습니다.");

        usertable.modify(uitr, get_self(), [&](UserInfo& u) {
            u.score += 10;
            u.lastSelfIsolationUpdate = ct;
        });
    } else {
        eosio::check(!uitr->setSelfIsolationTime.sec_since_epoch(), "자가격리자를 해지했을 경우 다시 등록이 불가능합니다.");

        usertable.modify(uitr, get_self(), [&](UserInfo& u) {
            u.status |= BIT(types::STATUS_QUARANTINED_CASES);
            u.score = 20;
            u.setSelfIsolationTime = ct;
            u.lastSelfIsolationUpdate = ct;
        });
    }
}

void corona::unregsuspect(const eosio::name& name) {
    require_auth(name);

    usersTable usertable(get_self(), get_self().value);
    auto uitr = usertable.find(name.value);
    eosio::check(uitr != usertable.end(), "등록된 회원이 아닙니다.");
    eosio::check(uitr->status & BIT(types::STATUS_SUSPECTED_CASES), "의심증상자로 등록되어 있지 않습니다.");

    usertable.modify(uitr, get_self(), [&](UserInfo& u) {
        u.status &= ~BIT(types::STATUS_SUSPECTED_CASES);
    });
}

void corona::unregisolate(const eosio::name& name) {
    require_auth(name);

    usersTable usertable(get_self(), get_self().value);
    auto uitr = usertable.find(name.value);
    eosio::check(uitr != usertable.end(), "등록된 회원이 아닙니다.");
    eosio::check(uitr->status & BIT(types::STATUS_QUARANTINED_CASES), "자가격리자로 등록되어 있지 않습니다.");

    usertable.modify(uitr, get_self(), [&](UserInfo& u) {
        u.status &= ~BIT(types::STATUS_QUARANTINED_CASES);
    });
}

void corona::locationserv(const eosio::name& name, const bool& agreement) {
    require_auth(name);

    usersTable usertable(get_self(), get_self().value);
    auto uitr = usertable.find(name.value);
    eosio::check(uitr != usertable.end(), "등록된 회원이 아닙니다.");

    eosio::check((uitr->status & BIT(types::STATUS_LOCATION_AGREEMENT)) != agreement, "이미 위치서비스 제공에 대한 동의 및 거절이 승인된 상태입니다.");

    usertable.modify(uitr, get_self(), [&](UserInfo& u) {
        u.status |= BIT(types::STATUS_LOCATION_AGREEMENT);
    });
}
};  // namespace corona