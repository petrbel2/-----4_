#pragma once

template <typename T> class ListSequence;

template <typename T> class LinkedList {
    private:
        friend class ListSequence<T>;

        struct Node {
            T data;
            Node *next;
        };

        Node *head;
        Node *tail;
        int length;
    public:
        LinkedList();

        LinkedList(const LinkedList<T>& other);
        LinkedList(const T *items, int count);

        LinkedList<T>& operator=(const LinkedList<T>& other);

        ~LinkedList();

        const T &get(int index) const;
        const T &operator[](int index) const;

        int get_length() const;
        
        void append(const T &item);
        void prepend(const T &item);
        void insert(const T &item, int index);
        void remove(int index);

        LinkedList<T> get_sublist(int start_index, int end_index) const;
        LinkedList<T>* concat(LinkedList<T> &other);

};

#include "LinkedList.tpp"
