#include <gtest/gtest.h>
#include "../structures/data_store.cpp"
#include "../structures/skip_list.cpp"
#include <cmath>

class SkipListTest : public ::testing::Test {
protected:
    SkipList list;
};

TEST_F(SkipListTest, InsertAndScore) {
EXPECT_TRUE(list.insert("a", 1.0));
EXPECT_TRUE(list.insert("b", 2.0));
EXPECT_TRUE(list.insert("c", 3.0));

EXPECT_EQ(list.score("a"), 1.0);
EXPECT_EQ(list.score("b"), 2.0);
EXPECT_EQ(list.score("c"), 3.0);
}

TEST_F(SkipListTest, InsertDuplicate) {
EXPECT_TRUE(list.insert("a", 1.0));
EXPECT_FALSE(list.insert("a", 2.0));
EXPECT_EQ(list.score("a"), 2.0);
}

TEST_F(SkipListTest, Remove) {
EXPECT_TRUE(list.insert("a", 1.0));
EXPECT_TRUE(list.insert("b", 2.0));

EXPECT_TRUE(list.remove("a"));
EXPECT_FALSE(list.score("a").has_value());
EXPECT_TRUE(list.score("b").has_value());

EXPECT_FALSE(list.remove("c"));
}

TEST_F(SkipListTest, Range) {
list.insert("a", 1.0);
list.insert("b", 2.0);
list.insert("c", 3.0);
list.insert("d", 4.0);

auto result = list.range(1.5, 3.5, 0, 10);
EXPECT_EQ(result.size(), 2);
EXPECT_EQ(result[0].first, "b");
EXPECT_EQ(result[1].first, "c");

result = list.range(0.0, 5.0, 1, 2);
EXPECT_EQ(result.size(), 2);
EXPECT_EQ(result[0].first, "b");
EXPECT_EQ(result[1].first, "c");
}

TEST_F(SkipListTest, RangeDelete) {
list.insert("a", 1.0);
list.insert("b", 2.0);
list.insert("c", 3.0);
list.insert("d", 4.0);

list.range_delete(1.5, 3.5, 0, 10);

EXPECT_TRUE(list.score("a").has_value());
EXPECT_FALSE(list.score("b").has_value());
EXPECT_FALSE(list.score("c").has_value());
EXPECT_TRUE(list.score("d").has_value());
}

TEST_F(SkipListTest, Query) {
list.insert("a", 1.0);
list.insert("b", 2.0);
list.insert("c", 3.0);
list.insert("d", 4.0);

auto result = list.query(1.5, "a", 3.5, "d", 0, 10);
EXPECT_EQ(result.size(), 2);
EXPECT_EQ(result[0].first, "b");
EXPECT_EQ(result[1].first, "c");

result = list.query(0.0, "a", 5.0, "d", 1, 2);
EXPECT_EQ(result.size(), 2);
EXPECT_EQ(result[0].first, "b");
EXPECT_EQ(result[1].first, "c");
}

class DataStoreTest : public ::testing::Test {
protected:
    DataStore store;
};

TEST_F(DataStoreTest, ZAdd) {
EXPECT_TRUE(store.zadd("myset", 1.0, "a"));
EXPECT_FALSE(store.zadd("myset", 2.0, "a"));
EXPECT_TRUE(store.zadd("myset", 3.0, "b"));
}

TEST_F(DataStoreTest, ZRem) {
store.zadd("myset", 1.0, "a");
store.zadd("myset", 2.0, "b");

EXPECT_TRUE(store.zrem("myset", "a"));
EXPECT_FALSE(store.zrem("myset", "c"));
EXPECT_FALSE(store.zrem("nonexistent", "a"));
}

TEST_F(DataStoreTest, ZScore) {
store.zadd("myset", 1.0, "a");
store.zadd("myset", 2.0, "b");

auto score_a = store.zscore("myset", "a");
EXPECT_TRUE(score_a.has_value());
EXPECT_DOUBLE_EQ(*score_a, 1.0);

auto score_c = store.zscore("myset", "c");
EXPECT_FALSE(score_c.has_value());

auto score_nonexistent = store.zscore("nonexistent", "a");
EXPECT_FALSE(score_nonexistent.has_value());
}

TEST_F(DataStoreTest, ZRange) {
store.zadd("myset", 1.0, "a");
store.zadd("myset", 2.0, "b");
store.zadd("myset", 3.0, "c");
store.zadd("myset", 4.0, "d");

auto result = store.zrange("myset", 1.5, 3.5, 0, 10);
EXPECT_EQ(result.size(), 2);
EXPECT_EQ(result[0].first, "b");
EXPECT_EQ(result[1].first, "c");

result = store.zrange("myset", 0.0, 5.0, 1, 2);
EXPECT_EQ(result.size(), 2);
EXPECT_EQ(result[0].first, "b");
EXPECT_EQ(result[1].first, "c");

result = store.zrange("nonexistent", 0.0, 1.0, 0, 10);
EXPECT_EQ(result.size(), 0);
}

TEST_F(DataStoreTest, ZQuery) {
store.zadd("myset", 1.0, "a");
store.zadd("myset", 2.0, "b");
store.zadd("myset", 3.0, "c");
store.zadd("myset", 4.0, "d");

auto result = store.zquery("myset", 1.5, "a", 3.5, "d", 0, 10);
EXPECT_EQ(result.size(), 2);
EXPECT_EQ(result[0].first, "b");
EXPECT_EQ(result[1].first, "c");

result = store.zquery("myset", 0.0, "a", 5.0, "d", 1, 2);
EXPECT_EQ(result.size(), 2);
EXPECT_EQ(result[0].first, "b");
EXPECT_EQ(result[1].first, "c");

result = store.zquery("nonexistent", 0.0, "a", 1.0, "b", 0, 10);
EXPECT_EQ(result.size(), 0);
}

TEST_F(DataStoreTest, ZRangeDel) {
store.zadd("myset", 1.0, "a");
store.zadd("myset", 2.0, "b");
store.zadd("myset", 3.0, "c");
store.zadd("myset", 4.0, "d");

store.zrange_del("myset", 1.5, 3.5, 0, 10);

EXPECT_TRUE(store.zscore("myset", "a").has_value());
EXPECT_FALSE(store.zscore("myset", "b").has_value());
EXPECT_FALSE(store.zscore("myset", "c").has_value());
EXPECT_TRUE(store.zscore("myset", "d").has_value());

store.zrange_del("nonexistent", 0.0, 1.0, 0, 10);
// Should not throw or cause any issues
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}