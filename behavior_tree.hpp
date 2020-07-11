
#ifndef BEHAVIOR_TREE_H
#define BEHAVIOR_TREE_H

#include <cstdint>
#include <cstring>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>


#ifndef BEHAVIOR_TREE_STATUS_H
#define BEHAVIOR_TREE_STATUS_H


namespace bt
{

enum class Status
{
    Initial,
    Running,
    Success,
    Failure,
    Suspended,
};

inline std::ostream& operator<<(std::ostream& os, const Status& s)
{
    if (s == Status::Success) os << "Success";
    if (s == Status::Running) os << "Running";
    if (s == Status::Failure) os << "Failure";
    if (s == Status::Initial) os << "Initial";
    if (s == Status::Suspended) os << "Suspended";
    return os;
}

}

#endif

#ifndef BEHAVIOR_TREE_NODES_H
#define BEHAVIOR_TREE_NODES_H


namespace bt
{

class Node
{
public:
    virtual const char* name() const noexcept { return "Node"; }
    Status status() const noexcept { return nodeStatus; }
    virtual void traverse(class Visitor& visitor) const;
    virtual ~Node() {}
    friend class Scheduler;
protected:
    virtual void start(class Scheduler& scheduler) noexcept {}
    virtual Status update() noexcept = 0;
    virtual void stop(class Scheduler& scheduler) noexcept {}
private:
    void tick(class Scheduler& scheduler) noexcept
    {
        if (nodeStatus != Status::Running)
            start(scheduler);
        nodeStatus = update();
        std::cout << "Tick Node: " << name() << " " << nodeStatus << std::endl;
    }
    Status nodeStatus = Status::Initial;
    class Observer* observer = nullptr;
};


class Observer
{
public:
    friend class Scheduler;
protected:
    virtual void onComplete(class Scheduler& scheduler, const Node& node, Status status) noexcept = 0;
};


class NamedNode: public Node
{
public:
    NamedNode(const char* name) : nodeName(name) {}
    virtual const char* name() const noexcept override { return nodeName; }
private:
    const char* nodeName;
};


class SubTree : public NamedNode, public Observer
{
public:
    SubTree(const char* name, const std::shared_ptr<class BehaviorTree>& tree)
        : NamedNode(name), tree(tree) {}

    virtual void traverse(Visitor& visitor) const override;
    void traverseSubTree(Visitor& visitor) const;
protected:
    virtual void start(class Scheduler& scheduler) noexcept override;
    virtual Status update() noexcept override { return Status::Suspended; }
    virtual void stop(class Scheduler& scheduler) noexcept override;
    virtual void onComplete(class Scheduler& scheduler, const Node& root, Status status) noexcept override;
private:
    std::shared_ptr<class BehaviorTree> tree;
};


typedef bool (*ConditionFunction) ();

class Condition: public NamedNode
{
public:
    Condition(const char* name, ConditionFunction check)
        : NamedNode(name), check(check) {}
protected:
    virtual Status update() noexcept override
    {
        try
        {
            return check() ? Status::Success : Status::Failure;
        }
        catch (...)
        {
            return Status::Failure;
        }
    }
private:
    ConditionFunction check;
};


typedef Status (*ActionFunction) ();

class Action : public NamedNode
{
public:
    Action(const char* name, ActionFunction action)
        : NamedNode(name), action(action) {}
protected:
    virtual Status update() noexcept override
    {
        try
        {
            return action();
        }
        catch (...)
        {
            return Status::Failure;
        }
    }
private:
    ActionFunction action;
};


class AsyncNode: public Node
{
public:
    virtual const char* name() const noexcept override { return "Async Node"; }
    void succeeded() noexcept;
    void failed() noexcept;
protected:
    virtual void start() noexcept = 0;
    virtual void start(class Scheduler& scheduler) noexcept override;
    virtual Status update() noexcept override { return Status::Suspended; }
private:
    class Scheduler* scheduler;
};


typedef void (*AsyncActionFunction) (class AsyncAction&);

class AsyncAction : public AsyncNode
{
public:
    AsyncAction(const char* name, AsyncActionFunction onStart, AsyncActionFunction onStop = nullptr)
        : nodeName(name), onStart(onStart), onStop(onStop) {}
    virtual const char* name() const noexcept override { return nodeName; }
protected:
    virtual void start() noexcept override
    {
        try
        {
            onStart(*this);
        }
        catch (...)
        {
            failed();
        }
    }

