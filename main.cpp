#include "sorts.hpp"

#include <array>
#include <iostream>
#include <limits>
#include <random>
#include <string>

namespace {
using Values = std::vector<std::int32_t>;
using Sort = void (*)(Values&);

const std::array<std::pair<const char*, Sort>, 8> algorithms{{
    {"bubble", [](Values& a) { sorting::bubble_sort(a); }},
    {"selection", [](Values& a) { sorting::selection_sort(a); }},
    {"insertion", [](Values& a) { sorting::insertion_sort(a); }},
    {"shell", [](Values& a) { sorting::shell_sort(a); }},
    {"merge", [](Values& a) { sorting::merge_sort(a); }},
    {"quick", [](Values& a) { sorting::quick_sort(a); }},
    {"heap", [](Values& a) { sorting::heap_sort(a); }},
    {"counting", [](Values& a) { sorting::counting_sort(a); }},
}};

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void test() {
    std::vector<Values> cases{{}, {1}, {2, 1}, {1, 2, 3, 4},
        {4, 3, 2, 1}, {7, 7, 7, 7}, {-5, 0, 3, -5, 2, 0}};
    std::mt19937 engine{42};
    std::uniform_int_distribution<int> size_dist{0, 128};
    std::uniform_int_distribution<std::int32_t> value_dist{-100, 100};
    for (int trial = 0; trial < 300; ++trial) {
        Values values(static_cast<std::size_t>(size_dist(engine)));
        for (auto& value : values) value = value_dist(engine);
        cases.push_back(std::move(values));
    }
    for (const auto& [name, sort] : algorithms) {
        for (const auto& input : cases) {
            auto expected = input;
            std::sort(expected.begin(), expected.end());
            auto actual = input;
            sort(actual);
            require(actual == expected, std::string{name} + " incorrect result");
        }
    }

    // 比较排序还要覆盖整数极值和降序比较器。
    const Values extremes{std::numeric_limits<std::int32_t>::max(), 0,
        std::numeric_limits<std::int32_t>::min(), -1, 1};
    for (std::size_t i = 0; i < algorithms.size() - 1; ++i) {
        auto actual = extremes;
        auto expected = extremes;
        std::sort(expected.begin(), expected.end());
        algorithms[i].second(actual);
        require(actual == expected, "integer extremes failed");
    }
    const std::array<Sort, 7> descending{{
        [](Values& a) { sorting::bubble_sort(a, std::greater<>{}); },
        [](Values& a) { sorting::selection_sort(a, std::greater<>{}); },
        [](Values& a) { sorting::insertion_sort(a, std::greater<>{}); },
        [](Values& a) { sorting::shell_sort(a, std::greater<>{}); },
        [](Values& a) { sorting::merge_sort(a, std::greater<>{}); },
        [](Values& a) { sorting::quick_sort(a, std::greater<>{}); },
        [](Values& a) { sorting::heap_sort(a, std::greater<>{}); },
    }};
    for (const auto sort : descending) {
        for (const auto& input : cases) {
            auto expected = input;
            auto actual = input;
            std::sort(expected.begin(), expected.end(), std::greater<>{});
            sort(actual);
            require(actual == expected, "descending order failed");
        }
    }

    // 稳定排序应保留相同 key 的原始顺序。
    using Record = std::pair<int, int>;
    const auto by_key = [](const Record& a, const Record& b) { return a.first < b.first; };
    const std::vector<Record> records{{2, 0}, {1, 1}, {2, 2}, {1, 3}, {2, 4}};
    auto expected = records;
    std::stable_sort(expected.begin(), expected.end(), by_key);
    auto actual = records;
    sorting::bubble_sort(actual, by_key);
    require(actual == expected, "bubble stability failed");
    actual = records;
    sorting::insertion_sort(actual, by_key);
    require(actual == expected, "insertion stability failed");
    actual = records;
    sorting::merge_sort(actual, by_key);
    require(actual == expected, "merge stability failed");

    auto wide = extremes;
    bool rejected = false;
    try { sorting::counting_sort(wide); }
    catch (const std::length_error&) { rejected = true; }
    require(rejected && wide == extremes, "counting range guard failed");
    for (const auto bound : {std::numeric_limits<std::int32_t>::min(),
                             std::numeric_limits<std::int32_t>::max()}) {
        Values same{bound, bound};
        sorting::counting_sort(same);
        require(same == Values{bound, bound}, "counting boundary failed");
    }
}
} // namespace

int main() {
    try {
        test();
        std::cout << "All sorting tests passed.\n\n";
        for (const auto& [name, sort] : algorithms) {
            Values values{38, -4, 27, 0, 12, 27, 9};
            sort(values);
            std::cout << name << ": ";
            for (const auto value : values) std::cout << value << ' ';
            std::cout << '\n';
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAILED: " << error.what() << '\n';
        return 1;
    }
}
