# ######################
# ## BUILD RASPBERRY  ##
# ######################

# min version
cmake_minimum_required(VERSION 2.8)

# project name
project( knxApp )


# var
set (CMAKE_CURRENT_SOURCE_DIR 	../../)
set (srcDir 			${CMAKE_CURRENT_SOURCE_DIR})
set (srcDirKnx 			${srcDir}knx/)

set (PANUI_ROOT 		/home/dev/c++/panui/)
set (PANUI_ROOT_LIB 		${PANUI_ROOT}build/raspberry/)


# file
file( GLOB srcs ${srcDir}*.cpp ${srcDir}*.h   ${srcDirKnx}*.cpp ${srcDirKnx}*.h )

# include
include_directories( /usr/local/include/freetype2 )
include_directories( /usr/local/include )
include_directories( /opt/vc/include )
include_directories( /opt/vc/include/interface/vcos/pthreads )
include_directories( /opt/vc/include/interface/vmcs_host/linux )
include_directories( ${PANUI_ROOT})

# define #######################################################
add_definitions (-Dpl_pi)
add_definitions (-DGLM_FORCE_RADIANS)

# link libs
link_directories    ( /opt/vc/lib/ /usr/local/lib ${PANUI_ROOT_LIB})


# c++ 11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	message(STATUS "compiler supports c++11 flag")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

elseif(COMPILER_SUPPORTS_CXX0X)
	message(STATUS "compiler supports c++0x flag")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()



# set executable path
SET(EXECUTABLE_OUTPUT_PATH raspberry/)#${PROJECT_BINARY_DIR}/ubuntu)

# linux
# create executable
add_executable (knxApp ${srcs})


# link libs
target_link_libraries( knxApp panui freetype pthread )
