cmake_minimum_required(VERSION 3.10)
project(ACO_BankSorting C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c ACO_API/API.c ACO_API/API.h bagAPI.h)

add_executable(ACO_BankSorting ${SOURCE_FILES})