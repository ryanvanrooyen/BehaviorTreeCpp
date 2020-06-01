
#include <initializer_list>
#include <vector>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "../bt.h"

using namespace bt;
using std::cout;

int main()
{
    Node* root = new Sequence({
        new MockNode("Act1", {Status::Running, Status::Success, Status::Failure}),
        new MockNode("Act2", {Status::Running, Status::Success})});

    NodeSerializer serializer(cout);
    int numbOfTicks = 2;
    for (int i = 0; i < numbOfTicks; ++i)
    {
        Status status = root->tick();
        root->traverse(serializer);
        sleep(1);
    }
}
