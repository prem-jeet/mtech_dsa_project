#ifndef EXTENDIBLE_HASHING_H
#define EXTENDIBLE_HASHING_H

#include <iostream>
#include <vector>
#include <bitset>
#include <cmath>
#include <algorithm>

// Constants
const int BUCKET_SIZE = 2; // Max keys a bucket can hold

class Bucket {
public:
    int localDepth;
    std::vector<int> keys;

    Bucket(int depth);

    bool isFull() const;
    void insert(int key);
    bool contains(int key) const;
};

class ExtendibleHashing {
private:
    int globalDepth;
    std::vector<Bucket *> directory;

    int hash(int key);
    void splitBucket(int index);

public:
    ExtendibleHashing(int depth);
    ~ExtendibleHashing();

    void insert(int key);
    bool search(int key);
    void remove(int key);
    void print();
};

#endif
