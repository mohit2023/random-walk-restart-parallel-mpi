#include "priorityQueue.h"

template <typename T>
T MINPQ<T>::top(){
    if(empty()){
        return {0,0};
    }
    else{
        return pq[0];
    }
}

template <typename T>
void MINPQ<T>::insert(T val){
    this->push_back(val);
    int ci=this->size()-1;
    int pi=(ci-1)/2;
    while(ci>0){
        if(pq[pi]<=pq[ci]){
            break;
        }
        else{
            T temp=pq[ci];
            pq[ci]=pq[pi];
            pq[pi]=temp;
            ci=pi;
            pi=(ci-1)/2;
        }
    }
}

template <typename T>
void MINPQ<T>::pop(){
    if(empty()){
        return;
    }
    pq[0]=pq[this->size()-1];
    this->pop_back();

    if(empty()){
        return;
    }

    int pi=0;
    int lci=2*pi+1;
    int rci=lci+1;
    int mi=pi;
    int n=this->size();
    while(lci<n){
        if(pq[lci]<pq[mi]){
            mi=lci;
        }

        if(rci<n && pq[rci]<pq[mi]){
            mi=rci;
        }
        if(mi==pi){
            break;
        }
        T temp=pq[pi];
        pq[pi]=pq[mi];
        pq[mi]=temp;

        pi=mi;
        lci=2*pi+1;
        rci=lci+1;
    }
}

// class MINPQ{

//     T *pq;
//     int curr;

//     void push_back(T val) {
//         pq[curr] = val;
//         curr++;
//     }

//     void pop_back() {
//         curr--;
//     }

// public:

//     MINPQ(int size){
//         pq = new int[size];
//         curr = 0;
//     }
//     ~MINPQ(){

//     }

//     int size(){
//         return curr;
//     }
//     bool empty(){
//         return (curr==0);
//     }
//     int top(){
//         if(empty()){
//             return 0;
//         }
//         else{
//             return pq[0];
//         }
//     }
//     void insert(int val){
//         this->push_back(val);
//         int ci=this->size()-1;
//         int pi=(ci-1)/2;
//         while(ci>0){
//             if(pq[pi]<=pq[ci]){
//                 break;
//             }
//             else{
//                 int temp=pq[ci];
//                 pq[ci]=pq[pi];
//                 pq[pi]=temp;
//                 ci=pi;
//                 pi=(ci-1)/2;
//             }
//         }
//     }

//     void pop(){
//         if(empty()){
//             return;
//         }
//         pq[0]=pq[this->size()-1];
//         this->pop_back();

//         if(empty()){
//             return;
//         }

//         int pi=0;
//         int lci=2*pi+1;
//         int rci=lci+1;
//         int mi=pi;
//         int n=this->size();
//         while(lci<n){
//             if(pq[lci]<pq[mi]){
//                 mi=lci;
//             }

//             if(rci<n && pq[rci]<pq[mi]){
//                 mi=rci;
//             }
//             if(mi==pi){
//                 break;
//             }
//             int temp=pq[pi];
//             pq[pi]=pq[mi];
//             pq[mi]=temp;

//             pi=mi;
//             lci=2*pi+1;
//             rci=lci+1;
//         }
//     }
// };
