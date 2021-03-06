## Copyright 2019-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

include(openvkl_ispc)
include(openvkl_macros)

include_directories_ispc(
  ${PROJECT_SOURCE_DIR}/openvkl/include
  ${RKCOMMON_INCLUDE_DIRS}
)

option(VKL_BUILD_VDB_ITERATOR_SIZE_HELPER "Build helper program that computes sizeof(VdbIterator)" OFF)

# width-specific builds
foreach(TARGET_WIDTH 4 8 16)

  openvkl_get_width_enabled(${TARGET_WIDTH} TARGET_WIDTH_ENABLED)

  set(TARGET_WIDTH_ENABLED_${TARGET_WIDTH} ${TARGET_WIDTH_ENABLED})

  if(NOT ${TARGET_WIDTH_ENABLED})
    continue()
  endif()

  set(TARGET_NAME "openvkl_module_cpu_device_${TARGET_WIDTH}")

  if (TARGET_WIDTH EQUAL 4)
    set(ISPC_TARGETS_OVERRIDE ${OPENVKL_ISPC_TARGET_LIST_4})
  elseif (TARGET_WIDTH EQUAL 8)
    set(ISPC_TARGETS_OVERRIDE ${OPENVKL_ISPC_TARGET_LIST_8})
  elseif (TARGET_WIDTH EQUAL 16)
    set(ISPC_TARGETS_OVERRIDE ${OPENVKL_ISPC_TARGET_LIST_16})
  endif()

  set(ISPC_DEFINITIONS "-DVKL_TARGET_WIDTH=${TARGET_WIDTH}")

  openvkl_add_library_ispc(${TARGET_NAME} SHARED
    api/CPUDevice.cpp
    api/CPUDevice.ispc
    iterator/DefaultIterator.cpp
    iterator/DefaultIterator.ispc
    iterator/GridAcceleratorIterator.cpp
    iterator/GridAcceleratorIterator.ispc
    iterator/GridAcceleratorIteratorSize.ispc
    iterator/UnstructuredIterator.cpp
    iterator/UnstructuredIterator.ispc
    observer/Observer.cpp
    observer/ObserverRegistry.cpp
    observer/ObserverRegistry.ispc
    sampler/Sampler.cpp
    sampler/Sampler.ispc
    value_selector/ValueSelector.cpp
    value_selector/ValueSelector.ispc
    volume/amr/AMRAccel.cpp
    volume/amr/AMRData.cpp
    volume/amr/AMRVolume.cpp
    volume/amr/AMRVolume.ispc
    volume/amr/CellRef.ispc
    volume/amr/DualCell.ispc
    volume/amr/method_current.ispc
    volume/amr/method_finest.ispc
    volume/amr/method_octant.ispc
    volume/particle/ParticleVolume.cpp
    volume/particle/ParticleVolume.ispc
    volume/GridAccelerator.ispc
    volume/SharedStructuredVolume.ispc
    volume/StructuredVolume.cpp
    volume/StructuredRegularVolume.cpp
    volume/StructuredSphericalVolume.cpp
    volume/UnstructuredVolume.cpp
    volume/UnstructuredVolume.ispc
    volume/Volume.ispc
    volume/vdb/VdbVolume.cpp
    volume/vdb/VdbVolume.ispc
    volume/vdb/VdbSampler.cpp
    volume/vdb/VdbSampler.ispc
    volume/vdb/VdbSampler_valueRange.ispc
    volume/vdb/VdbSampler_computeSample.ispc
    volume/vdb/VdbSampler_computeSample_uniform.ispc
    volume/vdb/VdbSampler_computeSample_stream.ispc
    volume/vdb/VdbSampler_computeSampleM.ispc
    volume/vdb/VdbSampler_computeSampleM_uniform.ispc
    volume/vdb/VdbSampler_computeSampleM_stream.ispc
    volume/vdb/VdbSampler_computeGradient.ispc
    volume/vdb/VdbSampler_computeGradient_stream.ispc
    volume/vdb/VdbInnerNodeObserver.cpp
    volume/vdb/VdbIterator.cpp
    volume/vdb/VdbIterator.ispc
    volume/vdb/VdbLeafAccessObserver.cpp
  )

  unset(ISPC_DEFINITIONS)
  unset(ISPC_TARGETS_OVERRIDE)

  # hidden symbols by default, so each target width library can use its internal
  # ISPC-built components without conflict
  set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 1
  )

  target_compile_definitions(${TARGET_NAME} PRIVATE
    "VKL_TARGET_WIDTH=${TARGET_WIDTH}")

  set(width_compile_options "")
  openvkl_get_compile_options_for_width(${TARGET_WIDTH} width_compile_options)
  separate_arguments(width_compile_options UNIX_COMMAND "${width_compile_options}")

  message(STATUS "using compile options for width ${TARGET_WIDTH}: ${width_compile_options}")

  target_compile_options(${TARGET_NAME} PRIVATE ${width_compile_options})

  target_include_directories(${TARGET_NAME}
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
      $<BUILD_INTERFACE:${ISPC_TARGET_DIR}>
  )

  target_link_libraries(${TARGET_NAME}
    PUBLIC
      openvkl
      rkcommon::rkcommon
    PRIVATE
      embree
  )

  openvkl_install_library(${TARGET_NAME})

  # accumulate targets to be linked into single shared library
  list(APPEND ACTIVE_WIDTH_TARGETS ${TARGET_NAME})

endforeach(TARGET_WIDTH)


# shared library linking to all width-specific targets
add_library(openvkl_module_cpu_device SHARED exports.cpp ${VKL_RESOURCE})

foreach(TARGET_WIDTH 4 8 16)
  if (TARGET_WIDTH_ENABLED_${TARGET_WIDTH})
    target_compile_definitions(openvkl_module_cpu_device PUBLIC
      "VKL_TARGET_WIDTH_ENABLED_${TARGET_WIDTH}")
  endif()
endforeach(TARGET_WIDTH)

target_include_directories(openvkl_module_cpu_device
  INTERFACE $<BUILD_INTERFACE:${ISPC_TARGET_DIR}>)

target_link_libraries(openvkl_module_cpu_device
  PUBLIC
    openvkl
  PRIVATE
    ${ACTIVE_WIDTH_TARGETS}
)

openvkl_install_library(openvkl_module_cpu_device)
