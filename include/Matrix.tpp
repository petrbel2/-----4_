#pragma once

#include "Matrix.h"

template <typename T> Matrix<T>::Matrix()
    : items(), rows(0), columns(0) {}

template <typename T> Matrix<T>::Matrix(const T *source, int size)
    : items(), rows(size), columns(size) {
    if (size < 0) {
        throw std::invalid_argument("Matrix size cannot be negative");
    }

    int count = size * size;
    if (count > 0 && source == nullptr) {
        throw std::invalid_argument("Items cannot be null");
    }

    for (int index = 0; index < count; index++) {
        append_to_current(source[index]);
    }
}

template <typename T> Matrix<T>::Matrix(const T *source, int row_count, int column_count)
    : items(), rows(row_count), columns(column_count) {
    if (row_count < 0 || column_count < 0) {
        throw std::invalid_argument("Matrix size cannot be negative");
    }

    int count = row_count * column_count;
    if (count > 0 && source == nullptr) {
        throw std::invalid_argument("Items cannot be null");
    }

    for (int index = 0; index < count; index++) {
        append_to_current(source[index]);
    }
}

template <typename T> Matrix<T>::Matrix(const Matrix<T> &other)
    : items(other.items), rows(other.rows), columns(other.columns) {}

template <typename T> int Matrix<T>::get_rows() const {
    return rows;
}

template <typename T> int Matrix<T>::get_columns() const {
    return columns;
}

template <typename T> int Matrix<T>::index_transform(int row, int column) const {
    return row * columns + column;
}

template <typename T> void Matrix<T>::append_to_current(const T &value) {
    items.append(value);
}

template <typename T> void Matrix<T>::set_to_current(int row, int column, const T &value) {
    int real_index = index_transform(row, column);
    items.set(real_index, value);
}

template <typename T> const T &Matrix<T>::get_el(int row, int column) const {
    if (row < 0 || row >= rows) {
        throw std::out_of_range("Row index out of range");
    }
    if (column < 0 || column >= columns) {
        throw std::out_of_range("Column index out of range");
    }

    return items.get(index_transform(row, column));
}

template <typename T> Matrix<T> *Matrix<T>::set_el(int row, int column, const T &value) {
    if (row < 0 || row >= rows) {
        throw std::out_of_range("Row index out of range");
    }
    if (column < 0 || column >= columns) {
        throw std::out_of_range("Column index out of range");
    }

    Matrix<T> *result = Instance();
    result->set_to_current(row, column, value);
    return result;
}

template <typename T> Matrix<T> *Matrix<T>::add_to_self(const Matrix<T> &other) {
    if (rows != other.rows || columns != other.columns) {
        throw std::out_of_range("Matrices have different sizes");
    }

    Matrix<T> *result = Instance();

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            T value = result->get_el(row, column) + other.get_el(row, column);
            result->set_to_current(row, column, value);
        }
    }

    return result;
}

template <typename T> Matrix<T> *Matrix<T>::scalar_product(const T &scalar) {
    Matrix<T> *result = Instance();

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            T value = result->get_el(row, column) * scalar;
            result->set_to_current(row, column, value);
        }
    }

    return result;
}

template <typename T> Matrix<T> *Matrix<T>::matrix_product(const Matrix<T> &other) {
    if (columns != other.rows) {
        throw std::out_of_range("Matrices have incompatible sizes");
    }

    Matrix<T> *result = Empty();
    result->rows = rows;
    result->columns = other.columns;

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < other.columns; column++) {
            T value{};

            for (int k = 0; k < columns; k++) {
                value += get_el(row, k) * other.get_el(k, column);
            }

            result->append_to_current(value);
        }
    }

    return result;
}

template <typename T> double Matrix<T>::count_norm() const {
    double result = 0;

    for (int index = 0; index < rows * columns; index++) {
        double value = std::abs(items.get(index));
        result += value * value;
    }

    return std::sqrt(result);
}

template <typename T> Matrix<T> *Matrix<T>::operator+=(const Matrix<T> &other) {
    return add_to_self(other);
}

