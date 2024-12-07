project('nose', 'cpp',
  version: '0.1.0',
  default_options: ['warning_level=3', 'cpp_std=c++20', 'cpp_args=-Wno-return-type -Wno-return-type'])

# 获取C++编译器
cpp_compiler = meson.get_compiler('cpp')
# cpp_compiler.add_compile_options('-ftemplate-backtrace-limit=0')

# 添加编译选项，关闭特定警告
common_flags = [
  '-Wno-variadic-macros',
  '-Wno-unused-variable',
  '-ftemplate-backtrace-limit=0'
]

# 应用这些选项到整个项目
add_project_arguments(common_flags, language: 'cpp')


# 插件和依赖项的定义
#doctest_dep = dependency('doctest', required: false)
doctest_inc = include_directories('external/doctest')
#doctest_src = files('external/doctest/*.cpp')
#if not doctest_dep.found()
  #doctest_dep = declare_dependency(
    #include_directories: doctest_inc,
    #source: ['external/doctest/doctest/parts/doctest.cpp']
  #)
#endif

rfl_cpp_inc = include_directories('external/reflect-cpp/include')
refl_cpp_inc = include_directories('external/refl-cpp/include')
pybind11_inc = include_directories('external/pybind11/include')

src_inc = include_directories('src/core', 'src/core/visitor', 'src/core/loader', 'src/utils', 'src/system', 'src/system/graph')

python3_headers = dependency('python3', required : true)
python3_lib = dependency('python3', required : true)


all_includes = [src_inc, rfl_cpp_inc, refl_cpp_inc, doctest_inc, pybind11_inc ]


# 列出需要编译到动态库中的源文件
core_files = files(
  'src/core/TypeManager.h',
  'src/core/IPort.h',
  'src/core/Port.h',
  'src/core/Bundle.h',
  'src/core/Wire.h',
  'src/core/Channel.h',
  'src/core/Clock.h',
  'src/core/Component.h',
  'src/core/Tree.h',
  'src/core/Entity.h',
  'src/core/Entity.cpp',
  'src/core/String.h',
  'src/core/String.cpp',
  'src/core/Storage.h',
  'src/core/Registry.h',
  'src/core/loader/IniLoader.h',
  'src/core/visitor/Visitor.h',
  'src/core/visitor/ObjectBuildVisitor.h',
  'src/core/visitor/ComponentBindVisitor.h',
  'src/core/visitor/PortBindVisitor.h',
  'src/core/visitor/ChannelBindVisitor.h'
)

src_files = core_files + files(
  'src/core/Registry.cpp',
)


# 建立动态库
libnose = shared_library('nose', core_files, install: true, include_directories: all_includes, dependencies : [python3_headers, python3_lib])

link_dependencies = dependency('threads')

# 编译 main.cpp 作为 sample 可执行文件
sample_executable = executable('sample', 'samples/core/main.cpp', include_directories: all_includes, link_with: libnose, 
  dependencies: link_dependencies)

# 编译和运行测试用例
if get_option('doctest')
  test_files = files(
    'tests/test_basic.cpp'
  )

  test_executable = executable('tests', test_files, 
                                include_directories: [all_includes],
                                link_with: libnose,
                                dependencies: link_dependencies,
                                install : false)
  test('test_basic', test_executable)
endif

