#pragma once

#include "ArraySequence.h"

#include <cmath>
#include <complex>
#include <stdexcept>

template <typename T> class Matrix {
    protected:
        MutableArraySequence<T> items;
        int rows;
        int columns;

        virtual Matrix<T> *Instance() = 0;
        virtual Matrix<T> *Clone() const = 0;

        int index_transform(int row, int column) const;
        void append_to_current(const T &value);
        void set_to_current(int row, int column, const T &value);

    public:
        virtual Matrix<T> *Empty() const = 0;

        Matrix();
        Matrix(const T *items, int size);
        Matrix(const T *items, int row_count, int column_count);
        Matrix(const Matrix<T> &other);
        virtual ~Matrix() = default;

        int get_rows() const;
        int get_columns() const;
        const T &get_el(int row, int column) const;
        Matrix<T> *set_el(int row, int column, const T &value);

        Matrix<T> *add_to_self(const Matrix<T> &other);
        Matrix<T> *scalar_product(const T &scalar);
        Matrix<T> *matrix_product(const Matrix<T> &other);

        double count_norm() const;

        Matrix<T> *operator+=(const Matrix<T> &other);
        Matrix<T> *operator*=(const T &scalar);
        Matrix<T> *operator*=(const Matrix<T> &other);

        Matrix<T> *operator+(const Matrix<T> &other);
        Matrix<T> *operator*(const T &scalar);
        Matrix<T> *operator*(const Matrix<T> &other);

        Matrix<T> *swap_rows(int first, int second);
        Matrix<T> *multiply_row(int row, const T &scalar);
        Matrix<T> *add_row(int target, int source, const T &scalar);

        Matrix<T> *swap_columns(int first, int second);
        Matrix<T> *multiply_column(int column, const T &scalar);
        Matrix<T> *add_scaled_column(int target, int source, const T &scalar);
};

template <typename T> class MutableMatrix : public Matrix<T> {
    protected:
        Matrix<T> *Instance() override {
            return this;
        }

        Matrix<T> *Clone() const override {
            return new MutableMatrix<T>(*this);
        }

    public:
        Matrix<T> *Empty() const override {
            return new MutableMatrix<T>();
        }

        MutableMatrix() : Matrix<T>() {};
        MutableMatrix(const T *items, int size) : Matrix<T>(items, size) {};
        MutableMatrix(const T *items, int rows, int columns) : Matrix<T>(items, rows, columns) {};
        MutableMatrix(const Matrix<T> &other) : Matrix<T>(other) {};
};

template <typename T> class ImmutableMatrix : public Matrix<T> {
    protected:
        Matrix<T> *Instance() override {
            return Clone();
        }

        Matrix<T> *Clone() const override {
            return new ImmutableMatrix<T>(*this);
        }

    public:
        Matrix<T> *Empty() const override {
            return new ImmutableMatrix<T>();
        }

        ImmutableMatrix() : Matrix<T>() {};
        ImmutableMatrix(const T *items, int size) : Matrix<T>(items, size) {};
        ImmutableMatrix(const T *items, int rows, int columns) : Matrix<T>(items, rows, columns) {};
        ImmutableMatrix(const Matrix<T> &other) : Matrix<T>(other) {};
};

#include "Matrix.tpp"
