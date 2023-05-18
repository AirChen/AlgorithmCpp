//
//  PrimaryQueue.hpp
//  algorithm
//
//  Created by tuRen on 2021/5/12.
//

#ifndef PrimaryQueue_hpp
#define PrimaryQueue_hpp

#include <stdio.h>
#include <stdlib.h>

enum PrimaryQueueType {
    PrimaryQueueType_MAX, // default
    PrimaryQueueType_MIN
};

#define MEMSIZE 1024

template<typename T>
class PrimaryQueue {
    T* _db;
    size_t _N;  // mem size
    size_t _cur; // data size
    PrimaryQueueType _type;
    
    void _resize() {
        if (_cur == _N-1) {
            _N += MEMSIZE;
        }
        
        if (_cur == _N/2) {
            _N = _N/2;
        }
                
        _db = (T*)realloc(_db, _N);
    }
    
    void _exchange(size_t from, size_t to)
    {
        if (from > _cur || to > _cur || from < 0 || to < 0) {
            printf("error: out of range!");
            return;
        }
        
        if (from == to) {
            printf("warming: from and to can't same!");
            return;
        }
        
        int t = _db[from];
        _db[from] = _db[to];
        _db[to] = t;
    }
    
    // return: true order; false not order
    // warming: ldx < rdx
    bool _checkOrder(size_t ldx, size_t rdx) {
        if (ldx > rdx) {
            printf("error: index is not ordered!");
            return false;
        }
        
        return (_type == PrimaryQueueType_MAX) ? (_db[ldx] >= _db[rdx]) : (_db[ldx] <= _db[rdx]);
    }
    
    void _sink(size_t idx) {
        if (idx > _cur || idx < 0) {
            printf("error: out of range!");
            return;
        }
        
        while (idx < _cur) {
            size_t j = idx * 2;
            if (j < _cur && !_checkOrder(j, j+1)) {
                j++;
            }
            
            if (_checkOrder(idx, j) || j > _cur) {
                break;
            }
            
            _exchange(idx, j);
            idx = j;
        }
    }
    
    void _swin(size_t idx) {
        if (idx > _cur || idx < 0) {
            printf("error: out of range!");
            return;
        }
        
        while (idx > 1 && !_checkOrder(idx/2, idx)) {
            _exchange(idx, idx/2);
            idx = idx/2;
        }
    }
    
public:
    void insert(T v) {
        _resize();
        
        _db[++_cur] = v;
        _swin(_cur);
    }
    
    T top() {
        return _db[1];
    }
    
    bool delTop() {
        if (_cur == 1) {
            return false;
        }
        
        _resize();
        _exchange(1, _cur);
        _cur--;
        _sink(1);
        return true;
    }
    
    PrimaryQueue(PrimaryQueueType type = PrimaryQueueType_MAX)
    {
        _type = type;
        _N = MEMSIZE;
        _cur = 0;
        _db = (T*)malloc(_N * sizeof(T));
        _db[0] = -1000;
    };
    
    PrimaryQueue(size_t n, PrimaryQueueType type = PrimaryQueueType_MAX)
    {
        _type = type;
        _N = n;
        _cur = 0;
        _db = (T*)malloc(_N * sizeof(T));
        _db[0] = -1000;
    };
    
    ~PrimaryQueue()
    {
        printf("destruct! \n");
        free(_db);
    };
};

#endif /* PrimaryQueue_hpp */
