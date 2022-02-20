#ifndef __PRIORITY
#define __PRIORITY

template <typename T>
class MINPQ{

    T *pq;
    int curr;

    void push_back(T val) {
        pq[curr] = val;
        curr++;
    }
    void pop_back() {
        curr--;
    }

public:

    MINPQ(int size) {
        pq = new T[size];
        curr = 0;
    }
    ~MINPQ() {

    }

    int size() {
        return curr;
    }

    bool empty() {
        return (curr==0);
    }

    T top();
    void insert(T val);
    void pop();
};

#endif
