
#pragma once

#include <vector>
#include <initializer_list>
#include <iostream>

namespace bt
{

enum class Status
{
    Success,
    Running,
    Failure,
    Invalid,
};

std::ostream& operator<<(std::ostream& os, const Status& status);


class Node
{
public:
    Node(const char* name) : name{name} {}

    virtual void initialize() {}
    virtual Status update() = 0;
    virtual void terminate(Status) {}

    Status tick()
    {
        if (status != Status::Running) initialize();
        status = update();
        if (status != Status::Running) terminate(status);
        return status;
    }

    inline Status getStatus() const { return status; }
    inline const char* getName() const { return name; }

    virtual void traverse(class NodeVisitor& visitor);

protected:
    const char* name;

private:
    Status status = Status::Invalid;
};


class MockNode : public Node
{
public:
    MockNode(const char* name, std::initializer_list<Status> status);
    virtual Status update() override;
private:
    std::vector<Status> mockStatus;
    std::vector<Status>::iterator currentStatus;
};

}
