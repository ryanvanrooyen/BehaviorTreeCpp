
#include <iostream>
#include <unistd.h>
#include <memory>
#include <array>
#include "../source/bt.h"

using namespace bt;
using std::shared_ptr;


Status attackPlayer() { return Status::Failure; }
Status mockSuccessAction() { return Status::Success; }
Status mockRunningAction() { return Status::Running; }
bool canSeePlayer() { return true; }


class Custom: public Action
{
public:
    Custom(const char* name) : Action(name) {}
    virtual Status update() override
    {
        std::cout << "Performing custom action..." << std::endl;
        return Status::Success;
    }
    virtual ~Custom() override { std::cout << "Deleting custom action." << std::endl; }
};


shared_ptr<BehaviorTree> create()
{
    auto memory = std::make_shared<Memory>(1024);
    BehaviorTreeBuilder builder1(memory);
    BehaviorTreeBuilder builder2(memory);

    shared_ptr<BehaviorTree> patrol = builder1
        .sequence(3)
            .create<Custom>("CustomAction1")
            .action("GoToPointA", mockSuccessAction)
            .action("GoToPointB", mockRunningAction)
            .action("GoToPointC", mockSuccessAction)
        .end();

    shared_ptr<BehaviorTree> attack = builder2
        .sequence(3)
            .negate().check("CanSeePlayer", canSeePlayer)
            .action("GoToPlayer", mockRunningAction)
            .action("AttackPlayer", mockSuccessAction)
        .end();

    return builder2
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
