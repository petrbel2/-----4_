#pragma once

#include <stdexcept>

template <typename T> class Option {
    private:
        T value;
        bool contains_value;

    public:
        Option() : contains_value(false) {}
        Option(T initial_value) : value(initial_value), contains_value(true) {}

        bool has_value() const {
            return contains_value;
        }

        const T& get_value() const {
            if (!contains_value) {
                throw std::runtime_error("Option does not have a value");
            }
            return value;
        }

        T& get_value() {
            if (!contains_value) {
                throw std::runtime_error("Option does not have a value");
            }
            return value;
        }

        const T& operator*() const {
            return get_value();
        }

        const T* operator->() const {
            return &get_value();
        }

        T& operator*() {
            return get_value();
        }

        T* operator->() {
            return &get_value();
        }

        void set_value(T new_value) {
            value = new_value;
            contains_value = true;
        }

        void reset() {
            contains_value = false;
        }
};
