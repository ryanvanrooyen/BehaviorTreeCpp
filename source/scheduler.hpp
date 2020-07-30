
#ifndef BEHAVIOR_TREE_SCHEDULER_H
#define BEHAVIOR_TREE_SCHEDULER_H

#include <deque>
#include <algorithm>
#include "nodes.hpp"

namespace bt
{

class Scheduler
{
public:
    explicit Scheduler(size_t initialSize)
        : runningNodes(std::deque<Node*>()) {}

    void tick()
    {
        if (runningNodes.size() == 0)
            return;

        // Insert an end-of-update marker into the list of tasks.
        runningNodes.push_back(nullptr);

        // Keep going updating tasks until we encounter the nullptr marker:
        while (true)
        {
            Node* current = runningNodes.front();
            runningNodes.pop_front();

            if (current == nullptr)
                return;

            current->tick(*this);

            // If currently running, drop it into the queue for next tick:
            if (current->nodeStatus == Status::Running)
            {
                runningNodes.push_back(current);
            }
            else if (current->nodeStatus != Status::Suspended)
            {
                // Notify observer that task completed:
                if (current->observer)
                    current->observer->onComplete(*this, *current, current->nodeStatus);
            }
        }
    }

    void start(Node& node, Observer& observer) noexcept
    {
        node.observer = &observer;
        runningNodes.push_front(&node);
    }

    void completed(Node& node, Status result) noexcept
    {
        node.nodeStatus = result;
        if (node.observer)
            node.observer->onComplete(*this, node, result);
    }

    void stop(Node& node) noexcept
    {
        if (node.nodeStatus == Status::Running || node.nodeStatus == Status::Suspended)
        {
            node.stop(*this);
            node.nodeStatus = Status::Failure;
        }

        // Remove the node from the queue if it exists:
        auto node_pos = std::find(runningNodes.begin(), runningNodes.end(), &node);
        if (node_pos != runningNodes.end())
            runningNodes.erase(node_pos);
    }
private:
    std::deque<Node*> runningNodes;
};

}

#endif
