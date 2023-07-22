# Thread-Safe-Malloc
Used C to implement thread-safe malloc library.

### 1. Introduction: 
- Step 1: Implement 2 versions (First Fit, Best Fit) of malloc and free, each based on a different strategy for determining the memory region to allocate.
  ```C
  //First Fit malloc/free
  void *ff_malloc(size_t size);
  void ff_free(void *ptr);

  //Best Fit malloc/free
  void *bf_malloc(size_t size);
  void bf_free(void *ptr);
  ```
- Step 2: Made it thread-safe, with a locked version (pthread mutex) and a lock-free version (Thread Local Storage).
  ```C
  //Thread Safe malloc/free: locking version
  void *ts_malloc_lock(size_t size);
  void ts_free_lock(void *ptr);

  //Thread Safe malloc/free: non-locking version
  void *ts_malloc_nolock(size_t size);
  void ts_free_nolock(void *ptr);
  ```
### 2. Note: 
- Both Step 1 and Step 2 have `Report.pdf`, whcih include code logic and results & analysis of performance experiments.
