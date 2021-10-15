cmake_minimum_required(VERSION 3.16)


find_package(OpenSSL COMPONENTS SSL)
if(OPENSSL_FOUND)
	message(STATUS "Found OpenSSL::SSL, can now be used")
else()
	message(STATUS "Didn't find OpenSSL:SSL, grabbing from github")
endif()
