#pragma once

#include "IEnumerator.h"
#include "Option.h"

template <typename T> class Sequence {
    protected:
        virtual Sequence<T> *Instance() = 0;
        virtual Sequence<T> *Clone() const = 0;
        virtual void append_to_current(const T &item) = 0;
        virtual void reserve(int) {} 

    public:
        virtual ~Sequence() = default;

        virtual Sequence<T> *Empty() const = 0;

        virtual const T& get_first() const = 0;
        virtual const T& get_last() const = 0;
        virtual const T& get(int index) const = 0;

        virtual int get_length() const = 0;
        virtual Sequence<T>* get_subsequence(int start_index, int end_index) const;
        virtual Option<T> try_get_first() const = 0;
        virtual Option<T> try_get_last() const = 0;
        virtual Option<T> try_get(int index) const = 0;

        virtual Sequence<T>* append(const T& item) = 0;
        virtual Sequence<T>* prepend(const T& item) = 0;
        virtual Sequence<T>* insert(const T& item, int index) = 0;
        virtual Sequence<T>* remove(int index) = 0;

        virtual Sequence<T>* concat(const Sequence<T> &other);
        virtual Sequence<T>* map(T (*function)(const T& item));
        virtual Sequence<T>* where(bool (*predicate)(const T& item));
        virtual T reduce(T (*function)(const T& first, const T& second), const T& initial);

        virtual IEnumerator<T>* get_enumerator() const = 0;
};

#include "Sequence.tpp"
