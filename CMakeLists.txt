cmake_minimum_required(VERSION 2.8.11)

project(QGitHubReleaseAPI)

set(PROJECT_VERSION 0.2)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include(GenerateExportHeader)
include(CheckIncludeFile)

find_package(Qt5Gui)
find_package(Qt5Network)

CHECK_INCLUDE_FILE(mkdio.h HAVE_MKDIO_H)
find_library(MARKDOWN_LIBRARIES markdown)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_CAST_TO_ASCII -DQT_USE_FAST_OPERATOR_PLUS
				-DQT_USE_FAST_CONCATENATION -DQT_STRICT_ITERATORS -DQT_NO_URL_CAST_FROM_STRING
				-D_GLIBCXX_VISIBILITY=0 -D_NDEBUG -DQT_NO_DEBUG_OUTPUT
				-DPROJECTVERSION="${PROJECT_VERSION}")

if(${HAVE_MKDIO_H})
add_definitions(-DHAVE_MKDIO_H)
endif(${HAVE_MKDIO_H})

set(LIB_SRCS src/qgithubreleaseapi.cpp src/qgithubreleaseapi_p.cpp src/filedownloader.cpp)

add_compiler_export_flags()
add_library(qgithubreleaseapi SHARED ${LIB_SRCS})
add_library(qgithubreleaseapi_static STATIC ${LIB_SRCS})

generate_export_header(qgithubreleaseapi EXPORT_FILE_NAME export.h)

set_property(TARGET qgithubreleaseapi PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(qgithubreleaseapi PROPERTIES VERSION 0 SOVERSION 0.0.0)

target_link_libraries(qgithubreleaseapi  Qt5::Gui)
target_link_libraries(qgithubreleaseapi Qt5::Network)
target_link_libraries(qgithubreleaseapi_static Qt5::Gui)
target_link_libraries(qgithubreleaseapi_static Qt5::Network)

if(${HAVE_MKDIO_H})
target_link_libraries(qgithubreleaseapi ${MARKDOWN_LIBRARIES})
endif(${HAVE_MKDIO_H})

configure_file(${CMAKE_SOURCE_DIR}/qgithubreleaseapi.pc.in
			   ${PROJECT_BINARY_DIR}/qgithubreleaseapi.pc @ONLY)

install(TARGETS qgithubreleaseapi DESTINATION lib)
install(TARGETS qgithubreleaseapi_static DESTINATION lib)
install(FILES src/qgithubreleaseapi.h DESTINATION include/qgithubreleaseapi)
install(FILES ${PROJECT_BINARY_DIR}/export.h DESTINATION include/qgithubreleaseapi)
install(FILES ${PROJECT_BINARY_DIR}/qgithubreleaseapi.pc DESTINATION lib/pkgconfig)
