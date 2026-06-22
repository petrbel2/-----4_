#pragma once

#include "LazyCache.hpp"

template <typename T> LazyCache<T>::LazyCache(int capacity)
    : buffer(capacity),
      capacity(capacity),
      elem_number(0),
      first_index(0),
      first_physical_index(0) {
    if (capacity < 0) {
        throw std::invalid_argument("Cache capacity cannot be negative");
    }
}

template <typename T> LazyCache<T>::LazyCache(const Sequence<T> &source, int capacity)
    : buffer(capacity),
      capacity(capacity),
      elem_number(0),
      first_index(0),
      first_physical_index(0) {
    if (capacity < 0) {
        throw std::invalid_argument("Cache capacity cannot be negative");
    }

    for (int index = 0; index < source.get_length(); index++) {
        push(source.get(index));
    }
}

template <typename T> int LazyCache<T>::get_physical_index(int local_index) const {


    if (local_index < 0 || local_index >= elem_number) {
        throw std::out_of_range("Local index out of range");
    }

    return (first_physical_index + local_index) % buffer.get_size();
}

template <typename T> void LazyCache<T>::ensure_unlimited_capacity() { 

    if (capacity != 0) {
        return;
    }
    if (elem_number < buffer.get_size()) {
        return;
    }

    int new_size = buffer.get_size();
    if (new_size == 0) {
        new_size = 1;
    } else {
        new_size *= 2;
    }

    buffer.resize(new_size);
}

template <typename T> bool LazyCache<T>::contains(int index) const {
    return elem_number > 0 && index >= first_index && index < first_index + elem_number;
}

template <typename T> bool LazyCache<T>::forgotten(int index) const {
    return elem_number > 0 && index < first_index;
}

template <typename T> bool LazyCache<T>::empty() const {
    return elem_number == 0;
}

template <typename T> const T &LazyCache<T>::get(int index) const {
    if (!contains(index)) {
        throw std::out_of_range("Element is not materialized in lazy cache");
    }

    return get_by_local_index(index - first_index);
}

template <typename T> const T &LazyCache<T>::get_by_local_index(int local_index) const {
    if (local_index < 0 || local_index >= elem_number) {
        throw std::out_of_range("Local index out of range");
    }

    return buffer.get(get_physical_index(local_index));
}

template <typename T> const T &LazyCache<T>::get_last() const {
    if (elem_number == 0) {
        throw std::out_of_range("Cache is empty");
    }

    return get_by_local_index(elem_number - 1);
}

template <typename T> void LazyCache<T>::push(const T &value) {
    if (capacity == 0) {
        ensure_unlimited_capacity();
        buffer.set(elem_number, value);
        elem_number++;
        return;
    }

    if (elem_number < capacity) {
        int physical_index = (first_physical_index + elem_number) % capacity;
        buffer.set(physical_index, value);
        elem_number++;
        return;
    }

    buffer.set(first_physical_index, value);
    first_physical_index = (first_physical_index + 1) % capacity;
    first_index++;
}

template <typename T> void LazyCache<T>::clear() {
    elem_number = 0;
    first_index = 0;
    first_physical_index = 0;
}

template <typename T> void LazyCache<T>::set_capacity(int new_capacity) {
    if (new_capacity < 0) {
        throw std::invalid_argument("Cache capacity cannot be negative");
    }

    if (new_capacity == capacity) {
        return;
    }

    int kept_count = elem_number;
    int skipped_count = 0;
    if (new_capacity > 0 && kept_count > new_capacity) {
        skipped_count = kept_count - new_capacity;
        kept_count = new_capacity;
    }

    int new_buffer_size = new_capacity;
    if (new_capacity == 0) {
        new_buffer_size = kept_count;
    }

    DynamicArray<T> replacement(new_buffer_size);
    for (int index = 0; index < kept_count; index++) {
        replacement.set(index, get_by_local_index(skipped_count + index));
    }

    buffer = replacement;
    capacity = new_capacity;
    elem_number = kept_count;
    first_index += skipped_count;
    first_physical_index = 0;
}

template <typename T> int LazyCache<T>::get_capacity() const {
    return capacity;
}

template <typename T> int LazyCache<T>::get_count() const {
    return elem_number;
}

template <typename T> int LazyCache<T>::get_first_index() const {
    return first_index;
}

template <typename T> int LazyCache<T>::get_index_after_last() const {
    return first_index + elem_number;
}
