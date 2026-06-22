#pragma once

template <typename T> LazySequence<T>::LazySequence() 
    : cache(0), generator(nullptr), length(Ordinal::finite(0)), next_index(0) {}

template <typename T> LazySequence<T>::LazySequence(const Sequence<T> &sequence, int cache_limit)
    : cache(cache_limit), generator(nullptr), length(Ordinal::finite(sequence.get_length())),
    next_index(0) {
    for (int index = 0; index < sequence.get_length(); index++) {
        cache.push(sequence.get(index));
    }
    next_index = sequence.get_length();
}

template <typename T> LazySequence<T>::LazySequence(Generator<T> *generator, Ordinal length,
    int cache_limit) : cache(cache_limit), generator(generator), length(length), next_index(0) {}

template <typename T> LazySequence<T>::LazySequence(T (*rule)(int), Ordinal length, 
    int cache_limit) : cache(cache_limit), generator(new IndexGenerator<T>(rule)), length(length),
    next_index(0) {}

template <typename T> LazySequence<T>::LazySequence(
    const Sequence<T> &initial_items,
    T (*rule)(int index, const LazyCache<T> &materialized),
    Ordinal length,
    int cache_limit)
    : cache(cache_limit), generator(new FunctionGenerator<T>(rule)), length(length), next_index(0) {
    int initial_count = initial_items.get_length();
    if (length.is_finite() && initial_count > length.value()) {
        throw std::invalid_argument("Initial item count cannot exceed finite length");
    }

    for (int index = 0; index < initial_count; index++) {
        cache.push(initial_items.get(index));
    }
    next_index = initial_count;
}

template <typename T> LazySequence<T>::LazySequence(const LazySequence<T> &other)
    : cache(other.cache),
      generator(nullptr),
      length(other.length),
      next_index(other.next_index),
      ordinal_value(other.ordinal_value) {
    if (other.generator != nullptr) {
        generator = other.generator->clone();
    }
}

template <typename T> LazySequence<T> &LazySequence<T>::operator=(const LazySequence<T> &other) {
    if (this == &other) {
        return *this;
    }

    delete generator;
    cache = other.cache;
    if (other.generator == nullptr) {
        generator = nullptr;
    } else {
        generator = other.generator->clone();
    }
    length = other.length;
    next_index = other.next_index;
    ordinal_value = other.ordinal_value;
    return *this;
}


template <typename T> LazySequence<T>::~LazySequence() {
    delete generator;
}

template <typename T> bool LazySequence<T>::is_valid_index(int index) const {
    if (index < 0) {
        return false;
    }
    return length.is_infinite() || index < length.value();
}

template <typename T> bool LazySequence<T>::is_valid_index(const Ordinal &index) const {
    return index < length;
}

template <typename T> void LazySequence<T>::materialize_until(int index) {
    if (!is_valid_index(index)) {
        throw std::out_of_range("Index out of range");
    }

    if (cache.contains(index)) {
        return;
    }

    if (cache.forgotten(index)) {
        throw std::out_of_range("Element was deleted from Lazy Cache");
    }

    while (!cache.contains(index)) {
        if (length.is_finite() && next_index >= length.value()) {
            throw std::out_of_range("Index out of range");
        }
        if (generator == nullptr) {
            throw std::out_of_range("No generator for requested element");
        }

        T next_value = generator->generate(next_index, cache);
        cache.push(next_value);
        next_index++;
    }
}

template <typename T> const T &LazySequence<T>::get_first() {
    return get(0);
}

template <typename T> const T &LazySequence<T>::get_last() {
    if (length.is_infinite()) {
        throw std::logic_error("Infinite Lazy Sequence does not have last element");
    }
    if (length.value() == 0) {
        throw std::out_of_range("Sequence is empty");
    }
    return get(length.value() - 1);
}

template <typename T> const T &LazySequence<T>::get(int index) {
    materialize_until(index);
    return cache.get(index);
}

