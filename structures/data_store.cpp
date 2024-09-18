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
#include "skip_list.cpp"

class DataStore {
private:
    std::unordered_map<std::string, SkipList> zsets_;
    std::unordered_map<std::string, std::string> strings_;
    std::unordered_map<std::string, std::list<std::string>> lists_;
    std::unordered_map<std::string, std::set<std::string>> sets_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> hashes_;


public:
    bool zadd(const std::string& key, double score, const std::string& member) {
        std::cout << "ZADD key=" << key << ", score=" << score << ", member=" << member << std::endl;
        return zsets_[key].insert(member, score);
    }

    bool zrem(const std::string& key, const std::string& member) {
        std::cout << "ZREM key=" << key << ", member=" << member << std::endl;
        auto it = zsets_.find(key);
        if (it == zsets_.end()) {
            return false;
        }
        return it->second.remove(member);
    }

    std::optional<double> zscore(const std::string& key, const std::string& member) {
        std::cout << "ZSCORE key=" << key << ", member=" << member << std::endl;
        auto it = zsets_.find(key);
        if (it == zsets_.end()) {
            return std::nullopt;
        }
        return it->second.score(member);
    }

    std::vector<std::pair<std::string, double>> zrange(const std::string& key, double min_score, double max_score, int64_t offset, int64_t count) {
        std::cout << "ZRANGE key=" << key << ", min_score=" << min_score
                  << ", max_score=" << max_score << ", offset=" << offset << ", count=" << count << std::endl;
        auto it = zsets_.find(key);
        if (it == zsets_.end()) {
            return {};
        }
        return it->second.range(min_score, max_score, offset, count);
    }

    std::vector<std::pair<std::string, double>> zquery(const std::string& key, double min_score, const std::string& min_member,
                                                       double max_score, const std::string& max_member,
                                                       int64_t offset, int64_t count) {
        std::cout << "ZQUERY key=" << key << ", min_score=" << min_score << ", min_member=" << min_member
                  << ", max_score=" << max_score << ", max_member=" << max_member
                  << ", offset=" << offset << ", count=" << count << std::endl;
        auto it = zsets_.find(key);
        if (it == zsets_.end()) {
            return {};
        }
        return it->second.query(min_score, min_member, max_score, max_member, offset, count);
    }

    void zrange_del(const std::string& key, double min_score, double max_score, int64_t offset, int64_t count) {
        std::cout << "ZRANGE_DEL key=" << key << ", min_score=" << min_score
                  << ", max_score=" << max_score << ", offset=" << offset << ", count=" << count << std::endl;
        auto it = zsets_.find(key);
        if (it == zsets_.end()) {
            return;
        }

        it->second.range_delete(min_score, max_score, offset, count);
    }

    void string_set(const std::string& key, const std::string& val) {
        strings_[key] = val;
    }

    std::optional<std::string> string_get(const std::string& key) {
        auto it = strings_.find(key);
        if (it == strings_.end()) {
            return std::nullopt;
        } else return strings_[key];
    }

    bool string_del(const std::string& key) {
        auto it = strings_.find(key);
        if (it == strings_.end()) {
            return false;
        }
        strings_.erase(key);
        return true;
    }

    std::optional<int64_t> incrby(const std::string& key, int& amt) {
        auto it = strings_.find(key);
        if (it == strings_.end()) {
            return std::nullopt;
        }
        int64_t val = atoi(strings_[key].c_str());

        if (!isdigit(val)) {
            return std::nullopt;
        }

        val += amt;
        strings_[key] = std::to_string(val);
        return val;
    }

    void lpush(const std::string& key, const std::string& val) {
        lists_[key].push_front(val);
    }

    void rpush(const std::string& key, const std::string& val) {
        lists_[key].push_back(val);
    }

    std::optional<std::string> lpop(const std::string& key) {
        if (lists_[key].empty()) {
            return std::nullopt;
        } else {
            auto val = lists_[key].front();
            lists_[key].pop_back();
            return val;
        }
    }

