# Converter test suite. Included by the top-level CMakeLists (enable_testing()
# + add_subdirectory(tests)); also usable from a standalone test build.
add_executable(FileToByteTests test_convert.cpp ../source/FTB.cpp)

target_include_directories(FileToByteTests PRIVATE ../source)
target_compile_features(FileToByteTests PRIVATE cxx_std_23)

# Same warning level as the app target.
if(MSVC)
  target_compile_options(FileToByteTests PRIVATE /W4)
else()
  target_compile_options(FileToByteTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME convert COMMAND FileToByteTests)
