#!/bin/bash
set -e
# -*- coding: utf-8 -*-
# 初次配置项目
export CXX=g++-11
rm -rf builddir
meson setup builddir
#meson configure builddir

# 构建项目
#meson compile -C builddir
ninja -v -C builddir 2>&1 |tee ninja.log

# 运行 doctest 测试
#meson test -C builddir

# 运行 sample, 可执行文件位于 builddir 下
#./builddir/sample

