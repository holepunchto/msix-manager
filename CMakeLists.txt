cmake_minimum_required(VERSION 4.0)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)

project(msix_manager C CXX)

add_bare_module(msix_manager)

target_sources(
  ${msix_manager}
  PRIVATE
    binding.cc
)

target_compile_options(
  ${msix_manager}
  PRIVATE
    /GR-
)

target_precompile_headers(
  ${msix_manager}
  PRIVATE
    lib/windows-sdk.h
)
