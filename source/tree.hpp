
#ifndef BEHAVIOR_TREE_TREE_H
#define BEHAVIOR_TREE_TREE_H

#include "nodes.hpp"
#include "visitors.hpp"
#include "memory.hpp"
#include "scheduler.hpp"

namespace bt
{

class BehaviorTree : public Observer
{
public:
    Status tick()
    {
        if (schedulerStopped)
        {
            schedulerStopped = false;
            scheduler->start(*root, *this);
        }
        scheduler->tick();
        return root->status();
    }

    void stop()
    {
        scheduler->stop(*root);
    }

    void traverse(Visitor& visitor) const
    {
        visitor.begin();
        root->traverse(visitor);
        visitor.end();
    }

    ~BehaviorTree()
    {
        stop();
        root->~Node();
        root = nullptr;
    }

    friend class Memory;
    friend class SubTree;
    Status status() const noexcept { return root->status(); }
protected:
    virtual void onComplete(class Scheduler& scheduler, const Node& root, Status status) noexcept override
    {
        schedulerStopped = true;
    }
private:
    BehaviorTree(Node& root,
        const std::shared_ptr<Memory>& memory,
        const std::shared_ptr<Scheduler>& scheduler)
        : root(&root), memory(memory), scheduler(scheduler) {}

    Node* root;
    std::shared_ptr<Memory> memory;
    std::shared_ptr<Scheduler> scheduler;
    bool schedulerStopped = true;
};

std::ostream& operator<<(std::ostream& os, const BehaviorTree& tree)
{
    TextSerializer serializer(os, true);
    tree.traverse(serializer);
    return os;
}

}

#endif
