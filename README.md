
```markdown
# Hash Table / Postal Code Lookup

A hash table implementation in C using open addressing with linear probing, tombstone deletion, and dynamic resizing — built as a general-purpose key/value store, then used as the engine behind a small postal code lookup tool.

## What it does

The hash table (`ht.c`/`ht.h`) is a standalone, reusable data structure: create it, insert/lookup/remove keys, free it. It knows nothing about postal codes or cities — it just maps arbitrary string keys to arbitrary `void *` values.

`pcode.c` builds a small application on top of it: it reads a CSV file of `city,postal_code` pairs, groups postal codes by city (since one city can have many), and then lets you type a city name and see every postal code on file for it.

## Features

- Hash table with linear probing for collision resolution
- Tombstone-based deletion, so removed slots can be reused by future insertions without breaking probe chains
- Automatic resizing once the table gets roughly 2/3 full, with all existing entries rehashed into the larger table
- `ht_foreach` for applying a function to every stored key/value pair (used to clean up nested allocations before freeing the table)
- `ht_print_dist` for visualizing how clustered/spread out the table is
- A city → postal code lookup tool built entirely on the public hash table interface, with no access to its internals

## Building

```
make          # builds pcode and the three test programs
make testing  # builds and runs all three tests
make clean    # removes built binaries
```

## Running

```
./pcode cities.txt
```

The program loads the file, then reads city names from standard input (one per line) and prints the postal codes on file for each, comma-separated:

```
Edmonton
T5J1A1,T6G2R3,T5K0L4
Calgary
T2P1J9
```

Typing a city with no matches on file just produces no output for that line.

### Input file format

One `city,postal_code` pair per line:

```
Edmonton,T5J1A1
Edmonton,T6G2R3
Calgary,T2P1J9
Toronto,M5H2N2
Edmonton,T5K0L4
```

## How the hash table works

**Linear probing.** Rather than each bucket holding a linked list of collisions (separate chaining), every slot in the table holds at most one entry. On a collision, the table just checks the next slot, then the next, wrapping around, until it finds an empty one. This keeps everything in one contiguous array, which is more cache-friendly than chasing pointers around the heap.

**Tombstones.** Deleting an entry can't just mark its slot "empty" — a later lookup that has to probe past that slot to find a different key would incorrectly stop early and report "not found." So `ht_remove` marks the slot `DELETED` instead. Lookups keep probing through `DELETED` slots, and insertions are allowed to reuse the first tombstone they encounter along a key's probe chain, so deleted space doesn't sit wasted forever.

**Resizing.** Every insert checks whether the table has crossed roughly 2/3 capacity used (`(used + 1) * 3 > capacity * 2`, done with integer math to avoid floating point). If so, the table doubles-and-adds-one, allocates a fresh array, and re-inserts every live entry into it — tombstones are dropped for free in the process, since only `OCCUPIED` slots get carried over.

## Testing

Three focused test programs live in `testing/`, each targeting one part of the hash table's behavior:

- `test_ht.c` — basic insert, lookup, remove, and confirms a removed key stops being found
- `test_resize.c` — inserts enough keys to force the table through a resize, then confirms every key is still found afterward
- `test_update.c` — inserts the same key twice with different values and confirms the value is replaced rather than duplicated

All three pass cleanly under Valgrind with zero leaks and zero errors. See `testing/testing_strategy.txt` for the full write-up of what was tested and why.

## Design Notes

- **Integer-only load factor check**: instead of computing `used / capacity` as a float and comparing to `2.0/3.0`, the resize check is written as `(used + 1) * 3 > capacity * 2`. This avoids floating-point division entirely and checks the load factor *after* the pending insertion, so the table never briefly exceeds the intended threshold before resizing.

- **`used` vs `size`**: the table tracks two counts — `size` (currently occupied slots) and `used` (occupied *or* tombstoned slots). Resizing is triggered off `used`, not `size`, because a table full of tombstones from heavy insert/delete churn still needs to be resized (or really, rebuilt) even though it may have very few *live* entries — otherwise a pathological insert/delete pattern could turn every lookup into a near-linear scan through tombstones.

- **Duplicate-key insert frees the old value**: `ht_insert` checks for an existing key along the probe chain before falling back to inserting new, and if found, frees the old value before overwriting it with the new one. This keeps a repeated insert of the same key from silently leaking whatever was there before.

- **Separation between the hash table and the application**: `pcode.c` never touches the hash table's internal `struct ht` — it only uses the functions declared in `ht.h`. The city → postal-code mapping problem needed one key to hold *multiple* values, so `pcode.c` solves that on its own side by storing a small linked list (`code_list`) as the hash table's value type, rather than changing the hash table itself to support multi-value keys. This keeps the hash table generic and reusable for anything, not just this one lookup problem.

## Author

Gurshmeer Singh
```