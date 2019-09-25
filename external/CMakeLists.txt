cmake_minimum_required(VERSION 2.8)

PROJECT("external")

#----------------------------------------------------------------------------
#
# Construct the host ID for the configure command.
#
IF(${CMAKE_CROSSCOMPILING})
	IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		# Get the prefix of the compiler.
		GET_FILENAME_COMPONENT(GCC_BASENAME ${CMAKE_C_COMPILER} NAME)
		IF(GCC_BASENAME MATCHES "([^-]+-[^-]+-[^-]+)-gcc")
			SET(CONFIGURE_HOST "${CMAKE_MATCH_1}")
		ELSE(GCC_BASENAME MATCHES "([^-]+-[^-]+-[^-]+)-gcc")
			MESSAGE(FATAL_ERROR "Failed to extract the compiler prefix from the C compiler ${CMAKE_C_COMPILER}")
		ENDIF(GCC_BASENAME MATCHES "([^-]+-[^-]+-[^-]+)-gcc")
	ELSE(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		MESSAGE(FATAL_ERROR "Cross compiling detected, but not using GCC. This is currently not supported.")
	ENDIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
ELSE(${CMAKE_CROSSCOMPILING})
	# Set the host even if not cross compiling.
	# Otherwise GNUTLS detects the CPU and tries to build with special
	# support for it. In the first place this sounds very nice. But it
	# fails on a 32bit OS running on a 64bit CPU - which is the case for
	# Docker 32bit images.
	IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			SET(CONFIGURE_HOST "${JONCHKI_PLATFORM_CPU_ARCH}-pc-linux-gnu")
		ELSE(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			MESSAGE(FATAL_ERROR "Trying to detect the system with a non-GCC compiler. This is currently not supported.")
		ENDIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	ELSE(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		MESSAGE(FATAL_ERROR "Trying to detect the system on a non-linux OS. This is currently not supported.")
	ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
ENDIF(${CMAKE_CROSSCOMPILING})

MESSAGE("CONFIGURE_HOST = ${CONFIGURE_HOST}")

#----------------------------------------------------------------------------
#
# Include the projects in the subdirectories.
#

ADD_SUBDIRECTORY(lzma)
ADD_SUBDIRECTORY(libxml2)
ADD_SUBDIRECTORY(liblxi)
