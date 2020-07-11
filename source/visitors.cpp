
#include "visitors.hpp"

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
