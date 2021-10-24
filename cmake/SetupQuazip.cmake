cmake_minimum_required(VERSION 3.16)

include(FetchContent)

#for building statically
set(OLD_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS NO)

FetchContent_Declare(quazip
	GIT_REPOSITORY https://github.com/stachenov/quazip.git
	GIT_TAG v1.1)

FetchContent_MakeAvailable(quazip)


set(BUILD_SHARED_LIBS ${OLD_BUILD_SHARED_LIBS})
