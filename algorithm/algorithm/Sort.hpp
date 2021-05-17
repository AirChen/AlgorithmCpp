//
//  Sort.hpp
//  algorithm
//
//  Created by tuRen on 2021/5/10.
//

#ifndef Sort_hpp
#define Sort_hpp

#include <stdio.h>
#include <vector>

using std::vector;

class Sort {
public:
    virtual void sort(vector<int>& as) = 0;
    
    void exchange(vector<int>& as, int from, int to);
    bool isOrdered(vector<int>& as);
    
    ~Sort() {};
};

enum QuickSortType {
    QuickSortType_normal,
    QuickSortType_3way
};

class QuickSort: public Sort {
public:
    virtual void sort(vector<int>& as);
    void sort(vector<int>& as, QuickSortType type);
    
private:
    void _sortImp(vector<int>& as, int lo, int hi);
    void _sort3WayImp(vector<int>& as, int lo, int hi);
};

class InsertionSort: public Sort {
public:
    virtual void sort(vector<int>& as);
    
    ~InsertionSort() {};
};

class SelectionSort: public Sort {
public:
    virtual void sort(vector<int>& as);
};

class ShellSort: public Sort {
public:
    virtual void sort(vector<int>& as);
};

class MergeSort: public Sort {
public:
    virtual void sort(vector<int>& as);
private:
    void _sortImp(vector<int>& as, int lo, int hi);
    void _merge(vector<int>& as, int lo, int mid, int hi);
};

class HeapSort: public Sort {
public:
    virtual void sort(vector<int>& as);
private:
    void _sink(vector<int>& as, int cur, int n);
};

#endif /* Sort_hpp */
