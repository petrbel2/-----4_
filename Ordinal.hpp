#pragma once

#include <stdexcept>

class Ordinal {
    private:
        int omega_count;
        int finite_part;
    
    public:
        Ordinal() : omega_count(0), finite_part(0) {}
        Ordinal(int omega_count, int finite_count) : omega_count(omega_count), finite_part(finite_count) {
            if (finite_count < 0 || omega_count < 0) {
                throw std::invalid_argument("Ordinal value cannot be negative");
            }
        }

        static Ordinal finite(int value) {
            return Ordinal(0, value);
        }

        static Ordinal infinity() {
            return Ordinal(1, 0);
        }

        static Ordinal omega_times(int k) { 
            return Ordinal(k, 0); 
        }
        
        static Ordinal zero() { 
            return Ordinal(0, 0); 
        }

        int is_infinite() const {
            return omega_count > 0;
        }

        int is_finite() const {
            return omega_count == 0;
        }

        int get_omega_count() const {
            return omega_count; 
        }

        int get_finite_part() const {
            return finite_part; 
        }

        int get_value() const {
            if (omega_count) {
                throw std::logic_error("Infinite Ordinal does not have finite value");
            }
            return finite_part;
        }

        int value() const {
            return get_value();
        }

        bool operator==(const Ordinal &other) const {
            return omega_count == other.omega_count && 
                    (finite_part == other.finite_part);
        }

        bool operator!=(const Ordinal &other) const {
            return !(*this == other);
        }

        bool operator<(const Ordinal &other) const {
            if (omega_count != other.omega_count) {
                return omega_count < other.omega_count;
            }
            return finite_part < other.finite_part;
        }

        bool operator<=(const Ordinal &other) const {
            return *this < other || *this == other;
        }

        bool operator>(const Ordinal &other) const {
            return other < *this;
        }

        bool operator>=(const Ordinal &other) const {
            return other <= *this;
        }

        Ordinal operator+(const Ordinal &other) const {
            if (other.omega_count > 0) {
                return Ordinal(omega_count + other.omega_count, other.finite_part);
            }
            return Ordinal(omega_count, finite_part + other.finite_part);
        }


        Ordinal operator-(const Ordinal& other) const {
            if (omega_count == 0 && other.omega_count > 0) {
                throw std::logic_error("finite Ordinal - infinite Ordinal will give negative value");
            }

            if (omega_count == other.omega_count) {
                if (finite_part < other.finite_part) {
                    throw std::logic_error("Ordinal subtraction underflow");
                }

                return Ordinal(0, finite_part - other.finite_part);
            }

            if (omega_count > other.omega_count) {

                return Ordinal(omega_count - other.omega_count, finite_part);
            }

            throw std::logic_error("Omega underflow");
        }
};