    virtual void stop(class Scheduler& scheduler) noexcept override
    {
        try
        {
            if (onStop)
                onStop(*this);
        }
        catch (...)
        {
        }
    }
private:
    const char* nodeName;
    AsyncActionFunction onStart;
    AsyncActionFunction onStop;
};

}

#endif

#ifndef BEHAVIOR_TREE_DECORATORS_H
#define BEHAVIOR_TREE_DECORATORS_H


namespace bt
{

class Decorator : public Node, public Observer
{
public:
    void setChild(Node* child) { childNode = child; }
    Node* child() const { return childNode; }
    virtual void traverse(class Visitor& visitor) const override;
    virtual ~Decorator() override;
protected:
    virtual void start(class Scheduler& scheduler) noexcept override;
    virtual Status update() noexcept override { return Status::Suspended; }
    virtual void stop(class Scheduler& scheduler) noexcept override;
private:
    Node* childNode = nullptr;
};


class Negate : public Decorator
{
public:
    virtual const char* name() const noexcept override { return "Not"; }
protected:
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) noexcept override;
};

}

#endif

#ifndef BEHAVIOR_TREE_COMPOSITES_H
#define BEHAVIOR_TREE_COMPOSITES_H


namespace bt
{

class Composite : public Node, public Observer
{
public:
    Composite(Node** children, uint16_t childCount)
        : children(children), childCount(childCount) {}

    void addChild(Node* child);
    virtual void traverse(class Visitor& visitor) const override;
    void traverseChildren(class Visitor& visitor) const;
    virtual ~Composite() override;
protected:
    virtual void start(class Scheduler& scheduler) noexcept override;
    virtual Status update() noexcept override { return Status::Suspended; }
    virtual void stop(class Scheduler& scheduler) noexcept override;
    Node** children;
    const uint16_t childCount;
    uint16_t currentIndex = 0;
};


class Sequence : public Composite
{
public:
    using Composite::Composite;
    virtual const char* name() const noexcept override { return "Sequence"; }
protected:
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) noexcept override;
};


class Selector : public Composite
{
public:
    using Composite::Composite;
    virtual const char* name() const noexcept override { return "Selector"; }
protected:
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) noexcept override;
};


class Parallel : public Composite {
public:
    enum class Policy { RequireOne, RequireAll };

    Parallel(Node** children, uint16_t childCount, Policy success, Policy failure)
        : Composite(children, childCount), successPolicy(success), failurePolicy(failure) {}

    virtual const char* name() const noexcept override { return "Parallel"; }
protected:
    virtual void start(class Scheduler& scheduler) noexcept override;
    virtual void onComplete(class Scheduler& scheduler, const Node& child, Status status) noexcept override;
private:
    uint16_t successCount = 0;
    uint16_t failureCount = 0;
    Policy successPolicy;
    Policy failurePolicy;
};

}

#endif

#ifndef BEHAVIOR_TREE_VISITORS_H
#define BEHAVIOR_TREE_VISITORS_H


namespace bt
{

class Visitor
{
public:
    virtual void begin() {}
    virtual void beforeChildNodes(const Composite& node) {}
    virtual void visit(const Node& node) = 0;
    virtual void visit(const Composite& node) { visit((Node&)node); }
    virtual void visit(const Decorator& node) { visit((Node&)node); }
    virtual void visit(const SubTree& tree) { visit((Node&)tree); }
    virtual void afterChildNodes(const Composite& node) {}
    virtual void end() {}
};


class TextSerializer : public Visitor
{
public:
    TextSerializer(std::ostream& out, bool expand = false) : out{out}, expand{expand} {}
    virtual void begin() override { depth = 0; }
    virtual void beforeChildNodes(const Composite& node) override { ++depth; }
    virtual void visit(const Node& node) override { print(node.name(), node.status()); }
    virtual void afterChildNodes(const Composite& node) override { --depth; }
    virtual void visit(const Decorator& node) override;
    virtual void visit(const SubTree& tree) override;
protected:
    virtual void print(const char* name, Status status, const char* prefix = nullptr);
private:
    int depth = 0;
    bool expand;
    std::ostream& out;
};

}

