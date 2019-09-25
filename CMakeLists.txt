CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT("lualxi")

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

# Swig builds the wrapper.
FIND_PACKAGE(SWIG REQUIRED)

ADD_SUBDIRECTORY(lualxi)

#-----------------------------------------------------------------------------
