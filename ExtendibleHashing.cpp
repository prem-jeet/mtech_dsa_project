#include "ExtendibleHashing.h"
#include <functional>

// FNV-1a Hash Function
unsigned int fnv1aHash(const std::string& key) {
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

void Bucket::insert(const std::string& key, const std::string& value) {
    keyValuePairs[key] = value; // Add or update the key-value pair
}

bool Bucket::contains(const std::string& key) const {
    return keyValuePairs.find(key) != keyValuePairs.end();
}

std::string Bucket::getValue(const std::string& key) const {
    return keyValuePairs.at(key); // Retrieve the value for the given key
}

void Bucket::remove(const std::string& key) {
    keyValuePairs.erase(key); // Remove the key-value pair
}

// ExtendibleHashing class implementation
ExtendibleHashing::ExtendibleHashing(int depth) : globalDepth(depth) {
    int size = 1 << globalDepth; // Directory size = 2^globalDepth
    directory.resize(size, new Bucket(globalDepth));
}

ExtendibleHashing::~ExtendibleHashing() {
    std::vector<Bucket*> uniqueBuckets;

    // Identify unique buckets
    for (Bucket* bucket : directory) {
        if (std::find(uniqueBuckets.begin(), uniqueBuckets.end(), bucket) == uniqueBuckets.end()) {
            uniqueBuckets.push_back(bucket);
        }
    }

    // Delete unique buckets
    for (Bucket* bucket : uniqueBuckets) {
        delete bucket;
    }
}

int ExtendibleHashing::hash(const std::string& key) {
    return fnv1aHash(key) & ((1 << globalDepth) - 1); // Extract globalDepth bits
}

void ExtendibleHashing::splitBucket(int index) {
    Bucket* oldBucket = directory[index];
    int oldLocalDepth = oldBucket->localDepth;

    // Increase local depth of the bucket
    oldBucket->localDepth++;

    // If local depth exceeds global depth, double the directory
    if (oldBucket->localDepth > globalDepth) {
        globalDepth++;
        int size = directory.size();
        directory.resize(size * 2);
        for (int i = 0; i < size; ++i) {
            directory[size + i] = directory[i];
        }
    }

    // Create a new bucket
    Bucket* newBucket = new Bucket(oldBucket->localDepth);

    // Redistribute key-value pairs
    auto oldKeyValuePairs = oldBucket->keyValuePairs;
    oldBucket->keyValuePairs.clear();

    for (const auto& [key, value] : oldKeyValuePairs) {
        int hashValue = hash(key);
        if ((hashValue & (1 << (oldLocalDepth))) == 0) {
            oldBucket->insert(key, value);
        } else {
            newBucket->insert(key, value);
        }
    }

    // Update directory pointers
    for (int i = 0; i < directory.size(); ++i) {
        if (directory[i] == oldBucket && ((i >> oldLocalDepth) & 1)) {
            directory[i] = newBucket;
        }
    }
}

void ExtendibleHashing::insert(const std::string& key, const std::string& value) {
    int hashValue = hash(key);
    Bucket* bucket = directory[hashValue];

    if (bucket->isFull()) {
        splitBucket(hashValue);
        insert(key, value); // Reinsert the key-value pair after splitting
    } else {
        bucket->insert(key, value);
    }
}

bool ExtendibleHashing::search(const std::string& key, std::string& value) {
    int hashValue = hash(key);
    Bucket* bucket = directory[hashValue];

    if (bucket->contains(key)) {
        value = bucket->getValue(key); // Retrieve the value
        return true;
    } else {
        return false;
    }
}

void ExtendibleHashing::remove(const std::string& key) {
    int hashValue = hash(key);
    Bucket* bucket = directory[hashValue];

    if (bucket->contains(key)) {
        bucket->remove(key);
        std::cout << "Key " << key << " removed from the hash table.\n";
    } else {
        std::cout << "Key " << key << " not found in the hash table.\n";
    }
}

void ExtendibleHashing::print() {
    std::cout << "\nGlobal Depth: " << globalDepth << "\n";
    std::cout << "Directory:\n";

    for (int i = 0; i < directory.size(); ++i) {
        std::cout << "  Index " << std::bitset<8>(i).to_string().substr(8 - globalDepth)
                  << " -> Bucket [Keys: ";
        for (const auto& [key, value] : directory[i]->keyValuePairs) {
            std::cout << key << " ";
        }
        std::cout << "] (Local Depth: " << directory[i]->localDepth << ")\n";
    }
}
