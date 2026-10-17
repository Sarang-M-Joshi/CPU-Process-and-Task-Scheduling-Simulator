#pragma once

#include <vector>

namespace dsa {

inline int linearSearch(const std::vector<int>& values, int key) {
    for (std::size_t i = 0; i < values.size(); ++i)
        if (values[i] == key) return static_cast<int>(i);
    return -1;
}

inline int binarySearch(const std::vector<int>& values, int key) {
    std::size_t first = 0, last = values.size();
    while (first < last) {
        const std::size_t middle = first + (last - first) / 2;
        if (values[middle] == key) return static_cast<int>(middle);
        if (values[middle] < key) first = middle + 1;
        else last = middle;
    }
    return -1;
}

inline void bubbleSort(std::vector<int>& values) {
    for (std::size_t end = values.size(); end > 1; --end) {
        bool changed = false;
        for (std::size_t i = 1; i < end; ++i)
            if (values[i - 1] > values[i]) { int t = values[i]; values[i] = values[i - 1]; values[i - 1] = t; changed = true; }
        if (!changed) break;
    }
}

inline void insertionSort(std::vector<int>& values) {
    for (std::size_t i = 1; i < values.size(); ++i) {
        const int key = values[i]; std::size_t j = i;
        while (j > 0 && values[j - 1] > key) { values[j] = values[j - 1]; --j; }
        values[j] = key;
    }
}

inline void quickSortRange(std::vector<int>& values, int low, int high) {
    while (low < high) {
        int i = low, j = high;
        const int pivot = values[low + (high - low) / 2];
        while (i <= j) {
            while (values[i] < pivot) ++i;
            while (values[j] > pivot) --j;
            if (i <= j) { int t = values[i]; values[i] = values[j]; values[j] = t; ++i; --j; }
        }
        // Recurse into the smaller side and iterate over the larger side to bound stack use.
        if (j - low < high - i) { if (low < j) quickSortRange(values, low, j); low = i; }
        else { if (i < high) quickSortRange(values, i, high); high = j; }
    }
}

inline void quickSort(std::vector<int>& values) {
    if (values.size() > 1) quickSortRange(values, 0, static_cast<int>(values.size() - 1));
}

}  // namespace dsa
