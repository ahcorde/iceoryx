// Copyright (c) 2022 by Apex.AI Inc. All rights reserved.
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
//
// SPDX-License-Identifier: Apache-2.0

#ifndef IOX_HOOFS_DESIGN_PATTERN_POLYMORPHIC_HANDLER_HPP
#define IOX_HOOFS_DESIGN_PATTERN_POLYMORPHIC_HANDLER_HPP

#include <atomic>
#include <iostream>
#include <type_traits>

#include "iceoryx_hoofs/design_pattern/static_lifetime_guard.hpp"

namespace iox
{
namespace design_pattern
{
namespace detail
{

/// @brief default hooks for the PolymorphicHandler
/// @tparam Interface the handler interface
/// @note template hooks to avoid forced virtual inheritance
template <typename Interface>
struct DefaultHooks
{
    /// @brief called if the polymorphic handler is set or reset after finalize
    /// @param currentInstance the current instance of the handler singleton
    /// @param newInstance the instance of the handler singleton to be set
    static void onSetAfterFinalize(Interface& currentInstance, Interface& newInstance) noexcept;
};

} // namespace detail

/// @brief Implements a singleton handler that has a default instance and can be changed
/// to another instance at runtime. All instances have to derive from the same interface.
/// The singleton handler owns the default instance but all other instances are created externally.
/// @tparam Interface The interface of the handler instances.
/// @tparam Default The type of the default instance. Must be equal to or derive from Interface.
/// @tparam Hooks A struct that implements onSetAfterFinalize which is called when
/// attempting to set or reset the handler after finalize was called.
///
/// @note In the special case where Default equals Interface, no polymorphism is required.
/// It is then possible to e.g. switch between multiple instances of Default type.
/// @note The lifetime of external non-default instances must exceed the lifetime of the PolymorphicHandler.
/// @note The PolymorphicHandler is guaranteed to provide a valid handler during the whole program lifetime (static).
/// It is hence not advisable to have other static variables depend on the PolymorphicHandler.
/// It must be ensured that they are destroyed before the PolymorphicHandler.
/// @note Hooks must implement
/// static void onSetAfterFinalize(Interface& /*currentInstance*/, Interface& /*newInstance*/).
template <typename Interface, typename Default, typename Hooks = detail::DefaultHooks<Interface>>
class PolymorphicHandler
{
    static_assert(std::is_base_of<Interface, Default>::value, "Default must inherit from Interface");

    using Self = PolymorphicHandler<Interface, Default, Hooks>;
    friend class StaticLifetimeGuard<Self>;

  public:
    /// @brief set the current singleton instance
    /// @return the current instance
    /// @note we cannot be sure to use the current handler unless we call get,
    /// i.e. a reference obtained from get may reference a previous handler
    /// (that is still functional but inactive)
    static Interface& get() noexcept;

    /// @brief set the current singleton instance
    /// @param handlerGuard a guard to the handler instance to be set
    /// @return pointer to the previous instance or nullptr if the handler could not be replaced
    /// @note the handler cannot be replaced if it was finalized
    /// @note using a guard in the interface prevents the handler to be destroyed while it is used,
    ///       passing the guard by value is necessary (it has no state anyway)
    template <typename Handler>
    static Interface* set(StaticLifetimeGuard<Handler> handlerGuard) noexcept;

    /// @brief reset the current singleton instance to the default instance
    /// @return pointer to the previous instance
    static Interface* reset() noexcept;

    /// @brief finalizes the instance, afterwards Hooks::onSetAfterFinalize
    ///        will be called during the remaining program lifetime
    ///        when attempting to set or reset the handler
    static void finalize() noexcept;

    /// @brief returns a lifetime guard whose existence guarantees
    /// the created PolymorphicHandler singleton instance will exist at least as long as the guard.
    /// @return opaque lifetime guard object for the (implicit) PolymorphicHandler instance
    /// @note the PolymorphicHandler will exist once any of the static methods (get, set etc.)
    ///  are called
    static StaticLifetimeGuard<Self> guard() noexcept;

  private:
    // should a defaultHandler be created, this delays its destruction
    StaticLifetimeGuard<Default> m_defaultGuard;
    std::atomic_bool m_isFinal{false};
    std::atomic<Interface*> m_current;

    PolymorphicHandler() noexcept;

    static PolymorphicHandler& instance() noexcept;

    static Default& getDefault() noexcept;

    static Interface* getCurrent() noexcept;

    static Interface* setHandler(Interface& handler) noexcept;
};

} // namespace design_pattern
} // namespace iox

#include "iceoryx_hoofs/internal/design_pattern/polymorphic_handler.inl"

#endif // IOX_HOOFS_DESIGN_PATTERN_POLYMORPHIC_HANDLER_HPP
