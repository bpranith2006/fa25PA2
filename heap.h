// Created by Manju Muralidharan on 10/19/25.

#ifndef HEAP_H
#define HEAP_H

// Simple array min-heap that stores NODE INDICES.
// Comparisons are done using the external weightArr[] passed in.
struct MinHeap {
    int data[64];
    int size;

    MinHeap() { size = 0; }

    void push(int idx, int weightArr[]) {
        // insert at end then bubble up
        data[size] = idx;
        upheap(size, weightArr);
        size++;
    }

    int pop(int weightArr[]) {
        // remove root, move last to root, then bubble down
        int ret = data[0];
        size--;
        data[0] = data[size];
        downheap(0, weightArr);
        return ret;
    }

    void upheap(int pos, int weightArr[]) {
        // bubble up while child < parent
        while (pos > 0) {
            int p = (pos - 1) / 2;
            if (weightArr[data[pos]] < weightArr[data[p]]) {
                int tmp = data[pos]; data[pos] = data[p]; data[p] = tmp;
                pos = p;
            } else break;
        }
    }

    void downheap(int pos, int weightArr[]) {
        // bubble down while parent > any child
        while (true) {
            int l = 2 * pos + 1;
            int r = 2 * pos + 2;
            int s = pos;
            if (l < size && weightArr[data[l]] < weightArr[data[s]]) s = l;
            if (r < size && weightArr[data[r]] < weightArr[data[s]]) s = r;
            if (s != pos) {
                int tmp = data[pos]; data[pos] = data[s]; data[s] = tmp;
                pos = s;
            } else break;
        }
    }
};

#endif
