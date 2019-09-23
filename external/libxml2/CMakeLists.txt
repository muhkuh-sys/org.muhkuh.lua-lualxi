cmake_minimum_required(VERSION 3.7)

PROJECT("libgmp")

INCLUDE(ExternalProject)


#----------------------------------------------------------------------------
#
# Build the project.
#
ExternalProject_Add(TARGET_libxml2
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libxml2
                    URL ${CMAKE_CURRENT_SOURCE_DIR}/libxml2-2.9.9.tar.gz
                    URL_HASH SHA1=96686d1dd9fddf3b35a28b1e2e4bbacac889add3
                    CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/libxml2/src/TARGET_libxml2/configure --prefix=${CMAKE_BINARY_DIR}/install --enable-static=yes --enable-shared=no --with-pic --without-python --host=${CONFIGURE_HOST}
                    BUILD_COMMAND make
                    INSTALL_COMMAND make install
)

SET(LIBXML2_PREFIX       ${CMAKE_CURRENT_BINARY_DIR}/install   CACHE INTERNAL "The complete path to the libxml2 installation.")
SET(LIBXML2_LIBRARY_DIR  ${LIBXML2_PREFIX}/lib                 CACHE INTERNAL "The complete path to the libxml2 library.")
SET(LIBXML2_LIBRARY      ${LIBXML2_LIBRARY_DIR}/libxml2.a      CACHE INTERNAL "The complete path to the libxml2 library.")
SET(LIBXML2_INCLUDE_DIR  ${LIBXML2_PREFIX}/include             CACHE INTERNAL "The include folder for the libxml2 library.")
