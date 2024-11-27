#include "ExtendibleHashing.h"

// Bucket class implementation
Bucket::Bucket(int depth) : localDepth(depth) {}

bool Bucket::isFull() const {
    return keys.size() >= BUCKET_SIZE;
}

void Bucket::insert(int key) {
    keys.push_back(key);
}

bool Bucket::contains(int key) const {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
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

int ExtendibleHashing::hash(int key) {
    return key & ((1 << globalDepth) - 1); // Extract globalDepth bits
}

void ExtendibleHashing::splitBucket(int index) {
    Bucket *oldBucket = directory[index];
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
    Bucket *newBucket = new Bucket(oldBucket->localDepth);

    // Redistribute keys
    std::vector<int> oldKeys = oldBucket->keys;
    oldBucket->keys.clear();

    for (int key : oldKeys) {
        int hashValue = hash(key);
        if ((hashValue & (1 << oldLocalDepth)) == 0) {
            oldBucket->insert(key);
        } else {
            newBucket->insert(key);
        }
    }

    // Update directory pointers
    for (int i = 0; i < directory.size(); ++i) {
        if (directory[i] == oldBucket && ((i >> (oldLocalDepth)) & 1)) {
            directory[i] = newBucket;
        }
    }
}

void ExtendibleHashing::insert(int key) {
    int hashValue = hash(key);
    Bucket *bucket = directory[hashValue];

    if (bucket->contains(key)) {
        std::cout << "Key " << key << " already exists in the bucket.\n";
        return;
    }

    if (bucket->isFull()) {
        splitBucket(hashValue);
        insert(key); // Reinsert the key after splitting
    } else {
        bucket->insert(key);
    }
}

bool ExtendibleHashing::search(int key) {
    int hashValue = hash(key);
    Bucket *bucket = directory[hashValue];

    if (bucket->contains(key)) {
        std::cout << "Key " << key << " found in the hash table.\n";
        return true;
    } else {
        std::cout << "Key " << key << " not found in the hash table.\n";
        return false;
    }
}

void ExtendibleHashing::remove(int key) {
    std::cout << "Key " << key << " marked as deleted. (No structural change yet)\n";
}

void ExtendibleHashing::print() {
    std::cout << "\nGlobal Depth: " << globalDepth << "\n";
    std::cout << "Directory:\n";

    for (int i = 0; i < directory.size(); ++i) {
        std::cout << "  Index " << std::bitset<8>(i).to_string().substr(8 - globalDepth)
                  << " -> Bucket [Keys: ";
        for (int key : directory[i]->keys) {
            std::cout << key << " ";
        }
        std::cout << "] (Local Depth: " << directory[i]->localDepth << ")\n";
    }
}
