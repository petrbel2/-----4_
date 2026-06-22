#pragma once

#include "include/Sequence.h"
#include "include/Vector.h"
#include "include/Matrix.h"
#include "util.h"

#include <cmath>
#include <complex>
#include <iostream>

inline void print_sequence_kind_menu() {
    std::cout << "\nChoose sequence type:\n";
    std::cout << "1. MutableArraySequence\n";
    std::cout << "2. ImmutableArraySequence\n";
    std::cout << "3. MutableListSequence\n";
    std::cout << "4. ImmutableListSequence\n";
}

inline void print_object_kind_menu() {
    std::cout << "\nChoose object type:\n";
    std::cout << MUTABLE_ARRAY_SEQUENCE_KIND << ". MutableArraySequence<int>\n";
    std::cout << IMMUTABLE_ARRAY_SEQUENCE_KIND << ". ImmutableArraySequence<int>\n";
    std::cout << MUTABLE_LIST_SEQUENCE_KIND << ". MutableListSequence<int>\n";
    std::cout << IMMUTABLE_LIST_SEQUENCE_KIND << ". ImmutableListSequence<int>\n";
    std::cout << VECTOR_KIND << ". Vector<int>\n";
    std::cout << MUTABLE_MATRIX_KIND << ". MutableMatrix<int>\n";
    std::cout << IMMUTABLE_MATRIX_KIND << ". ImmutableMatrix<int>\n";
    std::cout << LAZY_SEQUENCE_KIND << ". LazySequence<int>\n";
    std::cout << STREAM_STATISTICS_KIND << ". Stream online statistics demo\n";
    std::cout << "0. Exit\n";
}

inline void print_vector_kind_menu() {
    std::cout << "\nChoose vector type:\n";
    std::cout << VECTOR_KIND << ". Vector<int>\n";
}

inline void print_matrix_kind_menu() {
    std::cout << "\nChoose matrix type:\n";
    std::cout << MUTABLE_MATRIX_KIND << ". MutableMatrix<int>\n";
    std::cout << IMMUTABLE_MATRIX_KIND << ". ImmutableMatrix<int>\n";
}

extern int passed;
extern int failed;

inline void expect(bool condition, const std::string &name) {
    if (condition) {
        passed++;
        std::cout << "[PASS] " << name << '\n';
    } else {
        failed++;
        std::cout << "[FAIL] " << name << '\n';
    }
}

template <class T>
void expect_vector_eq(const Vector<T> &vector, const T *expected, int count, const std::string &name) {
    IEnumerator<T> *enumerator = vector.get_enumerator();
    int index = 0;
    while (enumerator->move_next()) {
        expect(index < count, name + " item " + std::to_string(index) + " exists");
        if (index < count) {
            expect_eq(enumerator->get_curr(), expected[index], name + " item " + std::to_string(index));
        }
        index++;
    }
    delete enumerator;
    expect_eq(index, count, name + " length");
}

inline const char *sequence_kind_name(int kind) {
    switch (kind) {
        case MUTABLE_ARRAY_SEQUENCE_KIND:
            return "MutableArraySequence";
        case IMMUTABLE_ARRAY_SEQUENCE_KIND:
            return "ImmutableArraySequence";
        case MUTABLE_LIST_SEQUENCE_KIND:
            return "MutableListSequence";
        case IMMUTABLE_LIST_SEQUENCE_KIND:
            return "ImmutableListSequence";
        default:
            return "Unknown";
    }
}

inline const char *vector_kind_name(int kind) {
    switch (kind) {
        case VECTOR_KIND:
            return "Vector<int>";
        default:
            return "Unknown";
    }
}

inline const char *matrix_kind_name(int kind) {
    switch (kind) {
        case MUTABLE_MATRIX_KIND:
            return "MutableMatrix<int>";
        case IMMUTABLE_MATRIX_KIND:
            return "ImmutableMatrix<int>";
        default:
            return "Unknown";
    }
}

inline void print_sequence(const Sequence<int> &sequence) {
    std::cout << "[";
    for (int index = 0; index < sequence.get_length(); index++) {
        if (index > 0) {
            std::cout << ", ";
        }
        std::cout << sequence.get(index);
    }
    std::cout << "]";
}

inline void print_sequence(const char *name, const Sequence<int> &sequence) {
    std::cout << name << " = ";
    print_sequence(sequence);
    std::cout << "\n";
}

inline void print_current_sequence(const Sequence<int> &sequence, int kind) {
    std::cout << sequence_kind_name(kind) << " length=" << sequence.get_length() << " value=";
    print_sequence(sequence);
    std::cout << "\n";
}

inline void print_option(const Option<int> &option) {
    if (option.has_value()) {
        std::cout << "Value: " << option.get_value() << "\n";
    } else {
        std::cout << "No value\n";
    }
}

inline void print_option(const char *name, const Option<int> &option) {
    std::cout << name << " = ";
    if (option.has_value()) {
        std::cout << option.get_value();
    } else {
        std::cout << "None";
    }
    std::cout << "\n";
}

