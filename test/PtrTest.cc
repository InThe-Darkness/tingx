#include "core/tingx_refcount.hpp"

#include <iostream>

using namespace std;
using namespace tingx;

class Test : public RefCounted {
public:
    Test() {
        cout << "Created" << endl;
    }

    ~Test() {
        cout << "Delete" << endl;
    }

private:
    int count = 0;    
};


int main() {

    Ptr<Test> container;

    {
        Ptr<Test> pTest{new Test()};
        container.Attach(pTest.Detach());
    }

    int a = 0;
    return 0;
}