template <typename T> Matrix<T> *Matrix<T>::operator*=(const T &scalar) {
    return scalar_product(scalar);
}

template <typename T> Matrix<T> *Matrix<T>::operator*=(const Matrix<T> &other) {
    Matrix<T> *result = Instance();
    Matrix<T> *product = result->matrix_product(other);

    result->items = product->items;
    result->rows = product->rows;
    result->columns = product->columns;

    delete product;
    return result;
}

template <typename T> Matrix<T> *Matrix<T>::operator+(const Matrix<T> &other) {
    Matrix<T> *result = Clone();
    Matrix<T> *updated = (*result += other);
    if (updated != result) {
        delete result;
    }
    return updated;
}

template <typename T> Matrix<T> *Matrix<T>::operator*(const T &scalar) {
    Matrix<T> *result = Clone();
    Matrix<T> *updated = (*result *= scalar);
    if (updated != result) {
        delete result;
    }
    return updated;
}

template <typename T> Matrix<T> *Matrix<T>::operator*(const Matrix<T> &other) {
    Matrix<T> *result = Clone();
    Matrix<T> *updated = (*result *= other);
    if (updated != result) {
        delete result;
    }
    return updated;
}

template <typename T> Matrix<T> *Matrix<T>::swap_rows(int first, int second) {
    if (first < 0 || first >= rows || second < 0 || second >= rows) {
        throw std::out_of_range("Row index out of range");
    }

    Matrix<T> *result = Instance();

    for (int column = 0; column < columns; column++) {
        T first_value = result->get_el(first, column);
        T second_value = result->get_el(second, column);

        result->set_to_current(first, column, second_value);
        result->set_to_current(second, column, first_value);
    }

    return result;
}

template <typename T> Matrix<T> *Matrix<T>::multiply_row(int row, const T &scalar) {
    if (row < 0 || row >= rows) {
        throw std::out_of_range("Row index out of range");
    }

    Matrix<T> *result = Instance();

    for (int column = 0; column < columns; column++) {
        T value = result->get_el(row, column) * scalar;
        result->set_to_current(row, column, value);
    }

    return result;
}

template <typename T> Matrix<T> *Matrix<T>::add_row(int target, int source, const T &scalar) {
    if (target < 0 || target >= rows || source < 0 || source >= rows) {
        throw std::out_of_range("Row index out of range");
    }

    Matrix<T> *result = Instance();

    for (int column = 0; column < columns; column++) {
        T value = result->get_el(target, column) + scalar * result->get_el(source, column);
        result->set_to_current(target, column, value);
    }

    return result;
}

template <typename T> Matrix<T> *Matrix<T>::swap_columns(int first, int second) {
    if (first < 0 || first >= columns || second < 0 || second >= columns) {
        throw std::out_of_range("Column index out of range");
    }

    Matrix<T> *result = Instance();

    for (int row = 0; row < rows; row++) {
        T first_value = result->get_el(row, first);
        T second_value = result->get_el(row, second);

        result->set_to_current(row, first, second_value);
        result->set_to_current(row, second, first_value);
    }

    return result;
}

template <typename T> Matrix<T> *Matrix<T>::multiply_column(int column, const T &scalar) {
    if (column < 0 || column >= columns) {
        throw std::out_of_range("Column index out of range");
    }

    Matrix<T> *result = Instance();

    for (int row = 0; row < rows; row++) {
        T value = result->get_el(row, column) * scalar;
        result->set_to_current(row, column, value);
    }

    return result;
}

template <typename T> Matrix<T> *Matrix<T>::add_scaled_column(int target, int source, const T &scalar) {
    if (target < 0 || target >= columns || source < 0 || source >= columns) {
        throw std::out_of_range("Column index out of range");
    }

    Matrix<T> *result = Instance();

    for (int row = 0; row < rows; row++) {
        T value = result->get_el(row, target) + scalar * result->get_el(row, source);
        result->set_to_current(row, target, value);
    }

    return result;
}
