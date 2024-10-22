####################
#      Global      #
####################

cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)


#####################
#      Project      #
#####################

# Project variables
set(LOCAL_PROJECT_NAME        "lidar")
set(LOCAL_PROJECT_VERSION     "0.0.1")
set(LOCAL_PROJECT_DESCRIPTION "Description")

# Header files (relative to "include" directory)
set(HEADERS
)

# Source files (relative to "src" directory)
set(SOURCES
    main.cpp
)

# Compiler definitions
set(DEFINES

)

# Compiler options
set(OPTIONS

)

# Project setup
project(${LOCAL_PROJECT_NAME}
        VERSION ${LOCAL_PROJECT_VERSION}
        DESCRIPTION ${LOCAL_PROJECT_DESCRIPTION}
        LANGUAGES CXX)

# Create executable
add_executable(${LOCAL_PROJECT_NAME})

# Adding the GLM header-only library
add_subdirectory(libs/glm)  # This adds glm to your project

# Organize headers and sources
list(TRANSFORM HEADERS PREPEND "include/")
list(TRANSFORM SOURCES PREPEND "src/")

# Include headers
target_include_directories(${LOCAL_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(${LOCAL_PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS})

# Compiler definitions and flags
target_compile_definitions(${LOCAL_PROJECT_NAME} PRIVATE ${DEFINES})
target_compile_options(${LOCAL_PROJECT_NAME} PRIVATE ${OPTIONS})

# Set output directory
set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "bin")

####################
#   Dependencies   #
####################

find_package(OpenGL REQUIRED)  # Find OpenGL
find_package(GLEW REQUIRED)    # Find GLEW
find_package(glfw3 REQUIRED)   # Find GLFW (installed via package manager)

# Link required libraries (OpenGL, GLEW, GLFW)
target_link_libraries(${LOCAL_PROJECT_NAME} PRIVATE OpenGL::GL GLEW::GLEW glfw glm)