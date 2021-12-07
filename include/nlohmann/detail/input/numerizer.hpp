
#include <cstdlib> // strtof, strtod, strtold, strtoll, strtoull

#include <nlohmann/detail/macro_scope.hpp>

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

    JSON_HEDLEY_NON_NULL(2)
    static unsigned long long int strtoull(const char* str, char** endptr, int base)
    {
        return std::strtoull(str, endptr, base);
    }

    JSON_HEDLEY_NON_NULL(2)
    static long long int strtoll(const char* str, char** endptr, int base)
    {
        return std::strtoll(str, endptr, base);
    }
};

}  // namespace detail
}  // namespace nlohmann
