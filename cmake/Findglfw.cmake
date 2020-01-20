set(FIND_GLFW_PATHS ~/glfw/)

#find_path(GAINPUT_INCLUDE_DIR PATH_SUFFIXES lib/include PATHS ${FIND_GAINPUT_PATHS})
set(GLFW_INCLUDE_DIR ~/glfw/include/)

#find_library(GAINPUT_LIBRARY NAMES libgainputstatic PATHS ${FIND_GAINPUT_PATHS}build/lib)
find_library(GLFW_LIBRARY PATHS ~/glfw/glfw-build/src/ NAMES libglfw3.a)
#set(GAINPUT_LIBRARY ~/gainput/build/lib/libgainputstatic.a)