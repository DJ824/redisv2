#pragma once

#include <string>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>
#include <limits>
#include <iostream>
#include <list>
#include <set>
#include <shared_mutex>
#include "skip_list.cpp"

class DataStore {
private:
    std::unordered_map<std::string, SkipList> zsets_;
    std::unordered_map<std::string, std::string> strings_;
    std::unordered_map<std::string, std::list<std::string>> lists_;
    std::unordered_map<std::string, std::set<std::string>> sets_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> hashes_;
    mutable std::shared_mutex mutex_;

public:
    bool zadd(const std::string& key, double score, const std::string& member) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto& zset = zsets_[key];
        auto result = zset.insert(member, score);
        return result;
    }

    bool zrem(const std::string &key, const std::string &member) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        std::cout << "ZREM key=" << key << ", member=" << member << std::endl;
        auto it = zsets_.find(key);
        if (it == zsets_.end()) {
            return false;
        }
        return it->second.remove(member);
    }

    std::optional<double> zscore(const std::string &key, const std::string &member) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        std::cout << "ZSCORE key=" << key << ", member=" << member << std::endl;
        auto it = zsets_.find(key);
        if (it == zsets_.end()) {
            return std::nullopt;
        }
        return it->second.score(member);
    }

    std::vector<std::pair<std::string, double>> zrange(const std::string &key, double min_score, double max_score, int64_t offset, int64_t count) {
        std::shared_lock<std::shared_mutex> lock(mutex_);

        std::cout << "ZRANGE key=" << key << ", min_score=" << min_score
                  << ", max_score=" << max_score << ", offset=" << offset << ", count=" << count << std::endl;
        auto it = zsets_.find(key);
        if (it == zsets_.end()) {
            return {};
        }
        return it->second.range(min_score, max_score, offset, count);
    }

    std::vector<std::pair<std::string, double>>
    zquery(const std::string &key, double min_score, const std::string &min_member,
           double max_score, const std::string &max_member,
           int64_t offset, int64_t count) {
        std::shared_lock<std::shared_mutex> lock(mutex_);

        std::cout << "ZQUERY key=" << key << ", min_score=" << min_score << ", min_member=" << min_member
                  << ", max_score=" << max_score << ", max_member=" << max_member
                  << ", offset=" << offset << ", count=" << count << std::endl;
        auto it = zsets_.find(key);
        if (it == zsets_.end()) {
            return {};
        }
        return it->second.query(min_score, min_member, max_score, max_member, offset, count);
    }

    void zrange_del(const std::string &key, double min_score, double max_score, int64_t offset, int64_t count) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        std::cout << "ZRANGE_DEL key=" << key << ", min_score=" << min_score
                  << ", max_score=" << max_score << ", offset=" << offset << ", count=" << count << std::endl;
        auto it = zsets_.find(key);
        if (it == zsets_.end()) {
            return;
        }

        it->second.range_delete(min_score, max_score, offset, count);
    }

    void string_set(const std::string &key, const std::string &val) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        strings_[key] = val;
    }

    std::optional<std::string> string_get(const std::string &key) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = strings_.find(key);
        if (it == strings_.end()) {
            return std::nullopt;
        } else return strings_[key];
    }

    bool string_del(const std::string &key) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto it = strings_.find(key);
        if (it == strings_.end()) {
            return false;
        }
        strings_.erase(key);
        return true;
    }

    std::optional<int64_t> incrby(const std::string &key, int amt) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto it = strings_.find(key);
        if (it == strings_.end()) {
            strings_[key] = "0";
            it = strings_.find(key);
        }

        int64_t val;
        try {
            val = std::stoll(it->second);
        } catch (const std::invalid_argument &e) {
            return std::nullopt;
        } catch (const std::out_of_range &e) {
            return std::nullopt;
        }

        val += amt;
        it->second = std::to_string(val);
        return val;
    }

    void lpush(const std::string &key, const std::string &val) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        lists_[key].push_front(val);
    }

    void rpush(const std::string &key, const std::string &val) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        lists_[key].push_back(val);
    }

    std::optional<std::string> lpop(const std::string &key) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (lists_[key].empty()) {
            return std::nullopt;
        } else {
            auto val = lists_[key].front();
            lists_[key].pop_front();
            return val;
        }
    }

    std::optional<std::string> rpop(const std::string &key) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (lists_[key].empty()) {
            return std::nullopt;
        } else {
            auto val = lists_[key].back();
            lists_[key].pop_back();
            return val;
        }
    }

    size_t llen(const std::string &key) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return lists_[key].size();
    }

    std::optional<std::string>
    lmove(const std::string &key1, const std::string &key2, const std::string &dir1, const std::string &dir2) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (lists_[key1].empty()) {
            return std::nullopt;
        }

        std::string val;
        if (dir1 == "LEFT") {
            val = lists_[key1].front();
            lists_[key1].pop_front();
        } else if (dir1 == "RIGHT") {
            val = lists_[key1].back();
            lists_[key1].pop_back();
        } else {
            return std::nullopt;
        }

        if (dir2 == "LEFT") {
            lists_[key2].push_front(val);
        } else if (dir2 == "RIGHT") {
            lists_[key2].push_back(val);
        } else {
            if (dir1 == "LEFT") {
                lists_[key1].push_front(val);
            } else {
                lists_[key1].push_back(val);
            }
            return std::nullopt;
        }

        return val;
    }

    std::optional<std::vector<std::string>> lrange(const std::string &key, int start, int stop) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = lists_.find(key);
        if (it == lists_.end()) {
            return std::nullopt;
        }

        const auto &list = it->second;
        int size = static_cast<int>(list.size());

        if (start < 0) {
            start = std::max(size + start, 0);
        }
        if (stop < 0) {
            stop = std::max(size + stop, 0);
        }

        start = std::min(start, size);
        stop = std::min(stop, size - 1);

        if (start > stop || start >= size) {
            return std::vector<std::string>();
        }

        std::vector<std::string> result;
        auto it_start = list.begin();
        std::advance(it_start, start);

        auto it_stop = list.begin();
        std::advance(it_stop, stop + 1);

        result.insert(result.end(), it_start, it_stop);
        return result;
    }

    bool ltrim(const std::string &key, int start, int stop) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto it = lists_.find(key);
        if (it == lists_.end()) {
            return false;
        }

        auto &list = it->second;
        int size = static_cast<int>(list.size());

        if (start < 0) start = std::max(size + start, 0);
        if (stop < 0) stop = std::max(size + stop, 0);

        start = std::min(start, size);
        stop = std::min(stop, size - 1);

        if (start > stop || start >= size) {
            list.clear();
        } else {
            auto it_start = list.begin();
            std::advance(it_start, start);

            auto it_stop = list.begin();
            std::advance(it_stop, stop + 1);

            list.erase(list.begin(), it_start);
            list.erase(it_stop, list.end());
        }

        return true;
    }

    std::optional<int64_t> sadd(const std::string &key, const std::string member) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (sets_[key].find(member) != sets_[key].end()) {
            return 0;
        }

        sets_[key].insert(member);
        return 1;
    }

    std::optional<int64_t> srem(const std::string &key, const std::string &member) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (sets_.find(key) == sets_.end()) {
            return std::nullopt;
        }

        if (sets_[key].find(member) == sets_[key].end()) {
            return 0;
        }

        sets_[key].erase(member);
        return 1;
    }

    std::optional<int64_t> sismember(const std::string &key, const std::string &member) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (sets_.find(key) == sets_.end()) {
            return std::nullopt;
        }
        if (sets_[key].find(member) == sets_[key].end()) {
            return 0;
        } else {
            return 1;
        }
    }

    std::optional<std::vector<std::string>> sinter(const std::vector<const std::string> keys) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        std::vector<std::string> result;

        auto it = keys.begin();
        auto prev = it;
        ++it;

        while (it != keys.end()) {
            if (sets_.find(*it) == sets_.end() || sets_.find(*prev) == sets_.end()) {
                return std::nullopt;
            }
            std::set_intersection(sets_[*prev].begin(), sets_[*prev].end(), sets_[*it].begin(), sets_[*it].end(),
                                  std::inserter(result, result.begin()));
            prev = it;
            ++it;
        }
        return result;
    }

    size_t scard(const std::string &key) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (sets_.find(key) == sets_.end()) {
            return 0;
        }
        return sets_.find(key)->second.size();
    }

    int64_t hset(const std::string &key, const std::vector<std::pair<std::string, std::string>> &fields) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto &hash = hashes_[key];

        int64_t ct = 0;
        for (const auto &[field, value]: fields) {
            auto [it, inserted] = hash.insert_or_assign(field, value);
            if (inserted) {
                ++ct;
            }
        }

        return ct;
    }

    std::optional<std::string> hget(const std::string &key, const std::string &field) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto hash_it = hashes_.find(key);
        if (hash_it == hashes_.end()) {
            return std::nullopt;
        }

        auto field_it = hash_it->second.find(field);
        if (field_it == hash_it->second.end()) {
            return std::nullopt;
        }

        return field_it->second;
    }

    std::optional<std::vector<std::string>> hmget(const std::string &key, const std::vector<std::string> fields) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (hashes_.find(key) == hashes_.end()) {
            return std::nullopt;
        }

        std::vector<std::string> result;

        auto it = fields.begin();
        while (it != fields.end()) {
            if (hashes_[key][*it] != "") {
                result.push_back(hashes_[key][*it]);
            }
            ++it;
        }
        return result;
    }

    std::optional<int64_t> hincrby(const std::string &key, const std::string &field, int64_t increment) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto hash_it = hashes_.find(key);
        if (hash_it == hashes_.end()) {
            return std::nullopt;
        }

        auto &hash = hash_it->second;
        auto field_it = hash.find(field);

        int64_t curr_value = 0;
        if (field_it != hash.end()) {
            try {
                curr_value = std::stoll(field_it->second);
            } catch (const std::exception &) {
                return std::nullopt;
            }
        } else {
            hash[field] = "0";
        }

        int64_t new_value = curr_value + increment;
        hash[field] = std::to_string(new_value);

        return new_value;
    }


};