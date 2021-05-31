
// Copyright Henrik Steffen Gaßmann 2020
//
// Distributed under the Boost Software License, Version 1.0.
//         (See accompanying file LICENSE or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <dplx/dp/object_def.hpp>
#include <dplx/dp/tag_invoke.hpp>
#include <dplx/dp/tuple_def.hpp>

namespace dplx::dp
{

inline constexpr struct layout_descriptor_for_fn
{
    template <typename T>
        requires tag_invocable<layout_descriptor_for_fn, std::type_identity<T>>
    constexpr auto operator()(std::type_identity<T>) const
            noexcept(nothrow_tag_invocable<layout_descriptor_for_fn,
                                           std::type_identity<T>>)
                    -> tag_invoke_result_t<layout_descriptor_for_fn,
                                           std::type_identity<T>>
    {
        return ::dplx::dp::cpo::tag_invoke(*this, std::type_identity<T>{});
    }

    // clang-format off
    template <typename T>
    requires is_object_def_v<std::remove_cvref_t<decltype(T::layout_descriptor)>>
    friend constexpr decltype(auto)
            tag_invoke(layout_descriptor_for_fn, std::type_identity<T>) noexcept
    // clang-format on
    {
        return T::layout_descriptor;
    }

    // clang-format off
    template <typename T>
    requires is_tuple_def_v<std::remove_cvref_t<decltype(T::layout_descriptor)>>
    friend constexpr decltype(auto)
            tag_invoke(layout_descriptor_for_fn, std::type_identity<T>) noexcept
    // clang-format on
    {
        return T::layout_descriptor;
    }

} layout_descriptor_for;

template <typename T>
concept packable
        = tag_invocable<layout_descriptor_for_fn, std::type_identity<T>>;

template <typename T>
concept packable_object = packable<T> && is_object_def_v<std::remove_cvref_t<
        decltype(layout_descriptor_for(std::type_identity<T>{}))>>;

template <typename T>
concept packable_tuple = packable<T> && is_tuple_def_v<std::remove_cvref_t<
        decltype(layout_descriptor_for(std::type_identity<T>{}))>>;

inline constexpr std::uint32_t null_def_version = 0xffff'ffffu;

} // namespace dplx::dp

namespace dplx::dp::detail
{

template <typename T>
constexpr auto versioned_decoder_enabled(T const &descriptor) noexcept -> bool
{
    return descriptor.allow_versioned_auto_decoder
        || descriptor.version == null_def_version;
}

} // namespace dplx::dp::detail
