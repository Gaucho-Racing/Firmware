# CSV-25
CSV interpreters for CAN IDs and such that spit out #defines

## Modify Top-Level CMakeLists.txt
Put the following after the last line

```cmake
# Add CSV-25 integration
add_dependencies(${CMAKE_PROJECT_NAME} executeCSV25)
add_custom_target(
    executeCSV25
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/CSV-25/executeCSV.sh
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/grCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/GR-ID.csv
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/msgCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/MSG-ID.csv
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/customIdCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSV-25/CUSTOM-ID.csv
    COMMENT "Updating GR ID and MSG ID header files from CSVs"
    USES_TERMINAL
)
```
