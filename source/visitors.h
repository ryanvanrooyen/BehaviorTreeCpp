
#pragma once

#include "nodes.h"
#include "composites.h"
#include "decorators.h"
#include <iostream>

namespace bt
{

class BehaviorTreeVisitor
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


class Serializer : public BehaviorTreeVisitor
{
public:
    Serializer(std::ostream& out, bool status = true, bool expand = false)
        : out{out}, expand{expand}, status{status}, depth{0} {}
    virtual void begin() override { depth = 0; }
    virtual void beforeChildNodes(const Composite& node) override { ++depth; }
    virtual void visit(const Node& node) override { print(node); }
    virtual void visit(const Decorator& node) override;
    virtual void visit(const SubTree& tree) override;
    virtual void afterChildNodes(const Composite& node) override { --depth; }
private:
    void print(const Node& node);
    int depth;
    bool expand;
    bool status;
    std::ostream& out;
};

}
