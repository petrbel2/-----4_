#pragma once

#include "Sequence.h"
#include "DynamicArray.h"
#include "IEnumerator.h"
#include <stdexcept>

template <typename T> class ArraySequence : public Sequence<T> {
    protected:
        DynamicArray<T> items;
        int count;

        virtual ArraySequence<T> *Instance() override = 0;
        virtual ArraySequence<T> *Clone() const override = 0;

        void append_to_current(const T &item) override;
        void reserve(int capacity) override;
        void reallocate(int required);

    public:
        virtual ArraySequence<T> *Empty() const override = 0;

        ArraySequence();
        ArraySequence(const T *items, int count);
        ArraySequence(ArraySequence<T> &other);
        ArraySequence(const ArraySequence<T> &other);
        ArraySequence<T> &operator=(const ArraySequence<T> &other);

        const T& get_first() const override;
        const T& get_last() const override;
        const T& get(int index) const override;

        Option<T> try_get_first() const override;
        Option<T> try_get_last() const override;
        Option<T> try_get(int index) const override;

        int get_length() const override;

        Sequence<T> *append(const T& item) override;
        Sequence<T> *prepend(const T& item) override;
        Sequence<T> *insert(const T& item, int index) override;
        Sequence<T> *remove(int index) override;

        class Enumerator : public IEnumerator<T> {
            private:
                    const DynamicArray<T> &sequence;
                    int current_index;
                    int count; // Сколько считаем
            public:
                Enumerator(const DynamicArray<T> &array, int count) : sequence(array), current_index(-1), count(count) {}

                bool move_next() override {
                    if (current_index + 1 < count) {
                        current_index++;
                        return true;
                    }

                    current_index = count;
                    return false;
                }

                const T &get_curr() const override {
                    if (current_index < 0 || current_index >= count) {
                        throw std::out_of_range("Enumerator is not positioned on an element");
                    }

                    return sequence.get(current_index);
                }

                bool reset() override {
                    current_index = -1;
                    return true;
                }
        };

        IEnumerator<T> *get_enumerator() const override {
            return new Enumerator(items, count);
        }
};

template <typename T> class MutableArraySequence : public ArraySequence<T> {
    protected:
        ArraySequence<T> *Instance() override {
            return this;
        }

        ArraySequence<T> *Clone() const override {
            return new MutableArraySequence<T>(*this);
        }

    public:
        ArraySequence<T> *Empty() const override {
            return new MutableArraySequence<T>();
        }

        MutableArraySequence() : ArraySequence<T>() {};
        MutableArraySequence(const T *items, int count) : ArraySequence<T>(items, count) {};
        MutableArraySequence(const ArraySequence<T> &other) : ArraySequence<T>(other) {};

        void set(int index, const T &value);
};

template <typename T> class ImmutableArraySequence : public ArraySequence<T> {
    protected:
        ArraySequence<T> *Instance() override {
            return Clone();
        }

        ArraySequence<T> *Clone() const override {
            return new ImmutableArraySequence<T>(*this);
        }

    public:
        ArraySequence<T> *Empty() const override {
            return new ImmutableArraySequence<T>();
        }

        ImmutableArraySequence() : ArraySequence<T>() {};
        ImmutableArraySequence(const T *items, int count) : ArraySequence<T>(items, count) {};
        ImmutableArraySequence(const ArraySequence<T> &other) : ArraySequence<T>(other) {};
};

#include "ArraySequence.tpp"
