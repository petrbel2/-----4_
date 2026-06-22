#pragma once

#include "LazyCache.h"
#include "Ordinal.h"

template <typename T> class LazySequence;

template <typename T> class Generator {
    public:
        virtual ~Generator() = default;

        virtual T generate(int index, const LazyCache<T> &materialized) = 0;
        virtual T generate(const Ordinal &index, const LazyCache<T> &materialized);
        virtual Generator<T> *clone() const = 0;
};

template <typename T> class IndexGenerator : public Generator<T> {
    private:
        T (*rule)(int);

    public:
        IndexGenerator(T (*rule)(int));

        T generate(int index, const LazyCache<T> &materialized) override;

        Generator<T> *clone() const override;
};

template <typename T> class FunctionGenerator : public Generator<T> {
    private:
        T (*rule)(int index, const LazyCache<T> &materialized);

    public:
        FunctionGenerator(T (*rule)(int index, const LazyCache<T> &materialized));

        T generate(int index, const LazyCache<T> &materialized) override;

        Generator<T> *clone() const override;
};

class ArithmeticIntGenerator : public Generator<int> {
    private:
        int start;
        int step;

    public:
        ArithmeticIntGenerator(int start, int step);

        int generate(int index, const LazyCache<int> &materialized) override;

        Generator<int> *clone() const override;
};

template <typename T> class SubsequenceGenerator : public Generator<T> {
    private:
        LazySequence<T> *source;
        Ordinal start_index;

    public:
        SubsequenceGenerator(const LazySequence<T> &source, const Ordinal &start_index);
        ~SubsequenceGenerator();

        T generate(int index, const LazyCache<T> &materialized) override;
        T generate(const Ordinal &index, const LazyCache<T> &materialized) override;
        Generator<T> *clone() const override;
};

template <typename T> class AppendGenerator : public Generator<T> {
    private:
        LazySequence<T> *source;
        Ordinal source_length;
        T item;

    public:
        AppendGenerator(const LazySequence<T> &source, const T &item);
        ~AppendGenerator();

        T generate(int index, const LazyCache<T> &materialized) override;
        T generate(const Ordinal &index, const LazyCache<T> &materialized) override;
        Generator<T> *clone() const override;
};

template <typename T> class PrependGenerator : public Generator<T> {
    private:
        LazySequence<T> *source;
        T item;

    public:
        PrependGenerator(const LazySequence<T> &source, const T &item);
        ~PrependGenerator();

        T generate(int index, const LazyCache<T> &materialized) override;
        T generate(const Ordinal &index, const LazyCache<T> &materialized) override;
        Generator<T> *clone() const override;
};

template <typename T> class InsertGenerator : public Generator<T> {
    private:
        LazySequence<T> *source;
        Ordinal insert_index;
        T item;
        bool is_index_shifted_by_insert(const Ordinal &index) const;
        Ordinal get_source_index_after_insert(const Ordinal &index) const;

    public:
        InsertGenerator(const LazySequence<T> &source, const T &item,
            const Ordinal &insert_index);
        ~InsertGenerator();

        T generate(int index, const LazyCache<T> &materialized) override;
        T generate(const Ordinal &index, const LazyCache<T> &materialized) override;
        Generator<T> *clone() const override;
};

template <typename T> class RemoveGenerator : public Generator<T> {
    private:
        LazySequence<T> *source;
        Ordinal remove_index;
        bool is_index_shifted_by_remove(const Ordinal &index) const;
        Ordinal get_source_index_after_remove(const Ordinal &index) const;

    public:
        RemoveGenerator(const LazySequence<T> &source, const Ordinal &remove_index);
        ~RemoveGenerator();

        T generate(int index, const LazyCache<T> &materialized) override;
        T generate(const Ordinal &index, const LazyCache<T> &materialized) override;
        Generator<T> *clone() const override;
};

template <typename T> class ConcatGenerator : public Generator<T> {
    private:
        LazySequence<T> *left;
        LazySequence<T> *right;
        Ordinal left_length;

    public:
        ConcatGenerator(const LazySequence<T> &left, const LazySequence<T> &right);
        ~ConcatGenerator();

        T generate(int index, const LazyCache<T> &materialized) override;
        T generate(const Ordinal &index, const LazyCache<T> &materialized) override;
        Generator<T> *clone() const override;
};

template <typename T> class MapGenerator : public Generator<T> {
    private:
        LazySequence<T> *source;
        T (*function)(const T &item);

    public:
        MapGenerator(const LazySequence<T> &source, T (*function)(const T &item));
        ~MapGenerator();

        T generate(int index, const LazyCache<T> &materialized) override;
        T generate(const Ordinal &index, const LazyCache<T> &materialized) override;
        Generator<T> *clone() const override;
};

template <typename T> class WhereGenerator : public Generator<T> {
    private:
        LazySequence<T> *source;
        bool (*predicate)(const T &item);
        Ordinal start_index;
        Ordinal cursor;
        bool has_next() const;
        void reset_to(const Ordinal &index);
        T get_next();

    public:
        WhereGenerator(const LazySequence<T> &source, bool (*predicate)(const T &item),
            const Ordinal &start_index = Ordinal::zero());
        ~WhereGenerator();

        T generate(int index, const LazyCache<T> &materialized) override;
        T generate(const Ordinal &index, const LazyCache<T> &materialized) override;
        Generator<T> *clone() const override;
};
