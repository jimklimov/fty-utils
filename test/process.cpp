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
#include "fty/process.h"
#include <catch2/catch.hpp>

TEST_CASE("Process")
{
    SECTION("Run process")
    {
        auto process = fty::Process("echo", {"-n", "hello"});

        if (auto ret = process.run()) {
            auto pid = process.wait();
            CHECK(process.readAllStandardOutput() == "hello");
        } else {
            FAIL(ret.error());
        }
    }

    SECTION("Std err")
    {
        auto process = fty::Process("sh", {"-c", "1>&2 echo -n hello"});

        if (auto ret = process.run()) {
            auto pid = process.wait();
            CHECK("hello" == process.readAllStandardError());
        } else {
            FAIL(ret.error());
        }
    }

    SECTION("Run invalid process")
    {
        auto process = fty::Process("/usr/bin/bad", {"-n", "hello"});
        auto pid     = process.run();
        CHECK(!pid.isValid());
        CHECK(!pid);
        CHECK("posix_spawnp failed with error: No such file or directory" == pid.error());
    }

    SECTION("Set environment variable")
    {
        auto process = fty::Process("sh", {"-c", "export | grep MYVAR"});
        process.setEnvVar("MYVAR", "test-value");

        if (auto pid = process.run()) {
            CHECK(process.readAllStandardOutput() == "export MYVAR='test-value'\n");

            if (auto status = process.wait()) {
                CHECK(*status == 0);
            } else {
                FAIL(status.error());
            }
        } else {
            FAIL(pid.error());
        }
    }

    SECTION("Kill process")
    {
        auto process = fty::Process("/usr/bin/python", {});

        if (auto pid = process.run()) {
            auto status = process.wait(100);
            CHECK(!status);
            CHECK("timeout" == status.error());
            process.kill();
            CHECK(!process.exists());
        } else {
            FAIL(pid.error());
        }
    }

    SECTION("Interrupt process")
    {
        auto process = fty::Process("/bin/cat", {});

        if (auto pid = process.run()) {
            auto status = process.wait(100);
            CHECK(!status);
            process.interrupt();
            CHECK(!process.exists());
        } else {
            FAIL(pid.error());
        }
    }
}
