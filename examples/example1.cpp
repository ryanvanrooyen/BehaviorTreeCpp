
#include <iostream>
#include <unistd.h>
#include <initializer_list>
#include <memory>
#include <array>
#include "../source/bt.h"

using namespace bt;
using std::shared_ptr;


Status attackPlayer() { return Status::Failure; }
Status mockSuccessAction() { return Status::Success; }
Status mockRunningAction() { return Status::Running; }
Status mockFailureAction() { return Status::Failure; }
bool canSeePlayer() { return true; }


class Custom1: public Node
{
public:
    Custom1(const std::vector<Status>& results) : results(results) {}
    virtual const char* name() const noexcept override { return "Custom1"; }
    virtual Status update() noexcept override
    {
        if (++index >= results.size())
            index = 0;
        return results[index];
    }
private:
    std::vector<Status> results;
    int index = -1;
};


class Custom2: public Node
{
public:
    virtual const char* name() const noexcept override { return "Custom2"; }
    virtual Status update() noexcept override
    {
        std::cout << "Performing custom action 2..." << std::endl;
        return Status::Success;
    }
    virtual ~Custom2() override { std::cout << "Deleting custom action 2." << std::endl; }
};


shared_ptr<BehaviorTree> create()
{
    BehaviorTreeBuilder builder(1024);

    Custom1 c1({Status::Success, Status::Success});

    shared_ptr<BehaviorTree> attack = builder
        .sequence(3)
            .negate().check("CanSeePlayer", canSeePlayer)
            .action("GoToPlayer", mockSuccessAction)
            .action("AttackPlayer", mockFailureAction)
        .end();

    shared_ptr<BehaviorTree> patrol = builder
        .sequence(5)
            .action("GoToPointA", mockSuccessAction)
            .create<Custom1>(std::vector<Status>({Status::Running, Status::Success}))
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
