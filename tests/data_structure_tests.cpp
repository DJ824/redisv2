#include <gtest/gtest.h>
#include "../structures/data_store.cpp"

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
}


TEST_F(DataStoreTest, StringSetGet) {
    store.string_set("key1", "value1");
    auto result = store.string_get("key1");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, "value1");

    result = store.string_get("nonexistent");
    EXPECT_FALSE(result.has_value());
}

TEST_F(DataStoreTest, StringDel) {
    store.string_set("key1", "value1");
    EXPECT_TRUE(store.string_del("key1"));
    EXPECT_FALSE(store.string_get("key1").has_value());
    EXPECT_FALSE(store.string_del("nonexistent"));
}

TEST_F(DataStoreTest, Incrby) {
    store.string_set("counter", "10");
    int amt = 5;
    auto result = store.incrby("counter", amt);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 15);

    store.string_set("non_number", "abc");
    result = store.incrby("non_number", amt);
    EXPECT_FALSE(result.has_value());

    result = store.incrby("nonexistent", amt);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, amt);

    result = store.incrby("counter", -3);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 12);

    store.string_set("large_number", "9223372036854775800");
    result = store.incrby("large_number", 5);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 9223372036854775805LL);

}


TEST_F(DataStoreTest, LPushRPush) {
    store.lpush("mylist", "a");
    store.rpush("mylist", "b");
    store.lpush("mylist", "c");

    EXPECT_EQ(store.llen("mylist"), 3);
}

TEST_F(DataStoreTest, LPopRPop) {
    store.rpush("mylist", "a");
    store.rpush("mylist", "b");
    store.rpush("mylist", "c");

    auto result = store.lpop("mylist");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, "a");

    result = store.rpop("mylist");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, "c");

    EXPECT_EQ(store.llen("mylist"), 1);

    result = store.lpop("nonexistent");
    EXPECT_FALSE(result.has_value());
}

TEST_F(DataStoreTest, LMove) {
    store.rpush("list1", "a");
    store.rpush("list1", "b");
    store.rpush("list1", "c");

    auto result = store.lmove("list1", "list2", "LEFT", "RIGHT");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, "a");

    result = store.lmove("list1", "list2", "RIGHT", "LEFT");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, "c");

    EXPECT_EQ(store.llen("list1"), 1);
    EXPECT_EQ(store.llen("list2"), 2);

    result = store.lmove("nonexistent", "list2", "LEFT", "RIGHT");
    EXPECT_FALSE(result.has_value());
}

TEST_F(DataStoreTest, LRange) {
    store.rpush("mylist", "a");
    store.rpush("mylist", "b");
    store.rpush("mylist", "c");
    store.rpush("mylist", "d");

    auto result = store.lrange("mylist", 1, 2);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 2);
    EXPECT_EQ((*result)[0], "b");
    EXPECT_EQ((*result)[1], "c");

    result = store.lrange("nonexistent", 0, -1);
    EXPECT_FALSE(result.has_value());
}

TEST_F(DataStoreTest, LTrim) {
    store.rpush("mylist", "a");
    store.rpush("mylist", "b");
    store.rpush("mylist", "c");
    store.rpush("mylist", "d");

    EXPECT_TRUE(store.ltrim("mylist", 1, 2));

    auto range = store.lrange("mylist", 0, -1);
    EXPECT_TRUE(range.has_value());
    EXPECT_EQ(range->size(), 2) << "List size after trim is incorrect";

    if (range->size() == 2) {
        EXPECT_EQ((*range)[0], "b") << "First element after trim is incorrect";
        EXPECT_EQ((*range)[1], "c") << "Second element after trim is incorrect";
    }

    EXPECT_FALSE(store.ltrim("nonexistent", 0, 1));
}

TEST_F(DataStoreTest, SAdd) {
    auto result = store.sadd("myset", "a");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 1);

    result = store.sadd("myset", "a");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);

    result = store.sadd("myset", "b");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 1);
}

TEST_F(DataStoreTest, SRem) {
    store.sadd("myset", "a");
    store.sadd("myset", "b");

    auto result = store.srem("myset", "a");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 1);

    result = store.srem("myset", "c");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);

    result = store.srem("nonexistent", "a");
    EXPECT_FALSE(result.has_value());
}

TEST_F(DataStoreTest, SIsMember) {
    store.sadd("myset", "a");
    store.sadd("myset", "b");

    auto result = store.sismember("myset", "a");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 1);

    result = store.sismember("myset", "c");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);

    result = store.sismember("nonexistent", "a");
    EXPECT_FALSE(result.has_value());
}

TEST_F(DataStoreTest, SInter) {
    store.sadd("set1", "a");
    store.sadd("set1", "b");
    store.sadd("set1", "c");
    store.sadd("set2", "b");
    store.sadd("set2", "c");
    store.sadd("set2", "d");

    auto result = store.sinter({"set1", "set2"});
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 2);
    EXPECT_TRUE(std::find(result->begin(), result->end(), "b") != result->end());
    EXPECT_TRUE(std::find(result->begin(), result->end(), "c") != result->end());

    result = store.sinter({"set1", "nonexistent"});
    EXPECT_FALSE(result.has_value());
}

TEST_F(DataStoreTest, SCard) {
    store.sadd("myset", "a");
    store.sadd("myset", "b");
    store.sadd("myset", "c");

    EXPECT_EQ(store.scard("myset"), 3);
    EXPECT_EQ(store.scard("nonexistent"), 0);
}


TEST_F(DataStoreTest, HSet) {
    std::vector<std::pair<std::string, std::string>> fields = {{"field1", "value1"}, {"field2", "value2"}};
    auto result = store.hset("myhash", fields);
    EXPECT_EQ(result, 2);

    fields = {{"field1", "newvalue1"}, {"field3", "value3"}};
    result = store.hset("myhash", fields);
    EXPECT_EQ(result, 1);
}

TEST_F(DataStoreTest, HGet) {
    store.hset("myhash", {{"field1", "value1"}, {"field2", "value2"}});

    auto result = store.hget("myhash", "field1");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, "value1");

    result = store.hget("myhash", "nonexistent");
    EXPECT_FALSE(result.has_value());

    result = store.hget("nonexistent", "field1");
    EXPECT_FALSE(result.has_value());
}

TEST_F(DataStoreTest, HMGet) {
    store.hset("myhash", {{"field1", "value1"}, {"field2", "value2"}, {"field3", "value3"}});

    auto result = store.hmget("myhash", {"field1", "field2", "nonexistent"});
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 2);
    EXPECT_EQ((*result)[0], "value1");
    EXPECT_EQ((*result)[1], "value2");

    result = store.hmget("nonexistent", {"field1"});
    EXPECT_FALSE(result.has_value());
}

TEST_F(DataStoreTest, HIncrBy) {
    store.hset("myhash", {{"counter", "10"}});

    auto result = store.hincrby("myhash", "counter", 5);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 15);

    result = store.hincrby("myhash", "nonexistent", 5);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 5);

    store.hset("myhash", {{"non_number", "abc"}});
    result = store.hincrby("myhash", "non_number", 5);
    EXPECT_FALSE(result.has_value());

    result = store.hincrby("nonexistent", "counter", 5);
    EXPECT_FALSE(result.has_value());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}