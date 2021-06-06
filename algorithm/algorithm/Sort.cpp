//
//  Sort.cpp
//  algorithm
//
//  Created by tuRen on 2021/5/10.
//

#include "Sort.hpp"

using std::swap;

bool Sort::isOrdered(vector<int> &as) {
    if (as.size() > 1) {
        bool isReverse = (as[0] > as[1]);
        for (int i = 1; i < as.size(); i++) {
            if (isReverse && as[i-1] <= as[i]) {
                return false;
            }
            
            if (!isReverse && as[i-1] > as[i]) {
                return false;
            }
        }
    }
    
    return true;
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
        swap(as[i], as[j]);
    }
    
    swap(as[lo], as[j]);
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
    while (i <= gt) {
        if (as[i] < base) {
            swap(as[lt++], as[i++]);
        } else if (as[i] > base) {
            swap(as[gt--], as[i]);
        } else {
            i++;
        }
    }
    
    _sort3WayImp(as, lo, lt-1);
    _sort3WayImp(as, gt+1, hi);
}

void InsertionSort::sort(vector<int> &as) {
    int n = static_cast<int>(as.size());
    if (n < 1) {
        return;
    }
    
    for (int i = 0 ; i < n; i++) {
        for (int j = i; j > 0 && (as[j] < as[j-1]); j--) {
            swap(as[j], as[j-1]);
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
        
        swap(as[minIdx], as[i]);
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
                swap(as[j], as[j-h]);
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
        swap(as[1], as[n--]);
        _sink(as, 1, n);
    }
    
    int idx = 0;
    while (idx < (as.size()-1) && as[idx] > as[idx+1]) {
        swap(as[idx], as[idx+1]);
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
        swap(as[cur], as[j]);        
        cur = j;
    }
}
