cmake_minimum_required(VERSION 3.8)

add_library(p6)
add_library(p6::p6 ALIAS p6)
target_compile_features(p6 PRIVATE cxx_std_11)

if(MSVC)
    target_compile_options(p6 PRIVATE /WX /W4)
else()
    target_compile_options(p6 PRIVATE -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wsign-conversion)
endif()

# ---Add glpp-extended---
add_subdirectory(third-party/glpp-extended)
target_compile_definitions(glpp PRIVATE $<$<CONFIG:Debug>:GLPP_CHECK_ERRORS>)
target_link_libraries(p6 PUBLIC glpp::extended)
# ---Add img---
add_subdirectory(third-party/img)
target_link_libraries(p6 PUBLIC img::img)
# ---Add glfw---
add_subdirectory(third-party/glfw)
target_link_libraries(p6 PUBLIC glfw)

# ---Add p6 files---
target_include_directories(p6 PUBLIC include)
target_sources(p6 PRIVATE
    src/Context.cpp
    src/Shader.cpp
    src/Image.cpp
    src/details/UniqueGlfwWindow.cpp
    src/details/Time/Clock.cpp
    src/details/Time/Clock_FixedTimestep.cpp
    src/details/Time/Clock_Realtime.cpp
    src/details/RectRenderer.cpp
)

# ---Copy files and folders---
# NB: these functions require policy CMP0112 which was set to NEW in CMake 3.19 (https://cmake.org/cmake/help/latest/policy/CMP0112.html#policy:CMP0112)
# If you are using a version of CMake lower than 3.19 then you need to call `cmake_policy(SET CMP0112 NEW)` just after `cmake_minimum_required(...)` in your main CMakeLists.txt file
# Otherwise you will get a "Cyclic dependencies" error
function(internal_p6_copy FILE_OR_DIR COMMAND_NAME)
    string(SHA1 TARGET_HASH ${FILE_OR_DIR})      # Create a unique name for our custom target. We can't use ${FILE_OR_DIR} directly because it could contain invalid characters like spaces
    set(TARGET_NAME CopyToOutput_${TARGET_HASH}) #
    add_custom_target(${TARGET_NAME}
        COMMENT "Copying \"${FILE_OR_DIR}\""
        COMMAND ${CMAKE_COMMAND} -E ${COMMAND_NAME}
            ${CMAKE_SOURCE_DIR}/${FILE_OR_DIR}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/${FILE_OR_DIR}
    )
    add_dependencies(${PROJECT_NAME} ${TARGET_NAME}) # Mark our custom target as a dependency of the main project, so that whenever the main project is launched our custom target is triggered and files are copied
endfunction()

function(p6_copy_file FILENAME)
    internal_p6_copy(${FILENAME} copy)
endfunction()

function(p6_copy_folder FOLDERNAME)
    internal_p6_copy(${FOLDERNAME} copy_directory)
endfunction()