cmake_minimum_required (VERSION 3.14)

set(CMAKE_C_COMPILER "/usr/bin/gcc") 

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl -lpthread")

project("main")

set(CMAKE_C_STANDARD)

# Separator dir in WIN
set(SEP_WIN "\\\\")

# Path to lib DLL 
# set(PATH_LIB "${CMAKE_SOURCE_DIR}/lib")

# Exe filename
set(EXECUTABLE_FILENAME "main")

# Init PID path
set(PID_PATH "")

set(EXECUTABLE_FILENAME_FULL "${EXECUTABLE_FILENAME}.exe")

# configure_file (config.h.in config.h @ONLY)
#configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)

# Добавьте источник в исполняемый файл этого проекта.
add_executable("${EXECUTABLE_FILENAME}" 
"main.c"
"lib/error_functions.h"
"lib/error_functions.c"
"lib/tlpi_hdr.h"
"lib/get_num.c"
"lib/get_num.h"
)

set_property(TARGET "${EXECUTABLE_FILENAME}" PROPERTY C_STANDARD 11)
target_compile_features("${EXECUTABLE_FILENAME}" PRIVATE c_std_11)