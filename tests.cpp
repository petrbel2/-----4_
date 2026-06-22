#include "include/ArraySequence.h"
#include "include/LazySequence.h"
#include "include/OnlineStatistics.h"
#include "include/Stream.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

int tests_run = 0;

void require(bool condition, const std::string &message) {
    tests_run++;
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void require_near(double actual, double expected, const std::string &message) {
    require(std::fabs(actual - expected) < 1e-9, message);
}

#define REQUIRE_THROWS(ExceptionType, message, expression) \
    do {                                                   \
        tests_run++;                                       \
        bool was_exception = false;                        \
        try {                                              \
            expression;                                    \
        } catch (const ExceptionType &error) {             \
            was_exception = true;                          \
        }                                                  \
        if (!was_exception) {                              \
            throw std::runtime_error(message);             \
        }                                                  \
    } while (false)

int square_rule(int index) {
    return index * index;
}

int double_value(const int &value) {
    return value * 2;
}

bool is_even(const int &value) {
    return value % 2 == 0;
}

bool is_positive(const int &value) {
    return value > 0;
}

bool is_negative(const int &value) {
    return value < 0;
}

int add_values(const int &left, const int &right) {
    return left + right;
}

int fibonacci_rule(int index, const LazyCache<int> &cache) {
    return cache.get(index - 1) + cache.get(index - 2);
}

int deserialize_int(const std::string &text) {
    return std::stoi(text);
}

std::string serialize_int(const int &value) {
    return std::to_string(value);
}

void test_online_statistics() {
    OnlineStatistics<int> stats;

    require(stats.get_count() == 0, "empty statistics count");
    require(stats.get_sum() == 0, "empty statistics sum");
    REQUIRE_THROWS(std::logic_error, "empty mean throws", stats.get_mean());
    REQUIRE_THROWS(std::logic_error, "empty min throws", stats.get_min());
    REQUIRE_THROWS(std::logic_error, "empty max throws", stats.get_max());
    REQUIRE_THROWS(std::logic_error, "empty median throws", stats.get_median());

    stats.add(42);
    require(stats.get_count() == 1, "single statistics count");
    require(stats.get_sum() == 42, "single statistics sum");
    require(stats.get_min() == 42, "single statistics min");
    require(stats.get_max() == 42, "single statistics max");
    require_near(stats.get_mean(), 42.0, "single mean");
    require_near(stats.get_median(), 42.0, "single median");

    stats = OnlineStatistics<int>();
    int values[] = {5, 1, 9, 1, -3};
    for (int value : values) {
        stats.add(value);
    }

    require(stats.get_count() == 5, "statistics count");
    require(stats.get_sum() == 13, "statistics sum");
    require(stats.get_min() == -3, "statistics min");
    require(stats.get_max() == 9, "statistics max");
    require_near(stats.get_mean(), 2.6, "statistics mean");
    require_near(stats.get_median(), 1.0, "odd median");

    stats.add(100);
    require_near(stats.get_median(), 3.0, "even median");

    OnlineStatistics<double> doubles;
    doubles.add(1.5);
    doubles.add(-2.0);
    doubles.add(3.5);
    doubles.add(3.5);
    require(doubles.get_count() == 4, "double statistics count");
    require_near(doubles.get_sum(), 6.5, "double sum");
    require_near(doubles.get_mean(), 1.625, "double mean");
    require_near(doubles.get_median(), 2.5, "double median");
    require_near(doubles.get_min(), -2.0, "double min");
    require_near(doubles.get_max(), 3.5, "double max");
}

void test_sequence_read_stream() {
    int raw[] = {4, 8, 15};
    MutableArraySequence<int> sequence(raw, 3);
    SequenceReadStream<int> stream(sequence);

    require(stream.is_can_go_to_index(), "sequence read can seek");
    require(stream.is_can_go_back(), "sequence read can go back");
    REQUIRE_THROWS(std::logic_error, "end before open throws", stream.is_end_of_stream());
    REQUIRE_THROWS(std::logic_error, "read before open throws", stream.read());
    REQUIRE_THROWS(std::logic_error, "seek before open throws", stream.go_to_index(0));

    stream.open();
    require(stream.get_position() == 0, "initial stream position");
    require(!stream.is_end_of_stream(), "stream is not ended at start");
    require(stream.go_forward() == 1, "sequence read go forward");
    require(stream.read() == 8, "read after go forward");
    require(stream.go_back() == 1, "sequence read go back");
    require(stream.read() == 8, "read reread after go back");
    require(stream.go_to_index(0) == 0, "seek to start");
    require(stream.read() == 4, "read after seek to start");
    require(stream.go_to_index(2) == 2, "seek to last index");
    require(stream.read() == 15, "read after seek");
    require(stream.is_end_of_stream(), "stream ended");
    require(stream.go_to_index(3) == 3, "seek to end");
    REQUIRE_THROWS(std::out_of_range, "read past end throws", stream.read());
    REQUIRE_THROWS(std::out_of_range, "seek past end throws", stream.go_to_index(4));
    REQUIRE_THROWS(std::out_of_range, "seek negative throws", stream.go_to_index(-1));
    stream.close();
    REQUIRE_THROWS(std::logic_error, "read after close throws", stream.read());
}

void test_sequence_write_stream() {
    MutableArraySequence<int> sequence;
    SequenceWriteStream<int> stream(sequence);

    require(stream.is_can_go_to_index(), "sequence write can seek");
    require(stream.is_can_go_back(), "sequence write can go back");
    REQUIRE_THROWS(std::logic_error, "write before open throws", stream.write(1));
    REQUIRE_THROWS(std::logic_error, "write seek before open throws", stream.go_to_index(0));

    stream.open();
    require(stream.get_position() == 0, "write stream starts at end of empty sequence");
    REQUIRE_THROWS(std::out_of_range, "write go forward beyond end throws", stream.go_forward());
    require(stream.write(10) == 1, "first write position");
    require(stream.write(20) == 2, "second write position");
    require(stream.go_back() == 1, "write go back");
    stream.go_to_index(1);
    require(stream.write(30) == 2, "overwrite position");
    REQUIRE_THROWS(std::out_of_range, "write seek past end throws", stream.go_to_index(3));
    REQUIRE_THROWS(std::out_of_range, "write seek negative throws", stream.go_to_index(-1));
    stream.close();
    REQUIRE_THROWS(std::logic_error, "write after close throws", stream.write(40));

    require(sequence.get_length() == 2, "written sequence length");
    require(sequence.get(0) == 10, "first written value");
    require(sequence.get(1) == 30, "overwritten value");
}

void test_lazy_sequence() {
    LazySequence<int> squares(square_rule, Ordinal::finite(5));

    require(squares.get_length().is_finite(), "finite lazy length");
    require(squares.get_length().value() == 5, "lazy length value");
    require(squares.get(0) == 0, "lazy first value");
    require(squares.get(3) == 9, "lazy generated value");
    require(squares.get_materialized_count() == 4, "lazy materialized count");
    REQUIRE_THROWS(std::out_of_range, "lazy out of range throws", squares.get(5));

    LazySequence<int> *first_three = squares.take(3);
    require(first_three->get_length().value() == 3, "take length");
    require(first_three->get_last() == 4, "take last");

    LazySequence<int> *doubled = first_three->map(double_value);
    require(doubled->get(2) == 8, "lazy map");

    LazySequence<int> *filtered = doubled->where(is_even);
    require(filtered->get_length().value() == 3, "lazy where keeps evens");
    require(filtered->reduce(add_values, 0) == 10, "lazy reduce");

    LazySequence<int> *appended = first_three->append(99);
    require(appended->get_length().value() == 4, "lazy append length");
    require(appended->get(3) == 99, "lazy append value");

    LazySequence<int> *prepended = first_three->prepend(77);
    require(prepended->get(0) == 77, "lazy prepend value");
    require(prepended->get(3) == 4, "lazy prepend keeps old values");

    LazySequence<int> *inserted = first_three->insert(55, 1);
    require(inserted->get_length().value() == 4, "lazy insert length");
    require(inserted->get(1) == 55, "lazy insert value");
    require(inserted->get(2) == 1, "lazy insert shifts values");

    LazySequence<int> *removed = inserted->remove(1);
    require(removed->get_length().value() == 3, "lazy remove length");
    require(removed->get(1) == 1, "lazy remove value");

    LazySequence<int> *concatenated = first_three->concat(*removed);
    require(concatenated->get_length().value() == 6, "lazy concat length");
    require(concatenated->get(5) == 4, "lazy concat last value");

    delete concatenated;
    delete removed;
    delete inserted;
    delete prepended;
    delete appended;
    delete filtered;
    delete doubled;
    delete first_three;

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> fibonacci(initial, fibonacci_rule, Ordinal::finite(7));
    require(fibonacci.get(6) == 8, "lazy recurrent rule");
}

void test_lazy_sequence_accessors_and_boundaries() {
    LazySequence<int> empty;
    require(empty.get_length().is_finite(), "empty finite length");
    require(empty.get_length().value() == 0, "empty length value");
    require(empty.get_materialized_count() == 0, "empty materialized count");
    // require_no_value(empty.try_get_first(), "empty try_get_first");
    // require_no_value(empty.try_get_last(), "empty try_get_last");
    // require_no_value(empty.try_get(0), "empty try_get index");
    REQUIRE_THROWS(std::out_of_range, "empty get_first throws", empty.get_first());
    REQUIRE_THROWS(std::out_of_range, "empty get_last throws", empty.get_last());
    REQUIRE_THROWS(std::out_of_range, "empty get throws", empty.get(0));
    REQUIRE_THROWS(std::invalid_argument, "negative take throws", empty.take(-1));

    int raw[] = {3, 1, 4};
    MutableArraySequence<int> values(raw, 3);
    LazySequence<int> finite(values);
    require(finite.get_first() == 3, "finite get_first");
    require(finite.get_last() == 4, "finite get_last");
    // require_value(finite.try_get_first(), 3, "finite try_get_first");
    // require_value(finite.try_get_last(), 4, "finite try_get_last");
    // require_value(finite.try_get(1), 1, "finite try_get");
    // require_no_value(finite.try_get(-1), "finite try_get negative");
    // require_no_value(finite.try_get(3), "finite try_get out of range");
    require(finite.is_materialized(2), "finite sequence from source is materialized");

    LazySequence<int> *sub = finite.get_subsequence(1, 2);
    require(sub->get_length().value() == 2, "subsequence length");
    require(sub->get(0) == 1, "subsequence first");
    require(sub->get(1) == 4, "subsequence second");
    delete sub;
    REQUIRE_THROWS(std::out_of_range, "bad subsequence order", finite.get_subsequence(2, 1));
    REQUIRE_THROWS(std::out_of_range, "bad subsequence start", finite.get_subsequence(-1, 1));
    REQUIRE_THROWS(std::out_of_range, "bad subsequence end", finite.get_subsequence(0, 3));
    REQUIRE_THROWS(std::out_of_range, "take too many throws", finite.take(4));

    LazySequence<int> *empty_take = finite.take(0);
    require(empty_take->get_length().value() == 0, "take zero length");
    delete empty_take;

    LazySequence<int> *filtered_from_middle = finite.where(is_even, 2);
    require(filtered_from_middle->get_length().value() == 1, "where finite start length");
    require(filtered_from_middle->get(0) == 4, "where finite start value");
    delete filtered_from_middle;
    LazySequence<int> *filtered_without_matches = finite.where(is_negative);
    require(filtered_without_matches->get_length().value() == 3,
        "where finite no matches keeps source length limit");
    REQUIRE_THROWS(std::out_of_range, "where finite no matches get throws",
        filtered_without_matches->get(0));
    delete filtered_without_matches;
    REQUIRE_THROWS(std::out_of_range, "where finite start beyond end throws",
        finite.where(is_even, 4));

    LazySequence<int> *inserted_at_end = finite.insert(9, 3);
    require(inserted_at_end->get(3) == 9, "insert at end value");
    delete inserted_at_end;
    REQUIRE_THROWS(std::out_of_range, "insert negative throws", finite.insert(9, -1));
    REQUIRE_THROWS(std::out_of_range, "insert beyond end throws", finite.insert(9, 4));
    REQUIRE_THROWS(std::out_of_range, "remove negative throws", finite.remove(-1));
    REQUIRE_THROWS(std::out_of_range, "remove beyond end throws", finite.remove(3));

    int single_raw[] = {99};
    MutableArraySequence<int> single_values(single_raw, 1);
    LazySequence<int> single(single_values);
    LazySequence<int> *removed_single = single.remove(0);
    require(removed_single->get_length().value() == 0, "remove single leaves empty");
    delete removed_single;

    LazySequence<int> infinite(square_rule);
    require(infinite.get_length().is_infinite(), "infinite lazy length");
    require(infinite.get(6) == 36, "infinite lazy get");
    // require_no_value(infinite.try_get_last(), "infinite try_get_last");
    REQUIRE_THROWS(std::logic_error, "infinite get_last throws", infinite.get_last());
    LazySequence<int> *infinite_appended = infinite.append(99);
    require(infinite_appended->get_length().is_infinite(), "infinite append length is infinite");
    require(infinite_appended->get_length().get_finite_part() == 1, "infinite append finite tail");
    require(infinite_appended->get(5) == 25, "infinite append keeps finite values");
    require(infinite_appended->get(Ordinal::infinity()) == 99, "infinite append omega value");
    LazySequence<int> *infinite_appended_twice = infinite_appended->append(100);
    LazySequence<int> *omega_tail = infinite_appended_twice->get_subsequence(1, 0, 1, 1);
    require(omega_tail->get_length().value() == 2, "omega subsequence length");
    require(omega_tail->get(0) == 99, "omega subsequence first");
    require(omega_tail->get(1) == 100, "omega subsequence second");
    delete omega_tail;

    LazySequence<int> *finite_inserted_before_tail = infinite_appended->insert(60, 2);
    require(finite_inserted_before_tail->get_length() == Ordinal(1, 1),
        "finite insert before omega tail keeps length");
    require(finite_inserted_before_tail->get(2) == 60,
        "finite insert before omega tail value");
    require(finite_inserted_before_tail->get(3) == 4,
        "finite insert before omega tail shifts finite index");
    require(finite_inserted_before_tail->get(Ordinal::infinity()) == 99,
        "finite insert before omega tail keeps omega index");
    delete finite_inserted_before_tail;

    delete infinite_appended_twice;
    delete infinite_appended;

    LazySequence<int> *infinite_prepended = infinite.prepend(7);
    require(infinite_prepended->get_length() == Ordinal::infinity(), "infinite prepend length");
    require(infinite_prepended->get(0) == 7, "infinite prepend first");
    require(infinite_prepended->get(3) == 4, "infinite prepend shifts values");
    delete infinite_prepended;

    LazySequence<int> *infinite_inserted = infinite.insert(50, 2);
    require(infinite_inserted->get(0) == 0, "infinite insert keeps first");
    require(infinite_inserted->get(2) == 50, "infinite insert value");
    require(infinite_inserted->get(3) == 4, "infinite insert shifts");
    delete infinite_inserted;

    LazySequence<int> *infinite_tail_inserted = infinite.insert(70, Ordinal::infinity());
    require(infinite_tail_inserted->get_length() == Ordinal(1, 1),
        "infinite tail insert length");
    require(infinite_tail_inserted->get(4) == 16, "infinite tail insert keeps finite values");
    require(infinite_tail_inserted->get(Ordinal::infinity()) == 70,
        "infinite tail insert omega value");
    delete infinite_tail_inserted;

    LazySequence<int> *infinite_with_tail = infinite.append(99);
    LazySequence<int> *omega_inserted = infinite_with_tail->insert(80, Ordinal::infinity());
    require(omega_inserted->get_length() == Ordinal(1, 2), "omega insert length");
    require(omega_inserted->get(Ordinal::infinity()) == 80, "omega insert value");
    require(omega_inserted->get(1, 1) == 99, "omega insert shifts old tail");
    delete omega_inserted;
    delete infinite_with_tail;
    REQUIRE_THROWS(std::out_of_range, "ordinal insert beyond infinite length throws",
        infinite.insert(90, 1, 1));

    LazySequence<int> *infinite_removed = infinite.remove(1);
    require(infinite_removed->get(0) == 0, "infinite remove keeps first");
    require(infinite_removed->get(1) == 4, "infinite remove shifts");
    delete infinite_removed;

    LazySequence<int> *infinite_tail_source = infinite.append(99);
    LazySequence<int> *finite_removed_before_tail = infinite_tail_source->remove(2);
    require(finite_removed_before_tail->get_length() == Ordinal(1, 1),
        "finite remove before omega tail keeps length");
    require(finite_removed_before_tail->get(2) == 9,
        "finite remove before omega tail shifts finite index");
    require(finite_removed_before_tail->get(Ordinal::infinity()) == 99,
        "finite remove before omega tail keeps omega index");
    delete finite_removed_before_tail;

    LazySequence<int> *omega_removed = infinite_tail_source->remove(Ordinal::infinity());
    require(omega_removed->get_length() == Ordinal::infinity(), "omega remove length");
    require(omega_removed->get(4) == 16, "omega remove keeps finite values");
    delete omega_removed;
    delete infinite_tail_source;

    LazySequence<int> *infinite_mapped = infinite.map(double_value);
    require(infinite_mapped->get(4) == 32, "infinite map value");
    delete infinite_mapped;

    LazySequence<int> *infinite_filtered = infinite.where(is_even);
    require(infinite_filtered->get(0) == 0, "infinite where first");
    require(infinite_filtered->get(2) == 16, "infinite where later");
    delete infinite_filtered;
    REQUIRE_THROWS(std::logic_error, "infinite reduce throws", infinite.reduce(add_values, 0));

    LazySequence<int> *infinite_sub = infinite.get_subsequence(2, 4);
    require(infinite_sub->get_length().value() == 3, "infinite subsequence length");
    require(infinite_sub->get(0) == 4, "infinite subsequence first");
    require(infinite_sub->get(2) == 16, "infinite subsequence last");
    delete infinite_sub;
    LazySequence<int> *finite_window = infinite.get_subsequence(5, 8);
    require(finite_window->get_length().value() == 4, "finite window length");
    require(finite_window->get(0) == 25, "finite window first");
    require(finite_window->get(3) == 64, "finite window last");
    delete finite_window;
    LazySequence<int> *infinite_take = infinite.take(3);
    require(infinite_take->get(2) == 4, "infinite take value");
    delete infinite_take;

    LazySequence<int> cached(square_rule, Ordinal::infinity(), 2);
    require(cached.get_cache_limit() == 2, "cache limit");
    require(cached.get(0) == 0, "cached first value");
    require(cached.get(3) == 9, "cached later value");
    require(cached.get_materialized_count() == 2, "limited cache count");
    require(cached.get_cache_start_index() == 2, "limited cache start");
    require(!cached.is_materialized(0), "forgotten value not materialized");
    require(cached.is_materialized(3), "latest value materialized");
    REQUIRE_THROWS(std::out_of_range, "forgotten value throws", cached.get(0));
    REQUIRE_THROWS(std::invalid_argument, "negative cache limit throws", cached.set_cache_limit(-1));
}

void test_lazy_sequence_copy_assignment_and_enumerator() {
    LazySequence<int> original(square_rule, Ordinal::finite(5));
    require(original.get(2) == 4, "original generated before copy");

    LazySequence<int> copied(original);
    require(copied.get(4) == 16, "copy keeps generator");
    require(original.get(3) == 9, "original still usable after copy");

    LazySequence<int> assigned;
    assigned = copied;
    require(assigned.get(4) == 16, "assignment keeps generator");
    assigned = assigned;
    require(assigned.get(1) == 1, "self assignment keeps data");

    IEnumerator<int> *enumerator = copied.get_enumerator();
    REQUIRE_THROWS(std::out_of_range, "enumerator before first throws", enumerator->get_curr());
    require(enumerator->move_next(), "enumerator moves first");
    require(enumerator->get_curr() == 0, "enumerator first value");
    require(enumerator->move_next(), "enumerator moves second");
    require(enumerator->get_curr() == 1, "enumerator second value");
    require(enumerator->reset(), "enumerator reset");
    require(enumerator->move_next(), "enumerator moves after reset");
    require(enumerator->get_curr() == 0, "enumerator reset value");
    int count = 1;
    while (enumerator->move_next()) {
        count++;
    }
    require(count == 5, "enumerator iterates all values");
    require(!enumerator->move_next(), "enumerator remains ended");
    REQUIRE_THROWS(std::out_of_range, "enumerator after end throws", enumerator->get_curr());
    delete enumerator;

    LazySequence<int> empty;
    LazySequence<int> *mapped_empty = empty.map(double_value);
    LazySequence<int> *filtered_empty = empty.where(is_positive);
    require(mapped_empty->get_length().value() == 0, "map empty length");
    require(filtered_empty->get_length().value() == 0, "where empty length");
    delete filtered_empty;
    delete mapped_empty;
    require(empty.reduce(add_values, 5) == 5, "reduce empty returns initial");

    int left_raw[] = {1, 2};
    int right_raw[] = {3};
    MutableArraySequence<int> left_values(left_raw, 2);
    MutableArraySequence<int> right_values(right_raw, 1);
    LazySequence<int> left(left_values);
    LazySequence<int> right(right_values);
    LazySequence<int> *joined = left.concat(right);
    require(joined->get_length().value() == 3, "concat finite length");
    require(joined->get(2) == 3, "concat finite last");
    delete joined;

    LazySequence<int> infinite(square_rule);
    LazySequence<int> *finite_then_infinite = left.concat(infinite);
    require(finite_then_infinite->get_length() == Ordinal::infinity(),
        "finite plus infinite length");
    require(finite_then_infinite->get(0) == 1, "finite plus infinite first");
    require(finite_then_infinite->get(2) == 0, "finite plus infinite generated first");
    require(finite_then_infinite->get(4) == 4, "finite plus infinite generated later");
    delete finite_then_infinite;

    LazySequence<int> *infinite_then_finite = infinite.concat(left);
    require(infinite_then_finite->get_length().get_finite_part() == 2,
        "infinite plus finite tail length");
    require(infinite_then_finite->get(3) == 9, "infinite plus finite keeps finite index");
    require(infinite_then_finite->get(Ordinal::infinity()) == 1,
        "infinite plus finite omega first");
    require(infinite_then_finite->get(1, 1) == 2, "infinite plus finite omega second");
    delete infinite_then_finite;

    LazySequence<int> arithmetic(new ArithmeticIntGenerator(100, 1));
    LazySequence<int> *two_omegas = infinite.concat(arithmetic);
    require(two_omegas->get_length() == Ordinal::omega_times(2), "two omega concat length");
    LazySequence<int> *inserted_in_second_omega = two_omegas->insert(500, Ordinal::infinity());
    require(inserted_in_second_omega->get_length() == Ordinal::omega_times(2),
        "insert inside second omega keeps length");
    require(inserted_in_second_omega->get(Ordinal::infinity()) == 500,
        "insert inside second omega value");
    require(inserted_in_second_omega->get(1, 1) == 100,
        "insert inside second omega shifts old value");
    delete inserted_in_second_omega;
    LazySequence<int> *inserted_after_two_omegas = two_omegas->insert(600,
        Ordinal::omega_times(2));
    require(inserted_after_two_omegas->get_length() == Ordinal(2, 1),
        "insert after two omegas length");
    require(inserted_after_two_omegas->get(2, 0) == 600, "insert after two omegas value");
    delete inserted_after_two_omegas;
    LazySequence<int> *removed_in_second_omega = two_omegas->remove(Ordinal::infinity());
    require(removed_in_second_omega->get_length() == Ordinal::omega_times(2),
        "remove inside second omega keeps length");
    require(removed_in_second_omega->get(Ordinal::infinity()) == 101,
        "remove inside second omega shifts value");
    delete removed_in_second_omega;
    LazySequence<int> *two_omegas_with_tail = two_omegas->append(700);
    LazySequence<int> *removed_after_two_omegas = two_omegas_with_tail->remove(
        Ordinal::omega_times(2));
    require(removed_after_two_omegas->get_length() == Ordinal::omega_times(2),
        "remove after two omegas length");
    delete removed_after_two_omegas;
    delete two_omegas_with_tail;

    LazySequence<int> *filtered_two_omegas = two_omegas->where(is_even);
    require(filtered_two_omegas->get(Ordinal::infinity()) == 100,
        "where second omega first match");
    require(filtered_two_omegas->get(1, 1) == 102, "where second omega second match");
    delete filtered_two_omegas;

    LazySequence<int> *filtered_from_second_omega = two_omegas->where(is_even,
        Ordinal::infinity());
    require(filtered_from_second_omega->get_length() == Ordinal::infinity(),
        "where start second omega length");
    require(filtered_from_second_omega->get(0) == 100,
        "where start second omega first match");
    require(filtered_from_second_omega->get(1) == 102,
        "where start second omega second match");
    delete filtered_from_second_omega;

    LazySequence<int> later_arithmetic(new ArithmeticIntGenerator(200, 1));
    LazySequence<int> *three_omegas = two_omegas->concat(later_arithmetic);
    LazySequence<int> *filtered_three_omegas = three_omegas->where(is_even);
    require(filtered_three_omegas->get(2, 0) == 200, "where third omega first match");
    require(filtered_three_omegas->get(2, 1) == 202, "where third omega second match");
    delete filtered_three_omegas;
    LazySequence<int> *filtered_from_third_omega = three_omegas->where(is_even, 2, 0);
    require(filtered_from_third_omega->get_length() == Ordinal::infinity(),
        "where start third omega length");
    require(filtered_from_third_omega->get(0) == 200,
        "where start third omega first match");
    require(filtered_from_third_omega->get(1) == 202,
        "where start third omega second match");
    delete filtered_from_third_omega;
    delete three_omegas;

    delete two_omegas;
}

void test_lazy_stream() {
    LazySequence<int> squares(square_rule, Ordinal::finite(3));
    LazySequenceReadStream<int> stream(squares);

    require(stream.is_can_go_to_index(), "lazy read can seek");
    require(stream.is_can_go_back(), "lazy read can go back");
    REQUIRE_THROWS(std::logic_error, "lazy read before open throws", stream.read());

    stream.open();
    require(stream.read() == 0, "lazy stream first");
    require(stream.read() == 1, "lazy stream second");
    require(stream.go_back() == 1, "lazy stream go back");
    require(stream.read() == 1, "lazy stream reread");
    require(stream.read() == 4, "lazy stream third");
    require(stream.is_end_of_stream(), "lazy stream end");
    require(stream.go_to_index(3) == 3, "lazy stream seek to end");
    REQUIRE_THROWS(std::out_of_range, "lazy stream read past end", stream.read());
    REQUIRE_THROWS(std::out_of_range, "lazy stream seek past end", stream.go_to_index(4));
    REQUIRE_THROWS(std::out_of_range, "lazy stream seek negative", stream.go_to_index(-1));
    stream.close();
    REQUIRE_THROWS(std::logic_error, "lazy read after close throws", stream.read());

    LazySequence<int> infinite(square_rule);
    LazySequenceReadStream<int> infinite_stream(infinite);
    infinite_stream.open();
    require(!infinite_stream.is_end_of_stream(), "infinite lazy stream does not end at open");
    require(infinite_stream.go_to_index(10) == 10, "infinite lazy stream seek");
    require(infinite_stream.read() == 100, "infinite lazy stream read after seek");
    require(!infinite_stream.is_end_of_stream(), "infinite lazy stream still not ended");
    infinite_stream.close();
}

void test_file_streams() {
    const std::string path = "stream_numbers_test.txt";
    const std::string missing_path = "missing_stream_numbers_test_987654321.txt";

    FileWriteStream<int> writer(path, serialize_int);
    require(!writer.is_can_go_to_index(), "file write cannot seek before open");
    require(!writer.is_can_go_back(), "file write cannot go back before open");
    REQUIRE_THROWS(std::logic_error, "file write before open throws", writer.write(1));

    writer.open();
    require(writer.get_position() == 0, "file write initial position");
    require(writer.write(12) == 1, "file write first position");
    require(writer.write(-5) == 2, "file write second position");
    require(!writer.is_can_go_to_index(), "file write cannot seek");
    require(!writer.is_can_go_back(), "file write cannot go back");
    REQUIRE_THROWS(std::logic_error, "file write seek throws", writer.go_to_index(0));
    REQUIRE_THROWS(std::logic_error, "file write forward throws", writer.go_forward());
    REQUIRE_THROWS(std::logic_error, "file write back throws", writer.go_back());
    writer.close();
    REQUIRE_THROWS(std::logic_error, "file write after close throws", writer.write(99));

    FileReadStream<int> reader(path, deserialize_int);
    require(reader.is_can_go_to_index(), "file read can seek");
    require(reader.is_can_go_back(), "file read can go back");
    REQUIRE_THROWS(std::logic_error, "file read before open throws", reader.read());
    REQUIRE_THROWS(std::logic_error, "file end before open throws", reader.is_end_of_stream());

    reader.open();
    require(reader.get_position() == 0, "file read initial position");
    require(!reader.is_end_of_stream(), "file read not ended at start");
    require(reader.read() == 12, "file read first");
    require(reader.get_position() == 1, "file read position after first");
    require(reader.read() == -5, "file read second");
    require(reader.is_end_of_stream(), "file read end");
    require(reader.go_to_index(0) == 0, "file read seek start");
    require(reader.go_forward() == 1, "file read go forward");
    require(reader.go_back() == 0, "file read go back");
    require(reader.go_to_index(1) == 1, "file read seek");
    require(reader.read() == -5, "file read after seek");
    REQUIRE_THROWS(std::out_of_range, "file read seek past end throws", reader.go_to_index(3));
    REQUIRE_THROWS(std::out_of_range, "file read seek negative throws", reader.go_to_index(-1));
    reader.close();
    REQUIRE_THROWS(std::logic_error, "file read after close throws", reader.read());

    FileReadStream<int> missing_reader(missing_path, deserialize_int);
    REQUIRE_THROWS(std::runtime_error, "missing file open throws", missing_reader.open());

    std::remove(path.c_str());
}

}

int main() {
    try {
        test_online_statistics();
        test_sequence_read_stream();
        test_sequence_write_stream();
        test_lazy_sequence();
        test_lazy_sequence_accessors_and_boundaries();
        test_lazy_sequence_copy_assignment_and_enumerator();
        test_lazy_stream();
        test_file_streams();
    } catch (const std::exception &error) {
        std::cerr << "Test failed: " << error.what() << '\n';
        return 1;
    }

    std::cout << "All " << tests_run << " checks passed.\n";
    return 0;
}
