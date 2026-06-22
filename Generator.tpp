#pragma once

#include "Generator.hpp"

template <typename T> T Generator<T>::generate(const Ordinal &index,
    const LazyCache<T> &materialized) {
    if (index.is_infinite()) {
        throw std::logic_error("Generator does not support infinite ordinal index");
    }

    return generate(index.value(), materialized);
}

template <typename T> IndexGenerator<T>::IndexGenerator(T (*rule)(int)) : rule(rule) {}

template <typename T> T IndexGenerator<T>::generate(int index,
    const LazyCache<T> &materialized) {
    (void)materialized;
    return rule(index);
}

template <typename T> Generator<T> *IndexGenerator<T>::clone() const {
    return new IndexGenerator<T>(*this);
}

template <typename T>
FunctionGenerator<T>::FunctionGenerator(T (*rule)(int index,
    const LazyCache<T> &materialized)) : rule(rule) {}

template <typename T>
T FunctionGenerator<T>::generate(int index, const LazyCache<T> &materialized) {
    return rule(index, materialized);
}

template <typename T> Generator<T> *FunctionGenerator<T>::clone() const {
    return new FunctionGenerator<T>(*this);
}

inline ArithmeticIntGenerator::ArithmeticIntGenerator(int start, int step)
    : start(start), step(step) {}

inline int ArithmeticIntGenerator::generate(int index, const LazyCache<int> &materialized) {
    (void)materialized; // Добавлен чтобы убрать warnings
    return start + index * step;
}

inline Generator<int> *ArithmeticIntGenerator::clone() const {
    return new ArithmeticIntGenerator(*this);
}

template <typename T>
SubsequenceGenerator<T>::SubsequenceGenerator(const LazySequence<T> &source,
    const Ordinal &start_index)
    : source(new LazySequence<T>(source)), start_index(start_index) {}

template <typename T> SubsequenceGenerator<T>::~SubsequenceGenerator() {
    delete source;
}

template <typename T>
T SubsequenceGenerator<T>::generate(int index, const LazyCache<T> &materialized) {
    return generate(Ordinal::finite(index), materialized);
}

template <typename T>
T SubsequenceGenerator<T>::generate(const Ordinal &index, const LazyCache<T> &materialized) {
    (void)materialized;
    return source->get(start_index + index);
}

template <typename T> Generator<T> *SubsequenceGenerator<T>::clone() const {
    return new SubsequenceGenerator<T>(*source, start_index);
}

template <typename T>
AppendGenerator<T>::AppendGenerator(const LazySequence<T> &source, const T &item)
    : source(new LazySequence<T>(source)), source_length(source.get_length()), item(item) {}

template <typename T> AppendGenerator<T>::~AppendGenerator() {
    delete source;
}

template <typename T> T AppendGenerator<T>::generate(int index, const LazyCache<T> &materialized) {
    return generate(Ordinal::finite(index), materialized);
}

template <typename T>
T AppendGenerator<T>::generate(const Ordinal &index, const LazyCache<T> &materialized) {
    (void)materialized;
    if (index == source_length) {
        return item;
    }
    if (index < source_length) {
        return source->get(index);
    }

    throw std::out_of_range("Index out of range");
}

template <typename T> Generator<T> *AppendGenerator<T>::clone() const {
    return new AppendGenerator<T>(*source, item);
}

template <typename T>
PrependGenerator<T>::PrependGenerator(const LazySequence<T> &source, const T &item)
    : source(new LazySequence<T>(source)), item(item) {}

template <typename T> PrependGenerator<T>::~PrependGenerator() {
    delete source;
}

template <typename T> T PrependGenerator<T>::generate(int index,
    const LazyCache<T> &materialized) {
    return generate(Ordinal::finite(index), materialized);
}

template <typename T>
T PrependGenerator<T>::generate(const Ordinal &index, const LazyCache<T> &materialized) {
    (void)materialized;
    if (index == Ordinal::zero()) {
        return item;
    }
    if (index.is_finite()) {
        return source->get(index.value() - 1);
    }

    return source->get(index);
}

template <typename T> Generator<T> *PrependGenerator<T>::clone() const {
    return new PrependGenerator<T>(*source, item);
}

template <typename T>
InsertGenerator<T>::InsertGenerator(const LazySequence<T> &source, const T &item,
    const Ordinal &insert_index)
    : source(new LazySequence<T>(source)), insert_index(insert_index), item(item) {}

template <typename T> InsertGenerator<T>::~InsertGenerator() {
    delete source;
}

template <typename T>
bool InsertGenerator<T>::is_index_shifted_by_insert(const Ordinal &index) const {
    if (index <= insert_index) { // Этот учёт и для конечных и бесконечных ординалов
        return false; // То есть универсальный
    }

    if (insert_index.is_finite()) {
        return index.is_finite();
    }

    return index.get_omega_count() == insert_index.get_omega_count(); 
}   

template <typename T>
Ordinal InsertGenerator<T>::get_source_index_after_insert(const Ordinal &index) const {
    if (!is_index_shifted_by_insert(index)) {
        return index;
    }
    if (index.is_finite()) { 
        return Ordinal::finite(index.value() - 1);
    }

    return Ordinal(index.get_omega_count(), index.get_finite_part() - 1); 
}

template <typename T> T InsertGenerator<T>::generate(int index,
    const LazyCache<T> &materialized) {
    return generate(Ordinal::finite(index), materialized);
}

