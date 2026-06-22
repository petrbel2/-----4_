#pragma once
#include "Vector.h"
#include <cmath>
#include <complex>
#include <stdexcept>

template <typename T> Vector<T>::Vector() : items() {}

template <typename T> Vector<T>::Vector(const T *items, int count)
    : items(items, count) {}

template <typename T> Vector<T>::Vector(Vector<T> &other)
    : items(other.items) {}

template <typename T> Vector<T>::Vector(const Vector<T> &other)
    : items(other.items) {}

template <typename T> const Sequence<T> &Vector<T>::get_vector() const {
    return items;
}

template <typename T> int Vector<T>::get_dimensity() const {
    return items.get_length();
}

template <typename T> void Vector<T>::append_to_current(const T &item) {
    items.append(item);
}

template <typename T> Vector<T> *Vector<T>::sum(const Vector<T> &vector) {
    if (get_dimensity() != vector.get_dimensity()) {
        throw std::out_of_range("Vectors have diffrent dimensions");
    }

    for (int index = 0; index < get_dimensity(); index++) {
        items.set(index, items.get(index) + vector.items.get(index));
    }

    return this;
}

template <typename T> Vector<T> *Vector<T>::multiply_on_scalar(const T &scalar) {
    for (int index = 0; index < get_dimensity(); index++) {
        items.set(index, items.get(index) * scalar);
    }

    return this;
}

template <typename T> T Vector<T>::scalar_multiply(const Vector<T> &vector_1, const Vector<T> &vector_2) {

    if (vector_1.get_dimensity() != vector_2.get_dimensity()) {
        throw std::out_of_range("Vectors have diffrent dimensions");
    }

    T result{};

    for (int index = 0; index < vector_1.get_dimensity(); index++) {
        result += vector_1.items.get(index) * vector_2.items.get(index);
    }

    return result;
}

template <typename T> T Vector<T>::count_norm(const Vector<T> &vector) {
    
    T result{};

    for (int index = 0; index < vector.get_dimensity(); index++) {
        result += vector.items.get(index) * vector.items.get(index);
    }

    return std::sqrt(result);
}

template <typename T> Vector<T> *Vector<T>::set_component(int index, const T &value) {
    if (index < 0 || index >= get_dimensity()) {
        throw std::out_of_range("Index out of range");
    }

    items.set(index, value);
    return this;
}

template <typename T> Vector<T> *Vector<T>::operator+=(const Vector<T> &other) {
    return sum(other);
}

template <typename T> Vector<T> *Vector<T>::operator*=(const T &scalar) {
    return multiply_on_scalar(scalar);
}

template <typename T> Vector<T> *Vector<T>::operator+(const Vector<T> &other) {
    Vector<T> *result = new Vector<T>(*this);
    result->sum(other);
    return result;
}

template <typename T> Vector<T> *Vector<T>::operator*(const T &scalar) {
    Vector<T> *result = new Vector<T>(*this);
    result->multiply_on_scalar(scalar);
    return result;
}

template <typename T> T Vector<T>::operator*(const Vector<T> &other) {
    return scalar_multiply(*this, other);
}
