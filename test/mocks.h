
#include "../source/bt.h"
#include <vector>

struct MockNodeInfo
{
    bool destroyed = false;
};


class MockNode: public bt::NamedNode
{
public:
    MockNode(const char* name, MockNodeInfo& info, const std::vector<bt::Status>& results)
        : NamedNode(name), info(&info), results(results) {}

protected:
    virtual bt::Status update() noexcept override
    {
        if (++index >= results.size())
            index = 0;
        return results[index];
    }
    virtual ~MockNode() override { std::cout << "Deleting custom action." << std::endl; }
private:
    MockNodeInfo* info;
    std::vector<bt::Status> results;
    int index = -1;
};
