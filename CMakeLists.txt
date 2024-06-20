cmake_minimum_required(VERSION 3.20)
project(vdv)

include(cmake/buildcache.cmake)
include(cmake/pkg.cmake)


# --- LINT ---
option(VDV_LINT "Run clang-tidy with the compiler." OFF)
if (VDV_LINT)
    # clang-tidy will be run on all targets defined hereafter
    include(cmake/clang-tidy.cmake)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(vdv-compile-options
            -Weverything
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-newline-eof
            -Wno-missing-prototypes
            -Wno-padded
            -Wno-double-promotion
            -Wno-undef
            -Wno-undefined-reinterpret-cast
            -Wno-float-conversion
            -Wno-global-constructors
            -Wno-exit-time-destructors
            -Wno-switch-enum
            -Wno-c99-designator
            -Wno-zero-as-null-pointer-constant
            -Wno-missing-noreturn
            -Wno-undefined-func-template
            -Wno-unsafe-buffer-usage
            -Wno-c++20-compat
            -Wno-reserved-macro-identifier
            -Wno-documentation-unknown-command
            -Wno-duplicate-enum
            -Wno-ctad-maybe-unsupported
            -Wno-unknown-pragmas
            -Werror)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(vdv-compile-options -Wall -Wextra -Werror -Wno-unknown-pragmas)
elseif (MSVC)
    set(vdv-compile-options /WX)
else ()
    set(vdv-compile-options
            -Wall
            -Wextra
            -Wno-maybe-uninitialized)
    if (NOT CMAKE_CROSSCOMPILING)
        set(vdv-compile-options ${vdv-compile-options} -Werror)
    endif ()
endif ()


# --- LIB ---
file(GLOB_RECURSE vdv-files src/*.cc)
add_library(vdv ${vdv-files})
target_include_directories(vdv PUBLIC include)
target_link_libraries(vdv PUBLIC pugixml utl http-client date)
target_compile_features(vdv PUBLIC cxx_std_23)
target_compile_options(vdv PRIVATE ${vdv-compile-options})


# --- CLIENT ---
add_executable(vdv-client exe/client.cc)
target_compile_features(vdv-client PUBLIC cxx_std_23)
target_link_libraries(vdv-client vdv boost-program_options web-server http-client)


# --- SERVER ---
add_executable(vdv-server exe/server.cc)
target_compile_features(vdv-server PUBLIC cxx_std_23)
target_link_libraries(vdv-server vdv boost-program_options web-server http-client)


# --- TEST ---
add_library(vdv-generated INTERFACE)
target_include_directories(vdv-generated INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/generated)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_dir.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/generated/test_dir.h
)
file(GLOB_RECURSE vdv-test-files test/*.cc)
add_executable(vdv-test ${vdv-test-files})
target_link_libraries(vdv-test vdv gtest ianatzdb-res vdv-generated utl)
target_compile_options(vdv-test PRIVATE ${vdv-compile-options})
