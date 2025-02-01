cmake_minimum_required(VERSION 3.10)
project(nose VERSION 0.1.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置警告级别
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-return-type")

# 添加编译选项，关闭特定警告
add_compile_options(
  -Wno-variadic-macros
  -Wno-unused-variable
  -ftemplate-backtrace-limit=0
)

# 查找 doctest 库
find_package(doctest QUIET)
if(NOT doctest_FOUND)
  include_directories(external/doctest)
  add_library(doctest INTERFACE)
  target_sources(doctest INTERFACE external/doctest/doctest/parts/doctest.cpp)
endif()

add_subdirectory(external/reflect-cpp) # Add this project as a subdirectory

# 添加其他头文件路径
include_directories(
  external/reflect-cpp/include
  external/refl-cpp/include
  external/pybind11/include
  src/core/visitor
  src/core/loader
  src/core
  src/utils
  src/system
  src/system/graph
  external/cpputils
)


# 列出需要编译到动态库中的源文件
set(core_files
  src/core/TypeManager.h
  src/core/Port.h
  src/core/Port.cpp
  src/core/Bundle.h
  src/core/Wire.h
  src/core/Channel.h
  src/core/Clock.h
  src/core/Component.h
  src/core/Tree.h
  src/core/Entity.h
  src/core/Entity.cpp
  src/core/ObjPath.h
  src/core/Storage.h
  src/core/Registry.h
  src/core/Object.h
  src/core/ObjectRflParser.h
  src/core/loader/IniLoader.h
  src/core/visitor/Visitor.h
  src/core/visitor/ObjectBuildVisitor.h
  src/core/visitor/ComponentBindVisitor.h
  src/core/visitor/PortBindVisitor.h
  src/core/visitor/ChannelBindVisitor.h
)

set(src_files
  ${core_files}
  #src/core/Registry.cpp
)

# 建立动态库
add_library(nose SHARED ${src_files})
target_include_directories(nose PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nose PUBLIC reflectcpp)

# 编译 main.cpp 作为 sample 可执行文件
add_executable(sample samples/core/main.cpp)
target_link_libraries(sample PRIVATE nose)

# 编译和运行测试用例
option(BUILD_DOCTEST "Build with doctest support" ON)
if(BUILD_DOCTEST)
  set(test_files
    tests/test_property.cpp
    tests/test_basic.cpp
    tests/test_port.cpp
    tests/test_wire.cpp
    tests/test_component.cpp
    tests/test_channel.cpp
    tests/test_component_bind.cpp
    tests/test_typemanager.cpp
  )

  add_executable(tests ${test_files})
  target_link_libraries(tests PRIVATE nose)
  #add_test(NAME test_basic COMMAND tests)
endif()

