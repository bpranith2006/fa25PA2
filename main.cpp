// Created by Manju Muralidharan on 10/19/25.
//
#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include "heap.h"
using namespace std;

// Parallel arrays for the encoding tree (array-based, no dynamic alloc)
const int MAX_NODES = 64;
int weightArr[MAX_NODES];
int leftArr[MAX_NODES];
int rightArr[MAX_NODES];
char charArr[MAX_NODES];

// Prototypes
void buildFrequencyTable(int freq[], const string& filename);
int  createLeafNodes(int freq[]);
int  buildEncodingTree(int nextFree);
void generateCodes(int root, string codes[]);
void encodeMessage(const string& filename, string codes[]);

int main() {
    int freq[26] = {0};

    // 1) scan file and count letters (case-insensitive)
    buildFrequencyTable(freq, "input.txt");

    // 2) create leaves in parallel arrays
    int nextFree = createLeafNodes(freq);

    // Early exit if no letters (avoids building/traversing an empty tree)
    if (nextFree == 0) {
        cout << "No alphabetic characters found in input.\n";
        return 0;
    }

    // 3) greedy combine with min-heap → internal nodes up to single root
    int root = buildEncodingTree(nextFree);

    // 4) iterative DFS with stack assigns 0/1 strings (no recursion)
    string codes[26];
    generateCodes(root, codes);

    // 5) print code table and encoded bits
    encodeMessage("input.txt", codes);
    return 0;
}

/*------------------------------------------------------
    Function Definitions
  ------------------------------------------------------*/

// Step 1: Read file and count frequencies
void buildFrequencyTable(int freq[], const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: could not open " << filename << "\n";
        exit(1);
    }

    char ch;
    while (file.get(ch)) {
        if (ch >= 'A' && ch <= 'Z') ch = ch - 'A' + 'a';
        if (ch >= 'a' && ch <= 'z') freq[ch - 'a']++;
    }
    file.close();

    cout << "Frequency table built successfully.\n";
}

// Step 2: Create leaf nodes for each character
int createLeafNodes(int freq[]) {
    int nextFree = 0;

    // Build leaf nodes for chars that appear at least once
    for (int i = 0; i < 26; ++i) {
        if (freq[i] > 0) {
            if (nextFree >= MAX_NODES) { // keep arrays safe
                cerr << "Error: node limit exceeded.\n";
                exit(1);
            }
            charArr[nextFree]   = 'a' + i;
            weightArr[nextFree] = freq[i];
            leftArr[nextFree]   = -1;
            rightArr[nextFree]  = -1;
            nextFree++;
        }
    }
    cout << "Created " << nextFree << " leaf nodes.\n";
    return nextFree;
}

// Step 3: Build the encoding tree using heap operations
int buildEncodingTree(int nextFree) {
    if (nextFree == 0) return -1; // no letters
    if (nextFree == 1) return 0;  // single letter → that leaf is root

    MinHeap hp;

    // push all leaf indices [0..nextFree-1]
    for (int i = 0; i < nextFree; ++i) hp.push(i, weightArr);

    // combine until one node remains
    while (hp.size > 1) {
        // get two smallest subtrees
        int a = hp.pop(weightArr);
        int b = hp.pop(weightArr);

        if (nextFree >= MAX_NODES) {
            cerr << "Error: node limit exceeded.\n";
            exit(1);
        }

        // create internal parent
        int parent = nextFree++;
        charArr[parent]   = '\0'; // internal marker
        leftArr[parent]   = a;
        rightArr[parent]  = b;
        weightArr[parent] = weightArr[a] + weightArr[b];

        hp.push(parent, weightArr);
    }

    // final root
    return hp.pop(weightArr);
}

// Step 4: Use an STL stack to generate codes
void generateCodes(int root, string codes[]) {
    // reset
    for (int i = 0; i < 26; ++i) codes[i] = "";
    if (root == -1) return;

    // single-symbol input → assign "0"
    bool singleLeaf = (leftArr[root] == -1 && rightArr[root] == -1);

    // (node index, path so far). Iterative DFS, no recursion.
    stack<pair<int, string>> st;
    st.push({root, ""});

    while (!st.empty()) {
        auto cur = st.top(); st.pop();
        int u = cur.first;
        const string &path = cur.second;

        bool isLeaf = (leftArr[u] == -1 && rightArr[u] == -1);
        if (isLeaf) {
            char c = charArr[u];
            if (c >= 'a' && c <= 'z') {
                string code = path;
                if (code.empty() && singleLeaf) code = "0";
                codes[c - 'a'] = code;
            }
        } else {
            // Right gets '1', Left gets '0'. Order doesn’t affect validity.
            if (rightArr[u] != -1) st.push({ rightArr[u], path + "1" });
            if (leftArr[u]  != -1) st.push({ leftArr[u],  path + "0" });
        }
    }
}

// Step 5: Print table and encoded message
void encodeMessage(const string& filename, string codes[]) {
    cout << "\nCharacter : Code\n";
    for (int i = 0; i < 26; ++i) {
        if (!codes[i].empty())
            cout << char('a' + i) << " : " << codes[i] << "\n";
    }

    cout << "\nEncoded message:\n";

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: could not open " << filename << "\n";
        return;
    }

    char ch;
    while (file.get(ch)) {
        if (ch >= 'A' && ch <= 'Z') ch = ch - 'A' + 'a';
        if (ch >= 'a' && ch <= 'z') {
            const string &code = codes[ch - 'a'];
            if (!code.empty()) cout << code; // skip letters not in table
        }
    }
    cout << "\n";
    file.close();
}
