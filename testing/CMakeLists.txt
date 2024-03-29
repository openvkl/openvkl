## Copyright 2019 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

add_library(openvkl_testing_cpu STATIC
  apps/AppInit.cpp
  volume/TestingVdbTorusVolume.cpp
)

target_link_libraries(openvkl_testing_cpu PUBLIC
  openvkl
  rkcommon::rkcommon
  openvkl_utility
  openvkl_module_cpu_device
)

target_include_directories(openvkl_testing_cpu
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(openvkl_testing_cpu PUBLIC
  OPENVKL_TESTING_CPU
)

if (${OPENVKL_ENABLE_DEVICE_GPU})
  add_library(openvkl_testing_gpu STATIC
    apps/AppInit.cpp
  )

  target_link_libraries(openvkl_testing_gpu PUBLIC
    openvkl
    rkcommon::rkcommon
    openvkl_utility
    openvkl_module_gpu_device
  )

  target_include_directories(openvkl_testing_gpu
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}
  )

  target_compile_definitions(openvkl_testing_gpu PUBLIC
    OPENVKL_TESTING_GPU
  )
endif()

if (BUILD_BENCHMARKS)
  set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

  add_subdirectory(external/benchmark)
endif()

add_subdirectory(apps)
