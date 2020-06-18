
#ifndef BEHAVIOR_TREE_VISITORS_H
#define BEHAVIOR_TREE_VISITORS_H

#include "nodes.h"
#include "decorators.h"
#include "composites.h"

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

    virtual void visit(const Decorator& node) override
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

    virtual void visit(const SubTree& tree) override
    {
        if (expand)
            tree.traverseSubTree(*this);
        else
            visit((Node&) tree);
    }

protected:
    virtual void print(const char* name, Status status, const char* prefix = nullptr)
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

private:
    int depth = 0;
    bool expand;
    std::ostream& out;
};

}

#endif
