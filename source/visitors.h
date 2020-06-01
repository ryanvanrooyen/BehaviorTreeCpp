
#include "nodes.h"
#include "composites.h"
#include <iostream>

namespace bt
{

class NodeVisitor
{
public:
    virtual void visit(const Node& node) = 0;
    virtual void visit(const Composite& node) = 0;
};


class NodeSerializer : public NodeVisitor
{
public:
    NodeSerializer(std::ostream& out) : out{out}, depth{0} {}
    virtual void visit(const Node& node) override;
    virtual void visit(const Composite& node) override;
private:
    void print(const Node& node);
    int depth;
    std::ostream& out;
};

}
