cmake_minimum_required(VERSION 3.7)

PROJECT("libgmp")

INCLUDE(ExternalProject)


#----------------------------------------------------------------------------
#
# Build the project.
#
ExternalProject_Add(TARGET_liblxi
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/liblxi
                    URL ${CMAKE_CURRENT_SOURCE_DIR}/liblxi-1.13.tar.xz
                    URL_HASH SHA1=cac810ae5b45f168302106b22bc4992ce9c5a391
                    CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/liblxi/src/TARGET_liblxi/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/install --enable-static=yes --enable-shared=no --with-pic --disable-avahi --host=${CONFIGURE_HOST}
                    BUILD_COMMAND make
                    INSTALL_COMMAND make install
)

SET(LIBXML2_PREFIX       ${CMAKE_CURRENT_BINARY_DIR}/install   CACHE INTERNAL "The complete path to the libxml2 installation.")
SET(LIBXML2_LIBRARY_DIR  ${LIBXML2_PREFIX}/lib                 CACHE INTERNAL "The complete path to the libxml2 library.")
SET(LIBXML2_LIBRARY      ${LIBXML2_LIBRARY_DIR}/libxml2.a      CACHE INTERNAL "The complete path to the libxml2 library.")
SET(LIBXML2_INCLUDE_DIR  ${LIBXML2_PREFIX}/include             CACHE INTERNAL "The include folder for the libxml2 library.")
