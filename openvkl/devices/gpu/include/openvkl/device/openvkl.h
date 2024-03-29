// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <sycl/sycl.hpp>

#ifdef __cplusplus
#include <cstdint>
#include <cstdlib>
#else
#include <stdint.h>
#include <stdlib.h>
#endif

#include <openvkl/common.h>

#include "max_iterator_size.h"

#ifdef __cplusplus
extern "C" {
#endif

OPENVKL_INTERFACE void vklInit();

///////////////////////////////////////////////////////////////////////////////
// Sampling ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SYCL_EXTERNAL float vklComputeSample(
    const VKLSampler *sampler,
    const vkl_vec3f *objectCoordinates,
    unsigned int attributeIndex VKL_DEFAULT_VAL(= 0),
    float time VKL_DEFAULT_VAL(= 0),
    const VKLFeatureFlags featureFlags
        VKL_DEFAULT_VAL(= VKL_FEATURE_FLAGS_DEFAULT));

SYCL_EXTERNAL void vklComputeSampleM(
    const VKLSampler *sampler,
    const vkl_vec3f *objectCoordinates,
    float *samples,
    unsigned int M,
    const unsigned int *attributeIndices,
    float time VKL_DEFAULT_VAL(= 0),
    const VKLFeatureFlags featureFlags
        VKL_DEFAULT_VAL(= VKL_FEATURE_FLAGS_DEFAULT));

SYCL_EXTERNAL vkl_vec3f
vklComputeGradient(const VKLSampler *sampler,
                   const vkl_vec3f *objectCoordinates,
                   unsigned int attributeIndex VKL_DEFAULT_VAL(= 0),
                   float time VKL_DEFAULT_VAL(= 0),
                   const VKLFeatureFlags featureFlags
                       VKL_DEFAULT_VAL(= VKL_FEATURE_FLAGS_DEFAULT));

///////////////////////////////////////////////////////////////////////////////
// Interval iterators /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct IntervalIterator;
typedef struct IntervalIterator *VKLIntervalIterator;

SYCL_EXTERNAL size_t
vklGetIntervalIteratorSize(const VKLIntervalIteratorContext *context);

SYCL_EXTERNAL VKLIntervalIterator
vklInitIntervalIterator(const VKLIntervalIteratorContext *context,
                        const vkl_vec3f *origin,
                        const vkl_vec3f *direction,
                        const vkl_range1f *tRange,
                        float time,
                        void *buffer,
                        const VKLFeatureFlags featureFlags
                            VKL_DEFAULT_VAL(= VKL_FEATURE_FLAGS_DEFAULT));

typedef struct
{
  vkl_range1f tRange;
  vkl_range1f valueRange;
  float nominalDeltaT;
} VKLInterval;

SYCL_EXTERNAL int vklIterateInterval(
    VKLIntervalIterator iterator,
    VKLInterval *interval,
    const VKLFeatureFlags featureFlags
        VKL_DEFAULT_VAL(= VKL_FEATURE_FLAGS_DEFAULT));

///////////////////////////////////////////////////////////////////////////////
// Hit iterators //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct HitIterator;
typedef struct HitIterator *VKLHitIterator;

SYCL_EXTERNAL size_t
vklGetHitIteratorSize(const VKLHitIteratorContext *context);

SYCL_EXTERNAL VKLHitIterator
vklInitHitIterator(const VKLHitIteratorContext *context,
                   const vkl_vec3f *origin,
                   const vkl_vec3f *direction,
                   const vkl_range1f *tRange,
                   float time,
                   void *buffer,
                   const VKLFeatureFlags featureFlags
                       VKL_DEFAULT_VAL(= VKL_FEATURE_FLAGS_DEFAULT));

typedef struct
{
  float t;
  float sample;
  float epsilon;
} VKLHit;

SYCL_EXTERNAL int vklIterateHit(
    VKLHitIterator iterator,
    VKLHit *hit,
    const VKLFeatureFlags featureFlags
        VKL_DEFAULT_VAL(= VKL_FEATURE_FLAGS_DEFAULT));

#ifdef __cplusplus
}  // extern "C"
#endif
