#pragma once

#include <stdexcept>

template <typename T> ListSequence<T>::ListSequence() : items() {}

template <typename T> ListSequence<T>::ListSequence(const T *items, int count) : items(items, count) {}

template <typename T> ListSequence<T>::ListSequence(const ListSequence<T> &other) : items(other.items) {}

template <typename T> const T& ListSequence<T>::get_first() const {
    if (items.get_length() == 0) {
        throw std::out_of_range("Sequence is empty");
    }
    return items.get(0);
}

template <typename T> const T& ListSequence<T>::get_last() const {
    if (items.get_length() == 0) {
        throw std::out_of_range("Sequence is empty");
    }
    return items.get(items.get_length() - 1);
}

template <typename T> const T& ListSequence<T>::get(int index) const {
    if (index < 0 || index >= items.get_length()) {
        throw std::out_of_range("Index out of range");
    }

    return items.get(index);
}

template <typename T> Option<T> ListSequence<T>::try_get_first() const {
    if (items.get_length() == 0) {
        return Option<T>();
    }
    return Option<T>(items.get(0));
}

template <typename T> Option<T> ListSequence<T>::try_get_last() const {
    if (items.get_length() == 0) {
        return Option<T>();
    }
    return Option<T>(items.get(items.get_length() - 1));
}

template <typename T> Option<T> ListSequence<T>::try_get(int index) const {
    if (index < 0 || index >= items.get_length()) {
        return Option<T>();
    }

    return Option<T>(items.get(index));
}

template <typename T> int ListSequence<T>::get_length() const {
    return items.get_length();
}

template <typename T> Sequence<T> *ListSequence<T>::append(const T& item) {
    ListSequence<T> *result = Instance();
    result->append_to_current(item);
    return result;
}

template <typename T> Sequence<T> *ListSequence<T>::prepend(const T& item) {
    ListSequence<T> *result = Instance();
    result->items.prepend(item);
    return result;
}

template <typename T> Sequence<T> *ListSequence<T>::insert(const T& item, int index) {
    ListSequence<T> *result = Instance();
    result->items.insert(item, index);
    return result;
}

template <typename T> Sequence<T> *ListSequence<T>::remove(int index) {
    ListSequence<T> *result = Instance();
    result->items.remove(index);
    return result;
}

template <typename T> void ListSequence<T>::append_to_current(const T &item) {
    items.append(item);
}
