#pragma once

#include <stdexcept>

template <typename T> DynamicArray<T>::DynamicArray() : data(nullptr), size(0) {}

template <typename T> DynamicArray<T>::DynamicArray(int initial_size) : data(nullptr), size(initial_size) {
    if (initial_size < 0) {
        throw std::invalid_argument("Size cannot be negative");
    }
    if (initial_size > 0) {
        data = new T[initial_size];
    }
}

template <typename T>
DynamicArray<T>::DynamicArray(const T *items, int count) : data(nullptr), size(count) {
    if (count < 0) {
        throw std::invalid_argument("Count cannot be negative");
    }
    if (count > 0 && items == nullptr) {
        throw std::invalid_argument("Items cannot be null");
    }

    if (size > 0) {
        data = new T[size];
        for (int index = 0; index < size; index++) {
            data[index] = items[index];
        }
    }
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other) : data(nullptr), size(other.size) {
    if (size > 0) {
        data = new T[size];
        for (int index = 0; index < size; index++) {
            data[index] = other.data[index];
        }
    }
}

template <typename T> DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this == &other) {
        return *this;
    }

    T *replacement = nullptr;
    if (other.size > 0) {
        replacement = new T[other.size];
        for (int index = 0; index < other.size; index++) {
            replacement[index] = other.data[index];
        }
    }

    delete[] data;
    data = replacement;
    size = other.size;
    return *this;
}

template <typename T> DynamicArray<T>::~DynamicArray() {
    delete[] data;
}

template <typename T> const T& DynamicArray<T>::get(int index) const {
    if (index < 0 || index >= size) {
        throw std::out_of_range("Index out of range");
    }
    return data[index];
}

template <typename T> const T& DynamicArray<T>::operator[](int index) const {
    return get(index);
}

template <typename T> void DynamicArray<T>::set(int index, const T &value) {
    if (index < 0 || index >= size) {
        throw std::out_of_range("Index out of range");
    }

    data[index] = value;
}

template <typename T> void DynamicArray<T>::resize(int new_size) {
    if (new_size < 0) {
        throw std::invalid_argument("Size cannot be negative");
    }

    T *replacement = nullptr;
    if (new_size > 0) {
        replacement = new T[new_size];
        int will_copy; // Переделал время
        if (new_size < size) {
            will_copy = new_size;
        } else {
            will_copy = size;
        }
        for (int index = 0; index < will_copy; index++) {
            replacement[index] = data[index];
        }
    }

    delete[] data;
    data = replacement;
    size = new_size;
}

template <typename T> int DynamicArray<T>::get_size() const {
    return size;
}
