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

#include "../include/project_contracts/project_contracts.hpp"

namespace project_contracts {
void project_contracts::clean() {
    require_auth(get_self());

    eosio::printl("cleaning", 8);

    _cstate = getDefaultConfig();

    // table::cleanTable<projectsTable>(get_self(), get_self().value);
}

void project_contracts::transferevnt() {
    project_contracts::transferEventHandler([&](const types::eventArgs& e) {
        uint32_t hash = hash::constHash(e.action.c_str());
        switch (hash) {
        case hash::constHash("temp1"):
            break;
        case hash::constHash("temp2"):
            break;
        default:
            break;
        }
    });
}

template <typename T>
void project_contracts::transferEventHandler(T func) {
    auto transferData = eosio::unpack_action_data<types::transferArgs>();
    // TODO: 코드 작성

    if (memo.empty())
        return;

    types::eventArgs res;

    size_t prev = memo.find(':');
    res.from = from;
    res.quantity = transferData.quantity;
    res.action = memo.substr(0, prev);
    size_t pos = prev;
    while (true) {
        pos = memo.find(':', pos + 1);
        if (pos == std::string::npos) {
            res.param.emplace_back(memo.substr(prev + 1));
            break;
        } else {
            res.param.emplace_back(memo.substr(prev + 1, (pos - (prev + 1))));
        }
        prev = pos;
    }
    func(res);
}
}  // namespace project_contracts