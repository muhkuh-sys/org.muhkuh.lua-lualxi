cmake_minimum_required(VERSION 3.7)

PROJECT("libgmp")

INCLUDE(ExternalProject)


#----------------------------------------------------------------------------
#
# Build the project.
#
SET(net.zlib-zlib_DIR "${CMAKE_BINARY_DIR}/../lua5.1/build_requirements/jonchki/install/dev/cmake")
FIND_PACKAGE(net.zlib-zlib REQUIRED)
GET_TARGET_PROPERTY(ZLIB_LIBRARIES net.zlib-zlib::zlibstatic LOCATION)
SET(ZLIB_INCLUDE_DIR ${net.zlib-zlib_INCLUDE_DIR})
SET(ZLIB_FOUND TRUE)
MESSAGE("Found ZLIB: ${ZLIB_LIBRARIES} (found version ${net.zlib-zlib_VERSION})")

MESSAGE("PKG_CONFIG_PATH=${CMAKE_BINARY_DIR}/install/lib/pkgconfig")

ExternalProject_Add(TARGET_liblxi
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/liblxi
                    URL ${CMAKE_CURRENT_SOURCE_DIR}/liblxi-1.13.tar.xz
                    URL_HASH SHA1=cac810ae5b45f168302106b22bc4992ce9c5a391
                    PATCH_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/liblxi/src/TARGET_liblxi-build/src && ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/liblxi/src/TARGET_liblxi/src/vxi11core.rpcl ${CMAKE_CURRENT_BINARY_DIR}/liblxi/src/TARGET_liblxi-build/src/ && "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tools/apply_patches.py --working-folder ${CMAKE_CURRENT_BINARY_DIR}/liblxi/src/TARGET_liblxi --patch-folder ${CMAKE_CURRENT_SOURCE_DIR}/patches --strip 1
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env "LIBXML2_PC_CFLAGS=-I${CMAKE_BINARY_DIR}/install/include/libxml2 -I${ZLIB_INCLUDE_DIR}" "LIBXML2_PC_LIBS=-L${CMAKE_BINARY_DIR}/install/lib -llzma ${ZLIB_LIBRARIES} -lm" ${CMAKE_CURRENT_BINARY_DIR}/liblxi/src/TARGET_liblxi/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/install --enable-static=yes --enable-shared=no --with-pic --disable-avahi --host=${CONFIGURE_HOST}
                    BUILD_COMMAND make
                    INSTALL_COMMAND make install
)
