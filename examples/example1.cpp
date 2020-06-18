
#include <iostream>
#include <unistd.h>
#include <initializer_list>
#include <memory>
#include <array>
#include "../source/all.h"

using namespace bt;
using std::shared_ptr;


Status attackPlayer() { return Status::Failure; }
Status mockSuccessAction() { return Status::Success; }
Status mockRunningAction() { return Status::Running; }
Status mockFailureAction() { return Status::Failure; }
bool canSeePlayer() { return true; }

AsyncNode* testAsyncAction = nullptr;

void mockAsyncAction(AsyncAction& action)
{
    testAsyncAction = &action;
}


class Custom: public Node
{
public:
    Custom(const std::initializer_list<Status>& results) : results(results) {}
    virtual const char* name() const noexcept override { return "Custom"; }
protected:
    virtual Status update() noexcept override
    {
        if (++index >= results.size())
            index = 0;
        return results[index];
    }
    virtual ~Custom() override { std::cout << "Deleting custom action." << std::endl; }
private:
    std::vector<Status> results;
    int index = -1;
};


shared_ptr<BehaviorTree> create()
{
    BehaviorTreeBuilder builder(1024);

    shared_ptr<BehaviorTree> attack = builder
        .sequence(3)
            .negate().check("CanSeePlayer", canSeePlayer)
            .action("GoToPlayer", mockSuccessAction)
            .action("AttackPlayer", mockFailureAction)
        .end();

    shared_ptr<BehaviorTree> patrol = builder
        .parallel(5, Parallel::Policy::RequireAll, Parallel::Policy::RequireOne)
            .action("GoToPointA", mockSuccessAction)
            .negate().create<Custom, std::initializer_list<Status>>({Status::Running, Status::Success})
            // .action("AsyncTest1", mockAsyncAction)
            .action("GoToPointB", mockSuccessAction)
            .action("GoToPointC", mockSuccessAction)
            .action("GoToPointD", mockRunningAction)
        .end();

    return builder
        .selector(2)
            .action("Attack", attack)
            .action("Patrol", patrol)
        .end();
}


int main()
{
    auto tree = create();

    int numbOfTicks = 2;
    for (int i = 0; i < numbOfTicks; ++i)
    {
        tree->tick();
        std::cout << std::endl;
        std::cout << *tree << std::endl;
        sleep(1);
    }

    return 0;
}
