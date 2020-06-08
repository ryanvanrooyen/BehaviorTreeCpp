
#include "nodes.h"
#include "composites.h"
#include "visitors.h"

namespace bt
{

void TextSerializer::print(const char* name, Status status, const char* prefix)
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


void TextSerializer::visit(const Decorator& node)
{
    if (Node* child = node.child())
        print(child->name(), node.status(), node.name());
    else
        print(node.name(), node.status());
}


void TextSerializer::visit(const SubTree& tree)
{
    if (expand)
        tree.traverseSubTree(*this);
    else
        visit((Node&) tree);
}

}
