#include "Cache.hpp"
#include "Logger.hpp"

using namespace server::network;

LRUCache::LRUCache(): 
    curMemory{0}, totalMemory{0}{}

LRUCache::LRUCache(uint32_t capacityMB): 
    curMemory{0}, totalMemory{capacityMB * 1024 * 1024}{}

void LRUCache::put(std::string key, std::string&& value){
    if (!enabled) return;
    std::lock_guard lk(mu);

    auto it = map.find(key);

    if (it == map.end()){
        addCache(std::move(key), std::move(value));       
        return;
    }

    updateCache(std::move(key), std::move(value), it->second);
}

std::shared_ptr<const std::string> LRUCache::get(const std::string& key){
    if (!enabled) return nullptr;
    std::lock_guard lk(mu);

    auto found = map.find(key);
    if (found == map.end()) {
        LOG_DEBUG << "Cache miss on key " << key;
        return nullptr;
    }

    list.splice(list.begin(), list, found->second.it);

    LOG_DEBUG << "Key " << key << " retrieved from cache";

    return found->second.content;
}

void LRUCache::enable(uint32_t memoryLimit) {
    std::lock_guard lk(mu);
    LOG_DEBUG << "Cache is now enabled";

    enabled = true;
    totalMemory = memoryLimit * 1024 * 1024;   
}

void LRUCache::removeFromCacheIfNeeded(int32_t addingSize){
    while (!list.empty() && curMemory + addingSize > totalMemory) {
        std::string key = list.back();
        list.pop_back();

        auto it = map.find(key);
        if (it == map.end())
            continue;

        size_t size = it->second.content->size();
        curMemory -= size;
        map.erase(it);

        LOG_DEBUG << "Entry with key " << key << " evicted from cache due to the memory limit";
    }
}

void LRUCache::addCache(std::string key, std::string&& value){
    removeFromCacheIfNeeded(value.size());
    curMemory += value.size();

    std::shared_ptr<const std::string> ptr = 
        std::make_shared<const std::string>(std::move(value));

    LOG_DEBUG << "Key " << key << " stored in cache";

    list.push_front(std::move(key));
    LRUEntry entry{list.begin(), ptr};
    map.emplace(list.front(), entry);
}

void LRUCache::updateCache(
    std::string key, 
    std::string&& value, 
    LRUEntry& entry
){
    size_t oldSize = entry.content->size();
    int32_t delta = static_cast<int32_t>(value.size()) - static_cast<int32_t>(oldSize);
    removeFromCacheIfNeeded(delta);

    std::shared_ptr<const std::string> ptr = 
        std::make_shared<const std::string>(std::move(value));
        
    curMemory += delta;

    list.splice(list.begin(), list, entry.it);
    entry.content = ptr;

    LOG_DEBUG << "Key " << key << " updated in cache";
}
