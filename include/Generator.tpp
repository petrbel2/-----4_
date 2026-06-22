#pragma once

#include "Generator.hpp"

// Generator
template <typename T> T Generator<T>::generate(const Ordinal &index,
    const LazyCache<T> &materialized) {
    if (index.is_infinite()) {
        throw std::logic_error("Generator does not support infinite ordinal index");
    }

    return generate(index.value(), materialized);
}

// IndexGenerator
template <typename T> IndexGenerator<T>::IndexGenerator(T (*rule)(int)) : rule(rule) {}

template <typename T> T IndexGenerator<T>::generate(int index,
    const LazyCache<T> &materialized) {
    (void)materialized;
    return rule(index);
}

template <typename T> Generator<T> *IndexGenerator<T>::clone() const {
    return new IndexGenerator<T>(*this);
}

// FunctionGenerator
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

// ArithmeticIntGenerator
inline ArithmeticIntGenerator::ArithmeticIntGenerator(int start, int step)
    : start(start), step(step) {}

inline int ArithmeticIntGenerator::generate(int index, const LazyCache<int> &materialized) {
    (void)materialized; // Добавлен чтобы убрать warnings
    return start + index * step;
}

inline Generator<int> *ArithmeticIntGenerator::clone() const {
    return new ArithmeticIntGenerator(*this);
}

// SubsequenceGenerator
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

// AppendGenerator
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

// PrependGenerator
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

// InsertGenerator
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

    return index.get_omega_count() == insert_index.get_omega_count(); // Если в разных ординалах
    // То возвращаем false и внизу тупо генерируем индекс как без вставки (поскольку мы в разных б)
}   // если true, то значит, что мы уже после индекса (иначе бы вылетели на первом условии)
    // и в одной omega, то есть индекс надо сдвинуть и вернуть в нижней функции
    // return Ordinal(index.get_omega_count(), index.get_finite_part() - 1);

template <typename T>
Ordinal InsertGenerator<T>::get_source_index_after_insert(const Ordinal &index) const {
    if (!is_index_shifted_by_insert(index)) {
        return index;
    }
    if (index.is_finite()) { // Если последотельность конечна, и мы после нужного элемента, то сдвигаем элементы
        return Ordinal::finite(index.value() - 1);
    }

    return Ordinal(index.get_omega_count(), index.get_finite_part() - 1); /*Мы уже учитываем, что
    if (index <= insert_index) {
        return false;
    }
    */ 
}

template <typename T> T InsertGenerator<T>::generate(int index,
    const LazyCache<T> &materialized) {
    return generate(Ordinal::finite(index), materialized);
}

template <typename T>
T InsertGenerator<T>::generate(const Ordinal &index, const LazyCache<T> &materialized) {
    (void)materialized;
    if (index == insert_index) {
        return item; // Возвращаем вставку
    }

    return source->get(get_source_index_after_insert(index)); // генерируем по вышеописанным правилам
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
    if (index < remove_index) { // если мы до remove, то ничего не меняем
        return false;
    }

    if (remove_index.is_finite()) { // Если индекс конечный
        return index.is_finite();
    }

    return index.get_omega_count() == remove_index.get_omega_count();
}

template <typename T>
Ordinal RemoveGenerator<T>::get_source_index_after_remove(const Ordinal &index) const {
    if (!is_index_shifted_by_remove(index)) {
        return index; 
        /* Если мы до, то false, если мы в омега, а index конечный, то false,
           Если мы в разных омега, то false

           Если false, то просто копируем как есть, то есть до индекса ничего не удаляем,
           в разных омега аналогично
        */
    }
    if (index.is_finite()) { // Если индекс конечный, то возвращаем по правилу для конечных
        /*
        То есть
        индексы: 0 1 2 3 4 5 ...
        числа:   1 2 3 4 5 6 ...

        Допустим мы удаляем по индексу 3, то есть элемент 4, тогда:
        индексы: 0 1 2 3 4 ...
        числа:   1 2 3 5 6 ...

        Чтобы получить индекс в старой последовательности по новой нужно прибавить 1
        */
        return Ordinal::finite(index.value() + 1);
    }

    // Аналогично верхнемму, просто ещё по омега получаем
    return Ordinal(index.get_omega_count(), index.get_finite_part() + 1);
}

template <typename T> T RemoveGenerator<T>::generate(int index,
    const LazyCache<T> &materialized) {
    return generate(Ordinal::finite(index), materialized);
}

template <typename T>
T RemoveGenerator<T>::generate(const Ordinal &index, const LazyCache<T> &materialized) {
    (void)materialized;
    return source->get(get_source_index_after_remove(index)); // Мы по новым индексам получаем старые
}

template <typename T> Generator<T> *RemoveGenerator<T>::clone() const {
    return new RemoveGenerator<T>(*source, remove_index);
}

// Для примера с тремя последовательностями: Возьмём последовательности a, b, c
// Конкатинируем их: сначала получив ab, потом конкатинируем ab и с -> abc
// После обратимся к элементам b
// Логически: элементы a лежат в конченых индексах, b - omega, c - omega * 2
// Следовательно, чтобы обратится к b мы должны обратится по omega индексам

// Технически: LazySequence abc состоит из ab - left и c - right
// Попробуем обратится к b: omega < omega*2, следовательно обащаемся к вложенному
// генератору ab. Там обращаемся к right посольку omega < const вернёт false
// Далее получаем right

// ConcatGenerator
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

    return right->get(index - left_length); // Для конечных тривиально, для бесконечных вычитаем omega
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
    return function(source->get(index)); // Тут тупо применяем полученную функцию к генериремым элементам
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
