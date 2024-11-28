#ifndef EXTENDIBLE_HASHING_H
#define EXTENDIBLE_HASHING_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <algorithm>

// Constants
const int BUCKET_SIZE = 2; // Max number of key-value pairs a bucket can hold

// Bucket class declaration
class Bucket {
public:
    int localDepth;
    std::unordered_map<std::string, std::string> keyValuePairs; // Map for key-value pairs

    Bucket(int depth);

    bool isFull() const; // Check if the bucket is full
    void insert(const std::string& key, const std::string& value); // Insert a key-value pair
    bool contains(const std::string& key) const; // Check if the bucket contains a key
    std::string getValue(const std::string& key) const; // Get the value associated with a key
    void remove(const std::string& key); // Remove a key-value pair
};

// ExtendibleHashing class declaration
class ExtendibleHashing {
private:
    int globalDepth;
    std::vector<Bucket*> directory;

    int hash(const std::string& key); // Hash function
    void splitBucket(int index); // Split a bucket when it's full
    int flipLastBit(int index, int depth); // Get the buddy index

public:
    ExtendibleHashing(int depth);  // Constructor
    ~ExtendibleHashing();          // Destructor

    void insert(const std::string& key, const std::string& value); // Insert a key-value pair
    bool search(const std::string& key, std::string& value);       // Search for a key and get its value
    void remove(const std::string& key);                           // Remove a key-value pair
    void print();                                                  // Print the current state of the hash table
};

#endif
