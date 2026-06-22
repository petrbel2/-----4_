#include "include/ArraySequence.hpp"
#include "include/LazySequence.hpp"
#include "include/OnlineStatistics.hpp"
#include "include/Stream.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

int square_rule(int index) {
    return index * index;
}

int fibonacci_rule(int index, const LazyCache<int> &cache) {
    return cache.get(index - 1) + cache.get(index - 2);
}

int double_value(const int &value) {
    return value * 2;
}

bool is_even_value(const int &value) {
    return value % 2 == 0;
}

int sum_values(const int &left, const int &right) {
    return left + right;
}

int deserialize_int(const std::string &text) {
    return std::stoi(text);
}

void clear_input_line() {
    std::cin.clear();
    std::cin.ignore(10000, '\n');
}

void print_stats(const OnlineStatistics<int> &stats) {
    std::cout << "count=" << stats.get_count()
              << " sum=" << stats.get_sum()
              << " mean=" << std::fixed << std::setprecision(2) << stats.get_mean()
              << " min=" << stats.get_min()
              << " max=" << stats.get_max()
              << " median=" << stats.get_median() << '\n';
}

void process_stream(ReadOnlyStream<int> &stream) {
    OnlineStatistics<int> stats;
    stream.open();

    while (!stream.is_end_of_stream()) {
        int value = stream.read();
        stats.add(value);
        std::cout << "read " << value << " -> ";
        print_stats(stats);
    }

    stream.close();
}

void run_manual_mode() {
    int count = 0;
    std::cout << "How many values? ";
    std::cin >> count;

    if (!std::cin || count < 0) {
        throw std::invalid_argument("Count must be a non-negative integer");
    }

    MutableArraySequence<int> values;
    for (int index = 0; index < count; index++) {
        int value = 0;
        std::cout << "value[" << index << "] = ";
        std::cin >> value;
        if (!std::cin) {
            throw std::invalid_argument("Expected integer value");
        }
        values.append(value);
    }

    SequenceReadStream<int> stream(values);
    process_stream(stream);
}

void run_prepared_mode() {
    int prepared[] = {7, 1, 9, 3, 3, 10, -2, 8};
    MutableArraySequence<int> values(prepared, 8);
    SequenceReadStream<int> stream(values);
    process_stream(stream);
}

int read_generated_count() {
    int count = 0;
    std::cout << "How many generated values? ";
    std::cin >> count;

    if (!std::cin || count < 0) {
        throw std::invalid_argument("Count must be a non-negative integer");
    }

    return count;
}

void process_lazy_sequence(LazySequence<int> &generated) {
    LazySequenceReadStream<int> stream(generated);
    process_stream(stream);
}

void run_arithmetic_generator() {
    int count = read_generated_count();
    LazySequence<int> generated(new ArithmeticIntGenerator(10, 5), Ordinal::finite(count));
    process_lazy_sequence(generated);
}

void run_square_generator() {
    int count = read_generated_count();
    LazySequence<int> generated(square_rule, Ordinal::finite(count));
    process_lazy_sequence(generated);
}

void run_fibonacci_generator() {
    int count = read_generated_count();
    if (count == 0) {
        LazySequence<int> generated;
        process_lazy_sequence(generated);
        return;
    }
    if (count == 1) {
        int first[] = {0};
        MutableArraySequence<int> initial(first, 1);
        LazySequence<int> generated(initial);
        process_lazy_sequence(generated);
        return;
    }

    int first_two[] = {0, 1};
    MutableArraySequence<int> initial(first_two, 2);
    LazySequence<int> generated(initial, fibonacci_rule, Ordinal::finite(count));
    process_lazy_sequence(generated);
}

void run_generated_mode() {
    while (true) {
        std::cout << "\n1 - arithmetic progression\n"
                  << "2 - squares\n"
                  << "3 - Fibonacci numbers\n"
                  << "0 - back\n"
                  << "Generator: ";

        int choice = 0;
        std::cin >> choice;

        if (!std::cin) {
            clear_input_line();
            std::cout << "Enter a number from the menu.\n";
            continue;
        }

        if (choice == 0) {
            return;
        }
        if (choice == 1) {
            run_arithmetic_generator();
            return;
        }
        if (choice == 2) {
            run_square_generator();
            return;
        }
        if (choice == 3) {
            run_fibonacci_generator();
            return;
        }

        std::cout << "Unknown generator.\n";
    }
}

void run_file_stream_mode() {
    std::string path;
    std::cout << "File path with one integer per line: ";
    clear_input_line();
    std::getline(std::cin, path);

    if (path.empty()) {
        throw std::invalid_argument("File path cannot be empty");
    }

    FileReadStream<int> stream(path, deserialize_int);
    process_stream(stream);
}

