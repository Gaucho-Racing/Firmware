To Do:
Check head and tail at every test
- Test Inspection functions
    - get capacity
    - get occupied size
    - check if full or empty
- Test Initialization
- Test Push/Pop
    - Pop empty buffer --> should do nothing
    - Push full buffer --> test data overwrite
    - Push buffer past size limite --> test data loopback
- Test Peek
- Test Memory stuff (Address Sanitizer + Maybe Valgrind)
    - Test free
    - Make sure memory overwrite free's the old data
