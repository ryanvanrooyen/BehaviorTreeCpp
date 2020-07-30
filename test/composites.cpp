
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "mocks.hpp"

#include <vector>

using std::vector;
using namespace bt;


TEST_CASE("Parallel Failure 1")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .parallel(3, Parallel::Policy::RequireAll, Parallel::Policy::RequireAll)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Failure)
            .end();

        CHECK(tree->tick() == Status::Failure);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 3);
    CHECK(info.destroyCount == 3);
}

TEST_CASE("Parallel Failure 2")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .parallel(3, Parallel::Policy::RequireAll, Parallel::Policy::RequireAll)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Success)
                .create<MockNode>(info, Status::Failure)
            .end();

        CHECK(tree->tick() == Status::Failure);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 3);
    CHECK(info.destroyCount == 3);
}

TEST_CASE("Parallel Failure 3")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .parallel(3, Parallel::Policy::RequireAll, Parallel::Policy::RequireOne)
                .create<MockNode>(info, Status::Failure, "MockNode1")
                .create<MockNode>(info, Status::Success, "MockNode2")
                .create<MockNode>(info, Status::Success, "MockNode3")
            .end();

        CHECK(tree->tick() == Status::Failure);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 1);
    CHECK(info.destroyCount == 3);
}

TEST_CASE("Parallel Failure 4")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .parallel(3, Parallel::Policy::RequireAll, Parallel::Policy::RequireOne)
                .create<MockNode>(info, Status::Success, "MockNode1")
                .create<MockNode>(info, Status::Failure, "MockNode2")
                .create<MockNode>(info, Status::Success, "MockNode3")
            .end();

        CHECK(tree->tick() == Status::Failure);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 2);
    CHECK(info.destroyCount == 3);
}

TEST_CASE("Parallel Failure 5")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .parallel(3, Parallel::Policy::RequireAll, Parallel::Policy::RequireOne)
                .create<MockNode>(info, Status::Success, "MockNode1")
                .create<MockNode>(info, Status::Success, "MockNode2")
                .create<MockNode>(info, Status::Failure, "MockNode3")
            .end();

        CHECK(tree->tick() == Status::Failure);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 3);
    CHECK(info.destroyCount == 3);
}

TEST_CASE("Parallel Success 1")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .parallel(3, Parallel::Policy::RequireAll, Parallel::Policy::RequireAll)
                .create<MockNode>(info, Status::Success)
                .create<MockNode>(info, Status::Success)
                .create<MockNode>(info, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Success);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 3);
    CHECK(info.destroyCount == 3);
}

TEST_CASE("Parallel Success 2")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .parallel(3, Parallel::Policy::RequireOne, Parallel::Policy::RequireAll)
                .create<MockNode>(info, Status::Success)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Failure)
            .end();

        CHECK(tree->tick() == Status::Success);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 1);
    CHECK(info.destroyCount == 3);
}