int read_int(const char *prompt) {
    int value = 0;
    std::cout << prompt;
    std::cin >> value;

    if (!std::cin) {
        clear_input_line();
        throw std::invalid_argument("Expected integer value");
    }

    return value;
}

MutableArraySequence<int> read_array_sequence() {
    int count = read_int("How many values? ");
    if (count < 0) {
        throw std::invalid_argument("Count must be a non-negative integer");
    }

    MutableArraySequence<int> values;
    for (int index = 0; index < count; index++) {
        std::cout << "value[" << index << "] = ";
        int value = 0;
        std::cin >> value;
        if (!std::cin) {
            throw std::invalid_argument("Expected integer value");
        }
        values.append(value);
    }

    return values;
}

LazySequence<int> *read_lazy_sequence() {
    std::cout << "\nCreate LazySequence\n"
              << "1 - entered finite sequence\n"
              << "2 - finite arithmetic progression\n"
              << "3 - infinite arithmetic progression\n"
              << "4 - finite squares\n"
              << "5 - infinite squares\n"
              << "6 - finite Fibonacci numbers\n"
              << "7 - infinite Fibonacci numbers\n"
              << "Choice: ";

    int choice = 0;
    std::cin >> choice;
    if (!std::cin) {
        clear_input_line();
        throw std::invalid_argument("Expected menu item number");
    }

    if (choice == 1) {
        MutableArraySequence<int> values = read_array_sequence();
        return new LazySequence<int>(values);
    }
    if (choice == 2) {
        int count = read_int("How many values? ");
        if (count < 0) {
            throw std::invalid_argument("Count must be a non-negative integer");
        }
        return new LazySequence<int>(new ArithmeticIntGenerator(10, 5), Ordinal::finite(count));
    }
    if (choice == 3) {
        return new LazySequence<int>(new ArithmeticIntGenerator(10, 5));
    }
    if (choice == 4) {
        int count = read_int("How many values? ");
        if (count < 0) {
            throw std::invalid_argument("Count must be a non-negative integer");
        }
        return new LazySequence<int>(square_rule, Ordinal::finite(count));
    }
    if (choice == 5) {
        return new LazySequence<int>(square_rule);
    }
    if (choice == 6 || choice == 7) {
        Ordinal length = Ordinal::infinity();
        if (choice == 6) {
            int count = read_int("How many values? ");
            if (count < 0) {
                throw std::invalid_argument("Count must be a non-negative integer");
            }
            if (count == 0) {
                return new LazySequence<int>();
            }
            if (count == 1) {
                int first[] = {0};
                MutableArraySequence<int> initial(first, 1);
                return new LazySequence<int>(initial);
            }
            length = Ordinal::finite(count);
        }

        int first_two[] = {0, 1};
        MutableArraySequence<int> initial(first_two, 2);
        return new LazySequence<int>(initial, fibonacci_rule, length);
    }

    throw std::invalid_argument("Unknown LazySequence kind");
}

void print_lazy_prefix(LazySequence<int> &sequence, int requested_count) {
    if (requested_count < 0) {
        throw std::invalid_argument("Count must be a non-negative integer");
    }

    int count = requested_count;
    if (sequence.get_length().is_finite() && count > sequence.get_length().value()) {
        count = sequence.get_length().value();
    }

    std::cout << "first " << count << " values=[";
    for (int index = 0; index < count; index++) {
        if (index > 0) {
            std::cout << ", ";
        }
        std::cout << sequence.get(index);
    }
    std::cout << "] materialized=" << sequence.get_materialized_count() << '\n';
}

LazySequence<int> *read_entered_finite_lazy_sequence() {
    MutableArraySequence<int> values = read_array_sequence();
    return new LazySequence<int>(values);
}

void print_lazy_sequence(LazySequence<int> &sequence) {
    if (sequence.get_length().is_infinite()) {
        std::cout << "length=infinity materialized="
                  << sequence.get_materialized_count() << '\n';
        return;
    }

    int length = sequence.get_length().value();
    std::cout << "length=" << length
              << " materialized=" << sequence.get_materialized_count()
              << " values=[";

    for (int index = 0; index < length; index++) {
        if (index > 0) {
            std::cout << ", ";
        }
        std::cout << sequence.get(index);
    }

    std::cout << "]\n";
}

void replace_lazy_sequence(LazySequence<int> *&current, LazySequence<int> *replacement) {
    delete current;
    current = replacement;
    print_lazy_sequence(*current);
}

