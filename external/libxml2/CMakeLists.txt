cmake_minimum_required(VERSION 3.7)

PROJECT("libgmp")

INCLUDE(ExternalProject)


#----------------------------------------------------------------------------
#
# Build the project.
#
SET(ZLIB_DIR "${CMAKE_BINARY_DIR}/../build_requirements/jonchki/install/dev")
SET(LZMA_DIR "${CMAKE_BINARY_DIR}/install")

ExternalProject_Add(TARGET_libxml2
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libxml2
                    URL ${CMAKE_CURRENT_SOURCE_DIR}/libxml2-2.9.9.tar.gz
                    URL_HASH SHA1=96686d1dd9fddf3b35a28b1e2e4bbacac889add3
                    CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/libxml2/src/TARGET_libxml2/configure --prefix=${CMAKE_BINARY_DIR}/install --enable-static=yes --enable-shared=no --with-pic --without-ftp --without-http --without-python --with-zlib=${ZLIB_DIR} --with-lzma=${LZMA_DIR} --host=${CONFIGURE_HOST}
                    BUILD_COMMAND make
                    INSTALL_COMMAND make install
)
