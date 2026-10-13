#pragma once

#include <algorithm>

#include <cstdint>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace sorting {

// 比较器必须满足严格弱序；元素类型须支持复制和移动。

template<class T, class Compare = std::less<>>
void bubble_sort(std::vector<T>& a, Compare comp = {}) {
    for (auto end = a.size(); end > 1; --end) {
        bool changed = false;
        for (std::size_t i = 1; i < end; ++i) {
            if (comp(a[i], a[i - 1])) {
                std::swap(a[i], a[i - 1]);
                changed = true;
            }
        }
        if (!changed) break;
    }
}

template<class T, class Compare = std::less<>>
void selection_sort(std::vector<T>& a, Compare comp = {}) {
    for (std::size_t i = 0; i < a.size(); ++i) {
        auto best = i;
        for (auto j = i + 1; j < a.size(); ++j)
            if (comp(a[j], a[best])) best = j;
        if (best != i) std::swap(a[i], a[best]);
    }
}

template<class T, class Compare = std::less<>>
void insertion_sort(std::vector<T>& a, Compare comp = {}) {
    for (std::size_t i = 1; i < a.size(); ++i) {
        auto value = std::move(a[i]);
        auto j = i;
        while (j > 0 && comp(value, a[j - 1])) {
            a[j] = std::move(a[j - 1]);
            --j;
        }
        a[j] = std::move(value);
    }
}

template<class T, class Compare = std::less<>>
void shell_sort(std::vector<T>& a, Compare comp = {}) {
    for (auto gap = a.size() / 2; gap > 0; gap /= 2) {
        for (auto i = gap; i < a.size(); ++i) {
            auto value = std::move(a[i]);
            auto j = i;
            while (j >= gap && comp(value, a[j - gap])) {
                a[j] = std::move(a[j - gap]);
                j -= gap;
            }
            a[j] = std::move(value);
        }
    }
}

template<class T, class Compare = std::less<>>
void merge_sort(std::vector<T>& a, Compare comp = {}) {
    std::vector<T> buffer;
    buffer.reserve(a.size());
    // 半开区间 [first, last)，相等时先取左侧，保证稳定性。
    const auto merge = [&](auto&& self, std::size_t first, std::size_t last) -> void {
        if (last - first < 2) return;
        const auto mid = first + (last - first) / 2;
        self(self, first, mid);
        self(self, mid, last);
        buffer.clear();
        auto left = first;
        auto right = mid;
        while (left < mid && right < last) {
            const auto index = comp(a[right], a[left]) ? right++ : left++;
            buffer.push_back(std::move(a[index]));
        }
        while (left < mid) buffer.push_back(std::move(a[left++]));
        while (right < last) buffer.push_back(std::move(a[right++]));
        for (std::size_t i = 0; i < buffer.size(); ++i)
            a[first + i] = std::move(buffer[i]);
    };
    merge(merge, 0, a.size());
}

template<class T, class Compare = std::less<>>
void quick_sort(std::vector<T>& a, Compare comp = {}) {
    const auto partition = [&](auto&& self, std::size_t first, std::size_t last) -> void {
        while (last - first > 1) {
            const auto pivot = a[first + (last - first) / 2];
            auto lower = first;
            auto current = first;
            auto upper = last;
            // 三路划分：小于、等于、大于 pivot，避免重复值导致退化。
            while (current < upper) {
                if (comp(a[current], pivot)) {
                    std::swap(a[lower++], a[current++]);
                } else if (comp(pivot, a[current])) {
                    std::swap(a[current], a[--upper]);
                } else {
                    ++current;
                }
            }
            // 仅递归较短的一侧，栈空间限制在 O(log n)。
            if (lower - first < last - upper) {
                self(self, first, lower);
                first = upper;
            } else {
                self(self, upper, last);
                last = lower;
            }
        }
    };
    partition(partition, 0, a.size());
}

template<class T, class Compare = std::less<>>
void heap_sort(std::vector<T>& a, Compare comp = {}) {
    const auto sift_down = [&](std::size_t root, std::size_t count) {
        while (root < count / 2) {
            auto child = root * 2 + 1;
            if (child + 1 < count && comp(a[child], a[child + 1])) ++child;
            if (!comp(a[root], a[child])) break;
            std::swap(a[root], a[child]);
            root = child;
        }
    };
    for (auto i = a.size() / 2; i > 0; --i) sift_down(i - 1, a.size());
    for (auto end = a.size(); end > 1; --end) {
        std::swap(a[0], a[end - 1]);
        sift_down(0, end - 1);
    }
}

// int32_t 专用，支持负数。值域太大时拒绝分配，调用者可改用比较排序。
inline void counting_sort(std::vector<std::int32_t>& a,
                          std::size_t max_buckets = 1'000'000) {
    if (a.empty()) return;
    const auto [min_it, max_it] = std::minmax_element(a.begin(), a.end());
    const auto minimum = static_cast<std::int64_t>(*min_it);
    const auto range = static_cast<std::uint64_t>(
        static_cast<std::int64_t>(*max_it) - minimum + 1);
    if (range > max_buckets)
        throw std::length_error("counting_sort: value range exceeds max_buckets");
    std::vector<std::size_t> counts(static_cast<std::size_t>(range));
    for (const auto value : a)
        ++counts[static_cast<std::size_t>(static_cast<std::int64_t>(value) - minimum)];
    std::size_t output = 0;
    for (std::size_t i = 0; i < counts.size(); ++i)
        for (auto count = counts[i]; count > 0; --count)
            a[output++] = static_cast<std::int32_t>(minimum + static_cast<std::int64_t>(i));
}

} // namespace sorting
