#pragma once

#include "ArraySequence.h"

#include <stdexcept>

template <typename T> class OnlineStatistics {
    private:
        int count;
        T sum;
        T min_value;
        T max_value;
        bool has_items;
        MutableArraySequence<T> sorted_items;

        int find_insert_index(const T &value) const;
    
    public:
        OnlineStatistics();

        void add(const T &value);
        int get_count() const;
        T get_sum() const;
        double get_mean() const;
        T get_min() const;
        T get_max() const;
        double get_median() const;
};

#include "OnlineStatistics.tpp"
