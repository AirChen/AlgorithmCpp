#include "COWTest.hpp"
#include "gtest/gtest.h"

#include <vector>
#include <string>
#include <memory>

using std::cout;
using std::endl;
using std::vector;
using std::string;

class FakeVector {
    public:
    FakeVector() {
        data = std::make_shared<vector<string>>();
    }
    FakeVector(const FakeVector& fk) {
        data = fk.data;
    }

    FakeVector& operator=(const FakeVector& fk) {
        if (data.use_count() > 1) {
            data = std::make_shared<vector<string>>();
            for (auto item: *fk.data) {
                data->emplace_back(item);
            }
            cout << "re make data" << endl;
        } else {
            data = fk.data;
        }
        return *this;
    }

    bool appendStr(const string& item) {
        if (data.use_count() > 1) {
            std::shared_ptr<vector<string>> tmp = data;
            data = std::make_shared<vector<string>>();
            for (auto item: *tmp) {
                data->emplace_back(item);
            }
            cout << "re make data" << endl;
        }
        data->emplace_back(item);
        return true;
    }
    std::shared_ptr<vector<string>> data;
};

class COWTest : public ::testing::Test {
};

TEST_F(COWTest, test_basic) {
    FakeVector fv;
    for (auto i: {"one", "two", "tree", "four"}) {
        fv.data->emplace_back(i);
    }

    FakeVector fv1;
    fv1 = fv;
    fv.appendStr("five");
    fv1 = fv;
    fv1.appendStr("five");

};
