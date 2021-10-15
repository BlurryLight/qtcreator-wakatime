cmake_minimum_required(VERSION 3.16)


find_package(LibreSSL COMPONENTS SSL)
if(LibreSSL_FOUND)
	message(STATUS "Found LibreSSL::SSL, can now be used")
else()
	message(STATUS "Didn't find OpenSSL:SSL, grabbing from github")

	include(FetchContent)
	FetchContent_Declare( libressl
		URL https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.3.5.tar.gz
	)

	set(OLD_FETCHCONTENT_QUIET ${FETCHCONTENT_QUIET})
	set(FETCHCONTENT_QUIET OFF)

	# fetch zlib from github
	FetchContent_GetProperties( libressl )
	if(NOT libressl_POPULATED)
		FetchContent_Populate(libressl)
	endif()

	FetchContent_GetProperties( libressl )
	if(NOT libressl_POPULATED)
		message(FATAL_ERROR
			"Sorry, couldn't fetch zlib from https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.3.5.tar.gz")
	endif()

	#then cmake configure here
	message(STATUS "Cmake configuring libressl")
	execute_process(COMMAND ${CMAKE_COMMAND} -B ${libressl_BINARY_DIR}/${CMAKE_BUILD_TYPE}
		-S ${libressl_SOURCE_DIR}
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DCMAKE_INSTALL_PREFIX=${libressl_BINARY_DIR}/${CMAKE_BUILD_TYPE}/InstallZlibDir
		RESULT_VARIABLE LIBRESSL_CMAKE_CONFIGURED)
	if(NOT LIBRESSL_CMAKE_CONFIGURED EQUAL 0)
		message(FATAL_ERROR "Failed to run cmake configure step on zlib")
	endif()

	# then cmake build it here
	message(STATUS "Cmake building libressl here")
	execute_process(COMMAND ${CMAKE_COMMAND} --build ${libressl_BINARY_DIR}/${CMAKE_BUILD_TYPE}
		--config ${CMAKE_BUILD_TYPE} --target install
		RESULT_VARIABLE LIBRESSL_CMAKE_BUILD)
	if(NOT LIBRESSL_CMAKE_BUILD EQUAL 0)
		message(FATAL_ERROR "Failed to build and install libressl with cmake")
	endif()
	message(STATUS "Done building and installing libressl")

	# final setup of dir into cmake modules directory
#	file(COPY ${libressl_SOURCE_DIR}/FindLibreSSL.cmake
#		DESTINATION
#		${CMAKE_SOURCE_DIR}/cmake)
	set(LIBRESSL_ROOT_DIR
		${libressl_BINARY_DIR}/${CMAKE_BUILD_TYPE}/InstallZlibDir)

	# now setup and try finding zlib again here
	find_package(LibreSSL REQUIRED)

	message(STATUS "Done setting up libressl now available for use")
	set(FETCHCONTENT_QUIET ${OLD_FETCHCONTENT_QUIET})
endif()
