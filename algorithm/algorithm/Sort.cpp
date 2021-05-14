//
//  Sort.cpp
//  algorithm
//
//  Created by tuRen on 2021/5/10.
//

#include "Sort.hpp"

void Sort::exchange(vector<int> &as, int from, int to) {
    int t = as[from];
    as[from] = as[to];
    as[to] = t;
}

void QuickSort::sort(vector<int> &as) {
    _sortImp(as, 0, static_cast<int>(as.size()-1));
}

void QuickSort::sort(vector<int> &as, QuickSortType type) {
    int n = static_cast<int>(as.size()-1);
    switch (type) {
        case QuickSortType_normal:
            _sortImp(as, 0, n);
            break;
        case QuickSortType_3way:
            _sort3WayImp(as, 0, n);
            break;
            
        default:
            break;
    }
}

void QuickSort::_sortImp(vector<int>& as, int lo, int hi) {
    if (lo >= hi) {
        return;
    }
    
    int i = lo, j = hi + 1;
    int base = as[lo];
    
    while (true) {
        while (as[++i] < base) {
            if (i == hi) {
                break;
            }
        }
        
        while (as[--j] > base) {
            if (j == lo) {
                break;
            }
        }
        
        if (i >= j) {
            break;
        }
        exchange(as, i, j);
    }
    
    exchange(as, lo, j);
    
    _sortImp(as, lo, j-1);
    _sortImp(as, j+1, hi);
}

void QuickSort::_sort3WayImp(vector<int> &as, int lo, int hi) {
    if (hi <= lo) {
        return;
    }
    
    int lt = lo;
    int gt = hi;
    int i = lo+1;
    int base = as[lo];
    while (i < gt) {
        if (as[i] < base) {
            exchange(as, lt++, i++);
        } else if (as[i] > base) {
            exchange(as, gt--, i);
        } else {
            i++;
        }
    }
    
    _sortImp(as, lo, lt-1);
    _sortImp(as, gt+1, hi);
}

void InsertionSort::sort(vector<int> &as) {
    int n = static_cast<int>(as.size());
    if (n < 1) {
        return;
    }
    
    for (int i = 0 ; i < n; i++) {
        for (int j = i; j > 0 && (as[j] < as[j-1]); j--) {
            exchange(as, j, j-1);
        }
    }
}

void SelectionSort::sort(vector<int> &as) {
    int n = static_cast<int>(as.size());
    if (n < 1) {
        return;
    }
    
    for (int i = 0; i < n; i++) {
        int minIdx = i;
        for (int j = i; j < n; j++) {
            if (as[j] < as[minIdx]) {
                minIdx = j;
            }
        }
        
        exchange(as, minIdx, i);
    }
}

void ShellSort::sort(vector<int> &as) {
    int n = static_cast<int>(as.size());
    if (n < 1) {
        return;
    }
    
    int h = 1;
    while (h < n/3) {
        h = h * 3 + 1;
    }
    
    while (h >= 1) {
        for (int i = h; i < n; i++) {
            for (int j = i; j >= h && as[j] < as[j-h]; j-=h) {
                exchange(as, j, j-h);
            }
        }
        h = h/3;
    }
}

void MergeSort::sort(vector<int> &as) {
    int n = static_cast<int>(as.size());
    if (n < 1) {
        return;
    }
    
    _sortImp(as, 0, n-1);
}

void MergeSort::_sortImp(vector<int> &as, int lo, int hi) {
    if (lo >= hi) {
        return;
    }
    
    int mid = (hi + lo)/2;
    _sortImp(as, lo, mid);
    _sortImp(as, mid+1, hi);
    
    _merge(as, lo, mid, hi);
}

void MergeSort::_merge(vector<int> &as, int lo, int mid, int hi) {
    vector<int> tmp;
    for (int i = lo; i <= hi; i++) {
        tmp.push_back(as[i]);
    }
    
    int i = lo;
    int j = mid + 1;
    
    for (int k = lo; k <= hi; k++) {
        if (i > mid) {
            as[k] = tmp[(j++)-lo];
        } else if (j > hi) {
            as[k] = tmp[(i++)-lo];
        } else if (as[j] < as[i]) {
            as[k] = tmp[(j++)-lo];
        } else {
            as[k] = tmp[(i++)-lo];
        }
    }
}

void HeapSort::sort(vector<int> &as) {
    int n = static_cast<int>(as.size());
    if (n < 1) {
        return;
    }
    
    n++;
    int k = n/2;
    while (k >= 1) {
        _sink(as, k--, n);
    }
    
    while (n > 1) {
        exchange(as, 1, n--);
        _sink(as, 1, n);
    }
    
    int idx = 0;
    while (idx < (as.size()-1) && as[idx] > as[idx+1]) {
        exchange(as, idx, idx+1);
        idx++;
    }
}

void HeapSort::_sink(vector<int> &as, int cur, int n) {
    while (cur < n) {
        int j = cur * 2;
        if (j >= n) {
            break;
        }
        if (as[j] < as[j+1]) {
            j++;
        }
        
        if (as[cur] > as[j]) {
            break;
        }
        exchange(as, cur, j);
        cur = j;
    }
}
