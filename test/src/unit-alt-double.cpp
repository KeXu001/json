/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++ (test suite)
|  |  |__   |  |  | | | |  version 3.10.4
|_____|_____|_____|_|___|  https://github.com/nlohmann/json

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2018 Vitaliy Manushkin <agri@akamo.info>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "doctest_compatibility.h"

#include <cmath>
#include <limits>
#include <string>
#include <type_traits>

#include <nlohmann/json.hpp>

/*
 * This is virtually a fundamental numeric type.
 * It covers a fundamental numeric type under the hood.
 *
 * The only difference is, finite values are stringified with six digits of
 * precision
 */
template <typename raw_t = double>
class alt_double_tpl
{
  public:
    alt_double_tpl() = default;

    alt_double_tpl(raw_t f_)
        : f(f_)
    {}

    alt_double_tpl(std::int64_t i)
        : f(static_cast<raw_t>(i))
    {}

    alt_double_tpl(std::uint64_t i)
        : f(static_cast<raw_t>(i))
    {}

    alt_double_tpl(int i)
        : f(i)
    {}

    alt_double_tpl(const alt_double_tpl& a) = default;

    alt_double_tpl& operator=(const alt_double_tpl& a) = default;

    alt_double_tpl(alt_double_tpl&& a) noexcept = default;

    alt_double_tpl& operator=(alt_double_tpl&&) noexcept = default;

    ~alt_double_tpl() = default;

    bool operator==(const alt_double_tpl& a) const
    {
        return f == a.f;
    }

    bool operator!=(const alt_double_tpl& a) const
    {
        return f != a.f;
    }

    bool operator<(const alt_double_tpl& a) const
    {
        return f < a.f;
    }

    bool operator>(const alt_double_tpl& a) const
    {
        return f > a.f;
    }

    bool operator<=(const alt_double_tpl& a) const
    {
        return f <= a.f;
    }

    bool operator>=(const alt_double_tpl& a) const
    {
        return f >= a.f;
    }

    static bool isfinite(const alt_double_tpl& a)
    {
        return std::isfinite(a.f);
    }

    static std::string to_string(const alt_double_tpl& a)
    {
        return std::to_string(a.f);
    }

  private:
    raw_t f;
};

using alt_double = alt_double_tpl<double>;

struct alt_numerizer : nlohmann::detail::numerizer
{
    static void strtof(alt_double& x, const char* str, char** endptr) noexcept
    {
        x = alt_double(std::strtod(str, endptr));
    }

    static bool isfinite(const alt_double& x)
    {
        return alt_double::isfinite(x);
    }
};

template<typename U, typename V>
using alt_lexer = nlohmann::detail::lexer<U, V, alt_numerizer>;

struct alt_denumerizer : nlohmann::detail::denumerizer
{
    template <typename OutputAdapterType>
    static void dump_float(OutputAdapterType& o, const alt_double& x)
    {
        if (!alt_double::isfinite(x))
        {
            o->write_characters("null", 4);
            return;
        }

        std::string s(alt_double::to_string(x));

        o->write_characters(s.c_str(), s.length());
    }
};

template<typename U>
using alt_serializer = nlohmann::detail::serializer<U, alt_denumerizer>;

using alt_json = nlohmann::basic_json <
                 std::map,
                 std::vector,
                 std::string,
                 bool,
                 std::int64_t,
                 std::uint64_t,
                 alt_double,
                 std::allocator,
                 nlohmann::adl_serializer,
                 std::vector<uint8_t>,
                 alt_lexer,
                 alt_serializer >;


TEST_CASE("alternative double type")
{
    SECTION("dump")
    {
        {
            alt_json doc;
            doc["foo"] = alt_json(nlohmann::detail::value_t::number_float);
            std::string dump = doc.dump();
            CHECK(dump == R"({"foo":0.000000})");
        }

        {
            alt_json doc;
            doc["foo"] = alt_double(1.2);
            std::string dump = doc.dump();
            CHECK(dump == R"({"foo":1.200000})");
        }

        {
            alt_json doc;
            doc["foo"] = alt_double(std::numeric_limits<double>::quiet_NaN());
            std::string dump = doc.dump();
            CHECK(dump == R"({"foo":null})");
        }

        {
            alt_json doc;
            doc["foo"] = alt_double(std::numeric_limits<double>::infinity());
            std::string dump = doc.dump();
            CHECK(dump == R"({"foo":null})");
        }
    }

    SECTION("copy")
    {
        {
            alt_json doc;
            doc["foo"] = alt_double(1.2);

            alt_json doc_copy(doc);
            std::string dump = doc_copy.dump();
            CHECK(dump == R"({"foo":1.200000})");
        }

        {
            alt_json doc;
            doc["foo"] = alt_double(1.2);

            alt_json doc_copy = doc;
            std::string dump = doc_copy.dump();
            CHECK(dump == R"({"foo":1.200000})");
        }
    }

    SECTION("parse")
    {
        {
            auto doc = alt_json::parse(R"({"foo": 1.2})");
            std::string dump = doc.dump();
            CHECK(dump == R"({"foo":1.200000})");
        }

        {
            auto doc = alt_json::parse(R"({"foo": 5.0})");
            std::string dump = doc.dump();
            CHECK(dump == R"({"foo":5.000000})");
        }

        {
            auto doc = alt_json::parse(R"({"foo": 1.2})");
            for (const auto& item : doc.items())
            {
                CHECK(item.key() == "foo");
                CHECK(item.value() == alt_double(1.2));
                CHECK(alt_double(1.2) == item.value());
            }
        }

        {
            auto doc = alt_json::parse(R"({"foo": 1.2})");
            CHECK(doc["foo"] == alt_double(1.2));
            CHECK(alt_double(1.2) == doc["foo"]);
        }

        {
            alt_json doc = R"({"foo":1.2})"_json;
            std::string dump = doc.dump();
            CHECK(dump == R"({"foo":1.200000})");
        }
    }

    SECTION("equality")
    {
        {
            alt_json doc;
            doc["foo"] = alt_double(1.2);
            CHECK(doc["foo"] == alt_double(1.2));
            CHECK(alt_double(1.2) == doc["foo"]);
        }

        {
            alt_json doc;
            doc["foo"] = alt_double(1.2);
            for (const auto& item : doc.items())
            {
                CHECK(item.key() == "foo");
                CHECK(item.value() == alt_double(1.2));
                CHECK(alt_double(1.2) == item.value());
            }
        }

        {
            alt_json doc1;
            alt_json doc2;
            doc1["foo"] = alt_double(1.2);
            doc2["foo"] = alt_double(1.2);
            CHECK(doc1["foo"] == doc2["foo"]);
        }

        {
            alt_json doc;
            doc["foo"] = alt_double(5.0);
            CHECK(doc["foo"] == 5);
            CHECK(5 == doc["foo"]);
        }

        {
            alt_json doc;
            doc["foo"] = 5;
            CHECK(doc["foo"] == alt_double(5));
            CHECK(alt_double(5) == doc["foo"]);
        }

        {
            alt_json doc;
            doc["foo"] = alt_double(-5.0);
            CHECK(doc["foo"] == -5);
            CHECK(-5 == doc["foo"]);
        }

        {
            alt_json doc;
            doc["foo"] = -5;
            CHECK(doc["foo"] == alt_double(-5));
            CHECK(alt_double(-5) == doc["foo"]);
        }
    }
}
