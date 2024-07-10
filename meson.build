project('nose', 'cpp',
  version: '0.1.0',
  default_options: ['warning_level=3', 'cpp_std=c++17'])

# 插件和依赖项的定义
doctest_dep = dependency('doctest', required: false)
doctest_inc = include_directories('external/doctest/doctest')
if not doctest_dep.found()
  doctest_dep = declare_dependency(
    include_directories: doctest_inc,
    source: ['external/doctest/doctest/parts/doctest.cpp']
  )
endif

refl_cpp_inc = include_directories('external/refl-cpp/include')

src_inc = include_directories('src/core', 'src/core/visitor', 'src/core/loader')

all_includes = [src_inc, refl_cpp_inc, doctest_inc]

# 列出需要编译到动态库中的源文件
src_files = files(
  'src/core/TypeManager.h',
  'src/core/IPort.h',
  'src/core/Port.h',
  'src/core/Wire.h',
  'src/core/Channel.h',
  'src/core/Clock.h',
  'src/core/Component.h',
  'src/core/Tree.h',
  'src/core/Node.h',
  'src/core/String.h',
  'src/core/String.cpp',
  'src/core/Storage.h',
  'src/core/Registry.h',
  'src/core/Registry.cpp',
  'src/core/loader/IniLoader.h',
  'src/core/visitor/Visitor.h',
  'src/core/visitor/ObjectBuildVisitor.h',
  'src/core/visitor/ComponentBindVisitor.h',
  'src/core/visitor/PortBindVisitor.h',
  'src/core/visitor/ChannelBindVisitor.h'
)

# 建立动态库
libnose = shared_library('nose', src_files, install: true, include_directories: all_includes)

# 编译 main.cpp 作为 sample 可执行文件
sample_executable = executable('sample', 'samples/core/main.cpp', include_directories: all_includes, link_with: libnose)

# 编译和运行测试用例
if get_option('doctest')
  test_files = files(
    'tests/test_main.cpp'
  )

  test_executable = executable('tests', test_files, 
                                include_directories: [all_includes, doctest_inc],
                                link_with: libnose,
                               dependencies: [doctest_dep])
  test('doctest', test_executable)
endif
