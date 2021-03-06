/*  ========================================================================
    Copyright (C) 2020 Eaton
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    ========================================================================
*/
#include "fty/split.h"
#include <catch2/catch.hpp>


TEST_CASE("Split utils")
{
    SECTION("Vector")
    {
        auto vec = fty::split("It's dead, that's what's wrong with it.", " ");
        CHECK(std::vector<std::string>{"It's", "dead,", "that's", "what's", "wrong", "with", "it."} == vec);
        CHECK(fty::split("", ";").empty());
    }

    SECTION("Vector, skip empty")
    {
        auto vec = fty::split("this||is|an|ex-parrot|", "|");
        CHECK(std::vector<std::string>{"this", "is", "an", "ex-parrot"} == vec);
    }

    SECTION("Vector, nothing to split")
    {
        auto vec = fty::split("ex-parrot", "|");
        CHECK(std::vector<std::string>{"ex-parrot"} == vec);
    }

    SECTION("Vector, trim")
    {
        auto vec = fty::split("Norwegian    ,    Blue", ",");
        CHECK(std::vector<std::string>{"Norwegian", "Blue"} == vec);

        auto vec2 = fty::split("Norwegian    ,    Blue", ",", fty::SplitOption::NoTrim);
        CHECK(std::vector<std::string>{"Norwegian    ", "    Blue"} == vec2);
    }

    SECTION("Vector, split regex")
    {
        static std::regex re(",+");
        auto              vec = fty::split("this,,is,,,,an,,,ex-parrot", re, fty::SplitOption::KeepEmpty);
        CHECK(std::vector<std::string>{"this", "is", "an", "ex-parrot"} == vec);
        CHECK(fty::split("", re).empty());
    }

    SECTION("Tuple")
    {
        auto tuple = fty::split<std::string, int>("sense of life = 42", "=");
        CHECK(std::make_tuple("sense of life", 42) == tuple);
        auto [name, val] = tuple;
        CHECK("sense of life" == name);
        CHECK(42 == val);

        auto tuple2 = fty::split<std::string, int>("sense of life", "=");
        CHECK(std::make_tuple("sense of life", 0) == tuple2);

        auto tuple3 = fty::split<std::string, int>("sense of life = 42 = 66", "=");
        CHECK(std::make_tuple("sense of life", 42) == tuple3);

        auto [name1, val1] = fty::split<std::string, int>("", ";");
        CHECK(name1.empty());
        CHECK(0 == val1);
    }

    SECTION("Tuple, regex")
    {
        static std::regex re("=+");

        auto tuple = fty::split<std::string, int>("sense of life === 42", re);
        CHECK(std::make_tuple("sense of life", 42) == tuple);
        auto [name, val] = tuple;
        CHECK("sense of life" == name);
        CHECK(42 == val);

        auto tuple2 = fty::split<std::string, int>("sense of life", re);
        CHECK(std::make_tuple("sense of life", 0) == tuple2);

        auto tuple3 = fty::split<std::string, int>("sense of life = 42 === 66", re);
        CHECK(std::make_tuple("sense of life", 42) == tuple3);
    }
}

TEST_CASE("Strange split")
{
    static std::regex re("([a-zA-Z0-9]+)\\s*=\\s*\"([^\"]+)\"");
    auto [key, value] = fty::split<std::string, std::string>("key = \"value\"", re);
    CHECK("key" == key);
    CHECK("value" == value);
}