#endif

#ifndef BEHAVIOR_TREE_MEMORY_H
#define BEHAVIOR_TREE_MEMORY_H


namespace bt
{

class Memory
{
public:
    Memory(const size_t maxBytes)
        : buffer(new uint8_t[maxBytes]), maxBytes(maxBytes) {}

    Memory(const Memory& m)
        : buffer(new uint8_t[m.maxBytes]), maxBytes(m.maxBytes), offset(m.offset)
    {
        if (offset > 0)
            memcpy(buffer, m.buffer, offset);
    }

    Memory(Memory&& m) noexcept
        : buffer(m.buffer), offset(m.offset), maxBytes(m.maxBytes)
    {
        m.buffer = nullptr;
    }

    ~Memory() { delete[] buffer; }

    size_t size() const { return offset; }
    size_t maxSize() const { return maxBytes; }

    template <typename T, typename... Args>
    T* allocate(Args&&... args)
    {
        size_t size = sizeof(T);
        if ((offset + size) >= maxBytes)
            throw std::runtime_error("BehaviorTree Memory capacity exceeded.");
		T* instance = new ((void*)((uintptr_t)buffer + offset)) T(std::forward<Args>(args)...);
        offset += size;
        return instance;
    }

    template <typename T>
    T* allocateArray(int length)
    {
        size_t size = sizeof(T) * length;
        if ((offset + size) >= maxBytes)
            throw std::runtime_error("BehaviorTree Memory capacity exceeded.");
		T* instance = new ((void*)((uintptr_t)buffer + offset)) T [length];
        offset += size;
        return instance;
    }

private:
	uint8_t* buffer;
    size_t offset = 0;
    const size_t maxBytes;
};

}

#endif

#ifndef BEHAVIOR_TREE_SCHEDULER_H
#define BEHAVIOR_TREE_SCHEDULER_H


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
    }
private:
    std::deque<Node*> runningNodes;
};

}

#endif

#ifndef BEHAVIOR_TREE_TREE_H
#define BEHAVIOR_TREE_TREE_H


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

#ifndef BEHAVIOR_TREE_BUILDER_H
#define BEHAVIOR_TREE_BUILDER_H


namespace bt
{

class Builder
{
public:
    explicit Builder(const size_t maxBytes = 1024, const size_t initSchedulerSize = 10)
        : memory(std::make_shared<Memory>(maxBytes))
        , scheduler(std::make_shared<Scheduler>(initSchedulerSize)) {}
        // , scheduler(std::shared_ptr<Scheduler>(memory->allocate<Scheduler>(initSchedulerSize))) {}

    Builder(const std::shared_ptr<Memory>& memory, const std::shared_ptr<Scheduler>& scheduler)
        : memory(memory), scheduler(scheduler) {}

    // Nodes:
    Builder& action(const char* name, ActionFunction action) { return create<Action>(name, action); }
    Builder& action(const char* name, AsyncActionFunction onStart, AsyncActionFunction onTerminate = nullptr) { return create<AsyncAction>(name, onStart, onTerminate); }
    Builder& action(const char* name, const std::shared_ptr<BehaviorTree>& tree) { return create<SubTree>(name, tree); }
    Builder&  check(const char* name, ConditionFunction check) { return create<Condition>(name, check); }

    // Composites:
    Builder& selector(uint16_t childCount) { return composite<Selector>(childCount); }
    Builder& sequence(uint16_t childCount) { return composite<Sequence>(childCount); }
    Builder& parallel(uint16_t childCount, Parallel::Policy success, Parallel::Policy failure) { return composite<Parallel>(childCount, success, failure); }

    // Decorators:
    Builder& negate() { return group<Negate>(1); }