template <typename T> const T &LazySequence<T>::get(const Ordinal &index) {
    if (index.is_finite()) {
        return get(index.value());
    }
    if (!is_valid_index(index)) {
        throw std::out_of_range("Index out of range");
    }
    if (generator == nullptr) {
        throw std::out_of_range("No generator for requested element");
    }

    ordinal_value = generator->generate(index, cache);
    return ordinal_value;
}

template <typename T> const T &LazySequence<T>::get(int omega_count, int finite_part) {
    return get(Ordinal(omega_count, finite_part));
}


template <typename T> Ordinal LazySequence<T>::get_length() const {
    return length;
}

template <typename T> int LazySequence<T>::get_materialized_count() const {
    return cache.get_count();
}

template <typename T> int LazySequence<T>::get_cache_limit() const {
    return cache.get_capacity();
}

template <typename T> int LazySequence<T>::get_cache_start_index() const {
    return cache.get_first_index();
}

template <typename T> void LazySequence<T>::set_cache_limit(int limit) {
    cache.set_capacity(limit);
}

template <typename T> bool LazySequence<T>::is_materialized(int index) const {
    return cache.contains(index);
}

// Обёртка для конечных start_index end_index

template <typename T> LazySequence<T> *LazySequence<T>::get_subsequence(int start_index, int end_index) {
    if (start_index < 0 || end_index < 0) {
        throw std::out_of_range("Index out of range");
    }
    return get_subsequence(Ordinal::finite(start_index), Ordinal::finite(end_index));
}

// То же самое но для ординалов

template <typename T>
LazySequence<T> *LazySequence<T>::get_subsequence(const Ordinal &start_index,
    const Ordinal &end_index) {
    if (end_index < start_index) { // Тут очев
        throw std::out_of_range("Invalid subsequence range");
    }
    if (!is_valid_index(start_index) || !is_valid_index(end_index)) { // Тут вроде тоже
        throw std::out_of_range("Index out of range");
    }

    return new LazySequence<T>(new SubsequenceGenerator<T>(*this, start_index),
        end_index - start_index + Ordinal::finite(1));
}


template <typename T>
LazySequence<T> *LazySequence<T>::get_subsequence(int start_omega_count, int start_finite_part,
    int end_omega_count, int end_finite_part) {
    return get_subsequence(Ordinal(start_omega_count, start_finite_part),
        Ordinal(end_omega_count, end_finite_part));
}

template <typename T> LazySequence<T> *LazySequence<T>::take(int count) {
    if (count < 0) {
        throw std::invalid_argument("Count cannot be negative");
    }
    if (count == 0) {
        return new LazySequence<T>();
    }
    if (length.is_finite() && count > length.value()) {
        throw std::out_of_range("Cannot take more elements than finite sequence contains");
    }

    return get_subsequence(0, count - 1);
}


template <typename T> LazySequence<T> *LazySequence<T>::append(const T &item) {
    return new LazySequence<T>(new AppendGenerator<T>(*this, item),
        length + Ordinal::finite(1));
}

template <typename T> LazySequence<T> *LazySequence<T>::prepend(const T &item) {
    return new LazySequence<T>(new PrependGenerator<T>(*this, item),
        Ordinal::finite(1) + length);
}


template <typename T> LazySequence<T> *LazySequence<T>::insert(const T &item, int index) {
    if (index < 0) {
        throw std::out_of_range("Index out of range");
    }
    return insert(item, Ordinal::finite(index));
}

template <typename T>
LazySequence<T> *LazySequence<T>::insert(const T &item, const Ordinal &index) {
    if (index > length) {
        throw std::out_of_range("Index out of range");
    }

    Ordinal new_length = length;
    if (length.is_finite() || index.get_omega_count() == length.get_omega_count()) { 
        new_length = length + Ordinal::finite(1);
    } else if (index.is_finite()) {
        new_length = Ordinal::finite(1) + length; 
    }

    return new LazySequence<T>(new InsertGenerator<T>(*this, item, index), new_length);
}


template <typename T>
LazySequence<T> *LazySequence<T>::insert(const T &item, int omega_count, int finite_part) {
    return insert(item, Ordinal(omega_count, finite_part));
}


template <typename T> LazySequence<T> *LazySequence<T>::remove(int index) {
    if (index < 0) {
        throw std::out_of_range("Index out of range");
    }
    return remove(Ordinal::finite(index));
}

