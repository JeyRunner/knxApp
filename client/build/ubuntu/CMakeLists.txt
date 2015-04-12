
# files
# dir
set (CMAKE_CURRENT_SOURCE_DIR 	${CMAKE_CURRENT_SOURCE_DIR}/../../)
set (srcDir 					${CMAKE_CURRENT_SOURCE_DIR}src/)
set (srcDirKnx 					${srcDir}knx/)
set (srcDirProtocol				${CMAKE_CURRENT_SOURCE_DIR}../protocol/src/)

file( GLOB srcs ${srcDir}*.cpp ${srcDir}*.h   ${srcDirKnx}*.cpp ${srcDirKnx}*.h )
file( GLOB srcProtocol ${srcDirProtocol}*.cpp ${srcDirProtocol}*.h)

# include
include_directories( /usr/local/include/freetype2)
include_directories( /usr/local/include )
include_directories( /usr/local/include/SDL2 )
include_directories( /usr/local/ssl/include/ )

# ### panui #######################################
include_directories( /home/joshua/Dokumente/Projekte/panui/ )
add_definitions    ( -DGLM_FORCE_RADIANS)

# link libs
link_directories( /opt/vc/lib/ /usr/local/lib /home/joshua/Dokumente/Projekte/panui/build/ubuntu/ubuntu )
link_directories( /usr/local/ssl/lib)


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
SET(EXECUTABLE_OUTPUT_PATH )#${PROJECT_BINARY_DIR}/ubuntu)

# linux
# create executable
add_executable (knxApp ${srcProtocol} ${srcs})


# link libs
target_link_libraries( knxApp panui SDL2 freetype pthread libssl.a libcrypto.a dl)
