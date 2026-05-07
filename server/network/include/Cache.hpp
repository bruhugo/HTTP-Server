#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <list>
#include <shared_mutex>
#include <optional>
#include <memory>

namespace server{
namespace network{

struct LRUEntry {
    std::list<std::string>::iterator it;
    std::shared_ptr<const std::string> content;
};

// LRUCache implements the Least Recently Used cache
// algorithm, with the constraint being the memory
class LRUCache {
public:

    LRUCache();
    // Space is the total amount of memory in MB the cache will
    // support before eviction.
    LRUCache(uint32_t capacityMB);

    void enable(uint32_t memoryLimit);

    void put(std::string key, std::string&& value);   
    std::shared_ptr<const std::string> get(const std::string& key);
private:
    void addCache(std::string key, std::string&& value);
    void updateCache(std::string key, std::string&& value, LRUEntry& entry);
    void removeFromCacheIfNeeded(int32_t addingSize);

    uint32_t totalMemory;
    uint32_t curMemory;

    std::mutex mu;

    std::list<std::string> list;
    std::unordered_map<std::string, LRUEntry> map;

    bool enabled{false};
};   

} 
}