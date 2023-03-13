#ifndef ARCANE_LRU_H
#define ARCANE_LRU_H

#include <list>
#include <unordered_map>
#include <utility>

namespace arcane {

// List iterator must stable, 
// other iterator of list must valid after list insert or erase
template <
    typename Key, 
    typename T, 
    typename List = std::list<std::pair<Key, T>>,
    typename Map = std::unordered_map<Key, typename List::iterator>> 
class Lru {
public:
    explicit Lru(size_t max_size)
        : max_size_(max_size) {
    }

    Lru(const Lru& lru) 
        : list_(lru.list_),
          map_(lru.map_),
          max_size_(lru.max_size_) {
    }

    Lru(Lru&& lru) 
        : list_(std::move(lru.list_)),
          map_(std::move(lru.map_)),
          max_size_(lru.max_size_) {
    }

    Lru& operator=(const Lru& lru) {
        list_ = lru.list_;
        map_ = lru.map_;
        max_size_ = lru.max_size_;
        return *this;
    }

    Lru& operator=(Lru&& lru) {
        list_ = std::move(lru.list_);
        map_ = std::move(lru.map_);
        max_size_ = lru.max_size_;
        return *this;
    }

    void Put(const Key& key, const T& data) {
        auto it = list_.insert(list_.end(), std::make_pair(key, data));
        auto r = map_.insert(std::make_pair(key, it));
        if (r.second) {
            Purge();            
        } else {
            list_.erase(r.first->second);
            r.first->second = it;
        }
    }

    std::pair<T, bool> Get(const Key& key) {
        auto res = std::make_pair<T, bool>(T(), false);
        auto it = map_.find(key);
        if (it != map_.end()) {
            res.first = it->second->second;
            res.second = true;
            list_.splice(list_.end(), list_, it->second);
        }
        return res;
    }

    bool Exist(const Key& key) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            list_.splice(list_.end(), list_, it->second); 
        }
        return false;
    }

    void Delete(const Key& key) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            list_.erase(it->second);
            map_.erase(it);
        }
    }

    bool Empty() const {
        return list_.empty();
    }
    
    size_t Size() const {
        return list_.size();
    }

private:
    void Purge() {
        if (list_.size() >= max_size_) {
            map_.erase(list_.front().first);
            list_.pop_front();
        }
    }

    List list_;
    Map map_;
    size_t max_size_;
};

} // namespace arcane

#endif

