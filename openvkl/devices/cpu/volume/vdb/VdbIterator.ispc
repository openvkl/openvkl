// Copyright 2019-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "VdbGrid.h"
#include "VdbIterator.ih"
#include "VdbSampler.ih"
#include "common/export_util.h"
#include "math/box_utility.ih"
#include "rkcommon/math/math.ih"

// Generated headers.
#include "openvkl_vdb/VdbQueryVoxel_0.ih"
#include "openvkl_vdb/HDDA_0.ih"

// Ignore warning about exporting uniform-pointer-to-varying, as this is in
// fact legal.
#pragma ignore warning(all)
export void EXPORT_UNIQUE(VdbIterator_export,
                          uniform box1f &dummy_box1f,
                          const varying VdbIterator *uniform it)
{
}

void VdbIterator_iterateIntervalInternal(
    const int *uniform imask,
    void *uniform _self,
    void *uniform _interval,
    const uniform ValueRanges &valueRanges,
    const uniform bool /*elementaryCellIteration*/,
    uniform int *uniform _result);

export void EXPORT_UNIQUE(VdbIterator_Initialize,
                          const int *uniform imask,
                          void *uniform _self,
                          const void *uniform _sampler,
                          void *uniform _originObject,
                          void *uniform _directionObject,
                          void *uniform _tRangeWorld,
                          void *uniform _valueSelector)
{
  if (!imask[programIndex]) {
    return;
  }

  const uniform VdbSampler *uniform sampler =
      (const uniform VdbSampler *uniform)_sampler;
  const uniform VdbGrid *uniform grid = sampler->grid;

  varying VdbIterator *uniform self = (varying VdbIterator * uniform) _self;
  self->grid                        = grid;
  self->valueSelector = (uniform ValueSelector * uniform) _valueSelector;

  // Initialize for hit iteration.
  self->iterateInterval                  = VdbIterator_iterateIntervalInternal;
  self->elementaryCellIterationSupported = false;

  const vec3f origin    = *((const varying vec3f *uniform)_originObject);
  const vec3f direction = *((const varying vec3f *uniform)_directionObject);
  box1f tRange          = *((const varying box1f *uniform)_tRangeWorld);

  // Transform the ray to index space where leaf level voxels have
  // size (1,1,1) and the root is at (0,0,0).
  const vec3f rootOffset =
      make_vec3f(grid->rootOrigin.x, grid->rootOrigin.y, grid->rootOrigin.z);
  const vec3f rayOrg = xfmPoint(grid->objectToIndex, origin) - rootOffset;
  const vec3f rayDir = xfmVector(grid->objectToIndex, direction);

  // This is an estimate of how far apart voxels are along the ray in object
  // space. We are basically measuring here how much the volume is scaled along
  // the ray, and voxels in index space have size 1.
  self->nominalDeltaT = length(direction) / length(rayDir);

  // Clip on active region.
  float tEnter = pos_inf;
  float tExit  = neg_inf;

  intersect_box(
      rayOrg,
      dir_safe_rcp(rayDir),
      ((varying vec3f)make_vec3f(0.f, 0.f, 0.f)),
      ((varying vec3f)make_vec3f(grid->activeSize.x,
                                 grid->activeSize.y,
                                 grid->activeSize.z)),
      tEnter,
      tExit);

  if (tEnter > tExit) {
    self->tMax = neg_inf;
    self->dda.t = 0;
  } else {
    const float tMin = max(tRange.lower, tEnter * 0.999f);
    self->tMax = min(tRange.upper, tExit * 1.001f);
    ddaInit(rayOrg, rayDir, tMin, sampler->maxIteratorDepth, self->dda);
  }
}

inline void VdbIterator_iterateIntervalInternal(
    const int *uniform imask,
    void *uniform _self,
    void *uniform _interval,
    const uniform ValueRanges &inputValueRanges,
    const uniform bool /*elementaryCellIteration*/,
    uniform int *uniform _result)
{
  if (!imask[programIndex]) {
    return;
  }

  varying int *uniform result = (varying int *uniform)_result;
  *result                     = false;

  varying VdbIterator *uniform self = (varying VdbIterator * uniform) _self;
  if (!(self->dda.t <= self->tMax)) {
    return;
  }


  VdbVoxelDescriptor voxel;
  while (self->dda.t <= self->tMax) {
    uint32 exitDepth = 0;
    VdbIterator_queryVoxel(self->grid,
                           self->dda.idx[DDA_STATE_X_OFFSET(self->dda.level)],
                           self->dda.idx[DDA_STATE_Y_OFFSET(self->dda.level)],
                           self->dda.idx[DDA_STATE_Z_OFFSET(self->dda.level)],
                           self->dda.level,
                           inputValueRanges,
                           voxel);
    if (!voxel.isEmpty)
      break;

    hddaStep(self->grid, voxel, self->dda);
  }

  if (self->dda.t <= self->tMax) {
    varying Interval *uniform interval = (varying Interval * uniform) _interval;
    interval->valueRange               = voxel.valueRange;
    interval->tRange.lower             = self->dda.t;

    // Move on to the next voxel to fulfill invariant.
    hddaStep(self->grid, voxel, self->dda);

    interval->tRange.upper  = min(self->dda.t, self->tMax);
    interval->nominalDeltaT = self->nominalDeltaT;

    *result = true;
  }
}

export void EXPORT_UNIQUE(VdbIterator_iterateInterval,
                          const int *uniform imask,
                          void *uniform _self,
                          void *uniform _interval,
                          uniform int *uniform _result)
{
  if (!imask[programIndex]) {
    return;
  }

  varying VdbIterator *uniform self = (varying VdbIterator * uniform) _self;

  uniform ValueRanges valueRanges;
  if (self->valueSelector) {
    valueRanges = self->valueSelector->ranges;
  } else {
    valueRanges = make_ValueRanges_full();
  }

  VdbIterator_iterateIntervalInternal(
      imask, _self, _interval, valueRanges, false, _result);
}