template <typename T>
T InsertGenerator<T>::generate(const Ordinal &index, const LazyCache<T> &materialized) {
    (void)materialized;
    if (index == insert_index) {
        return item; 
    }

    return source->get(get_source_index_after_insert(index)); 
}

template <typename T> Generator<T> *InsertGenerator<T>::clone() const {
    return new InsertGenerator<T>(*source, item, insert_index);
}

// RemoveGenerator
template <typename T>
RemoveGenerator<T>::RemoveGenerator(const LazySequence<T> &source, const Ordinal &remove_index)
    : source(new LazySequence<T>(source)), remove_index(remove_index) {}

template <typename T> RemoveGenerator<T>::~RemoveGenerator() {
    delete source;
}

template <typename T>
bool RemoveGenerator<T>::is_index_shifted_by_remove(const Ordinal &index) const {
    if (index < remove_index) {
        return false;
    }

    if (remove_index.is_finite()) {
        return index.is_finite();
    }

    return index.get_omega_count() == remove_index.get_omega_count();
}

template <typename T>
Ordinal RemoveGenerator<T>::get_source_index_after_remove(const Ordinal &index) const {
    if (!is_index_shifted_by_remove(index)) {
        return index; 
    }
    if (index.is_finite()) { 
        return Ordinal::finite(index.value() + 1);
    }

    return Ordinal(index.get_omega_count(), index.get_finite_part() + 1);
}

template <typename T> T RemoveGenerator<T>::generate(int index,
    const LazyCache<T> &materialized) {
    return generate(Ordinal::finite(index), materialized);
}

template <typename T>
T RemoveGenerator<T>::generate(const Ordinal &index, const LazyCache<T> &materialized) {
    (void)materialized;
    return source->get(get_source_index_after_remove(index));
}

template <typename T> Generator<T> *RemoveGenerator<T>::clone() const {
    return new RemoveGenerator<T>(*source, remove_index);
}

template <typename T>
ConcatGenerator<T>::ConcatGenerator(const LazySequence<T> &left, const LazySequence<T> &right)
    : left(new LazySequence<T>(left)), right(new LazySequence<T>(right)),
      left_length(left.get_length()) {}

template <typename T> ConcatGenerator<T>::~ConcatGenerator() {
    delete left;
    delete right;
}

template <typename T> T ConcatGenerator<T>::generate(int index,
    const LazyCache<T> &materialized) {
    return generate(Ordinal::finite(index), materialized);
}

template <typename T>
T ConcatGenerator<T>::generate(const Ordinal &index, const LazyCache<T> &materialized) {
    (void)materialized;
    if (index < left_length) {
        return left->get(index);
    }

    return right->get(index - left_length); 
}

template <typename T> Generator<T> *ConcatGenerator<T>::clone() const {
    return new ConcatGenerator<T>(*left, *right);
}

// MapGenerator
template <typename T>
MapGenerator<T>::MapGenerator(const LazySequence<T> &source, T (*function)(const T &item))
    : source(new LazySequence<T>(source)), function(function) {}

template <typename T> MapGenerator<T>::~MapGenerator() {
    delete source;
}

template <typename T> T MapGenerator<T>::generate(int index,
    const LazyCache<T> &materialized) {
    return generate(Ordinal::finite(index), materialized);
}

template <typename T>
T MapGenerator<T>::generate(const Ordinal &index, const LazyCache<T> &materialized) {
    (void)materialized;
    return function(source->get(index)); 
}

template <typename T> Generator<T> *MapGenerator<T>::clone() const {
    return new MapGenerator<T>(*source, function);
}

// WhereGenerator
template <typename T>
WhereGenerator<T>::WhereGenerator(const LazySequence<T> &source,
    bool (*predicate)(const T &item), const Ordinal &start_index)
    : source(new LazySequence<T>(source)), predicate(predicate), start_index(start_index),
      cursor(start_index) {}

template <typename T> WhereGenerator<T>::~WhereGenerator() {
    delete source;
}

template <typename T>
bool WhereGenerator<T>::has_next() const {
    return cursor < source->get_length();
}

template <typename T>
void WhereGenerator<T>::reset_to(const Ordinal &index) {
    cursor = start_index + Ordinal(index.get_omega_count(), 0);
}

template <typename T> T WhereGenerator<T>::get_next() {
    while (has_next()) {
        const T &value = source->get(cursor);
        cursor = Ordinal(cursor.get_omega_count(), cursor.get_finite_part() + 1);
        if (predicate(value)) {
            return value;
        }
    }

    throw std::out_of_range("Index out of range");
}

template <typename T> T WhereGenerator<T>::generate(int index,
    const LazyCache<T> &materialized) {
    return generate(Ordinal::finite(index), materialized);
}

template <typename T>
T WhereGenerator<T>::generate(const Ordinal &index, const LazyCache<T> &materialized) {
    (void)materialized;
    reset_to(index);

    for (int current_index = 0; current_index <= index.get_finite_part(); current_index++) {
        T value = get_next();
        if (current_index == index.get_finite_part()) {
            return value;
        }
    }

    throw std::out_of_range("Index out of range");
}

template <typename T> Generator<T> *WhereGenerator<T>::clone() const {
    WhereGenerator<T> *result = new WhereGenerator<T>(*source, predicate, start_index);
    result->cursor = cursor;
    return result;
}