void run_lazy_sequence_manual_mode() {
    std::cout << "Create initial LazySequence\n";
    LazySequence<int> *current = read_lazy_sequence();
    print_lazy_sequence(*current);

    while (true) {
        std::cout << "\nLazySequence operations\n"
                  << "1 - show current sequence\n"
                  << "2 - show first N values\n"
                  << "3 - get element by index\n"
                  << "4 - replace by subsequence\n"
                  << "5 - take first N values\n"
                  << "6 - append value (finite)\n"
                  << "7 - prepend value (finite)\n"
                  << "8 - insert value (finite)\n"
                  << "9 - remove by index (finite)\n"
                  << "10 - concat with finite entered sequence (finite)\n"
                  << "11 - map: multiply by 2 (finite)\n"
                  << "12 - where: keep even values (finite)\n"
                  << "13 - reduce: sum values (finite)\n"
                  << "14 - show cache info\n"
                  << "15 - recreate sequence\n"
                  << "0 - back\n"
                  << "Operation: ";

        int choice = 0;
        std::cin >> choice;

        if (!std::cin) {
            clear_input_line();
            std::cout << "Enter a number from the menu.\n";
            continue;
        }

        try {
            if (choice == 0) {
                delete current;
                return;
            }
            if (choice == 1) {
                print_lazy_sequence(*current);
            } else if (choice == 2) {
                int count = read_int("How many values to show? ");
                print_lazy_prefix(*current, count);
            } else if (choice == 3) {
                int index = read_int("Index: ");
                std::cout << "value=" << current->get(index) << '\n';
            } else if (choice == 4) {
                int start = read_int("Start index: ");
                int end = read_int("End index: ");
                replace_lazy_sequence(current, current->get_subsequence(start, end));
            } else if (choice == 5) {
                int count = read_int("How many values to take? ");
                replace_lazy_sequence(current, current->take(count));
            } else if (choice == 6) {
                int value = read_int("Value: ");
                replace_lazy_sequence(current, current->append(value));
            } else if (choice == 7) {
                int value = read_int("Value: ");
                replace_lazy_sequence(current, current->prepend(value));
            } else if (choice == 8) {
                int value = read_int("Value: ");
                int index = read_int("Index: ");
                replace_lazy_sequence(current, current->insert(value, index));
            } else if (choice == 9) {
                int index = read_int("Index: ");
                replace_lazy_sequence(current, current->remove(index));
            } else if (choice == 10) {
                std::cout << "Second sequence\n";
                LazySequence<int> *other = read_entered_finite_lazy_sequence();
                try {
                    LazySequence<int> *replacement = current->concat(*other);
                    delete other;
                    replace_lazy_sequence(current, replacement);
                } catch (...) {
                    delete other;
                    throw;
                }
            } else if (choice == 11) {
                replace_lazy_sequence(current, current->map(double_value));
            } else if (choice == 12) {
                replace_lazy_sequence(current, current->where(is_even_value));
            } else if (choice == 13) {
                std::cout << "sum=" << current->reduce(sum_values, 0) << '\n';
            } else if (choice == 14) {
                std::cout << "materialized=" << current->get_materialized_count()
                          << " cache_limit=" << current->get_cache_limit()
                          << " cache_start_index=" << current->get_cache_start_index()
                          << '\n';
            } else if (choice == 15) {
                replace_lazy_sequence(current, read_lazy_sequence());
            } else {
                std::cout << "Unknown operation.\n";
            }
        } catch (const std::exception &error) {
            std::cout << "Error: " << error.what() << '\n';
        }
    }
}

int main() {
    std::cout << "Online stream statistics\n";

    while (true) {
        std::cout << "\n1 - manual input\n"
                  << "2 - prepared data\n"
                  << "3 - generated lazy stream\n"
                  << "4 - file data stream\n"
                  << "5 - manual LazySequence operations\n"
                  << "0 - exit\n"
                  << "Choice: ";

        int choice = 0;
        std::cin >> choice;

        if (!std::cin) {
            clear_input_line();
            std::cout << "Enter a number from the menu.\n";
            continue;
        }

        try {
            if (choice == 0) {
                break;
            }
            if (choice == 1) {
                run_manual_mode();
            } else if (choice == 2) {
                run_prepared_mode();
            } else if (choice == 3) {
                run_generated_mode();
            } else if (choice == 4) {
                run_file_stream_mode();
            } else if (choice == 5) {
                run_lazy_sequence_manual_mode();
            } else {
                std::cout << "Unknown menu item.\n";
            }
        } catch (const std::exception &error) {
            std::cout << "Error: " << error.what() << '\n';
        }
    }

    return 0;
}
