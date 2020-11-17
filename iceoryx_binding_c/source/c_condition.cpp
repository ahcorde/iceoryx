// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "iceoryx_binding_c/internal/cpp2c_enum_translation.hpp"
#include "iceoryx_posh/popo/condition.hpp"

using namespace iox;
using namespace iox::popo;

extern "C" {
#include "iceoryx_binding_c/condition.h"
}

bool iox_cond_has_triggered(iox_cond_t const self)
{
    return self->hasTriggered();
}

bool iox_cond_is_condition_variable_attached(iox_cond_t const self)
{
    return self->isConditionVariableAttached();
}

iox_ConditionType iox_cond_get_type(iox_cond_t const self)
{
    return cpp2c::ConditionType(self->getType());
}

