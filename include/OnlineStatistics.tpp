#pragma once

template <typename T> OnlineStatistics<T>::OnlineStatistics()
    : count(0), sum(T()), min_value(T()), max_value(T()), has_items(false), sorted_items() {}

template <typename T> int OnlineStatistics<T>::find_insert_index(const T &value) const {
    int index = 0;
    while (index < sorted_items.get_length() && sorted_items.get(index) < value) {
        index++;
    }
    return index;
}

template <typename T> void OnlineStatistics<T>::add(const T &value) {
    if (!has_items) {
        min_value = value;
        max_value = value;
        has_items = true;
    } else {
        if (value < min_value) {
            min_value = value;
        }
        if (value > max_value) {
            max_value = value;
        }
    }

    sum = sum + value;
    count++;
    sorted_items.insert(value, find_insert_index(value));
}

template <typename T> int OnlineStatistics<T>::get_count() const {
    return count;
}

template <typename T> T OnlineStatistics<T>::get_sum() const {
    return sum;
}

template <typename T> double OnlineStatistics<T>::get_mean() const {
    if (!has_items) {
        throw std::logic_error("No values");
    }

    return static_cast<double>(sum) / static_cast<double>(count);
}

template <typename T> T OnlineStatistics<T>::get_min() const {
    if (!has_items) {
        throw std::logic_error("No values");
    }

    return min_value;
}

template <typename T> T OnlineStatistics<T>::get_max() const {
    if (!has_items) {
        throw std::logic_error("No values");
    }

    return max_value;
}

template <typename T> double OnlineStatistics<T>::get_median() const {
    if (!has_items) {
        throw std::logic_error("No values");
    }

    int middle = sorted_items.get_length() / 2;
    if (sorted_items.get_length() % 2 == 1) {
        return static_cast<double>(sorted_items.get(middle));
    }

    return (static_cast<double>(sorted_items.get(middle - 1)) +
            static_cast<double>(sorted_items.get(middle))) / 2.0;
}
