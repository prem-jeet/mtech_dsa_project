#include <iostream>
#include <vector>
#include <bitset>
#include <cmath>
#include <algorithm>

// Constants
const int BUCKET_SIZE = 2; // Max keys a bucket can hold

class Bucket
{
public:
    int localDepth;
    std::vector<int> keys;

    Bucket(int depth) : localDepth(depth) {}

    bool isFull() const
    {
        return keys.size() >= BUCKET_SIZE;
    }

    void insert(int key)
    {
        keys.push_back(key);
    }

    bool contains(int key) const
    {
        return std::find(keys.begin(), keys.end(), key) != keys.end();
    }
};

class ExtendibleHashing
{
private:
    int globalDepth;
    std::vector<Bucket *> directory;

    int hash(int key)
    {
        return key & ((1 << globalDepth) - 1); // Extract globalDepth bits
    }

    void splitBucket(int index)
    {
        Bucket *oldBucket = directory[index];
        int oldLocalDepth = oldBucket->localDepth;

        // Increase local depth of the bucket
        oldBucket->localDepth++;

        // If local depth exceeds global depth, double the directory
        if (oldBucket->localDepth > globalDepth)
        {
            globalDepth++;
            int size = directory.size();
            directory.resize(size * 2);
            for (int i = 0; i < size; ++i)
            {
                directory[size + i] = directory[i];
            }
        }

        // Create a new bucket
        Bucket *newBucket = new Bucket(oldBucket->localDepth);

        // Redistribute keys
        std::vector<int> oldKeys = oldBucket->keys;
        oldBucket->keys.clear();

        for (int key : oldKeys)
        {
            int hashValue = hash(key);
            if ((hashValue & (1 << oldLocalDepth)) == 0)
            {
                oldBucket->insert(key);
            }
            else
            {
                newBucket->insert(key);
            }
        }

        // Update directory pointers
        for (int i = 0; i < directory.size(); ++i)
        {
            if (directory[i] == oldBucket && ((i >> (oldLocalDepth)) & 1))
            {
                directory[i] = newBucket;
            }
        }
    }

public:
    ExtendibleHashing(int depth) : globalDepth(depth)
    {
        int size = 1 << globalDepth; // Directory size = 2^globalDepth
        directory.resize(size, new Bucket(globalDepth));
    }

    void insert(int key)
    {
        int hashValue = hash(key);
        Bucket *bucket = directory[hashValue];

        if (bucket->contains(key))
        {
            std::cout << "Key " << key << " already exists in the bucket.\n";
            return;
        }

        if (bucket->isFull())
        {
            splitBucket(hashValue);
            insert(key); // Reinsert the key after splitting
        }
        else
        {
            bucket->insert(key);
        }
    }

    void print()
    {
        std::cout << "\nGlobal Depth: " << globalDepth << "\n";
        std::cout << "Directory:\n";

        for (int i = 0; i < directory.size(); ++i)
        {
            std::cout << "  Index " << std::bitset<8>(i).to_string().substr(8 - globalDepth)
                      << " -> Bucket [Keys: ";
            for (int key : directory[i]->keys)
            {
                std::cout << key << " ";
            }
            std::cout << "] (Local Depth: " << directory[i]->localDepth << ")\n";
        }

        std::cout << "\nBuckets:\n";
        std::vector<Bucket *> uniqueBuckets;
        for (Bucket *bucket : directory)
        {
            if (std::find(uniqueBuckets.begin(), uniqueBuckets.end(), bucket) == uniqueBuckets.end())
            {
                uniqueBuckets.push_back(bucket);
            }
        }
        for (size_t i = 0; i < uniqueBuckets.size(); ++i)
        {
            Bucket *bucket = uniqueBuckets[i];
            std::cout << "  Bucket " << i << ": [Keys: ";
            for (int key : bucket->keys)
            {
                std::cout << key << " ";
            }
            std::cout << "] (Local Depth: " << bucket->localDepth << ")\n";
        }
    }
};

int main()
{
    int initialDepth = 1;
    ExtendibleHashing hashTable(initialDepth);
    // Print the hash table
    hashTable.print();
    // Insert keys into the hash table
    hashTable.insert(1);
    // Print the hash table
    hashTable.print();
    hashTable.insert(5);
    // Print the hash table
    hashTable.print();
    hashTable.insert(2);
    // Print the hash table
    hashTable.print();
    hashTable.insert(3);
    // Print the hash table
    hashTable.print();
    hashTable.insert(6);
    // Print the hash table
    hashTable.print();
    hashTable.insert(4);
    // Print the hash table
    hashTable.print();
    hashTable.insert(23);
    // Print the hash table
    hashTable.print();
    hashTable.insert(15);
    // Print the hash table
    hashTable.print();
    hashTable.insert(11);

    // Print the hash table
    hashTable.print();

    return 0;
}