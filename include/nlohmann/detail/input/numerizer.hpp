
#pragma once


#include <cmath> // isfinite
#include <cstdint> // uint64_t, int64_t
#include <cstdlib> // strtof, strtod, strtold, strtoll, strtoull

#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/detail/meta/cpp_future.hpp>

namespace nlohmann
{
namespace detail
{

struct numerizer
{
    JSON_HEDLEY_NON_NULL(2)
    static void strtof(float& f, const char* str, char** endptr) noexcept
    {
        f = std::strtof(str, endptr);
    }

    JSON_HEDLEY_NON_NULL(2)
    static void strtof(double& f, const char* str, char** endptr) noexcept
    {
        f = std::strtod(str, endptr);
    }

    JSON_HEDLEY_NON_NULL(2)
    static void strtof(long double& f, const char* str, char** endptr) noexcept
    {
        f = std::strtold(str, endptr);
    }

    template <typename number_float_t,
              detail::enable_if_t<std::is_floating_point<number_float_t>::value,
                                  int> = 0>
    static bool isfinite(number_float_t f)
    {
        return std::isfinite(f);
    }

    JSON_HEDLEY_NON_NULL(2)
    static uint64_t strtoull(const char* str, char** endptr, int base)
    {
        return std::strtoull(str, endptr, base);
    }

    JSON_HEDLEY_NON_NULL(2)
    static int64_t strtoll(const char* str, char** endptr, int base)
    {
        return std::strtoll(str, endptr, base);
    }
};

}  // namespace detail
}  // namespace nlohmann
