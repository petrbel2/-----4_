#pragma once

#include "DynamicArray.h"
#include "Sequence.h"

#include <stdexcept>

template <typename T> class LazyCache {
    private:
        DynamicArray<T> buffer;
        int capacity; // Сколько максимум хранить
        int count; // Сколько по факту
        int first_index;
        int first_physical_index; // 6 1 2 3 5

        int get_physical_index(int local_index) const;
        void ensure_unlimited_capacity();
    
    public:
        LazyCache(int capacity = 0);
        LazyCache(const Sequence<T> &source, int capacity = 0);

        bool contains(int index) const;
        bool forgotten(int index) const;
        bool empty() const;

        const T &get(int index) const;
        const T &get_by_local_index(int local_index) const;
        const T &get_last() const;

        void push(const T &value);
        void clear();
        void set_capacity(int new_capacity);

        int get_capacity() const;
        int get_count() const;
        int get_first_index() const;
        int get_index_after_last() const;
};

#include "LazyCache.tpp"
