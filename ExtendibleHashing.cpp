#include "ExtendibleHashing.h"
#include <functional>
#include <algorithm>
#include <iostream>

// FNV-1a Hash Function
unsigned int fnv1aHash(const std::string &key) {
    const unsigned int FNV_PRIME = 16777619u;
    const unsigned int OFFSET_BASIS = 2166136261u;

    unsigned int hash = OFFSET_BASIS;
    for (char c : key) {
        hash ^= static_cast<unsigned int>(c);
        hash *= FNV_PRIME;
    }
    return hash;
}

// Bucket class implementation
Bucket::Bucket(int depth) : localDepth(depth) {}

bool Bucket::isFull() const {
    return keyValuePairs.size() >= BUCKET_SIZE;
}

void Bucket::insert(const std::string &key, const std::string &value) {
    keyValuePairs[key] = value;
}

bool Bucket::contains(const std::string &key) const {
    return keyValuePairs.find(key) != keyValuePairs.end();
}

std::string Bucket::getValue(const std::string &key) const {
    return keyValuePairs.at(key);
}

void Bucket::remove(const std::string &key) {
    keyValuePairs.erase(key);
}

// ExtendibleHashing class implementation
ExtendibleHashing::ExtendibleHashing(int depth) : globalDepth(depth) {
    int size = 1 << globalDepth;
    directory.resize(size, new Bucket(globalDepth));
}

ExtendibleHashing::~ExtendibleHashing() {
    std::vector<Bucket *> uniqueBuckets;

    for (Bucket *bucket : directory) {
        if (std::find(uniqueBuckets.begin(), uniqueBuckets.end(), bucket) == uniqueBuckets.end()) {
            uniqueBuckets.push_back(bucket);
        }
    }

    for (Bucket *bucket : uniqueBuckets) {
        delete bucket;
    }
}

int ExtendibleHashing::hash(const std::string &key) {
    return fnv1aHash(key) & ((1 << globalDepth) - 1);
}

void ExtendibleHashing::splitBucket(int index) {
    Bucket *oldBucket = directory[index];
    int oldLocalDepth = oldBucket->localDepth;
    oldBucket->localDepth++;

    if (oldBucket->localDepth > globalDepth) {
        globalDepth++;
        int size = directory.size();
        directory.resize(size * 2);
        for (int i = 0; i < size; ++i) {
            directory[size + i] = directory[i];
        }
    }

    Bucket *newBucket = new Bucket(oldBucket->localDepth);
    auto oldKeyValuePairs = oldBucket->keyValuePairs;
    oldBucket->keyValuePairs.clear();

    for (const auto &[key, value] : oldKeyValuePairs) {
        int hashValue = hash(key);
        if ((hashValue & (1 << (oldLocalDepth))) == 0) {
            oldBucket->insert(key, value);
        } else {
            newBucket->insert(key, value);
        }
    }

    for (int i = 0; i < directory.size(); ++i) {
        if (directory[i] == oldBucket && ((i >> oldLocalDepth) & 1)) {
            directory[i] = newBucket;
        }
    }
}

int ExtendibleHashing::flipLastBit(int index, int depth) {
    return index ^ (1 << (depth - 1));
}

void ExtendibleHashing::remove(const std::string &key) {
    int hashValue = hash(key);
    Bucket *bucket = directory[hashValue];

    if (!bucket->contains(key)) {
        return;
    }

    bucket->remove(key);

    while (bucket->keyValuePairs.empty() && bucket->localDepth > 0) {
        int buddyIndex = flipLastBit(hashValue, bucket->localDepth);
        Bucket *buddyBucket = directory[buddyIndex];

        if (buddyBucket->localDepth == bucket->localDepth) {
            int newLocalDepth = bucket->localDepth - 1;

            for (int i = 0; i < directory.size(); ++i) {
                if (directory[i] == bucket || directory[i] == buddyBucket) {
                    directory[i] = buddyBucket;
                }
            }

            buddyBucket->localDepth = newLocalDepth;
            delete bucket;
            bucket = buddyBucket;
        } else {
            break;
        }
    }

    while (globalDepth > 0) {
        bool canReduce = true;

        for (int i = 0; i < directory.size(); ++i) {
            if (directory[i]->localDepth == globalDepth) {
                canReduce = false;
                break;
            }
        }

        if (canReduce) {
            globalDepth--;
            directory.resize(1 << globalDepth);
        } else {
            break;
        }
    }
}

void ExtendibleHashing::insert(const std::string &key, const std::string &value) {
    int hashValue = hash(key);
    Bucket *bucket = directory[hashValue];

    if (bucket->isFull()) {
        splitBucket(hashValue);
        insert(key, value);
    } else {
        bucket->insert(key, value);
    }
}

bool ExtendibleHashing::search(const std::string &key, std::string &value) {
    int hashValue = hash(key);
    Bucket *bucket = directory[hashValue];

    if (bucket->contains(key)) {
        value = bucket->getValue(key);
        return true;
    }
    return false;
}

void ExtendibleHashing::print() {
    std::cout << "\nGlobal Depth: " << globalDepth << "\n";
    std::cout << "Directory:\n";

    for (int i = 0; i < directory.size(); ++i) {
        std::cout << "  Index " << std::bitset<8>(i).to_string().substr(8 - globalDepth)
                  << " -> Bucket [Keys: ";
        for (const auto &[key, value] : directory[i]->keyValuePairs) {
            std::cout << key << " ";
        }
        std::cout << "] (Local Depth: " << directory[i]->localDepth << ")\n";
    }
}
