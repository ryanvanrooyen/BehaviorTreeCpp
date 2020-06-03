
#include "nodes.h"
#include "composites.h"
#include "visitors.h"

namespace bt
{

void Serializer::print(const Node& node)
{
    for (int i = 0; i < depth; i++)
        out << "\t";

    if (status && node.status() != Status::Initial)
        out << node.name() << ": " << node.status() << std::endl;
    else
        out << node.name() << std::endl;
}


void Serializer::visit(const Decorator& node)
{
    for (int i = 0; i < depth; i++)
        out << "\t";

    if (Node* child = node.child())
    {
        if (status && node.status() != Status::Initial)
            out << node.name() << " " << child->name() << ": " << node.status() << std::endl;
        else
            out << node.name() << " " << child->name() << ": " << node.status() << std::endl;
    }
    else
    {
        if (status && node.status() != Status::Initial)
            out << node.name() << ": " << node.status() << std::endl;
        else
            out << node.name() << std::endl;
    }
}


void Serializer::visit(const SubTree& tree)
{
    if (expand)
        tree.traverseSubTree(*this);
    else
        visit((Node&) tree);
}

}
