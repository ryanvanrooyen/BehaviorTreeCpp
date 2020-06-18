
#ifndef BEHAVIOR_TREE_TREE_H
#define BEHAVIOR_TREE_TREE_H

#include "nodes.h"
#include "visitors.h"
#include "memory.h"
#include "scheduler.h"

namespace bt
{

class BehaviorTree : public Observer
{
public:
    void tick()
    {
        if (isFirstTick)
        {
            isFirstTick = false;
            scheduler->start(*root, *this);
        }
        scheduler->tick();
    }

    void stop()
    {
        if (root && scheduler)
            scheduler->stop(*root);
    }

    void traverse(Visitor& visitor) const
    {
        visitor.begin();
        if (root)
            root->traverse(visitor);
        visitor.end();
    }

    ~BehaviorTree()
    {
        if (root)
        {
            stop();
            root->~Node();
            root = nullptr;
        }
    }

    friend class Memory;
    friend class SubTree;
protected:
    virtual void onComplete(class Scheduler& scheduler, const Node& root, Status status) noexcept override
    {
        // When the root node completes, just reschedule it no matter if it succeeded or failed:
        scheduler.start(*this->root, *this);
    }
private:
    BehaviorTree(Node* root,
        const std::shared_ptr<Memory>& memory,
        const std::shared_ptr<Scheduler>& scheduler)
        : root(root), memory(memory), scheduler(scheduler) {}

    Node* root;
    std::shared_ptr<Memory> memory;
    std::shared_ptr<Scheduler> scheduler;
    bool isFirstTick = true;
};

std::ostream& operator<<(std::ostream& os, const BehaviorTree& tree)
{
    TextSerializer serializer(os, true);
    tree.traverse(serializer);
    return os;
}

}

#endif
