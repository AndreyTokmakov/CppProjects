# Randomized Set

## Problem Statement

Implement a data structure called `RandomizedSet` that stores a collection of **unique values** and supports the following operations:

- `insert(value)` — inserts a value into the collection if it does not already exist.
- `remove(value)` — removes a value from the collection if it exists.
- `contains(value)` — checks whether a value exists in the collection.
- `getRandom()` — returns a randomly selected value from the collection, with every stored value having an equal probability of being selected.

All operations must have **average O(1) time complexity**.

If an insertion or removal cannot be performed because the value already exists or does not exist, respectively, the operation should return `false`. Otherwise, it should return `true`.

If the collection is empty, `getRandom()` should indicate that no value is available.

## Implementation Requirements

The implementation should use two data structures internally:

1. `std::vector<T>` — stores all values contiguously and provides constant-time access by index.
2. `std::unordered_map<T, std::size_t>` — maps each value to its corresponding index in the vector.

The implementation must maintain the following invariant:

**For every value stored in the vector, the hash map contains that value and its correct index in the vector.**

The vector must not contain duplicate values.

## Implementation Details

### 1. Insert

To insert a value:

1. Attempt to insert the value into the hash map using `try_emplace()`.
2. If the value already exists, return `false`.
3. Otherwise, associate the value with the current size of the vector.
4. Append the value to the vector.
5. Return `true`.

Using `try_emplace()` avoids performing a separate lookup before inserting into the hash map.

Care must be taken to maintain consistency between the vector and the hash map if an operation throws an exception.

### 2. Remove

Removing an arbitrary element from a vector normally requires shifting subsequent elements, resulting in O(n) time complexity.

To avoid this, use the following approach:

1. Find the value in the hash map.
2. If the value does not exist, return `false`.
3. Obtain the index of the value in the vector.
4. If the value is not the last element:
    - Find the last element in the vector.
    - Update its index in the hash map to the position of the element being removed.
    - Move the last element into the position of the element being removed.
5. Remove the last element from the vector.
6. Erase the removed value from the hash map.
7. Return `true`.

This approach avoids shifting elements and allows removal in average O(1) time.

The order of elements in the vector is not preserved, which is acceptable because the data structure does not guarantee any particular ordering.

### 3. Contains

Use `std::unordered_map::find()` to check whether the value exists.

If the value is found, return `true`; otherwise, return `false`.

No traversal of the vector is necessary.

### 4. Get Random

To return a random element:

1. Check whether the vector is empty.
2. If it is empty, indicate that no value is available.
3. Otherwise, generate a uniformly distributed random index in the range `[0, size - 1]`.
4. Return the element at that index.

Because every stored value occupies exactly one vector position, selecting a uniformly distributed index ensures that every value has an equal probability of being selected.

Use `std::uniform_int_distribution` with an appropriate random number generator, such as `std::mt19937`.

## Time Complexity Analysis

Let `n` be the number of elements stored in the collection.

| Operation | Average Time Complexity | Explanation |
|---|---|---|
| `insert(value)` | O(1) | Hash map insertion and amortized vector append |
| `remove(value)` | O(1) | Hash map lookup, element replacement, and deletion |
| `contains(value)` | O(1) | Hash map lookup |
| `getRandom()` | O(1) | Generate a random index and access the vector |
| `size()` | O(1) | Return the vector size |
| `empty()` | O(1) | Check whether the vector is empty |

### Why Is the Complexity Average O(1)?

`std::unordered_map` provides average constant-time lookup, insertion, and deletion. However, these operations can take O(n) time in the worst case due to hash collisions or rehashing.

Appending to `std::vector` is amortized O(1), although an individual append can take O(n) time if the vector reallocates and moves its existing elements.

Therefore, the operations have **average O(1) time complexity**, with amortized O(1) insertion under the usual assumptions about hash-table behavior.

## Space Complexity

The data structure stores each value twice in terms of logical representation:

- Once in the vector.
- Once as a key in the hash map, together with its index.

For `n` unique values, the overall auxiliary storage requirement is **O(n)**.

The vector may allocate additional unused capacity, but this does not change the asymptotic space complexity.

## Expected Interface

A generic implementation may expose the following interface:

```cpp
template<typename T>
class RandomizedSet
{
public:
    [[nodiscard]]
    bool insert(const T& value);

    [[nodiscard]]
    bool remove(const T& value);

    [[nodiscard]]
    bool contains(const T& value) const;

    [[nodiscard]]
    std::optional<T> getRandom();

    [[nodiscard]]
    std::size_t size() const noexcept;

    [[nodiscard]]
    bool empty() const noexcept;
};
```

The implementation requires `T` to be compatible with `std::unordered_map`, including hash and equality operations. The concrete requirements for copying and moving `T` depend on the implementation details.

## Key Design Considerations

- **Uniqueness:** Each value can appear only once.
- **Index consistency:** Every map entry must refer to the correct vector position.
- **Efficient removal:** Replace the removed element with the last vector element instead of shifting elements.
- **Uniform random selection:** Select a uniformly distributed vector index rather than selecting from the hash map.
- **Exception safety:** Keep the vector and hash map consistent if insertion or removal throws an exception.
- **Ordering:** Element order is not guaranteed and may change after removal.

The central idea is to combine the vector's constant-time indexed access with the hash map's average constant-time lookup, enabling efficient random selection and modification of the collection.