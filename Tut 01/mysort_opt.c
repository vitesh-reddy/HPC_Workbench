#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000000              // 1 million elements
#define INSERTION_SORT_THRESHOLD 16
#define SWAP(a, b) { int tmp = a; a = b; b = tmp; }

// Insertion Sort for small subarrays
void insertionSort(int arr[], int low, int high) {
    for (int i = low + 1; i <= high; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= low && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

int medianOfThree(int arr[], int low, int high) {
    int mid = low + (high - low) / 2;
    if (arr[mid] < arr[low])  SWAP(arr[mid], arr[low]);
    if (arr[high] < arr[low]) SWAP(arr[high], arr[low]);
    if (arr[mid] < arr[high]) SWAP(arr[mid], arr[high]);
    return arr[high];
}

// Partition function
int partition(int arr[], int low, int high) {
    int pivot = medianOfThree(arr, low, high);
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            SWAP(arr[i], arr[j]);
        }
    }
    SWAP(arr[i + 1], arr[high]);
    return (i + 1);
}

// Optimized QuickSort with tail recursion elimination
void quickSort(int arr[], int low, int high) {
    while (low < high) {
        if (high - low < INSERTION_SORT_THRESHOLD) {
            insertionSort(arr, low, high);
            break;
        }
        int pi = partition(arr, low, high);
        // Recurse into smaller partition first (tail recursion elimination)
        if (pi - low < high - pi) {
            quickSort(arr, low, pi - 1);
            low = pi + 1;
        } else {
            quickSort(arr, pi + 1, high);
            high = pi - 1;
        }
    }
}

int main() {
    int *arr = (int*)malloc(SIZE * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed\n");
        return 1;
    }

    srand(time(NULL));

    // Fill array with random integers
    for (int i = 0; i < SIZE; i++)
        arr[i] = rand();

    // Measure execution time
    clock_t start = clock();
    quickSort(arr, 0, SIZE - 1);
    clock_t end = clock();

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Optimized QuickSort of %d elements took %.2f seconds.\n", SIZE, time_taken);

    free(arr);
    return 0;
}
