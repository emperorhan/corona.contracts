add_contract(corona corona
    ${CMAKE_CURRENT_SOURCE_DIR}/src/corona.cpp
)

target_include_directories(corona
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils
)

set_target_properties(corona
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
