#pragma once

#include "Sequence.h"
#include "ArraySequence.h"
#include "IEnumerator.h"

#include <stdexcept>

template <typename T> class Vector {
    protected:
        MutableArraySequence<T> items;

        void append_to_current(const T &item);
        int get_dimensity() const;

    public:
        Vector();
        Vector(const T *items, int count);
        Vector(Vector<T> &other);
        Vector(const Vector<T> &other);
        virtual ~Vector() = default;

        const Sequence<T> &get_vector() const;

        Vector<T> *sum(const Vector<T> &vector);
        Vector<T> *multiply_on_scalar(const T &scalar);
        T scalar_multiply(const Vector<T> &vector_1, const Vector<T> &vector_2);
        T count_norm(const Vector<T> &vector);
        Vector<T> *set_component(int index, const T &value);

        Vector<T> *operator+=(const Vector<T> &other);
        Vector<T> *operator*=(const T &scalar);

        Vector<T> *operator+(const Vector<T> &other);
        Vector<T> *operator*(const T &scalar);
        T operator*(const Vector<T> &other);

        IEnumerator<T> *get_enumerator() const {
            return items.get_enumerator();
        }
};

#include "Vector.tpp"
