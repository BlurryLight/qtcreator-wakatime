cmake_minimum_required(VERSION 3.16)

find_package(ZLIB COMPONENTS ZLIB)

if(ZLIB_FOUND)
	message(STATUS "Done settingup zlib now available for use")
else()
	message(STATUS "Did not find zlib from system packages, grabbing from github")

	include(FetchContent)
	FetchContent_Declare( zlib
		GIT_REPOSITORY https://github.com/madler/zlib.git
		GIT_TAG v1.2.11
		)

	set(OLD_FETCHCONTENT_QUIET ${FETCHCONTENT_QUIET})
	set(FETCHCONTENT_QUIET OFF)

	# fetch zlib from github
	FetchContent_GetProperties( zlib )
	if(NOT zlib_POPULATED)
		FetchContent_Populate(zlib)
	endif()

	FetchContent_GetProperties( zlib )
	if(NOT zlib_POPULATED)
		message(FATAL_ERROR "Sorry, couldn't fetch zlib from https://github.com/madler/zlib.git")
	endif()

	# then cmake configure it here
	message(STATUS "Cmake configuring zlib")
	execute_process(COMMAND ${CMAKE_COMMAND} -B ${zlib_BINARY_DIR}/${CMAKE_BUILD_TYPE}
		-S ${zlib_SOURCE_DIR}
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DCMAKE_INSTALL_PREFIX=${zlib_BINARY_DIR}/${CMAKE_BUILD_TYPE}/InstallZlibDir
		RESULT_VARIABLE ZLIB_CMAKE_CONFIGURED)
	if(NOT ZLIB_CMAKE_CONFIGURED EQUAL 0)
		message(FATAL_ERROR "Failed to run cmake configure step on zlib")
	endif()

	# then cmake build it here
	message(STATUS "Cmake building zlib here")
	execute_process(COMMAND ${CMAKE_COMMAND} --build ${zlib_BINARY_DIR}/${CMAKE_BUILD_TYPE}
		--config ${CMAKE_BUILD_TYPE} --target install
		RESULT_VARIABLE ZLIB_CMAKE_BUILD)
	if(NOT ZLIB_CMAKE_BUILD EQUAL 0)
		message(FATAL_ERROR "Failed to build and install zlib with cmake")
	endif()
	message(STATUS "Done building and installing zlib")

	# now setup and try finding zlib again here
	set(ZLIB_ROOT ${zlib_BINARY_DIR}/${CMAKE_BUILD_TYPE}/InstallZlibDir)
	find_package(ZLIB COMPONENTS ZLIB REQUIRED)

	message(STATUS "Done settingup zlib now available for use")
	set(FETCHCONTENT_QUIET ${OLD_FETCHCONTENT_QUIET})
endif()

