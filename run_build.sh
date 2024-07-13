#!/bin/bash
# -*- coding: utf-8 -*-
# 初次配置项目
meson setup builddir

# 构建项目
meson compile -C builddir

# 运行 doctest 测试
meson test -C builddir

# 运行 sample, 可执行文件位于 builddir 下
./builddir/sample

