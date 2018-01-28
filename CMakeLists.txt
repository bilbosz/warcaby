cmake_minimum_required(VERSION 3.7)

option(Warcaby_DEBUG "Tryb debugowania" OFF)
option(Warcaby_FULLSCREEN "Uruchamia program w trybie pełnoekranowym" ON)
option(Warcaby_CONSOLE_STOP "Po wykonaniu programu czeka, aż użytkownik naciśnie przycisk" OFF)
option(Warcaby_DOC "Generuje dokumentację Doxygen kodu źródłowego" OFF)

set(SFML_REQUIRED_VERSION 2.1.0)
set(Boost_REQUIRED_VERSION 1.55)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

if (NOT DEFINED ${Warcaby_SFML_ROOT})
    set(SFML_FIND_QUIETLY ON)
    find_package(SFML ${SFML_REQUIRED_VERSION})
    unset(SFML_ROOT)
    if (SFML_FOUND)
        get_filename_component(SFML_ROOT ${SFML_INCLUDE_DIR} DIRECTORY)
    endif()
endif()
set(Warcaby_SFML_ROOT "${SFML_ROOT}" CACHE PATH "Ścieżka do katalogu głównego biblioteki SFML")
unset(SFML_ROOT)
unset(SFML_FIND_QUIETLY)

if (NOT DEFINED ${Warcaby_Boost_ROOT})
    set(Boost_FIND_QUIETLY ON)
    find_package(Boost ${Boost_REQUIRED_VERSION})
    unset(Boost_ROOT)
    if (Boost_FOUND)
        set(Boost_ROOT ${Boost_INCLUDE_DIRS})
    endif()
endif()
set(Warcaby_Boost_ROOT "${Boost_ROOT}" CACHE PATH "Ścieżka do katalogu głównego biblioteki Boost")
unset(Boost_ROOT)
unset(Boost_FIND_QUIETLY)

set(CMAKE_BUILD_TYPE)
if (Warcaby_DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_BUILD_TYPE Release)
endif()

project("Warcaby")

set(Warcaby_VERSION_MAJOR 1)
set(Warcaby_VERSION_MINOR 0)
set(Warcaby_VERSION_PATCH 2)

set(MAIN_TARGET "Warcaby")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${MAIN_TARGET}")

configure_file(
    "${CMAKE_SOURCE_DIR}/src/ProjectConfig.h.in"
    "${CMAKE_BINARY_DIR}/config/ProjectConfig.h"
)
include_directories("${CMAKE_BINARY_DIR}/config")

if (Warcaby_DOC)
    add_subdirectory("${CMAKE_SOURCE_DIR}/doc")
endif()
set(RES "${CMAKE_SOURCE_DIR}/res")
file(COPY "${CMAKE_SOURCE_DIR}/res" DESTINATION "${CMAKE_BINARY_DIR}")

set(SRC)
file(
    GLOB SRC
    LIST_DICTIONARIES false 
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	add_executable(
		${MAIN_TARGET}
		WIN32
		${SRC}
	)
elseif()
	add_executable(
		${MAIN_TARGET}
		${SRC}
	)
endif()


set(SFML_ROOT ${Warcaby_SFML_ROOT})
set(SFML_STATIC_LIBRARIES FALSE)
set(SFML_REQUIRED_MODULES system window graphics audio network)
if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	set(SFML_REQUIRED_MODULES ${SFML_REQUIRED_MODULES} main)
endif()
find_package(SFML ${SFML_REQUIRED_VERSION} REQUIRED ${SFML_REQUIRED_MODULES})
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${MAIN_TARGET} ${SFML_DEPENDENCIES} ${SFML_LIBRARIES})
endif()

get_filename_component(SFML_ROOT ${SFML_INCLUDE_DIR} DIRECTORY)
set(SFML_BIN "${SFML_ROOT}/bin")
set(SFML_DYNAMIC_LIBRARIES)
file(
    GLOB SFML_DYNAMIC_LIBRARIES
    LIST_DICTIONARIES false
    "${SFML_BIN}/*.dll"
)
file(COPY ${SFML_DYNAMIC_LIBRARIES} DESTINATION "${CMAKE_BINARY_DIR}")

set(BOOST_ROOT ${Warcaby_Boost_ROOT})
find_package(Boost ${Boost_REQUIRED_VERSION} REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(${MAIN_TARGET} ${Boost_LIBRARIES})
endif()

file(
	GLOB Warcaby_DYNAMIC_LIBRARIES
	LIST_DICTIONARIES false
	"${CMAKE_BINARY_DIR}/*.dll"
	EXCLUDE
)

install(TARGETS "${MAIN_TARGET}" DESTINATION ".")
foreach(Warcaby_DYNAMIC_LIBRARY IN ITEMS ${Warcaby_DYNAMIC_LIBRARIES})
	if(NOT (Warcaby_DYNAMIC_LIBRARY MATCHES ".+-d-[0-9]+\\.dll"))
		install(FILES "${Warcaby_DYNAMIC_LIBRARY}" DESTINATION ".")
	endif()
endforeach(Warcaby_DYNAMIC_LIBRARY)
install(DIRECTORY "${RES}" DESTINATION ".")

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${Warcaby_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Warcaby_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${Warcaby_VERSION_PATCH}")
set(CPACK_MONOLITHIC_INSTALL 1)
include(CPack)
