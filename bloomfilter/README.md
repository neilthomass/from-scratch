A Bloom filter is a space-efficient probabilistic data structure that tests whether an item is a member of a set. It can return false positives ("probably in the set") but never false negatives, so a "no" answer is always certain while a "yes" answer might be wrong.

Bloom filter's are used as a quick check before an expensive operation (such as a disk read).

 - LSM-tree storage engines (LevelDB/RocksDB/Cassandra): skip reading an
 - SSTable from disk if its Bloom filter says the key isn't there.
 - Databases avoiding pointless index/disk probes for absent keys.
 - CDNs / caches: "is this object worth caching yet?" (seen-once filters).
 - Browsers (historically): checking URLs against malware blocklists.
 - Networking: deduplication, routing, packet seen-before checks.

 And my favoriate application: checking if an instagram username is available!

 