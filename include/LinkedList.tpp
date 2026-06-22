#pragma once

#include <stdexcept>

template <typename T> LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), length(0) {}

template <typename T>
LinkedList<T>::LinkedList(const T *items, int count) : head(nullptr), tail(nullptr), length(0) {
    if (count < 0) {
        throw std::invalid_argument("Count cannot be negative");
    }

    if (count > 0 && items == nullptr) {
        throw std::invalid_argument("Items cannot be null");
    }

    for (int index = 0; index < count; index++) {
        append(items[index]);
    }
}

template <typename T>
LinkedList<T>::LinkedList(const LinkedList<T>& other) : head(nullptr), tail(nullptr), length(0) {
    for (Node *current = other.head; current != nullptr; current = current->next) {
        append(current -> data);
    }
}

template <typename T> LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& other) {
    if (this == &other) {
        return *this;
    }

    Node *current = head;
    while (current != nullptr) {
        Node *tmp = current;
        current = current->next;
        delete tmp;
    }

    head = nullptr;
    tail = nullptr;
    length = 0;

    Node *other_current = other.head;
    while (other_current != nullptr) {
        append(other_current->data);
        other_current = other_current -> next;
    }

    return *this;
}

template <typename T> int LinkedList<T>::get_length() const {
    return length;
}

template <typename T> void LinkedList<T>::append(const T& item) {
    Node *new_node = new Node{item, nullptr};
    if (tail == nullptr) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
    length++;
}

template <typename T> void LinkedList<T>::prepend(const T& item) {
    Node *new_node = new Node{item, head};
    head = new_node;
    if (tail == nullptr) {
        tail = new_node;
    }
    length++;
}

template <typename T> void LinkedList<T>::insert(const T& item, int index) {
    if (index < 0 || index > length) {
        throw std::out_of_range("Index out of range");
    }
    if (index == 0) {
        prepend(item);
        return;
    }
    if (index == length) {
        append(item);
        return;
    }

    Node *current = head;

    for (int i = 0; i < index-1; i++) {
            current = current->next;
    }

    Node *new_node = new Node{item, current->next};
    current->next = new_node;

    length++;
}

template <typename T> const T &LinkedList<T>::get(int index) const {
    if (index < 0 || index >= length) {
        throw std::out_of_range("Index out of range");
    }
    Node *current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

template <typename T> LinkedList<T>* LinkedList<T>::concat(LinkedList<T> &other) {
    if (&other == this) {
        throw std::invalid_argument("Cannot concat list with itself");
    }

    if (other.head == nullptr) {
        return this;
    }

    if (head == nullptr) {
        head = other.head;
        tail = other.tail;
        length = other.length;

        other.head = nullptr;
        other.tail = nullptr;
        other.length = 0;
        return this;
    }

    tail->next = other.head;
    tail = other.tail;
    length += other.length;

    other.head = nullptr;
    other.tail = nullptr;
    other.length = 0;
    return this;
}

template <typename T> LinkedList<T> LinkedList<T>::get_sublist(int start_index, int end_index) const {
    if (start_index < 0 || end_index >= length || start_index > end_index ) {
        throw std::out_of_range("Index out of range");
    }
    LinkedList<T> sublist;
    Node *current = head;
    for (int i = 0; i < start_index; i++) {
        current = current->next;
    }
    for (int i = start_index; i<=end_index; i++) {
        sublist.append(current->data);
        current = current->next;
    }
    return sublist;
}

template <typename T> LinkedList<T>::~LinkedList() {
    Node *current = head;
    while (current != nullptr) {
        Node *tmp = current;
        current = current -> next;
        delete tmp;
    }
}

template <typename T> void LinkedList<T>::remove(int index) {
    if (index < 0 || index >= length) {
        throw std::out_of_range("Index out of range");
    }
    if (index == 0) {
        Node *tmp = head;
        head = head->next;
        delete tmp;
        if (head == nullptr) {
            tail = nullptr;
        }
        length--;
        return;
    }
    Node *current = head;
    for (int i = 0; i < index-1; i++) {
        current = current->next;
    }
    Node *tmp = current->next;
    current->next = tmp->next;
    if (tmp == tail) {
        tail = current;
    }
    delete tmp;
    length--;
}   

template <typename T> const T &LinkedList<T>::operator[](int index) const {
    return get(index);
}
