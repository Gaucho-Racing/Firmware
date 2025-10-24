To Do:
Check head and tail at every test
- [X] Test Inspection functions
    - [x] Get capacity
    - [x] get occupied size
    - [x] check if full or empty
- [X] Test Initialization
- [x] Test Push/Pop
    - [x] Pop empty buffer --> should do nothing
    - [x] Push full buffer --> test data overwrite
    - [x] Push buffer past size limite --> test data loopback
- [x] Test Peek
- Test Memory stuff (Address Sanitizer + Maybe Valgrind)
    - Test free
    - Make sure memory overwrite free's the old data
