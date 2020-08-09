
#include "doctest.h"
#include "mocks.hpp"
#include <vector>

using std::vector;
using namespace bt;


TEST_CASE("Node Update 1")
{
    MockNodeInfo info;
    {
        auto tree = Builder(2014)
            .create<MockNode>(info, vector<Status>{Status::Failure, Status::Success})
            .end();

        CHECK(tree->tick() == Status::Failure);
        CHECK(tree->tick() == Status::Success);
    }

    CHECK(info.createCount == 1);
    CHECK(info.updateCount == 2);
    CHECK(info.destroyCount == 1);
}
