cmake_minimum_required(VERSION 3.5)

project(contracts)

set(LEDGIS_WASM_OLD_BEHAVIOR "off")
find_package(eosio.cdt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${TARGET_NETWORK}")

add_subdirectory(project_contracts)