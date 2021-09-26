cmake_minimum_required(VERSION 3.16.3)

project(tests)

set(CMAKE_CXX_STANDARD 18)

set(GCC_COVERAGE_COMPILE_FLAGS "-Werror -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(SOURCE_FILES test_main.c ../../src/DatabaseReader/DatabaseReader.c ../../src/DatabaseReader/DatabaseReader.h ../../src/Helpers/ErrorHandler/ErrorHandler.c ../../src/Helpers/ErrorHandler/ErrorHandler.h ../../src/Helpers/Printer/Printer.c ../../src/Helpers/Printer/Printer.h ../../src/SubSetGenerator/SubSetGenerator.c ../../src/SubSetGenerator/SubSetGenerator.h ../../src/InputReader/InputReader.c ../../src/InputReader/InputReader.h ../../src/Helpers/MemoryUsageMeter/MemoryUsageMeter.c ../../src/Helpers/MemoryUsageMeter/MemoryUsageMeter.h ../../src/Helpers/Timer/Timer.c ../../src/Helpers/Timer/Timer.h ../../src/Searcher/Searcher.c ../../src/Searcher/Searcher.h ../../src/ArgumentParser/ArgumentParser.c ../../src/ArgumentParser/ArgumentParser.h ../../src/ScoreCalculator/ScoreCalculator.c ../../src/ScoreCalculator/ScoreCalculator.h ../../src/Helpers/UTF8/accents.h ../../src/Helpers/UTF8/accents.c ../../src/Helpers/UTF8/utf8.h ../../src/Helpers/UTF8/utf8.c Tester1.c Tester1.h Tester2.c Tester2.h)

add_executable(tests ${SOURCE_FILES})
target_link_libraries(tests m)