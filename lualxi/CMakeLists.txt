PROJECT("org.muhkuh.lua-lualxi"
        VERSION 1.0.0)

# Filter the jonchki configurations.
IF(WIN32)
	SET(JONCHKI_CFG_FILE "installer/lua${BUILDCFG_LUA_VERSION}/lualxi_windows.xml")
ELSE(WIN32)
	SET(JONCHKI_CFG_FILE "installer/lua${BUILDCFG_LUA_VERSION}/lualxi.xml")
ENDIF(WIN32)
CONFIGURE_FILE(${JONCHKI_CFG_FILE}
               ${CMAKE_BINARY_DIR}/lua${BUILDCFG_LUA_VERSION}-lualxi-${PROJECT_VERSION}.xml
               @ONLY)

#----------------------------------------------------------------------------
#
# Build the project.
#

IF((${BUILDCFG_ONLY_JONCHKI_CFG} STREQUAL "OFF"))
	# Add a custom target for the swig runtime for LUA.
	INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/muhkuh_swig_lua_runtime.cmake)

	INCLUDE(${SWIG_USE_FILE})

	# Select the LUA version.
	INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/muhkuh_lua.cmake)

	SET(net.zlib-zlib_DIR "${CMAKE_BINARY_DIR}/build_requirements/jonchki/install/dev/cmake")
	FIND_PACKAGE(net.zlib-zlib REQUIRED)
	MESSAGE("Found ZLIB ${net.zlib-zlib_VERSION}")

	SET(LIBLXI_INCLUDE_DIR "${CMAKE_BINARY_DIR}/../external/liblxi/install/include")
	SET(LIBLXI_LIB "${CMAKE_BINARY_DIR}/../external/liblxi/install/lib/liblxi.a")

	SET(LIBXML2_LIB "${CMAKE_BINARY_DIR}/../external/install/lib/libxml2.a")
	SET(LIBLZMA_LIB "${CMAKE_BINARY_DIR}/../external/install/lib/liblzma.a")

	IF(WIN32)
		SET(liboncrpc_DIR "${CMAKE_BINARY_DIR}/build_requirements/jonchki/install/dev/cmake")
		MESSAGE("liboncrpc_DIR = ${liboncrpc_DIR}")
		FIND_PACKAGE(liboncrpc REQUIRED)
		MESSAGE("Found liboncrpc ${liboncrpc_VERSION}")
	ENDIF(WIN32)

	SET_SOURCE_FILES_PROPERTIES(lualxi.i PROPERTIES CPLUSPLUS ON)
	SET_SOURCE_FILES_PROPERTIES(lualxi.i PROPERTIES SWIG_FLAGS "")
	SWIG_ADD_MODULE(TARGET_lualxi lua lualxi.i wrapper.cpp)
	if(WIN32)
		SWIG_LINK_LIBRARIES(TARGET_lualxi ${LUA_LIBRARIES} ${LIBLXI_LIB} liboncrpc::TARGET_oncrpc ${LIBXML2_LIB} ${LIBLZMA_LIB} net.zlib-zlib::zlibstatic)
	ELSE(WIN32)
		SWIG_LINK_LIBRARIES(TARGET_lualxi ${LUA_LIBRARIES} ${LIBLXI_LIB} ${LIBXML2_LIB} ${LIBLZMA_LIB} net.zlib-zlib::zlibstatic)
	ENDIF(WIN32)
	ADD_DEPENDENCIES(TARGET_lualxi TARGET_swigluarun)

	TARGET_INCLUDE_DIRECTORIES(TARGET_lualxi
	                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${LUA_INCLUDE_DIR} ${LIBLXI_INCLUDE_DIR} ${CMAKE_BINARY_DIR}/swig_runtime)

	# Set the name of the output file to "lualxi".
	SET_TARGET_PROPERTIES(TARGET_lualxi PROPERTIES PREFIX "" OUTPUT_NAME "lualxi")

	# On mingw link all compiler libraries static.
	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		set_property(TARGET TARGET_lualxi PROPERTY LINK_FLAGS "-static-libgcc -static-libstdc++")
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

	# Install the lua module.
	INSTALL(TARGETS TARGET_lualxi DESTINATION ${INSTALL_DIR_LUA_MODULES})

	# Add tests for this module.
	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		# Here are the MinGW specific tests.
		ADD_TEST(NAME lualxi_MinGW_DLL_dependencies
		         COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tests/mingw_dll_dependencies.py -u lua5.1 -u lua5.2 -u lua5.3 -u lua5.4 $<TARGET_FILE:TARGET_lualxi>)
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

	#----------------------------------------------------------------------------
	#
	# Build a package for the selected platform.
	#

	SET(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_BINARY_DIR}/../../repository/org/muhkuh/lua/lualxi/${PROJECT_VERSION})
	SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
	SET(CPACK_PACKAGE_FILE_NAME "lua${BUILDCFG_LUA_VERSION}-lualxi-${PROJECT_VERSION}-${JONCHKI_PLATFORM}")

	SET(CPACK_GENERATOR "TXZ")
	INCLUDE(CPack)

	IF(${JONCHKI_PLATFORM_DIST_ID} STREQUAL "windows")
		# Package a windows build.

		# NOTE: Do not install the library again, there is an "INSTALL" already.

		CONFIGURE_FILE(installer/lua${BUILDCFG_LUA_VERSION}/install_windows.lua
		               ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
		               @ONLY)

		INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
		        DESTINATION .)
	ELSE(${JONCHKI_PLATFORM_DIST_ID} STREQUAL "windows")
		# Package a linux build.

		# NOTE: Do not install the library again, there is an "INSTALL" already.

		CONFIGURE_FILE(installer/lua${BUILDCFG_LUA_VERSION}/install_linux.lua
		               ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
		               @ONLY)

		INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
		        DESTINATION .)
	ENDIF(${JONCHKI_PLATFORM_DIST_ID} STREQUAL "windows")

	CONFIGURE_FILE(installer/lua${BUILDCFG_LUA_VERSION}/lualxi.xml
	               ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-lualxi-${PROJECT_VERSION}-${JONCHKI_PLATFORM}.xml
	               @ONLY)


	#-----------------------------------------------------------------------------
	#
	# Create a custom target named "pack".
	# It builds the package and creates the hashes.
	#
	ADD_CUSTOM_TARGET(pack
	                  COMMAND ${CMAKE_MAKE_PROGRAM}
	                  COMMAND ${CMAKE_MAKE_PROGRAM} package
	                  COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tools/generate_hash.py ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-lualxi-${PROJECT_VERSION}-${JONCHKI_PLATFORM}.xml
	                  COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tools/generate_hash.py ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-lualxi-${PROJECT_VERSION}-${JONCHKI_PLATFORM}.tar.xz
	                  COMMENT "Pack the artifact and generate the hashes...")


	#-----------------------------------------------------------------------------
	#
	# Filter the pom.xml file.
	#
	SET(POM_ARTIFACT_ID  "lua${BUILDCFG_LUA_VERSION}-lualxi")
	SET(POM_GROUP_ID     "org.muhkuh.lua")
	SET(POM_VERSION      "${PROJECT_VERSION}")
	SET(POM_PACKAGING    "tar.xz")
	CONFIGURE_FILE(installer/lua${BUILDCFG_LUA_VERSION}/pom.xml
	               ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-lualxi-${PROJECT_VERSION}.pom
	               @ONLY)
ENDIF((${BUILDCFG_ONLY_JONCHKI_CFG} STREQUAL "OFF"))
