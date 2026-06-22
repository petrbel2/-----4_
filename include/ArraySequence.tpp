#pragma once

#include <stdexcept>

template <typename T> const T& ArraySequence<T>::get_first() const {
    if (count == 0) {
        throw std::out_of_range("Sequence is empty");
    }
    return items.get(0);
}

template <typename T> const T& ArraySequence<T>::get_last() const {
    if (count == 0) {
        throw std::out_of_range("Sequence is empty");
    }
    return items.get(count - 1);
}

template <typename T> const T& ArraySequence<T>::get(int index) const {
    if (index < 0 || index >= count) {
        throw std::out_of_range("Index out of range");
    }
    return items.get(index);
}

template <typename T> Option<T> ArraySequence<T>::try_get_first() const {
    if (count == 0) {
        return Option<T>();
    }
    return Option<T>(items.get(0));
}

template <typename T> Option<T> ArraySequence<T>::try_get_last() const {
    if (count == 0) {
        return Option<T>();
    }
    return Option<T>(items.get(count - 1));
}

template <typename T> Option<T> ArraySequence<T>::try_get(int index) const {
    if (index < 0 || index >= count) {
        return Option<T>();
    }
    return Option<T>(items.get(index));
}

template <typename T> int ArraySequence<T>::get_length() const {
    return count;
}

template <typename T> Sequence<T> *ArraySequence<T>::append(const T& item) {
    ArraySequence<T> *result = Instance();
    result->append_to_current(item);
    return result;
}

template <typename T> Sequence<T> *ArraySequence<T>::prepend(const T& item) {
    ArraySequence<T> *result = Instance();
    const int old_length = result->count;

    result->reallocate(old_length + 1);
    for (int index = old_length; index > 0; index--) {
        result->items.set(index, result->items.get(index - 1));
    }
    result->items.set(0, item);
    result->count = old_length + 1;
    return result;
}

template <typename T> Sequence<T> *ArraySequence<T>::insert(const T& item, int index) {
    ArraySequence<T> *result = Instance();
    const int old_length = result->count;
    if (index < 0 || index > old_length) {
        throw std::out_of_range("Index out of range");
    }

    result->reallocate(old_length + 1);
    for (int current_index = old_length; current_index > index; current_index--) {
        result->items.set(current_index, result->items.get(current_index - 1));
    }
    result->items.set(index, item);
    result->count = old_length + 1;
    return result;
}

template <typename T> Sequence<T> *ArraySequence<T>::remove(int index) {
    ArraySequence<T> *result = Instance();
    const int old_length = result->count;
    if (index < 0 || index >= old_length) {
        throw std::out_of_range("Index out of range");
    }

    for (int current_index = index; current_index < old_length - 1; current_index++) {
        result->items.set(current_index, result->items.get(current_index + 1));
    }
    result->count = old_length - 1;
    return result;
}

template <typename T> void ArraySequence<T>::append_to_current(const T &item) {
    const int old_length = count;
    reallocate(old_length + 1);
    items.set(old_length, item);
    count = old_length + 1;
}

template <typename T> void ArraySequence<T>::reserve(int capacity) {
    if (capacity < 0) {
        throw std::invalid_argument("Capacity cannot be negative");
    }
    reallocate(capacity);
}

template <typename T> void ArraySequence<T>::reallocate(int required) {
    if (required <= items.get_size()) {
        return;
    }

    int new_capacity = items.get_size();
    if (new_capacity == 0) {
        new_capacity = 1;
    }

    while (new_capacity < required) {
        new_capacity *= 2;
    }

    items.resize(new_capacity);
}

template <typename T> void MutableArraySequence<T>::set(int index, const T &value) {
    if (index < 0 || index >= this->count) {
        throw std::out_of_range("Index out of range");
    }

    this->items.set(index, value);
}
