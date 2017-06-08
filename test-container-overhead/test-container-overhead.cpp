#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <time.h>

using namespace std;
using namespace boost;
using namespace boost::multi_index;

#ifndef SIZE
#define SIZE 4
#endif

class A
{
public:
    char v[SIZE];
};

bool operator < (const A& lhs, const A& rhs)
{
    return lhs.v[0] < rhs.v[0];
}

typedef vector<A> vector_A;
typedef list<A> list_A;
typedef multiset<A> set_A;
typedef multi_index_container<A,indexed_by<sequenced<>>> mic_list_A;
typedef multi_index_container<A,indexed_by<sequenced<>,sequenced<>>> mic_dlist_A;

template <class T>
struct aux {};

template <> struct aux<vector_A> {
    vector_A::iterator ins(vector_A& v, const A& a) { v.push_back(a); return v.end() - 1; }
};

template <> struct aux<list_A> {
    list_A::iterator ins(list_A& v, const A& a) { v.push_back(a); list_A::iterator res = v.end(); return --res; }
};

template <> struct aux<set_A> {
    set_A::iterator ins(set_A& v, const A& a) { return v.insert(a); }
};

template <> struct aux<mic_list_A> {
    mic_list_A::iterator ins(mic_list_A& v, const A& a) { v.push_back(a); mic_list_A::iterator = v.end(); return --res; }
};

template <> struct aux<mic_dlist_A> {
    mic_dlist_A::iterator void ins(mic_dlist_A& v, const A& a) { v.push_back(a); mic_dlist_A::iterator res = v.end(); return --res; }
};


size_t get_vmem()
{
    size_t res;
    ifstream f("/proc/self/stat");
    string tmp;
    for (int i = 0; i < 22; ++i)
    {
        f >> tmp;
        //cerr << i << ": " << tmp << "\n";
    }
    f >> res;
    return res;
}

template <class T>
void test_container(const char* name)
{
    size_t n = 16*1024*1024;
    size_t start_time;
    size_t end_time;
    size_t vmem;
    size_t i;
    string tmp;

    cout << name << ":\n";
    start_time = time(NULL);
    T t;
    T::iterator it;
    aux<T> aux_obj;
    cout << "insert " << n << " elements... ";
    for (i = 0; i < n; ++i) {
        A a;
        a.v[0] = (char)(i % 256);
        it = aux_obj.ins(t, a);
    }
    end_time = time(NULL);
    vmem = get_vmem();
    cout << "time: " << end_time - start_time << " seconds; vmem: " << vmem << " bytes; overhead: " << ((double)vmem / n) - (double)SIZE << " bytes\n";
    t.clear();
}


int main()
{
    test_container<vector_A>("vector");
    test_container<list_A>("list");
    test_container<set_A>("set");
    test_container<mic_list_A>("mic_list");
    test_container<mic_dlist_A>("mic_dlist");

    return EXIT_SUCCESS;
}