inline void print_with_enumerator(const Sequence<int> &sequence) {
    IEnumerator<int> *enumerator = sequence.get_enumerator();

    std::cout << "[";
    bool first = true;
    while (enumerator->move_next()) {
        if (!first) {
            std::cout << ", ";
        }
        std::cout << enumerator->get_curr();
        first = false;
    }
    std::cout << "]\n";

    delete enumerator;
}

inline void print_with_enumerator(const char *name, const Sequence<int> &sequence) {
    std::cout << name << " = ";
    print_with_enumerator(sequence);
}

inline void print_operation_menu() {
    std::cout << "\nMenu:\n";
    std::cout << "1. Print sequence\n";
    std::cout << "2. Get first\n";
    std::cout << "3. Get last\n";
    std::cout << "4. Get by index\n";
    std::cout << "5. Try get by index\n";
    std::cout << "6. Append\n";
    std::cout << "7. Prepend\n";
    std::cout << "8. Insert\n";
    std::cout << "9. Remove\n";
    std::cout << "10. Get subsequence\n";
    std::cout << "11. Concat\n";
    std::cout << "12. Map square\n";
    std::cout << "13. Where even\n";
    std::cout << "14. Reduce sum\n";
    std::cout << "15. Print with enumerator\n";
    std::cout << "16. Create new sequence\n";
    std::cout << "0. Back\n";
}

inline void print_vector(const Vector<int> &vector) {
    IEnumerator<int> *enumerator = vector.get_enumerator();
    std::cout << "[";
    bool first = true;
    while (enumerator->move_next()) {
        if (!first) {
            std::cout << ", ";
        }
        std::cout << enumerator->get_curr();
        first = false;
    }
    std::cout << "]";
    delete enumerator;
}

inline void print_current_vector(const Vector<int> &vector, int kind) {
    std::cout << vector_kind_name(kind) << " dimension=" << vector.get_vector().get_length() << " value=";
    print_vector(vector);
    std::cout << "\n";
}

inline void print_vector_operation_menu() {
    std::cout << "\nVector menu:\n";
    std::cout << "1. Print vector\n";
    std::cout << "2. Set component\n";
    std::cout << "3. Sum with another vector\n";
    std::cout << "4. Multiply by scalar\n";
    std::cout << "5. Scalar product\n";
    std::cout << "6. Norm\n";
    std::cout << "7. Print with enumerator\n";
    std::cout << "8. Create new vector\n";
    std::cout << "0. Back\n";
}

inline void print_matrix(const Matrix<int> &matrix) {
    std::cout << "[\n";
    for (int row = 0; row < matrix.get_rows(); row++) {
        std::cout << "  [";
        for (int column = 0; column < matrix.get_columns(); column++) {
            if (column > 0) {
                std::cout << ", ";
            }
            std::cout << matrix.get_el(row, column);
        }
        std::cout << "]";
        if (row + 1 < matrix.get_rows()) {
            std::cout << ",";
        }
        std::cout << "\n";
    }
    std::cout << "]";
}

inline void print_current_matrix(const Matrix<int> &matrix, int kind) {
    std::cout << matrix_kind_name(kind) << " size=" << matrix.get_rows()
              << "x" << matrix.get_columns() << " value=";
    print_matrix(matrix);
    std::cout << "\n";
}

inline void print_matrix_operation_menu() {
    std::cout << "\nMatrix menu:\n";
    std::cout << "1. Print matrix\n";
    std::cout << "2. Get element\n";
    std::cout << "3. Set element\n";
    std::cout << "4. Sum with another matrix\n";
    std::cout << "5. Multiply by scalar\n";
    std::cout << "6. Multiply by another matrix\n";
    std::cout << "7. Norm\n";
    std::cout << "8. Swap rows\n";
    std::cout << "9. Multiply row\n";
    std::cout << "10. Add row\n";
    std::cout << "11. Swap columns\n";
    std::cout << "12. Multiply column\n";
    std::cout << "13. Add scaled column\n";
    std::cout << "14. Create new matrix\n";
    std::cout << "0. Back\n";
}

inline void print_subsequence(const Sequence<int> &sequence) {
    int start_index = read_int("Start index: ");
    int end_index = read_int("End index: ");

    Sequence<int> *result = sequence.get_subsequence(start_index, end_index);
    std::cout << "Result: ";
    print_sequence(*result);
    std::cout << "\n";
    delete result;
}

inline void print_mapped(Sequence<int> &sequence) {
    Sequence<int> *result = sequence.map(square);
    std::cout << "Result: ";
    print_sequence(*result);
    std::cout << "\n";
    delete result;
}

inline void print_filtered(Sequence<int> &sequence) {
    Sequence<int> *result = sequence.where(is_even);
    std::cout << "Result: ";
    print_sequence(*result);
    std::cout << "\n";
    delete result;
}
