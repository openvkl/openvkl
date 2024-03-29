// Copyright 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <type_traits>
#include "openvkl/openvkl.h"

enum VKLFeatureFlagsInternal : uint64_t
{
  VKL_FEATURE_FLAG_NONE = 0,

  // volume type
  VKL_FEATURE_FLAG_STRUCTURED_REGULAR_VOLUME   = 1 << 0,
  VKL_FEATURE_FLAG_STRUCTURED_SPHERICAL_VOLUME = 1 << 1,
  VKL_FEATURE_FLAG_UNSTRUCTURED_VOLUME         = 1 << 2,
  VKL_FEATURE_FLAG_PARTICLE_VOLUME             = 1 << 3,
  VKL_FEATURE_FLAG_AMR_VOLUME                  = 1 << 4,
  VKL_FEATURE_FLAG_VDB_VOLUME                  = 1 << 5,

  // filter (for sampling)
  VKL_FEATURE_FLAG_SAMPLE_FILTER_NEAREST = 1 << 6,
  VKL_FEATURE_FLAG_SAMPLE_FILTER_LINEAR  = 1 << 7,
  VKL_FEATURE_FLAG_SAMPLE_FILTER_CUBIC   = 1 << 8,

  // filter (for gradients)
  VKL_FEATURE_FLAG_GRADIENT_FILTER_NEAREST = 1 << 9,
  VKL_FEATURE_FLAG_GRADIENT_FILTER_LINEAR  = 1 << 10,
  VKL_FEATURE_FLAG_GRADIENT_FILTER_CUBIC   = 1 << 11,

  // temporal formats
  VKL_FEATURE_FLAG_HAS_TEMPORAL_FORMAT_CONSTANT     = 1 << 12,
  VKL_FEATURE_FLAG_HAS_TEMPORAL_FORMAT_STRUCTURED   = 1 << 13,
  VKL_FEATURE_FLAG_HAS_TEMPORAL_FORMAT_UNSTRUCTURED = 1 << 14,

  // for unstructured volumes only
  VKL_FEATURE_FLAG_HAS_CELL_TYPE_TETRAHEDRON = 1 << 15,
  VKL_FEATURE_FLAG_HAS_CELL_TYPE_HEXAHEDRON  = 1 << 16,
  VKL_FEATURE_FLAG_HAS_CELL_TYPE_WEDGE       = 1 << 17,
  VKL_FEATURE_FLAG_HAS_CELL_TYPE_PYRAMID     = 1 << 18,

  // for VDB volumes only
  VKL_FEATURE_FLAG_VDB_NODES_PACKED     = 1 << 19,
  VKL_FEATURE_FLAG_VDB_NODES_NOT_PACKED = 1 << 20,

  VKL_FEATURE_FLAG_ALL = 0xffffffffffffffff,
};

static_assert(std::is_same<std::underlying_type<VKLFeatureFlagsInternal>::type,
                           uint64_t>::value,
              "VKLFeatureFlagsInternal incompatible with VKLFeatureFlags");

static_assert(uint64_t(VKL_FEATURE_FLAGS_DEFAULT) == VKL_FEATURE_FLAG_ALL,
              "VKL_FEATURE_FLAGS_DEFAULT should match VKL_FEATURE_FLAG_ALL");

// convenience oeprators for working with the above
inline VKLFeatureFlagsInternal operator|(VKLFeatureFlagsInternal a,
                                         VKLFeatureFlagsInternal b)
{
  return static_cast<VKLFeatureFlagsInternal>(static_cast<int>(a) |
                                              static_cast<int>(b));
}

inline VKLFeatureFlagsInternal &operator|=(VKLFeatureFlagsInternal &a,
                                           VKLFeatureFlagsInternal b)
{
  return reinterpret_cast<VKLFeatureFlagsInternal &>(
      reinterpret_cast<int &>(a) |= static_cast<int>(b));
}
