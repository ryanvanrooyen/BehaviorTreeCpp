
#include "doctest.h"
#include "../behavior_tree.hpp"
#include "mocks.hpp"
#include <vector>

using std::vector;
using namespace bt;


TEST_CASE("Sequence Success")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .sequence(3)
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


TEST_CASE("Sequence Suspended 1")
{
    MockNodeInfo info1, info2, info3;
    {
        auto tree = Builder(2014)
            .sequence(3)
                .create<MockNode>(info1, Status::Running)
                .create<MockNode>(info2, Status::Success)
                .create<MockNode>(info3, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Suspended);
        CHECK(tree->tick() == Status::Suspended);
    }

    CHECK(info1.createCount == 1);
    CHECK(info1.updateCount == 2);
    CHECK(info1.destroyCount == 1);

    CHECK(info2.createCount == 1);
    CHECK(info2.updateCount == 0);
    CHECK(info2.destroyCount == 1);

    CHECK(info3.createCount == 1);
    CHECK(info3.updateCount == 0);
    CHECK(info3.destroyCount == 1);
}


TEST_CASE("Sequence Suspended 2")
{
    MockNodeInfo info1, info2, info3;
    {
        auto tree = Builder(2014)
            .sequence(3)
                .create<MockNode>(info1, Status::Success)
                .create<MockNode>(info2, Status::Running)
                .create<MockNode>(info3, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Suspended);
        CHECK(tree->tick() == Status::Suspended);
    }

    CHECK(info1.createCount == 1);
    CHECK(info1.updateCount == 1);
    CHECK(info1.destroyCount == 1);

    CHECK(info2.createCount == 1);
    CHECK(info2.updateCount == 2);
    CHECK(info2.destroyCount == 1);

    CHECK(info3.createCount == 1);
    CHECK(info3.updateCount == 0);
    CHECK(info3.destroyCount == 1);
}


TEST_CASE("Sequence Suspended 3")
{
    MockNodeInfo info1, info2, info3;
    {
        auto tree = Builder(2014)
            .sequence(3)
                .create<MockNode>(info1, Status::Success)
                .create<MockNode>(info2, vector<Status>{ Status::Running, Status::Success })
                .create<MockNode>(info3, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Suspended);
        CHECK(tree->tick() == Status::Success);
    }

    CHECK(info1.createCount == 1);
    CHECK(info1.updateCount == 1);
    CHECK(info1.destroyCount == 1);

    CHECK(info2.createCount == 1);
    CHECK(info2.updateCount == 2);
    CHECK(info2.destroyCount == 1);

    CHECK(info3.createCount == 1);
    CHECK(info3.updateCount == 1);
    CHECK(info3.destroyCount == 1);
}


TEST_CASE("Sequence Suspended 4")
{
    MockNodeInfo info1, info2, info3;
    {
        auto tree = Builder(2014)
            .sequence(3)
                .create<MockNode>(info1, Status::Success)
                .create<MockNode>(info2, Status::Suspended)
                .create<MockNode>(info3, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Suspended);
        CHECK(tree->tick() == Status::Suspended);
    }

    CHECK(info1.createCount == 1);
    CHECK(info1.updateCount == 1);
    CHECK(info1.destroyCount == 1);

    CHECK(info2.createCount == 1);
    CHECK(info2.updateCount == 1);
    CHECK(info2.destroyCount == 1);

    CHECK(info3.createCount == 1);
    CHECK(info3.updateCount == 0);
    CHECK(info3.destroyCount == 1);
}


TEST_CASE("Sequence Failure 1")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .sequence(3)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Failure)
            .end();

        CHECK(tree->tick() == Status::Failure);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 1);
    CHECK(info.destroyCount == 3);
}


TEST_CASE("Sequence Failure 2")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .sequence(3)
                .create<MockNode>(info, Status::Success)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Failure)
            .end();

        CHECK(tree->tick() == Status::Failure);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 2);
    CHECK(info.destroyCount == 3);
}


TEST_CASE("Sequence Failure 3")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .sequence(3)
                .create<MockNode>(info, Status::Success)
                .create<MockNode>(info, Status::Success)
                .create<MockNode>(info, Status::Failure)
            .end();

        CHECK(tree->tick() == Status::Failure);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 3);
    CHECK(info.destroyCount == 3);
}


TEST_CASE("Selector Success 1")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .selector(3)
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


