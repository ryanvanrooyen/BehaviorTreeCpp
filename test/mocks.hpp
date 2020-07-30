
#include "../include/all.hpp"
#include <vector>
#include <initializer_list>

struct MockNodeInfo
{
    int createCount = 0;
    int destroyCount = 0;
    int updateCount = 0;
};


class MockNode: public bt::NamedNode
{
public:
    MockNode(MockNodeInfo& info, const std::vector<bt::Status>& results, const char* name = "MockNode")
        : info(&info), results(results), NamedNode(name)
    {
        ++info.createCount; 
    }

    MockNode(MockNodeInfo& info, const bt::Status result, const char* name = "MockNode")
        : MockNode(info, std::vector<bt::Status>{result}, name) { }

protected:
    virtual bt::Status update() noexcept override
    {
        ++info->updateCount;
        if (++index >= results.size())
            index = 0;
        return results[index];
    }
    virtual ~MockNode() override
    {
        ++info->destroyCount;
    }
private:
    MockNodeInfo* info;
    std::vector<bt::Status> results;
    int index = -1;
};
