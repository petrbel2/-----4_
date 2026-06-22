#pragma once

#include "Sequence.hpp"
#include "DynamicArray.hpp"
#include "IEnumerator.hpp"
#include <stdexcept>

template <typename T> class DynamicSequence : public Sequence<T> {
    protected:
        DynamicArray<T> items;
        int count;

        virtual DynamicSequence<T> *Instance() override = 0;
        virtual DynamicSequence<T> *Clone() const override = 0;

        void append_to_current(const T &item) override;
        void reserve(int capacity) override;
        void reallocate(int required);

    public:
        virtual DynamicSequence<T> *Empty() const override = 0;

        DynamicSequence() : items(), count(0) {}

        DynamicSequence(const T *items, int count)
            : items(items, count), count(count) {}

        DynamicSequence(DynamicSequence<T> &other)
            : items(other.items), count(other.count) {}

        DynamicSequence(const DynamicSequence<T> &other)
            : items(other.items), count(other.count) {}

        DynamicSequence<T> &operator=(const DynamicSequence<T> &other) {
            if (this == &other) {
                return *this;
            }
            items = other.items;
            count = other.count;
            return *this;
            }

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

template <typename T> class MutableDynamicSequence : public DynamicSequence<T> {
    protected:
        DynamicSequence<T> *Instance() override {
            return this;
        }

        DynamicSequence<T> *Clone() const override {
            return new MutableDynamicSequence<T>(*this);
        }

    public:
        DynamicSequence<T> *Empty() const override {
            return new MutableDynamicSequence<T>();
        }

        MutableDynamicSequence() : DynamicSequence<T>() {};
        MutableDynamicSequence(const T *items, int count) : DynamicSequence<T>(items, count) {};
        MutableDynamicSequence(const DynamicSequence<T> &other) : DynamicSequence<T>(other) {};

        void set(int index, const T &value);
};

template <typename T> class ImmutableDynamicSequence : public DynamicSequence<T> {
    protected:
        DynamicSequence<T> *Instance() override {
            return Clone();
        }

        DynamicSequence<T> *Clone() const override {
            return new ImmutableDynamicSequence<T>(*this);
        }

    public:
        DynamicSequence<T> *Empty() const override {
            return new ImmutableDynamicSequence<T>();
        }

        ImmutableDynamicSequence() : DynamicSequence<T>() {};
        ImmutableDynamicSequence(const T *items, int count) : DynamicSequence<T>(items, count) {};
        ImmutableDynamicSequence(const DynamicSequence<T> &other) : DynamicSequence<T>(other) {};
};

#include "DynamicSequence.tpp"