TEST_CASE("Selector Success 2")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .selector(3)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Success)
                .create<MockNode>(info, Status::Failure)
            .end();

        CHECK(tree->tick() == Status::Success);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 2);
    CHECK(info.destroyCount == 3);
}


TEST_CASE("Selector Success 3")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .selector(3)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Success);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 3);
    CHECK(info.destroyCount == 3);
}


TEST_CASE("Selector Suspended 1")
{
    MockNodeInfo info1, info2, info3;
    {
        auto tree = Builder(2014)
            .selector(3)
                .create<MockNode>(info1, Status::Running)
                .create<MockNode>(info2, Status::Success)
                .create<MockNode>(info3, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Suspended);
        CHECK(tree->tick() == Status::Suspended);
    }

    CHECK(info1.createCount == 1);
    CHECK(info1.updateCount == 2);
    CHECK(info1.destroyCount == 1);

    CHECK(info2.createCount == 1);
    CHECK(info2.updateCount == 0);
    CHECK(info2.destroyCount == 1);

    CHECK(info3.createCount == 1);
    CHECK(info3.updateCount == 0);
    CHECK(info3.destroyCount == 1);
}


TEST_CASE("Selector Suspended 2")
{
    MockNodeInfo info1, info2, info3;
    {
        auto tree = Builder(2014)
            .selector(3)
                .create<MockNode>(info1, Status::Failure)
                .create<MockNode>(info2, Status::Running)
                .create<MockNode>(info3, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Suspended);
        CHECK(tree->tick() == Status::Suspended);
    }

    CHECK(info1.createCount == 1);
    CHECK(info1.updateCount == 1);
    CHECK(info1.destroyCount == 1);

    CHECK(info2.createCount == 1);
    CHECK(info2.updateCount == 2);
    CHECK(info2.destroyCount == 1);

    CHECK(info3.createCount == 1);
    CHECK(info3.updateCount == 0);
    CHECK(info3.destroyCount == 1);
}


TEST_CASE("Selector Suspended 3")
{
    MockNodeInfo info1, info2, info3;
    {
        auto tree = Builder(2014)
            .selector(3)
                .create<MockNode>(info1, Status::Failure)
                .create<MockNode>(info2, vector<Status>{ Status::Running, Status::Success })
                .create<MockNode>(info3, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Suspended);
        CHECK(tree->tick() == Status::Success);
    }

    CHECK(info1.createCount == 1);
    CHECK(info1.updateCount == 1);
    CHECK(info1.destroyCount == 1);

    CHECK(info2.createCount == 1);
    CHECK(info2.updateCount == 2);
    CHECK(info2.destroyCount == 1);

    CHECK(info3.createCount == 1);
    CHECK(info3.updateCount == 0);
    CHECK(info3.destroyCount == 1);
}


TEST_CASE("Selector Suspended 4")
{
    MockNodeInfo info1, info2, info3;
    {
        auto tree = Builder(2014)
            .selector(3)
                .create<MockNode>(info1, Status::Failure)
                .create<MockNode>(info2, Status::Suspended)
                .create<MockNode>(info3, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Suspended);
        CHECK(tree->tick() == Status::Suspended);
    }

    CHECK(info1.createCount == 1);
    CHECK(info1.updateCount == 1);
    CHECK(info1.destroyCount == 1);

    CHECK(info2.createCount == 1);
    CHECK(info2.updateCount == 1);
    CHECK(info2.destroyCount == 1);

    CHECK(info3.createCount == 1);
    CHECK(info3.updateCount == 0);
    CHECK(info3.destroyCount == 1);
}


TEST_CASE("Selector Failure")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .selector(3)
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
            .parallel(3, Parallel::Policy::RequireAll)
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
            .parallel(3, Parallel::Policy::RequireOne)
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


TEST_CASE("Parallel Success 3")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .parallel(3, Parallel::Policy::RequireOne)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Success)
                .create<MockNode>(info, Status::Failure)
            .end();

        CHECK(tree->tick() == Status::Success);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 2);
    CHECK(info.destroyCount == 3);
}


TEST_CASE("Parallel Success 4")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .parallel(3, Parallel::Policy::RequireOne)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Failure)
                .create<MockNode>(info, Status::Success)
            .end();

        CHECK(tree->tick() == Status::Success);
    }

    CHECK(info.createCount == 3);
    CHECK(info.updateCount == 3);
    CHECK(info.destroyCount == 3);
}
