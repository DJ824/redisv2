# Redis Clone

This project is a comprehensive clone of Redis, implementing a wide range of functionality found in a Redis server. It uses a client-server architecture and is built with C++ and Boost.Asio for networking.

## Features

- Multiple data structure implementations:
  - Sorted Sets (ZSETs) using Skip List
  - Strings
  - Lists
  - Sets
  - Hashes
- Server-client architecture using Boost.Asio
- Support for various operations on each data structure
- Comprehensive unit tests using Google Test

## Design Overview

The project is structured into several key components:

1. **Server**: Handles client connections and requests using Boost.Asio for asynchronous I/O.
2. **Client**: Provides a command-line interface for sending requests to the server.
3. **DataStore**: Manages the in-memory data storage for all supported data structures.
4. **SkipList**: Implements the core data structure for efficient sorted set operations.

### Data Structures

1. **Sorted Sets (ZSETs)**: Implemented using Skip List for efficient sorted operations.
2. **Strings**: Simple key-value storage for string data.
3. **Lists**: Doubly linked lists for fast insertion and deletion at both ends.
4. **Sets**: Unordered collections of unique elements.
5. **Hashes**: Hash tables storing fields and values.

## Time Complexities

Here are the time complexities for the main operations:

### Sorted Sets (ZSETs)
- ZADD: O(log N)
- ZREM: O(log N)
- ZSCORE: O(log N)
- ZRANGE: O(log N + M)
- ZQUERY: O(log N + M)

### Strings
- GET/SET: O(1)
- INCR/DECR: O(1)

### Lists
- LPUSH/RPUSH: O(1)
- LPOP/RPOP: O(1)
- LRANGE: O(N)

### Sets
- SADD/SREM: O(1)
- SISMEMBER: O(1)
- SINTER: O(N * M) where N is the size of the smallest set

### Hashes
- HSET/HGET: O(1)
- HINCRBY: O(1)

## Supported Commands

### Sorted Sets (ZSETs)
- `ZADD key score member`
- `ZREM key member`
- `ZSCORE key member`
- `ZRANGE key min_score max_score offset count`
- `ZQUERY key min_score min_member max_score max_member offset count`

### Strings
- `SET key value`
- `GET key`
- `DEL key`
- `INCRBY key increment`

### Lists
- `LPUSH key value`
- `RPUSH key value`
- `LPOP key`
- `RPOP key`
- `LLEN key`
- `LRANGE key start stop`
- `LTRIM key start stop`

### Sets
- `SADD key member`
- `SREM key member`
- `SISMEMBER key member`
- `SINTER key [key ...]`
- `SCARD key`

### Hashes
- `HSET key field value`
- `HGET key field`
- `HMGET key field [field ...]`
- `HINCRBY key field increment`

## Future Improvements

- Implement persistence (saving to disk)
- Add support for more Redis commands and data types
- Implement pub/sub functionality
- Add authentication and access control
- Optimize memory usage
- Implement distributed system features (replication, sharding)

