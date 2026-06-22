#pragma once

#include "Generator.hpp"
#include "IEnumerator.hpp"
#include "LazyCache.hpp"
#include "Option.hpp"
#include "Ordinal.hpp"
#include "Sequence.hpp"

#include <stdexcept>

template <typename T> class LazySequence {
    private:
        LazyCache<T> cache;
        Generator<T> *generator;
        Ordinal length;
        int next_index;
        T ordinal_value;

        void materialize_until(int index);
        bool is_valid_index(int index) const;
        bool is_valid_index(const Ordinal &index) const;

    public:
        LazySequence();
        LazySequence(const Sequence<T> &sequence, int cache_limit = 0);
        LazySequence(Generator<T> *generator,
                    Ordinal length = Ordinal::infinity(),
                    int cache_limit = 0);
        LazySequence(T (*rule)(int), 
                    Ordinal length = Ordinal::infinity(),
                    int cache_limit = 0);
        LazySequence(const Sequence<T> &initial_items,
                    T (*rule)(int index, const LazyCache<T> &materialized),
                    Ordinal length = Ordinal::infinity(),
                    int cache_limit = 0);
        LazySequence(const LazySequence<T> &other);
        LazySequence<T> &operator=(const LazySequence<T> &other);
        ~LazySequence();

        const T &get_first();
        const T &get_last();
        const T &get(int index);
        const T &get(const Ordinal &index);
        const T &get(int omega_count, int finite_part);

        Ordinal get_length() const;
        int get_materialized_count() const;
        int get_cache_limit() const;
        int get_cache_start_index() const;
        void set_cache_limit(int limit);
        bool is_materialized(int index) const;

        LazySequence<T> *get_subsequence(int start_index, int end_index);
        LazySequence<T> *get_subsequence(const Ordinal &start_index, const Ordinal &end_index);
        LazySequence<T> *get_subsequence(int start_omega_count, int start_finite_part,
            int end_omega_count, int end_finite_part);
        LazySequence<T> *take(int count);

        LazySequence<T> *append(const T &item);
        LazySequence<T> *prepend(const T &item);
        LazySequence<T> *insert(const T &item, int index);
        LazySequence<T> *insert(const T &item, const Ordinal &index);
        LazySequence<T> *insert(const T &item, int omega_count, int finite_part);
        LazySequence<T> *remove(int index);
        LazySequence<T> *remove(const Ordinal &index);
        LazySequence<T> *remove(int omega_count, int finite_part);
        LazySequence<T> *concat(LazySequence<T> &other);

        LazySequence<T> *map(T (*function)(const T &item));
        LazySequence<T> *where(bool (*predicate)(const T &item));
        LazySequence<T> *where(bool (*predicate)(const T &item), int start_index);
        LazySequence<T> *where(bool (*predicate)(const T &item), const Ordinal &start_index);
        LazySequence<T> *where(bool (*predicate)(const T &item), int omega_count,
            int finite_part);
        T reduce(T (*function)(const T &left, const T &right), const T &initial);
        
        // Выкинуть ошибку (deprecated методы)
        Sequence<T> *get_subsequence(int start_index, int end_index) const;
        Sequence<T> *concat(const Sequence<T> &other) const;
        Sequence<T> *map(T (*function)(const T &item)) const;
        Sequence<T> *where(bool (*predicate)(const T &item)) const;
        T reduce(T (*function)(const T &left, const T &right), const T &initial) const;
        
        class Enumerator : public IEnumerator<T> {
            private:
                LazySequence<T> &sequence;
                int current_index;
                bool at_end;
            
            public:
                Enumerator(LazySequence<T> &sequence)
                 : sequence(sequence), current_index(-1), at_end(false) {}

                bool move_next() override {
                    if (at_end) {
                        return false;
                    }

                    int next_index = current_index + 1;
                    if (sequence.get_length().is_finite() &&
                        next_index >= sequence.get_length().value()) {
                            current_index = next_index;
                            at_end = true;
                            return false;
                    }

                    try {
                        sequence.get(next_index);
                    } catch (const std::exception &error) {
                        current_index = next_index;
                        at_end = true;
                        return false;
                    }

                    current_index = next_index;
                    return true;
                }

                const T &get_curr() const override {
                    if (current_index < 0 || at_end) {
                        throw std::out_of_range("Enumerator is not positioned on an element");
                    }

                    return sequence.get(current_index);
                }

                bool reset() override {
                    current_index = -1;
                    at_end = false;
                    return true;
                }
        };

        IEnumerator<T> *get_enumerator();
};

#include "Generator.tpp"
#include "LazySequence.tpp"
