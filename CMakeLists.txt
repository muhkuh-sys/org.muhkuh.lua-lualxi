CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT(lualxi
        VERSION 1.0.0)

#-----------------------------------------------------------------------------
#
# The project options.
#
OPTION(BUILDCFG_ONLY_JONCHKI_CFG "Only build the jonchki configuration files. The default is OFF."
       "OFF")

MESSAGE("BUILDCFG_ONLY_JONCHKI_CFG: ${BUILDCFG_ONLY_JONCHKI_CFG}")

# Get the VCS version for the jonchki configuration.
INCLUDE(cmake/version.cmake)

# Detect the platform.
INCLUDE(cmake/platform.cmake)

# Set some installation folders.
INCLUDE(cmake/muhkuh_installation_folders.cmake)

# Python is used for the firmware build script and some test scripts.
FIND_PACKAGE(PythonInterp 2.7 REQUIRED)

# Filter the jonchki configurations.
CONFIGURE_FILE(lualxi/installer/lua${BUILDCFG_LUA_VERSION}/lualxi.xml
               ${CMAKE_CURRENT_BINARY_DIR}/lua${BUILDCFG_LUA_VERSION}-lualxi-${PROJECT_VERSION}.xml
               @ONLY)


IF( NOT BUILDCFG_ONLY_JONCHKI_CFG STREQUAL "ON" )
	ADD_SUBDIRECTORY(lualxi)
ENDIF( NOT BUILDCFG_ONLY_JONCHKI_CFG STREQUAL "ON" )

#-----------------------------------------------------------------------------
