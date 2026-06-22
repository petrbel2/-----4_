#pragma once

#include <stdexcept>

template <typename T> Sequence<T> *Sequence<T>::get_subsequence(int start_index, int end_index) const {
    if (start_index < 0 || end_index < 0 ||
        start_index >= get_length() || end_index >= get_length() ||
        start_index > end_index) {
        throw std::out_of_range("Index out of range");
    }

    Sequence<T> *result = Empty();
    result->reserve(end_index - start_index + 1);

    
    IEnumerator<T> *enumerator = get_enumerator();
    int index = 0;
    while (enumerator->move_next() && index <= end_index) {
        if (index >= start_index) {
            result->append_to_current(enumerator->get_curr()); 
        }
        index++;
    }
    delete enumerator;

    return result;
}

template <typename T> Sequence<T> *Sequence<T>::concat(const Sequence<T> &other) {


    Sequence<T> *result = Instance();
    result->reserve(result->get_length() + other.get_length());

    IEnumerator<T> *enumerator = other.get_enumerator();
    while (enumerator->move_next()) {
        result->append_to_current(enumerator->get_curr());
    }
    delete enumerator;

    return result;
}

template <typename T> Sequence<T> *Sequence<T>::map(T (*function)(const T &item)) {
    Sequence<T> *result = Empty();
    result->reserve(get_length());

    IEnumerator<T> *enumerator = get_enumerator();
    while (enumerator->move_next()) {
        result->append_to_current(function(enumerator->get_curr()));
    }
    delete enumerator;

    return result;
}

template <typename T> Sequence<T> *Sequence<T>::where(bool (*predicate)(const T &item)) {
    Sequence<T> *result = Empty();
    result->reserve(get_length());

    IEnumerator<T> *enumerator = get_enumerator();
    while (enumerator->move_next()) {
        const T &value = enumerator->get_curr();
        if (predicate(value)) {
            result->append_to_current(value);
        }
    }
    delete enumerator;

    return result;
}

template <typename T> T Sequence<T>::reduce(T (*function)(const T &first, const T &second), const T &initial) {
    T accumulated = initial;

    IEnumerator<T> *enumerator = get_enumerator();
    while (enumerator->move_next()) {
        accumulated = function(accumulated, enumerator->get_curr());
    }
    delete enumerator;

    return accumulated;
}
