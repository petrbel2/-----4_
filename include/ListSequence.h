#pragma once

#include "Sequence.h"
#include "LinkedList.h"

template <typename T> class ListSequence : public Sequence<T> {
    protected:
        LinkedList<T> items;

        virtual ListSequence<T> *Instance() override = 0;
        virtual ListSequence<T> *Clone() const override = 0;

        void append_to_current(const T &item) override;

    public:
        virtual ListSequence<T> *Empty() const override = 0;

        ListSequence();
        ListSequence(const T *items, int count);
        ListSequence(const ListSequence<T> &other);

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

        IEnumerator<T> *get_enumerator() const override {
            return new Enumerator(items);
        }

        class Enumerator : public IEnumerator<T> {
            private:
                const LinkedList<T> &sequence;
                const typename LinkedList<T>::Node *current;
                const typename LinkedList<T>::Node *next;

            public:
                Enumerator(const LinkedList<T> &linked_list) : sequence(linked_list), current(nullptr), next(linked_list.head) {}

                bool move_next() override {
                    if (next == nullptr) {
                        current = nullptr;
                        return false;
                    }

                    current = next;
                    next = next->next;
                    return true;
                }

                const T &get_curr() const override {
                    if (current == nullptr) {
                        throw std::out_of_range("Cannot display empty object");
                    }
                    return current->data;
                }

                bool reset() override {
                    current = nullptr;
                    next = sequence.head;
                    return true;
                }
        };
};

template <typename T> class MutableListSequence : public ListSequence<T> {
    protected:
        ListSequence<T> *Instance() override {
            return this;
        }

        ListSequence<T> *Clone() const override {
            return new MutableListSequence<T>(*this);
        }

    public:
        ListSequence<T> *Empty() const override {
            return new MutableListSequence<T>();
        }

        MutableListSequence() : ListSequence<T>() {};
        MutableListSequence(const T *items, int count) : ListSequence<T>(items, count) {};
        MutableListSequence(const ListSequence<T> &other) : ListSequence<T>(other) {};
};

template <typename T> class ImmutableListSequence : public ListSequence<T> {
    protected:
        ListSequence<T> *Instance() override {
            return Clone();
        }

        ListSequence<T> *Clone() const override {
            return new ImmutableListSequence<T>(*this);
        }

    public:
        ListSequence<T> *Empty() const override {
            return new ImmutableListSequence<T>();
        }

        ImmutableListSequence() : ListSequence<T>() {};
        ImmutableListSequence(const T *items, int count) : ListSequence<T>(items, count) {};
        ImmutableListSequence(const ListSequence<T> &other) : ListSequence<T>(other) {};
};

#include "ListSequence.tpp"
