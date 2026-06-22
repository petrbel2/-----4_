#pragma once

template <typename T>
class DynamicArray {
private:
    T* data;
    int size;
public:

    DynamicArray();
    DynamicArray(const T *items, int count);
    DynamicArray(int initial_size);
    DynamicArray(const DynamicArray<T>& other);
    DynamicArray<T>& operator=(const DynamicArray<T>& other);

    ~DynamicArray();

    const T &get(int index) const;
    const T &operator[](int index) const;

    int get_size() const;

    void set(int index, const T &value);
    void resize(int new_size);

};

#include "DynamicArray.tpp"
