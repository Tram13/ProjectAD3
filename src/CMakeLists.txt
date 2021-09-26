cmake_minimum_required(VERSION 3.16.3)

project(openseekmap)

set(CMAKE_CXX_STANDARD 18)

set(GCC_COVERAGE_COMPILE_FLAGS "-Werror -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(SOURCE_FILES main.c DatabaseReader/DatabaseReader.c DatabaseReader/DatabaseReader.h Helpers/ErrorHandler/ErrorHandler.c Helpers/ErrorHandler/ErrorHandler.h Helpers/Printer/Printer.c Helpers/Printer/Printer.h SubSetGenerator/SubSetGenerator.c SubSetGenerator/SubSetGenerator.h InputReader/InputReader.c InputReader/InputReader.h Helpers/MemoryUsageMeter/MemoryUsageMeter.c Helpers/MemoryUsageMeter/MemoryUsageMeter.h Helpers/Timer/Timer.c Helpers/Timer/Timer.h Searcher/Searcher.c Searcher/Searcher.h ArgumentParser/ArgumentParser.c ArgumentParser/ArgumentParser.h ScoreCalculator/ScoreCalculator.c ScoreCalculator/ScoreCalculator.h Helpers/UTF8/accents.h Helpers/UTF8/accents.c Helpers/UTF8/utf8.h Helpers/UTF8/utf8.c)

add_executable(openseekmap ${SOURCE_FILES})
target_link_libraries(openseekmap m)