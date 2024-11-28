#include "ExtendibleHashing.h"

int main() {
    ExtendibleHashing hashTable(1);

    // Insert key-value pairs
    hashTable.insert("key1", "Value1");
    hashTable.insert("key2", "Value2");
    hashTable.insert("key3", "Value3");

    // Search for keys
    std::string value;
    if (hashTable.search("key1", value)) {
        std::cout << "Key key1 found with value: " << value << "\n";
    } else {
        std::cout << "Key key1 not found.\n";
    }

    if (hashTable.search("key10", value)) {
        std::cout << "Key key10 found with value: " << value << "\n";
    } else {
        std::cout << "Key key10 not found.\n";
    }

    // Remove a key
    hashTable.remove("key2");

    // Print the hash table
    hashTable.print();

    return 0;
}
