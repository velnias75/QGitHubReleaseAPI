cmake_minimum_required(VERSION 2.8.11)

project(QGitHubReleaseAPI)

set(PROJECT_VERSION_MAJ 0)
set(PROJECT_VERSION_MIN 6)
set(PROJECT_VERSION_SOV 1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include(GenerateExportHeader)
include(CheckIncludeFile)
include(FindDoxygen)

find_package(Qt5Gui)
find_package(Qt5Network)

CHECK_INCLUDE_FILE(mkdio.h HAVE_MKDIO_H)
find_library(MARKDOWN_LIBRARIES markdown)

execute_process(COMMAND qmake -query QT_HOST_DATA OUTPUT_VARIABLE QMAKEMKSPECS
								OUTPUT_STRIP_TRAILING_WHITESPACE)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_CAST_TO_ASCII -DQT_USE_FAST_OPERATOR_PLUS
				-DQT_USE_FAST_CONCATENATION -DQT_STRICT_ITERATORS -DQT_NO_URL_CAST_FROM_STRING
				-D_GLIBCXX_VISIBILITY=0 -D_NDEBUG -DQT_NO_DEBUG_OUTPUT
				-DPROJECTVERSION="${PROJECT_VERSION_MAJ}.${PROJECT_VERSION_MIN}")

if(${HAVE_MKDIO_H})
add_definitions(-DHAVE_MKDIO_H)
endif(${HAVE_MKDIO_H})

set(LIB_SRCS src/qgithubreleaseapi.cpp src/qgithubreleaseapi_p.cpp src/filedownloader.cpp)

add_compiler_export_flags(EXPORT_FLAGS)
add_library(qgithubreleaseapi_static STATIC ${LIB_SRCS})

set_property(TARGET qgithubreleaseapi_static PROPERTY COMPILE_DEFINITIONS QT_STATIC)

if(${BUILD_SHARED_LIBS})
add_library(qgithubreleaseapi SHARED ${LIB_SRCS})
set_property(TARGET qgithubreleaseapi  PROPERTY COMPILE_FLAGS ${EXPORT_FLAGS})

set_target_properties(qgithubreleaseapi PROPERTIES VERSION
					  "${PROJECT_VERSION_SOV}.${PROJECT_VERSION_MAJ}.${PROJECT_VERSION_MIN}"
										SOVERSION ${PROJECT_VERSION_SOV})

target_link_libraries(qgithubreleaseapi Qt5::Gui)
target_link_libraries(qgithubreleaseapi Qt5::Network)
endif(${BUILD_SHARED_LIBS})

target_link_libraries(qgithubreleaseapi_static Qt5::Gui)
target_link_libraries(qgithubreleaseapi_static Qt5::Network)

if(${BUILD_SHARED_LIBS})
if(${HAVE_MKDIO_H})
target_link_libraries(qgithubreleaseapi ${MARKDOWN_LIBRARIES})
endif(${HAVE_MKDIO_H})
endif(${BUILD_SHARED_LIBS})

if(${DOXYGEN_FOUND})
configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
execute_process(COMMAND ${DOXYGEN_EXECUTABLE} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif(${DOXYGEN_FOUND})

configure_file(${CMAKE_SOURCE_DIR}/qgithubreleaseapi.pc.in
			   ${PROJECT_BINARY_DIR}/qgithubreleaseapi.pc @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/qgithubreleaseapi.prf.in
			   ${PROJECT_BINARY_DIR}/qgithubreleaseapi.prf @ONLY)

if(${BUILD_SHARED_LIBS})
install(TARGETS qgithubreleaseapi DESTINATION lib)
endif(${BUILD_SHARED_LIBS})

install(TARGETS qgithubreleaseapi_static DESTINATION lib)
install(FILES src/qgithubreleaseapi.h DESTINATION include/qgithubreleaseapi)
install(FILES ${PROJECT_BINARY_DIR}/qgithubreleaseapi.pc DESTINATION lib/pkgconfig)
install(FILES ${PROJECT_BINARY_DIR}/qgithubreleaseapi.prf
		DESTINATION ${QMAKEMKSPECS}/mkspecs/features)
if(${DOXYGEN_FOUND})
install(DIRECTORY ${PROJECT_BINARY_DIR}/html
				DESTINATION share/doc/QGitHubReleaseAPI-${PROJECT_VERSION}
				PATTERN "*.md5" EXCLUDE PATTERN "*.map" EXCLUDE)
endif(${DOXYGEN_FOUND})
