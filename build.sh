#!/bin/bash
# -*- coding: utf-8 -*-

export CXX=g++-11
export CXX_STD=17

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") >/dev/null && pwd)

export BUILD_DIR=${SCRIPT_DIR}/build
export SCC_SRC_DIR=${SCRIPT_DIR}/sc_tools/scc
#export SYSTEMC_DIR=${SCRIPT_DIR}/external/systemc

export SC_VERSION=2.3.4
#export SYSTEMC_HOME=${BUILD_DIR}/systemc
#export SYSTEMCAMS_HOME=${BUILD_DIR}/systemc
export SCC_BUILD_DIR=${BUILD_DIR}/scc

[ -z "${BUILD_TYPE}" ] && BUILD_TYPE=RelWithDebInfo                                                                                                         
CMD="Usage: build dbg|rel|reldbg|clean" 

if [ ! -z "$1" ]; then
    if [ $1 == dbg ]; then
      	BUILD_TYPE=Debug
        CMD="build $1"
    elif [ $1 == rel ]; then
        BUILD_TYPE=Debug
        CMD="build $1"
    elif [ $1 == clean ]; then
        CMD="clean"
    else
        CMD=$1
    fi
else
    CMD="build"
fi

CMAKE_BUILD_TYPE="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
CMAKE_COMMON_SETTINGS="-DBUILD_SHARED_LIBS=ON -DCMAKE_CXX_STANDARD=${CXX_STD}"
CMAKE_COMMON_PREFIX="-DCMAKE_INSTALL_PREFIX=$BUILD_DIR -DCMAKE_INSTALL_LIBDIR=lib"
#rm -rf builddir


#build_systemc() {
	#cmake -S ${SYSTEMC_DIR} -B ${SYSTEMC_HOME} ${CMAKE_BUILD_TYPE} ${CMAKE_COMMON_SETTINGS} -DCMAKE_INSTALL_PREFIX=${SYSTEMC_HOME} -DCMAKE_INSTALL_LIBDIR=lib -DENABLE_PHASE_CALLBACKS_TRACING=OFF || exit 1
	#cmake --build ${SYSTEMC_HOME} -j 20 --target install || exit 2
#}

build_scc() {
		#-DBoost_NO_SYSTEM_PATHS=TRUE -DBOOST_ROOT=${BOOST_INSTALL} -DBoost_NO_WARN_NEW_VERSIONS=ON 
	cmake -S ${SCC_SRC_DIR} -B ${SCC_BUILD_DIR} -Wno-dev ${CMAKE_BUILD_TYPE} ${CMAKE_COMMON_SETTINGS} ${CMAKE_COMMON_PREFIX} \
		-DSCC_LIB_ONLY=ON -DENABLE_CONAN=OFF || exit 1
	cmake --build ${SCC_BUILD_DIR} --target install -j 20 || exit 2
}

build_nose() {
	meson setup $BUILD_DIR
	ninja -v -C $BUILD_DIR 2>&1 |tee ninja.log
}

clean() {
    rm -rf $BUILD_DIR
}

build() {
    buildopt=$1
    #build_systemc
    build_scc
    build_nose
}

echo "running $CMD"
$CMD
