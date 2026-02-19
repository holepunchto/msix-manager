cmake_minimum_required(VERSION 4.0)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)
find_package(cmake-napi REQUIRED PATHS node_modules/cmake-napi)
find_package(cmake-npm REQUIRED PATHS node_modules/cmake-npm)

project(msix_manager C CXX)

add_bare_module(msix_manager_bare)

target_sources(
  ${msix_manager_bare}
  PRIVATE
    binding.cc
)

target_compile_options(
  ${msix_manager_bare}
  PRIVATE
    /GR-
)

target_precompile_headers(
  ${msix_manager_bare}
  PRIVATE
    lib/windows-sdk.h
)

target_link_libraries(
  ${msix_manager_bare}
  PUBLIC
    WindowsApp
)

add_napi_module(msix_manager_node)

target_sources(
  ${msix_manager_node}
  PRIVATE
    binding.cc
)

target_compile_options(
  ${msix_manager_node}
  PRIVATE
    /GR-
)

target_compile_definitions(
  ${msix_manager_node}
  PRIVATE
    NAPI_VERSION=9
)

target_precompile_headers(
  ${msix_manager_node}
  PRIVATE
    lib/windows-sdk.h
)

target_link_libraries(
  ${msix_manager_node}
  PUBLIC
    WindowsApp
)

resolve_node_module(bare-compat-napi compat)

target_include_directories(
  ${msix_manager_node}
  PRIVATE
    "${compat}/include"
)
