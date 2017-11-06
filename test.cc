#include <iostream>
#include "clist.h"

using namespace std;

int main()
{
    clist* mylist = new clist(0);

    mylist->insert(5);
    mylist->insert(4);
    mylist->insert(6);

    mylist->remove(0);
    mylist->remove(6);

    bool exists = mylist->lookup(17);
    cout << exists << endl;
    exists = mylist->lookup(5);
    cout << exists << endl;

    for (int i = 0; i < 2; ++i)
    {
        int myvalue = mylist->getElement(i);
        cout << myvalue << " ";
    }

    cout << endl;
    
    return 0;
}
