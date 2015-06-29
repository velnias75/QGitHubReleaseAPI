cmake_minimum_required(VERSION 2.8.7)

project(QGitHubReleaseAPI)

set(PROJECT_VERSION 0.1)

#set(CMAKE_BUILD_TYPE RELWITHDEBINFO)

include(GenerateExportHeader)
include(FindPkgConfig)
include(CheckIncludeFile)

find_package(Qt4)
pkg_search_module(QJSON QJson)

CHECK_INCLUDE_FILE(mkdio.h HAVE_MKDIO_H)
find_library(MARKDOWN_LIBRARIES markdown)

set(QT_USE_QTGUI TRUE)
set(QT_USE_QTCORE TRUE)
set(QT_USE_QTNETWORK TRUE)

include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_CAST_TO_ASCII -DQT_USE_FAST_OPERATOR_PLUS
				-DQT_USE_FAST_CONCATENATION -DQT_STRICT_ITERATORS -DQT_NO_URL_CAST_FROM_STRING
				-D_GLIBCXX_VISIBILITY=0 -D_NDEBUG -DQT_NO_DEBUG_OUTPUT
				-DPROJECTVERSION="${PROJECT_VERSION}")

include_directories(${QT_INCLUDE_DIR})
include_directories(${PROJECT_BINARY_DIR})

if(${HAVE_MKDIO_H})
add_definitions(-DHAVE_MKDIO_H)
endif(${HAVE_MKDIO_H})

if(${QJSON_FOUND})
add_definitions(-DQJSON_FOUND)
endif(${QJSON_FOUND})

set(LIB_SRCS src/qgithubreleaseapi.cpp src/qgithubreleaseapi_p.cpp src/filedownloader.cpp)
set(LIB_MOC_HDRS src/qgithubreleaseapi.h src/qgithubreleaseapi_p.h src/filedownloader.h)

qt4_wrap_cpp(LIB_MOC_SRCS ${LIB_MOC_HDRS})

add_compiler_export_flags()
add_library(qgithubreleaseapi SHARED ${LIB_MOC_SRCS} ${LIB_SRCS})
add_library(qgithubreleaseapi_static STATIC ${LIB_MOC_SRCS} ${LIB_SRCS})

generate_export_header(qgithubreleaseapi EXPORT_FILE_NAME export.h)

set_property(TARGET qgithubreleaseapi PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(qgithubreleaseapi PROPERTIES VERSION 0 SOVERSION 0.0.0)

target_link_libraries(qgithubreleaseapi ${QT_LIBRARIES})
#target_link_libraries(qgithubreleaseapi ${QT_QTCORE_LIBRARY})
#target_link_libraries(qgithubreleaseapi ${QT_QTNETWORK_LIBRARY})

if(${QJSON_FOUND})
set(QJSON_REQUIRED "Requires: QJson")
target_link_libraries(qgithubreleaseapi qjson)
endif(${QJSON_FOUND})

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
