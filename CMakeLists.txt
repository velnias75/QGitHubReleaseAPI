cmake_minimum_required(VERSION 2.8.7)

project(QGitHubReleaseAPI)

set(PROJECT_VERSION 0.5)

include(GenerateExportHeader)
include(CheckCCompilerFlag)
include(CheckIncludeFile)
include(FindPkgConfig)
include(FindDoxygen)

find_package(Qt4)
pkg_search_module(QJSON QJson)

CHECK_INCLUDE_FILE(mkdio.h HAVE_MKDIO_H)
find_library(MARKDOWN_LIBRARIES markdown)

set(QT_USE_QTGUI TRUE)
set(QT_USE_QTCORE TRUE)
set(QT_USE_QTNETWORK TRUE)

include(${QT_USE_FILE})

execute_process(COMMAND qmake -query QMAKE_MKSPECS OUTPUT_VARIABLE QMAKEMKSPECS
				OUTPUT_STRIP_TRAILING_WHITESPACE)

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

check_cxx_compiler_flag(-Wa,--noexecstack COMPILE_NOEXECSTACK)

if(${COMPILE_NOEXECSTACK})
set_property(SOURCE ${LIB_SRCS} ${LIB_MOC_SRCS} APPEND PROPERTY COMPILE_FLAGS -Wa,--noexecstack)
endif(${COMPILE_NOEXECSTACK})

add_compiler_export_flags(EXPORT_FLAGSS)

qt4_wrap_cpp(LIB_MOC_SRCS ${LIB_MOC_HDRS})

add_library(qgithubreleaseapi_static STATIC ${LIB_MOC_SRCS} ${LIB_SRCS})

generate_export_header(qgithubreleaseapi_static BASE_NAME qgithubreleaseapi
					   EXPORT_FILE_NAME export.h)

if(${BUILD_SHARED_LIBS})
add_library(qgithubreleaseapi SHARED ${LIB_MOC_SRCS} ${LIB_SRCS})
endif(${BUILD_SHARED_LIBS})

if(${BUILD_SHARED_LIBS})
generate_export_header(qgithubreleaseapi BASE_NAME qgithubreleaseapi EXPORT_FILE_NAME export.h)
set_property(TARGET qgithubreleaseapi PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(qgithubreleaseapi PROPERTIES VERSION 0 SOVERSION 0.0.0)
set_property(TARGET qgithubreleaseapi PROPERTY COMPILE_FLAGS ${EXPORT_FLAGSS})
target_link_libraries(qgithubreleaseapi ${QT_LIBRARIES})

if(${QJSON_FOUND})
target_link_libraries(qgithubreleaseapi qjson)
endif(${QJSON_FOUND})

if(${HAVE_MKDIO_H})
target_link_libraries(qgithubreleaseapi ${MARKDOWN_LIBRARIES})
endif(${HAVE_MKDIO_H})
endif(${BUILD_SHARED_LIBS})

set_property(TARGET qgithubreleaseapi_static PROPERTY
			 COMPILE_DEFINITIONS QGITHUBRELEASEAPI_STATIC_DEFINE)

configure_file(${CMAKE_SOURCE_DIR}/qgithubreleaseapi.pc.in
			   ${PROJECT_BINARY_DIR}/qgithubreleaseapi.pc @ONLY)

configure_file(${CMAKE_SOURCE_DIR}/qgithubreleaseapi.prf.in
			   ${PROJECT_BINARY_DIR}/qgithubreleaseapi.prf @ONLY)

if(${DOXYGEN_FOUND})
configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
execute_process(COMMAND ${DOXYGEN_EXECUTABLE} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif(${DOXYGEN_FOUND})

if(${BUILD_SHARED_LIBS})
install(TARGETS qgithubreleaseapi DESTINATION lib)
endif(${BUILD_SHARED_LIBS})

install(TARGETS qgithubreleaseapi_static DESTINATION lib)
install(FILES src/qgithubreleaseapi.h DESTINATION include/qgithubreleaseapi)
install(FILES ${PROJECT_BINARY_DIR}/export.h DESTINATION include/qgithubreleaseapi)
install(FILES ${PROJECT_BINARY_DIR}/qgithubreleaseapi.pc DESTINATION lib/pkgconfig)
install(FILES ${PROJECT_BINARY_DIR}/qgithubreleaseapi.prf DESTINATION ${QMAKEMKSPECS}/features)
if(${DOXYGEN_FOUND})
install(DIRECTORY ${PROJECT_BINARY_DIR}/html
		DESTINATION share/doc/QGitHubReleaseAPI-${PROJECT_VERSION}
		PATTERN "*.md5" EXCLUDE PATTERN "*.map" EXCLUDE)
endif(${DOXYGEN_FOUND})
