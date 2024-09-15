#pragma once
#include <string>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>
#include <limits>
#include <iostream>
#include "skip_list.cpp"



class DataStore {
private:
    std::unordered_map<std::string, SkipList> zsets_;

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
};