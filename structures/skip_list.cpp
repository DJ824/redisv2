//
// Created by Devang Jaiswal on 9/14/24.
//
#pragma once
#include <string>
#include <memory>
#include <random>
#include <vector>
#include <limits>
#include <iostream>
#include <optional>

class SkipList {
private:
    struct Node {
        std::string member_;
        double score_;
        std::vector<Node *> forward_;

        Node(const std::string &m, double s, int level)
                : member_(m), score_(s), forward_(level, nullptr) {}
    };

    static constexpr int MAX_LEVEL_ = 32;
    static constexpr float P_ = 0.5;

    Node *head_;
    int level_;
    std::mt19937 gen_;
    std::uniform_real_distribution<> dis_;

    int randomLevel() {
        int lvl = 1;
        while (dis_(gen_) < P_ && lvl < MAX_LEVEL_) {
            ++lvl;
        }
        return lvl;
    }

public:
    SkipList() : level_(1), gen_(std::random_device{}()), dis_(0.0, 1.0) {
        head_ = new Node("", std::numeric_limits<double>::lowest(), MAX_LEVEL_);
    }

    ~SkipList() {
        Node *current = head_;
        while (current != nullptr) {
            Node *next = current->forward_[0];
            delete current;
            current = next;
        }
    }

    bool insert(const std::string &member, double score) {
        std::vector<Node *> update(MAX_LEVEL_);
        auto x = head_;

        for (int i = level_ - 1; i >= 0; --i) {
            while (x->forward_[i] && (x->forward_[i]->score_ < score ||
                                      (x->forward_[i]->score_ == score && x->forward_[i]->member_ < member))) {
                x = x->forward_[i];
            }
            update[i] = x;
        }
        //x = x->forward_[0];

        if (x && x->member_ == member) {
            x->score_ = score;
            return false;
        }

        int newLevel = randomLevel();
        if (newLevel > level_) {
            for (int i = level_; i < newLevel; ++i) {
                update[i] = head_;
            }
            level_ = newLevel;
        }

        x = new Node(member, score, newLevel);

        for (int i = 0; i < newLevel; ++i) {
            x->forward_[i] = update[i]->forward_[i];
            update[i]->forward_[i] = x;
        }

        return true;
    }

    bool remove(const std::string &member) {
        std::vector<Node *> update(MAX_LEVEL_);
        auto x = head_;

        for (int i = level_ - 1; i >= 0; --i) {
            while (x->forward_[i] && x->forward_[i]->member_ < member) {
                x = x->forward_[i];
            }
            update[i] = x;
        }
        x = x->forward_[0];

        if (!x || x->member_ != member) {
            return false;
        }

        for (int i = 0; i < level_; ++i) {
            if (update[i]->forward_[i] != x) {
                break;
            }
            update[i]->forward_[i] = x->forward_[i];
        }

        delete x;

        while (level_ > 1 && head_->forward_[level_ - 1] == nullptr) {
            --level_;
        }

        return true;
    }

    std::optional<double> score(const std::string &member) {
        auto x = head_;

        for (int i = level_ - 1; i >= 0; --i) {
            while (x->forward_[i] && x->forward_[i]->member_ < member) {
                x = x->forward_[i];
            }
        }
        x = x->forward_[0];

        if (x && x->member_ == member) {
            return x->score_;
        }

        return std::nullopt;
    }

    std::vector<std::pair<std::string, double>>
    range(double min_score, double max_score, int64_t offset, int64_t count) {
        std::vector<std::pair<std::string, double>> result;
        auto x = head_;

        for (int i = level_ - 1; i >= 0; --i) {
            while (x->forward_[i] && x->forward_[i]->score_ < min_score) {
                x = x->forward_[i];
            }
        }
        x = x->forward_[0];

        while (x && offset > 0) {
            x = x->forward_[0];
            --offset;
        }

        while (x && x->score_ <= max_score && count > 0) {
            result.emplace_back(x->member_, x->score_);
            x = x->forward_[0];
            --count;
        }

        return result;
    }

    void range_delete(double min_score, double max_score, int64_t offset, int64_t count) {
        std::vector<Node *> update(MAX_LEVEL_);
        auto x = head_;
        for (int i = level_ - 1; i >= 0; --i) {
            while (x->forward_[i] && x->forward_[i]->score_ < min_score) {
                x = x->forward_[i];
            }
            update[i] = x;
        }
        x = x->forward_[0];

        while (x && offset > 0) {
            x = x->forward_[0];
            --offset;
        }

        while (x && x->score_ <= max_score && count > 0) {
            auto next = x->forward_[0];
            for (int i = 0; i < level_; ++i) {
                if (update[i]->forward_[i] != x) {
                    break;
                }
                update[i]->forward_[i] = x->forward_[i];
            }
            delete x;
            --count;
            while (level_ > 1 && head_->forward_[level_ - 1] == nullptr) {
                --level_;
            }
            x = next;
        }
    }

    std::vector<std::pair<std::string, double>> query(double min_score, const std::string &min_member,
                                                      double max_score, const std::string &max_member,
                                                      int64_t offset, int64_t count) {
        std::vector<std::pair<std::string, double>> result;
        auto x = head_;

        for (int i = level_ - 1; i >= 0; --i) {
            while (x->forward_[i] && (x->forward_[i]->score_ < min_score ||
                                      (x->forward_[i]->score_ == min_score && x->forward_[i]->member_ < min_member))) {
                x = x->forward_[i];
            }
        }
        x = x->forward_[0];

        while (x && offset > 0) {
            x = x->forward_[0];
            --offset;
        }

        while (x && count > 0 && (x->score_ < max_score ||
                                  (x->score_ == max_score && x->member_ <= max_member))) {
            result.emplace_back(x->member_, x->score_);
            x = x->forward_[0];
            --count;
        }

        return result;
    }


};