template <typename T> LazySequence<T> *LazySequence<T>::remove(const Ordinal &index) {
    if (!is_valid_index(index)) {
        throw std::out_of_range("Index out of range");
    }

    if (length.is_finite()) {
        if (length.value() == 1) {
            return new LazySequence<T>();
        }

        return new LazySequence<T>(new RemoveGenerator<T>(*this, index),
            Ordinal::finite(length.value() - 1));
    }

    Ordinal new_length = length;
    if (index.get_omega_count() == length.get_omega_count()) { 
        new_length = Ordinal(length.get_omega_count(), length.get_finite_part() - 1);
    }

    return new LazySequence<T>(new RemoveGenerator<T>(*this, index), new_length);
}


template <typename T>
LazySequence<T> *LazySequence<T>::remove(int omega_count, int finite_part) {
    return remove(Ordinal(omega_count, finite_part));
}


template <typename T> LazySequence<T> *LazySequence<T>::concat(LazySequence<T> &other) {
    return new LazySequence<T>(new ConcatGenerator<T>(*this, other), length + other.length);
}

template <typename T> LazySequence<T> *LazySequence<T>::map(T (*function)(const T &item)) {
    return new LazySequence<T>(new MapGenerator<T>(*this, function), length);
}

template <typename T> LazySequence<T> *LazySequence<T>::where(bool (*predicate)(const T &item)) {
    return where(predicate, Ordinal::zero());
}

template <typename T>
LazySequence<T> *LazySequence<T>::where(bool (*predicate)(const T &item), int start_index) {
    if (start_index < 0) {
        throw std::out_of_range("Index out of range");
    }

    return where(predicate, Ordinal::finite(start_index));
}

template <typename T>
LazySequence<T> *LazySequence<T>::where(bool (*predicate)(const T &item),
    const Ordinal &start_index) {
    if (start_index > length) {
        throw std::out_of_range("Index out of range");
    }

    Ordinal result_length = length - start_index;
    Generator<T> *where_generator = new WhereGenerator<T>(*this, predicate, start_index);
    return new LazySequence<T>(where_generator, result_length);
}


template <typename T>
LazySequence<T> *LazySequence<T>::where(bool (*predicate)(const T &item), int omega_count,
    int finite_part) {
    return where(predicate, Ordinal(omega_count, finite_part));
}


template <typename T> T LazySequence<T>::reduce(T (*function)(const T &left, const T &right), const T &initial) {
    if (length.is_infinite()) {
        throw std::logic_error("Use take(count) before reduce on an infinite sequence");
    }

    T accumulated = initial;
    for (int index = 0; index < length.value(); index++) {
        accumulated = function(accumulated, get(index));
    }
    return accumulated;
}

template <typename T> Sequence<T> *LazySequence<T>::get_subsequence(int start_index,
    int end_index) const {
    throw std::logic_error("get_subsequence(int,int) const not supported on LazySequence, use non-const LazySequence::get_subsequence");
}

template <typename T> Sequence<T> *LazySequence<T>::concat(const Sequence<T> &other) const {
    throw std::logic_error("concat(const Sequence&) const not supported on LazySequence, use concat(LazySequence&)");
}

template <typename T> Sequence<T> *LazySequence<T>::map(T (*function)(const T &item)) const {
    throw std::logic_error("map(T(*)(const T&)) const not supported on LazySequence, use non-const LazySequence::map");
}

template <typename T> Sequence<T> *LazySequence<T>::where(bool (*predicate)(const T &item)) const {
    throw std::logic_error("where(bool(*)(const T&)) const not supported on LazySequence, use non-const LazySequence::where");
}

template <typename T> T LazySequence<T>::reduce(T (*function)(const T &left, const T &right),
    const T &initial) const {
    throw std::logic_error("reduce(T(*)(const T&,const T&),T) const not supported on LazySequence, use non-const LazySequence::reduce");
}

template <typename T> IEnumerator<T> *LazySequence<T>::get_enumerator() {
    return new Enumerator(*this);
}