    template<typename T, typename... Args>
    Builder& create(Args&&... args) { return group<T>(0, std::forward<Args>(args)...); }

    std::shared_ptr<BehaviorTree> end();

protected:
    template<typename T, typename... Args>
    Builder& composite(uint16_t childCount, Args&&... args)
    {
        Node** children = memory->allocateArray<Node*>(childCount);
        return group<T>(childCount, children, childCount, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    Builder& group(uint16_t childCount, Args&&... args)
    {
        T* node = memory->allocate<T>(std::forward<Args>(args)...);
        addNode(node);
        if (childCount > 0)
            groups.push_back(Group(node, childCount));
        return *this;
    }

private:
    struct Group
    {
        Node* parent;
        int childrenLeftToAdd;
        Group(Node* parent, int children)
            : parent(parent), childrenLeftToAdd(children) {}
    };

    void addNode(Node* node);

    Node* root = nullptr;
    std::shared_ptr<Memory> memory;
    std::shared_ptr<Scheduler> scheduler;
    std::vector<Group> groups = std::vector<Group>();
};

}

#endif



namespace bt
{

inline void Node::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
}

inline void SubTree::start(Scheduler& scheduler) noexcept
{
    if (tree && tree->root)
        scheduler.start(*tree->root, *this);
}

inline void SubTree::stop(Scheduler& scheduler) noexcept
{
    if (tree && tree->root)
        scheduler.stop(*tree->root);
}

inline void SubTree::onComplete(Scheduler& scheduler, const Node& root, Status status) noexcept
{
    scheduler.completed(*this, status);
}

inline void SubTree::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
}

inline void SubTree::traverseSubTree(Visitor& visitor) const
{
    if (tree && tree->root)
        tree->root->traverse(visitor);
}

inline void AsyncNode::start(class Scheduler& scheduler) noexcept
{
    this->scheduler = &scheduler;
    this->start();
}

inline void AsyncNode::succeeded() noexcept
{
    if (status() == Status::Suspended && this->scheduler)
        this->scheduler->completed(*this, Status::Success);
}

inline void AsyncNode::failed() noexcept
{
    if (status() == Status::Suspended && this->scheduler)
        this->scheduler->completed(*this, Status::Failure);
}

}


namespace bt
{

inline void Decorator::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
    if (Composite* composite = dynamic_cast<Composite*>(childNode))
        composite->traverseChildren(visitor);
}

inline void Decorator::start(Scheduler& scheduler) noexcept
{
    scheduler.start(*childNode, *this);
}

inline void Decorator::stop(Scheduler& scheduler) noexcept
{
    if (childNode)
        scheduler.stop(*childNode);
}

inline Decorator::~Decorator()
{
    if (childNode)
    {
        childNode->~Node();
        childNode = nullptr;
    }
}

inline void Negate::onComplete(Scheduler& scheduler, const Node& child, Status status) noexcept
{
    if (status == Status::Success)
        status = Status::Failure;
    else if (status == Status::Failure)
        status = Status::Success;
    scheduler.completed(*this, status);
}

}


