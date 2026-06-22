#pragma once

#include "include/ArraySequence.h"
#include "include/DynamicArray.h"
#include "include/ListSequence.h"
#include "include/Sequence.h"

#include <cmath>
#include <complex>
#include <iostream>
#include <stdexcept>
#include <string>

void print_sequence_kind_menu();
inline void expect(bool condition, const std::string &name);

const int MUTABLE_ARRAY_SEQUENCE_KIND = 1;
const int IMMUTABLE_ARRAY_SEQUENCE_KIND = 2;
const int MUTABLE_LIST_SEQUENCE_KIND = 3;
const int IMMUTABLE_LIST_SEQUENCE_KIND = 4;
const int VECTOR_KIND = 5;
const int MUTABLE_MATRIX_KIND = 6;
const int IMMUTABLE_MATRIX_KIND = 7;
const int LAZY_SEQUENCE_KIND = 8;
const int STREAM_STATISTICS_KIND = 9;

inline int square(const int &value) {
    return value * value;
}

inline bool is_even(const int &value) {
    return value % 2 == 0;
}

inline int sum_values(const int &left, const int &right) {
    return left + right;
}

inline int sum(const int &left, const int &right) {
    return sum_values(left, right);
}

inline std::string serialize_int(const int &value) {
    return std::to_string(value);
}

inline int deserialize_int(const std::string &text) {
    return std::stoi(text);
}

inline bool equal_value(const int &actual, const int &expected) {
    return actual == expected;
}

inline bool equal_value(const double &actual, const double &expected) {
    return std::fabs(actual - expected) < 1e-9;
}

inline bool equal_value(const std::complex<double> &actual, const std::complex<double> &expected) {
    return std::abs(actual - expected) < 1e-9;
}

inline bool equal_value(const std::complex<int> &actual, const std::complex<int> &expected) {
    return actual == expected;
}

inline void ignore_line() {
    std::cin.clear();
    std::cin.ignore(10000, '\n');
}

template <class T>
void expect_eq(const T &actual, const T &expected, const std::string &name) {
    expect(equal_value(actual, expected), name);
}

inline int read_int(const std::string &prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            ignore_line();
            return value;
        }

        std::cout << "Invalid integer. Try again.\n";
        ignore_line();
    }
}

inline int read_non_negative_int(const std::string &prompt) {
    int value = read_int(prompt);
    while (value < 0) {
        std::cout << "Value must be non-negative.\n";
        value = read_int(prompt);
    }
    return value;
}

inline int read_sequence_kind() {
    print_sequence_kind_menu();
    int kind = read_int("Type: ");
    while (kind < MUTABLE_ARRAY_SEQUENCE_KIND || kind > IMMUTABLE_LIST_SEQUENCE_KIND) {
        std::cout << "Unknown type. Choose 1..4.\n";
        kind = read_int("Type: ");
    }
    return kind;
}

inline DynamicArray<int> read_values() {
    int count = read_non_negative_int("Element count: ");
    DynamicArray<int> values(count);

    for (int index = 0; index < count; index++) {
        values.set(index, read_int("Element " + std::to_string(index) + ": "));
    }

    return values;
}

inline Sequence<int> *create_sequence(int kind, const DynamicArray<int> &values) {
    Sequence<int> *sequence = nullptr;

    switch (kind) {
        case MUTABLE_ARRAY_SEQUENCE_KIND:
            sequence = new MutableArraySequence<int>();
            break;
        case IMMUTABLE_ARRAY_SEQUENCE_KIND:
            sequence = new ImmutableArraySequence<int>();
            break;
        case MUTABLE_LIST_SEQUENCE_KIND:
            sequence = new MutableListSequence<int>();
            break;
        case IMMUTABLE_LIST_SEQUENCE_KIND:
            sequence = new ImmutableListSequence<int>();
            break;
        default:
            throw std::invalid_argument("Unknown sequence type");
    }

    for (int index = 0; index < values.get_size(); index++) {
        Sequence<int> *next = sequence->append(values.get(index));
        if (next != sequence) {
            delete sequence;
            sequence = next;
        }
    }

    return sequence;
}

inline void reset_sequence(Sequence<int> *&sequence, int &kind) {
    int new_kind = read_sequence_kind();
    DynamicArray<int> values = read_values();
    Sequence<int> *replacement = create_sequence(new_kind, values);

    delete sequence;
    sequence = replacement;
    kind = new_kind;
}

inline void replace_sequence(Sequence<int> *&sequence, Sequence<int> *result) {
    if (result == nullptr) {
        throw std::runtime_error("Operation returned nullptr");
    }

    if (result != sequence) {
        delete sequence;
        sequence = result;
    }
}

inline void append_value(Sequence<int> *&sequence) {
    int value = read_int("Value: ");
    replace_sequence(sequence, sequence->append(value));
}

inline void prepend_value(Sequence<int> *&sequence) {
    int value = read_int("Value: ");
    replace_sequence(sequence, sequence->prepend(value));
}

inline void insert_value(Sequence<int> *&sequence) {
    int value = read_int("Value: ");
    int index = read_int("Index: ");
    replace_sequence(sequence, sequence->insert(value, index));
}

inline void remove_value(Sequence<int> *&sequence) {
    int index = read_int("Index: ");
    replace_sequence(sequence, sequence->remove(index));
}

inline void concat_sequence(Sequence<int> *&sequence) {
    std::cout << "Enter sequence to append.\n";
    DynamicArray<int> values = read_values();
    Sequence<int> *other = create_sequence(1, values);

    try {
        Sequence<int> *result = sequence->concat(*other);
        replace_sequence(sequence, result);
        delete other;
    } catch (...) {
        delete other;
        throw;
    }
}
