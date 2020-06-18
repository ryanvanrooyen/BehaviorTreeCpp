
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "mocks.h"

using std::vector;
using namespace bt;


int factorial(int number) { return number <= 1 ? number : factorial(number - 1) * number; }

TEST_CASE("Basic Node Tests") {

    MockNodeInfo info;
    {
        auto tree = BehaviorTreeBuilder(2014)
            .create<MockNode>("Node1", info, vector<Status>({Status::Failure, Status::Success}))
            .end();

        tree->tick();
    }

    CHECK(info.destroyed == true);
}
