add_contract(
    project_contracts project_contracts
    ${CMAKE_CURRENT_SOURCE_DIR}/src/project_contracts.cpp
)

target_include_directories(
    project_contracts
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRNET_SOURCE_DIR}/../utils
)

set_target_properties(
    project_contracts
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)