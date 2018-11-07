export STDFLAG="-std=c++2a"                                    # must be c++14 or c+17 or c++2a
#export GCC_ERROR_IGNORE="-Wno-deprecated -Wno-register"        # we get some warnings (which are turned into errors) with c++17
#export GCC_USE_GLIBCXX11_ABI="-D_GLIBCXX_USE_CXX11_ABI=0"      # gcc5+ uses a different ABI - we must specify this to avoid link errors

export BOOST_INC=/usr/include                                  # location of boost header files
export BOOST_LIB=/usr/lib64                                    # location of boost libs

export INSTALL_HOME=~/installs                                 # local installations of gcc+libs+headers
export INSTALL_HOME_CLANG=~/installs-clang                     # ... for clang/llvm ...

export PATH=${INSTALL_HOME_CLANG}/bin:${INSTALL_HOME}/bin:${PATH}
export LD_LIBRARY_PATH=${INSTALL_HOME_CLANG}/lib:${INSTALL_HOME}/lib64:${INSTALL_HOME}/lib:${LD_LIBRARY_PATH}
#export PATH=${INSTALL_HOME}/bin:${INSTALL_HOME_CLANG}/bin:${PATH}
#export LD_LIBRARY_PATH=${INSTALL_HOME}/lib64:${INSTALL_HOME}/lib:${LD_LIBRARY_PATH}:${INSTALL_HOME_CLANG}/lib:${LD_LIBRARY_PATH}

# build environment root
export ENV_ROOT=`pwd`
export PROJECT_ROOT=${ENV_ROOT}/..
