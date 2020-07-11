
#ifndef BEHAVIOR_TREE_STATUS_H
#define BEHAVIOR_TREE_STATUS_H

#include <iostream>

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
