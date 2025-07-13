# CSVGenerator
CSV interpreters for CAN IDs and such that generate C header files with #defines

## Modify Top-Level CMakeLists.txt
Put the following after the last line

```cmake
# Add CSVGenerator integration
add_dependencies(${CMAKE_PROJECT_NAME} executeCSV25)
add_custom_target(
    executeCSV25
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/CSVGenerator/executeCSV.sh
    DEPENDS ${CMAKE_SOURCE_DIR}/CSVGenerator/grCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSVGenerator/GR-ID.csv
    DEPENDS ${CMAKE_SOURCE_DIR}/CSVGenerator/msgCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSVGenerator/MSG-ID.csv
    DEPENDS ${CMAKE_SOURCE_DIR}/CSVGenerator/customIdCSVtoH.py
    DEPENDS ${CMAKE_SOURCE_DIR}/CSVGenerator/CUSTOM-ID.csv
    COMMENT "Updating GR ID and MSG ID header files from CSVs"
    USES_TERMINAL
)
```
