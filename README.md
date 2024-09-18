# Redis Clone

This project is a simplified clone of Redis, implementing some of the basic functionality of a Redis server with a focus on sorted sets (ZSETs). It uses a client-server architecture and is built with C++ and Boost.Asio for networking.

## Features

- Sorted Set (ZSET) implementation using a Skip List data structure
- Server-client architecture using Boost.Asio
- Support for basic ZSET operations: ZADD, ZREM, ZSCORE, ZRANGE, ZQUERY
- Unit tests using Google Test

## Design Overview

The project is structured into several key components:

1. **Server**: Handles client connections and requests using Boost.Asio for asynchronous I/O.
2. **Client**: Provides a command-line interface for sending requests to the server.
3. **DataStore**: Manages the in-memory data storage, particularly the ZSETs.
4. **SkipList**: Implements the core data structure for efficient sorted set operations.

### Skip List Implementation

The Skip List is used as the underlying data structure for ZSETs, providing efficient insertion, deletion, and range query operations. 
It maintains multiple layers of linked lists, with each layer being a subset of the layer below, allowing for faster traversal.

## Time Complexities

Here are the time complexities for the main operations:

- **ZADD** (Insert/Update): O(log N) average case, where N is the number of elements in the sorted set.
  - In the worst case (when inserting an element with the highest level), it can be O(N), but this is rare due to the probabilistic nature of the skip list.

- **ZREM** (Remove): O(log N) average and worst case.

- **ZSCORE** (Get score): O(log N) average and worst case.

- **ZRANGE** (Range query): O(log N + M), where N is the total number of elements and M is the number of elements in the specified range.

- **ZQUERY** (Complex range query): O(log N + M), where N is the total number of elements and M is the number of elements in the result set.

The space complexity of the Skip List is O(N) on average, with an additional O(log N) space for the multiple levels.

## Supported Commands

The following ZSET commands are supported:

1. `ZADD key score member`: Add a member with the specified score to the sorted set.
2. `ZREM key member`: Remove the specified member from the sorted set.
3. `ZSCORE key member`: Get the score of the specified member in the sorted set.
4. `ZRANGE key min_score max_score offset count`: Get members within the specified score range.
5. `ZQUERY key min_score min_member max_score max_member offset count`: Complex range query on the sorted set.

## Future Improvements

- Implement persistence (saving to disk)
- Add support for more Redis data types (Lists, Hashes, etc.)
- Implement pub/sub functionality
- Add authentication and access control
