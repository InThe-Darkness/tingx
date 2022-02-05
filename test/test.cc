#include "core/tingx_descriptor.hpp"

#include <iostream>
#include <vector>

using namespace tingx;
using namespace std;

int main() {

    Descriptor des(100);
    des.SetIsClose(true);

    Descriptor des2;
    des2 = des;

    Ptr<Descriptor> p(new Descriptor(200));
    p->SetIsClose(true);
    cout << "fd is " << p->Getfd() << endl;

    vector<Descriptor> v;
    v.push_back(Descriptor());



    return 0;
}