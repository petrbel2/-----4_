#pragma once

template <typename T> class IEnumerator {
    public:
        virtual ~IEnumerator() {};
        virtual bool move_next() = 0;
        virtual const T &get_curr() const = 0;
        virtual bool reset() = 0;
};
