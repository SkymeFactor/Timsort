#pragma once
#include <iostream>
#include <vector>

/******************************************************************************
 * Naive Timsort implementation (without galloping mode).
 * 
 * It's still significantly faster then the std::sort and std::stable_sort.
 * Timsort is pretty stable and runs for O(nlogn) in avg and worst cases and
 * O(n) in the best one, but as a price it stores O(n) additional data. In
 * comparison to the Quicksort, it's extremely fast on the nearly-sorted data.
 * 
 * Poject was made with educational purpose and never meant to be in production.
 * 
 * Created by: SkymeFactor
 * Last changes: 2020.04.26
 ******************************************************************************/

// Timsort can use any stable algorithm to sort small chunks of data.
// Though the Binary sort is preferrable, I decided to use the Insertion sort
template<class T>
void ins_sort(T &array, int left, int right, int start) {
    for (int i = start; i < right; ++i) {
        int tmp = array[i];
        int j = i - 1;
        while ((tmp < array[j]) && (j >= left)) {
            std::swap(array[j], array[j + 1]);
            j--;
        }
    }
}

// Second sorting algorithm Timsort uses to merge together previously sorted
// chunks of data. This case is not the best at all and can be significantly
// improved by implementing "galloping mode" feature.
// (and using one temporary array instead of two, of course).
template<class T>
void merge(T &array, int l, int m, int r){
    int len1 = m - l, len2 = r - m;
    int left[len1], right[len2];
    for (int i = 0; i < len1; i++)
        left[i] = array[l + i];
    for (int i = 0; i < len2; i++)
        right[i] = array[m + i];
    int i = 0, j = 0, k = l;
    while (i < len1 && j < len2){
        if (left[i] <= right[j]) {
            array[k] = left[i];
            i++;
        } else {
            array[k] = right[j];
            j++;
        }
        k++;
    }
    while (i < len1) {
        array[k] = left[i];
        k++; i++;
    }
    while (j < len2) {
        array[k] = right[j];
        k++; j++;
    } 
}

// Main algorithm that manipulates the previous two in order to
// achieve a better result.
template <class T>
void timsort(T &array, int size) {
    // Settin-up minRUN
    int r = 0, len = size;
    
    while (len >= 32) {
        r = r | (len & 1);
        len = len >> 1;
    }

    int min_run = len + r;
    std::vector<std::pair<int, int>> merge_queue;
    int curIdx = 0, run;
    int remainnig = size;

    do {
        int next = curIdx + 1;
        // Splitting
        if (next != size){
            if (array[next] - array[curIdx] >= 0) {
                while (next < size && (array[next] - array[next - 1]) >= 0)
                    next++;
            } else {
                while (next < size && (array[next] - array[next - 1]) < 0)
                    next++;
                // Reversing
                for (int i = 0; i < (int)((next - curIdx) / 2); i++){
                    int tmp = array[curIdx + i];
                    array[curIdx + i] = array[next - i - 1];
                    array[next - i - 1] = tmp;
                }
            }
            run = next - curIdx;
        } else {
            run = 1;
        }
        // Small chunks sorting
        if (run < min_run) {
            int remaining = size - curIdx;
            int run_extended = (remaining <= min_run) ? remaining : min_run;
            ins_sort(array, curIdx, curIdx + run_extended, curIdx + run);
            run = run_extended;
        }
        merge_queue.push_back(std::make_pair(curIdx, run));
        // Merging
        while (merge_queue.size() > 1) {
            int n = merge_queue.size() - 2;
            
            std::pair<int, int> X = merge_queue[n + 1];
            std::pair<int, int> Y = merge_queue[n];
            std::pair<int, int> Z = merge_queue[n - 1];

            if ((n > 0 && Z.second <= X.second + Y.second) ||
                (n > 1 && merge_queue[n - 2].second <= Z.second + Y.second)) {
                if (Z.second <= X.second) {
                    --n;
                    merge(array, Z.first, Y.first, Y.first + Y.second);
                } else {
                    merge(array, Y.first, X.first, X.first + X.second);
                }
                merge_queue[n].second += merge_queue[n + 1].second;
                if (n == merge_queue.size() - 3){
                    merge_queue[n + 1] = merge_queue[n + 2];
                }
                merge_queue.pop_back();
            } else if (Y.second <= X.second) {
                merge(array, Y.first, X.first, X.first + X.second);
                merge_queue[n].second += merge_queue[n + 1].second;
                merge_queue.pop_back();
            } else {
                break; // Stabilized
            }
        }
        curIdx += run;
        remainnig -= run;
    } while (curIdx < size -1);

    // Last merge if needed
    while (merge_queue.size() > 1) {
        int n = merge_queue.size() - 2;

        std::pair<int, int> X = merge_queue[n + 1];
        std::pair<int, int> Y = merge_queue[n];
        std::pair<int, int> Z = merge_queue[n - 1];

        if (n > 0 && Z.second < X.second) {
            --n;
            merge(array, Z.first, Y.first, Y.first + Y.second);
        } else {
            merge(array, Y.first, X.first, X.first + X.second);
        }
        merge_queue[n].second += merge_queue[n + 1].second;
        if (n == merge_queue.size() - 3){
            merge_queue[n + 1] = merge_queue[n + 2];
        }
        merge_queue.pop_back();
    }
}