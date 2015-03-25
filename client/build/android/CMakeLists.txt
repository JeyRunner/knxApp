set (target main)




# source files
set (CMAKE_CURRENT_SOURCE_DIR 	${CMAKE_CURRENT_SOURCE_DIR}/../../)
set (srcDir 					${CMAKE_CURRENT_SOURCE_DIR}src/)
set (srcDirProtocol				${CMAKE_CURRENT_SOURCE_DIR}../protocol/src/)

file( GLOB srcs ${srcDir}*.cpp ${srcDir}*.h   ${srcDir}knx/*.cpp ${srcDir}knx/*.h  ${srcDir}knx/*.hpp  ${srcDir}SDL/*.cpp)
file( GLOB srcProtocol ${srcDirProtocol}*.cpp ${srcDirProtocol}*.h)

# var
set (ANDROID_NDK_CPU		armeabi-v7a)

# ### panui #######################################
set (PANUI_PATH 		/home/joshua/Dokumente/Projekte/panui )
set (PANUI_INCLUDE 		${PANUI_PATH}/include )
set (PANUI_LIBFILE_PATH		${PANUI_PATH}/android/${ANDROID_NDK_CPU} )
link_directories ( ${PANUI_LIBFILE_PATH})
link_directories ( ${ANDROID_TOOLCHAIN}/arm-linux-androideabi/lib/armv7-a)

## link libs
set (LINK_LIBS			SDL2 GLESv1_CM GLESv2 log android c stdc++ panui gnustl_shared libfreetype.a )#${PANUI_PATH}/android/lib/libfreetype2-static.a)


# ### android #################################################################################################
# var
set (ANDROID_TOOLCHAIN 			/home/joshua/Dokumente/android/android-toolchain )
set (ANDROID_TOOLCHAIN_INCLUDE	${ANDROID_TOOLCHAIN}/include )
set (ANDROID_SYSROOT			${ANDROID_TOOLCHAIN}/sysroot/usr )
set (ANDROID_SYSROOT_LIB  		${ANDROID_SYSROOT}/lib )
set (ANDROID_SYSROOT_INCLUDE  	${ANDROID_SYSROOT}/include )
set (GCC_NAME 					arm-linux-androideabi-gcc)
set (ANDROID_SDL_PATH			${srcDir}SDL2/src/main/android )
set (LIB_FREETYPE_STATIC		/home/joshua/Dokumente/android/android_libs/freetype2/objs/.libs/)

# compiler
set (CMAKE_CXX_COMPILER 		${ANDROID_TOOLCHAIN}/bin/${GCC_NAME})

# SDL files
file(GLOB srcSDL ${srcDir}/SDL/*.cpp )

# lib
link_directories(${ANDROID_SYSROOT_LIB})
link_directories(${LIB_FREETYPE_STATIC})

# create lib
add_library (${target} SHARED ${srcProtocol} ${srcs} ${srcSDL})


# set the output directory for the .so files to point to the android project's 'lib/[cpuarch] directory
set(ANDROID_SO_OUTDIR ${CMAKE_CURRENT_SOURCE_DIR}android/libs/${ANDROID_NDK_CPU})
set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${ANDROID_SO_OUTDIR})
set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${ANDROID_SO_OUTDIR})
set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${ANDROID_SO_OUTDIR})


# include
include_directories(${ANDROID_TOOLCHAIN_INCLUDE})
include_directories(${ANDROID_SYSROOT_INCLUDE})
include_directories(${ANDROID_SYSROOT_INCLUDE}/SDL)
include_directories(${ANDROID_SDL_PATH})




# copy shared libraries over from the Android toolchain directory
# FIXME: this should be automated as post-build-step by invoking the ndk-depends command
# to find out the .so's, and copy them over
file(COPY ${ANDROID_SYSROOT_LIB}/libm.so 		DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY ${ANDROID_SYSROOT_LIB}/liblog.so 		DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY ${ANDROID_SYSROOT_LIB}/libdl.so 		DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY ${ANDROID_SYSROOT_LIB}/libc.so 		DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY ${ANDROID_SYSROOT_LIB}/libandroid.so 	DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY ${ANDROID_SYSROOT_LIB}/libGLESv2.so 	DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY ${ANDROID_SYSROOT_LIB}/libEGL.so 		DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY ${ANDROID_SYSROOT_LIB}/libSDL2.so 	DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY ${ANDROID_SYSROOT_LIB}/libstdc++.so 	DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY ${ANDROID_SYSROOT_LIB}/libc.so 		DESTINATION ${ANDROID_SO_OUTDIR})




# ### panui ###################################################################################################
add_definitions    (-Dpl_andr)
include_directories( 	${PANUI_PATH} )
include_directories( 	${PANUI_INCLUDE} )
include_directories( 	${ANDROID_SYSROOT_INCLUDE}/freetype2)
file(COPY 	     		${PANUI_LIBFILE_PATH}/libpanui.so 			DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY 	     		${LIB_FREETYPE_STATIC}/libfreetype.a 		DESTINATION ${ANDROID_SO_OUTDIR})
file(COPY 	     		${PANUI_LIBFILE_PATH}/libgnustl_shared.so 	DESTINATION ${ANDROID_SO_OUTDIR})
link_directories( 		${PANUI_LIBFILE_PATH})
add_definitions (   	-DGLM_FORCE_RADIANS)




# ### c++ 11 ####################################################################################################
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



# link libs
target_link_libraries(${target} ${LINK_LIBS} )

# ant and adb
#add_custom_command ( cd ${CMAKE_CURRENT_SOURCE_DIR}) 		# in android dir
#add_custom_command ( ant debug) 		         		# ant debug
#add_custom_command ( adb install -r bin/KnxApp-debug.apk ) 	# install 

