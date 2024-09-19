#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <vector>
#include <list>
#include <string>
#include <functional>

using namespace std;


template<typename K, typename V, typename Hash = hash<K>>
class HashTable {
    private:
        static const int DEFAULT_BUCKETS = 10;
        static constexpr double LOAD_FACTOR_THRESHOLD = 0.7;
        vector<list<pair<K, V>>> table;
        int size;

        Hash hasher;

        int hash(const K& key) const {
            return hasher(key) % table.size();
        }

        void resize(int new_size) {
        vector<list<pair<K, V>>> new_table(new_size);
            for (const auto& bucket : table) {
                for (const auto& pair : bucket) {
                    int new_index = hasher(pair.first) % new_size;
                    new_table[new_index].push_back(pair);
                }
            }
            table = move(new_table);
        }
    
    public:
        HashTable(int buckets = DEFAULT_BUCKETS, const Hash& h = Hash()) : table(buckets), size(0), hasher(h) {}

        void insert(const K& key, const V& value) {
            int index = hash(key);
            for (auto& pair : table[index]) {
                if (pair.first == key) {
                    pair.second = value;
                    return;
                }
            }
            table[index].emplace_back(key, value);
            size++;
        }

        bool remove(const K& key) {
            int index = hash(key);
            auto& bucket = table[index];
            for (auto it = bucket.begin(); it != bucket.end(); ++it) {
                if (it -> first == key){
                    bucket.erase(it);
                    size--;
                    return true;
                }
            }
            return false;
        }

        V* get(const K& key) {
            int index = hash(key);
            for (auto& pair : table[index]) {
                if (pair.first == key) {
                    return &pair.second;
                }
            }
            return nullptr;
        }

        int getSize() const {
            return size;
        }

        bool isEmpty() const {
            return size == 0;
        }

};


#endif // HASH_TABLE_H