namespace bt
{

inline void Composite::addChild(Node* child)
{
    if (child && currentIndex < childCount)
    {
        children[currentIndex] = child;
        ++currentIndex;
    }
}

inline void Composite::start(Scheduler& scheduler) noexcept
{
    currentIndex = 0;
    scheduler.start(*children[0], *this);
}

inline void Composite::traverse(Visitor& visitor) const
{
    visitor.visit(*this);
    traverseChildren(visitor);
}

inline void Composite::traverseChildren(Visitor& visitor) const
{
    visitor.beforeChildNodes(*this);
    for (int i = 0; i < childCount; ++i)
        children[i]->traverse(visitor);
    visitor.afterChildNodes(*this);
}

inline void Composite::stop(Scheduler& scheduler) noexcept
{
    if (children)
    {
        for (uint16_t i = 0; i < childCount; ++i)
            if (Node* child = children[i])
                scheduler.stop(*child);
    }
}

inline Composite::~Composite()
{
    if (children)
    {
        for (uint16_t i = 0; i < childCount; ++i)
            if (Node* child = children[i])
                child->~Node();
        children = nullptr;
    }
}

inline void Sequence::onComplete(Scheduler& scheduler, const Node& child, Status status) noexcept
{
    if (status == Status::Failure)
    {
        scheduler.completed(*this, status);
        return;
    }

    if (++currentIndex == childCount)
        scheduler.completed(*this, Status::Success);
    else
        scheduler.start(*children[currentIndex], *this);
}

inline void Selector::onComplete(Scheduler& scheduler, const Node& child, Status status) noexcept
{
    if (status == Status::Success)
    {
        scheduler.completed(*this, status);
        return;
    }

    if (++currentIndex == childCount)
        scheduler.completed(*this, Status::Failure);
    else
        scheduler.start(*children[currentIndex], *this);
}

inline void Parallel::start(Scheduler& scheduler) noexcept
{
    successCount = 0;
    failureCount = 0;
    currentIndex = 0;

    for (uint16_t i = 0; i < childCount; ++i)
        scheduler.start(*children[i], *this);
}

inline void Parallel::onComplete(Scheduler& scheduler, const Node& child, Status status) noexcept
{
    if (status == Status::Success)
    {
        ++successCount;
        if (successPolicy == Policy::RequireOne)
        {
            stop(scheduler);  // Stop the other running children
            scheduler.completed(*this, Status::Success);
            return;
        }
    }
    else if (status == Status::Failure)
    {
        ++failureCount;
        if (failurePolicy == Policy::RequireOne)
        {
            stop(scheduler);  // Stop the other running children
            scheduler.completed(*this, Status::Failure);
            return;
        }
    }

    // TODO: Handle the situation where both success and failure are set to RequireAll and some succeeded and some fail.
    if (failurePolicy == Policy::RequireAll && failureCount == childCount)
        scheduler.completed(*this, Status::Failure);
    else if (successPolicy == Policy::RequireAll && successCount == childCount)
        scheduler.completed(*this, Status::Success);
}

}


namespace bt
{

inline void TextSerializer::visit(const Decorator& node)
{
    if (Node* child = node.child())
    {
        Status status = node.status();
        if (status == Status::Suspended)
            status = child->status();
        print(child->name(), status, node.name());
    }
    else
    {
        print(node.name(), node.status());
    }
}

inline void TextSerializer::visit(const SubTree& tree)
{
    if (expand)
        tree.traverseSubTree(*this);
    else
        visit((Node&) tree);
}

inline void TextSerializer::print(const char* name, Status status, const char* prefix)
{
    for (int i = 0; i < depth; i++)
        out << "\t";
    if (prefix)
        out << prefix << " ";
    out << name;
    if (status != Status::Initial)
        out << ": " << status;
    out << std::endl;
}

}


namespace bt
{

inline std::shared_ptr<BehaviorTree> Builder::end()
{
    if (!root)
        return nullptr;
    if (groups.size())
        throw std::runtime_error("Invalid BehaviorTree definition. Number of child nodes does not match group node child counts.");
    BehaviorTree* treePtr = memory->allocate<BehaviorTree>(root, memory, scheduler);
    std::shared_ptr<BehaviorTree> tree(treePtr, [](BehaviorTree* t) { t->~BehaviorTree(); });
    root = nullptr;
    return tree;
}

inline void Builder::addNode(Node* node)
{
    if (!root)
    {
        root = node;
        return;
    }
    if (!groups.size())
        throw std::runtime_error("Invalid BehaviorTree definition. Number of child nodes does not match group node child counts.");

    Group& group = groups.back();
    if (group.parent && group.childrenLeftToAdd > 0)
    {
        if (Composite* parent = dynamic_cast<Composite*>(group.parent))
        {
            parent->addChild(node);
            group.childrenLeftToAdd -= 1;
        }
        else if (Decorator* parent = dynamic_cast<Decorator*>(group.parent))
        {
            parent->setChild(node);
            group.childrenLeftToAdd -= 1;
        }

        if (group.childrenLeftToAdd <= 0)
            groups.pop_back();
    }
}

}

#endif
