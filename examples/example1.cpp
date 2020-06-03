
#include <initializer_list>
#include <iostream>
#include <unistd.h>
#include "../source/bt.h"

using namespace bt;

Status attackPlayer() { return Status::Failure; }
Status mockSuccessAction() { return Status::Success; }
Status mockRunningAction() { return Status::Running; }
bool canSeePlayer() { return true; }


int main()
{
    BehaviorTreeBuilder builder;

    BehaviorTree* patrol = builder
        .sequence(3)
            .action("GotToPointA", mockSuccessAction)
            .action("GotToPointB", mockRunningAction)
            .action("GotToPointC", mockSuccessAction)
        .end();

    BehaviorTree* attack = builder
        .sequence(3)
            .negate().check("CanSeePlayer", canSeePlayer)
            .action("GoToPlayer", mockRunningAction)
            .action("AttackPlayer", mockSuccessAction)
        .end();

    BehaviorTree* mainTree = builder
        .selector(2)
            .action("Attack", *attack)
            .action("Patrol", *patrol)
        .end();

    int numbOfTicks = 2;
    for (int i = 0; i < numbOfTicks; ++i)
    {
        mainTree->tick();
        std::cout << *mainTree;
        sleep(1);
    }
}