    std::optional<std::string> rpop(const std::string& key) {
        if (lists_[key].empty()) {
            return std::nullopt;
        } else {
            auto val = lists_[key].back();
            lists_[key].pop_back();
            return val;
        }
    }

    size_t llen(const std::string& key) {
        return lists_[key].size();
    }

    std::optional<std::string> lmove(const std::string& key1, const std::string& key2, const std::string& dir1, const std::string& dir2) {
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
            return std::nullopt; // Invalid direction
        }

        return val;
    }

    std::optional<std::vector<std::string>> lrange(const std::string& key, int start, int stop) {
        if (lists_.find(key) == lists_.end()) {
            return std::nullopt;
        }
        std::vector<std::string> result;

        auto it = lists_[key].begin();
        int diff = stop - start;
        while (start > 0) {
            ++it;
        }

        while (diff > 0 || it != lists_[key].end()) {
            result.push_back(*it);
            ++it;
        }

        return result;
    }

    bool ltrim(const std::string& key, int start, int stop) {
        auto it = lists_.find(key);
        if (it == lists_.end()) {
            return false;
        }

        auto& list = it->second;
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
            list.clear();
            return true;
        }

        list.erase(list.begin(), std::next(list.begin(), start));

        if (stop + 1 < list.size()) {
            list.erase(std::next(list.begin(), stop - start + 1), list.end());
        }

        return true;
    }

    std::optional<int64_t> sadd(const std::string& key, const std::string member) {
        if (sets_.find(key) == sets_.end()) {
            return std::nullopt;
        }

        if (sets_[key].find(member) != sets_[key].end()) {
            return 0;
        }
        sets_[key].insert(member);
        return 1;
    }

    std::optional<int64_t> srem(const std::string& key, const std::string& member) {
        if (sets_.find(key) == sets_.end()) {
            return std::nullopt;
        }

        if (sets_[key].find(member) == sets_[key].end()) {
            return 0;
        }

        sets_[key].erase(member);
        return 1;
    }

    std::optional<int64_t> sismember(const std::string& key, const std::string& member) {
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
        std::vector<std::string> result;

        auto it = keys.begin();
        auto prev = it;
        ++it;


        while (it != keys.end()) {
            if (sets_.find(*it) == sets_.end() || sets_.find(*prev) == sets_.end()) {
                return std::nullopt;
            }
            std::set_intersection(sets_[*prev].begin(), sets_[*prev].end(), sets_[*it].begin(), sets_[*it].end(), std::inserter(result, result.begin()));
            prev = it;
            ++it;
        }
        return result;
    }

    size_t scard(const std::string& key) {
        if (sets_.find(key) == sets_.end()) {
            return 0;
        }
        return sets_.find(key)->second.size();
    }

    int64_t hset(const std::string& key, const std::vector<std::pair<std::string, std::string>> fields) {
        if (hashes_.find(key) == hashes_.end()) {
            return 0;
        }

        auto it = fields.begin();
        int ct = 0;
        while (it != fields.end()) {
            hashes_[key][it->first] = it->second;
            ++it;
            ++ct;
        }

        return ct;
    }

    std::optional<std::string> hget(const std::string& key, const std::string& field) {
        if (hashes_.find(key) == hashes_.end()) {
            return std::nullopt;
        }

        if (hashes_[key][field] != "") {
            return nullptr;
        }
        return hashes_[key][field];
    }

    std::optional<std::vector<std::string>> hmget(const std::string& key, const std::vector<std::string> fields) {
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

    std::optional<int> hincrby(const std::string& key, const std::string& field, int val) {
        if (hashes_.find(key) == hashes_.end()) {
            return std::nullopt;
        }

        auto curr = atoi(hashes_[key][field].c_str());

        if (isdigit(curr)) {
            curr += val;
            hashes_[key][field] = curr;
        }

        return curr;
    }



























};