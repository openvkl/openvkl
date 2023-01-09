// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../cpu/volume/GridAccelerator.h"
#include "../../cpu/volume/GridAcceleratorShared.h"
#include "vklCompute.h"

#include <rkcommon/common.h>
#include <rkcommon/math/math.ih>

using namespace rkcommon::math;

#include "common.h"

#include "../common/Data.ih"

// from vec.ih
inline vec3i bitwise_AND(const vec3i &a, const int b)
{
  return {a.x & b, a.y & b, a.z & b};
}

// from Hit.ih
// this should match the layout of VKLHit
struct Hit
{
  float t;
  float sample;
  float epsilon;
};

// from Interval.ih
struct Interval
{
  box1f tRange;
  box1f valueRange;
  float nominalDeltaT;
};

inline void resetInterval(Interval &interval)
{
  interval.tRange.lower     = 1.f;
  interval.tRange.upper     = -1.f;
  interval.valueRange.lower = 0.f;
  interval.valueRange.upper = 0.f;
  interval.nominalDeltaT    = 0.f;
}

// from math.ih
inline float divide_safe(float f)
{
  return 1.f / (abs(f) < flt_min ? (f >= 0.f ? flt_min : -flt_min) : f);
}

inline vec3f divide_safe(vec3f f)
{
  return vec3f{
      1.f / (abs(f.x) < flt_min ? (f.x >= 0.f ? flt_min : -flt_min) : f.x),
      1.f / (abs(f.y) < flt_min ? (f.y >= 0.f ? flt_min : -flt_min) : f.y),
      1.f / (abs(f.z) < flt_min ? (f.z >= 0.f ? flt_min : -flt_min) : f.z)};
}

// from box_utility.ih
inline bool isempty1f(const box1f &box)
{
  return box.upper <= box.lower;
}

inline bool overlaps1f(const box1f &box1, const box1f &box2)
{
  return box1.upper >= box2.lower && box1.lower <= box2.upper;
}

inline box1f intersectBox(const vec3f &origin,
                          const vec3f &direction,
                          const box3f &box,
                          const box1f &rangeLimit)
{
  const vec3f mins = (box.lower - origin) * divide_safe(direction);
  const vec3f maxs = (box.upper - origin) * divide_safe(direction);

  box1f result{0.f, 0.f};
  vec3f mm     = min(mins, maxs);
  vec3f MM     = max(mins, maxs);
  result.lower = reduce_max(vec4f(mm.x, mm.y, mm.z, rangeLimit.lower));
  result.upper = reduce_min(vec4f(MM.x, MM.y, MM.z, rangeLimit.upper));

  return result;
}

#include "../../cpu/iterator/IteratorContextShared.h"

// from ValueRanges.ih
inline bool valueRangesOverlap(const ispc::ValueRanges &valueRanges,
                               const box1f &r)
{
  if (valueRanges.numRanges == 0) {
    return true;
  }

  if (overlaps1f(
          box1f{valueRanges.rangesMinMax.lower, valueRanges.rangesMinMax.upper},
          r)) {
    for (int i = 0; i < valueRanges.numRanges; i++) {
      if (overlaps1f(
              box1f{valueRanges.ranges[i].lower, valueRanges.ranges[i].upper},
              r)) {
        return true;
      }
    }
  }

  return false;
}

// from GridAcceleratorIterator.ih
struct GridAcceleratorIteratorIntervalState
{
  vec3i currentCellIndex;
  float nominalDeltaT;  // constant for all intervals
};

struct GridAcceleratorIteratorHitState
{
  bool activeCell;
  vec3i currentCellIndex;
  box1f currentCellTRange;
};

struct GridAcceleratorIterator
{
  const ispc::IntervalIteratorContext *context;
  vec3f origin;
  vec3f direction;
  box1f tRange;
  float time;

  // common state
  box1f boundingBoxTRange;

  // interval iterator state
  GridAcceleratorIteratorIntervalState intervalState;

  // hit iterator state
  GridAcceleratorIteratorHitState hitState;
};

#include "../../cpu/volume/StructuredVolumeShared.h"

// from GridAcceleratorIterator.ispc
inline void GridAcceleratorIteratorU_Init(void *_self,
                                          void *_context,
                                          void *_origin,
                                          void *_direction,
                                          void *_tRange,
                                          void *_time)
{
  GridAcceleratorIterator *self = (GridAcceleratorIterator *)_self;

  self->context   = (const ispc::IntervalIteratorContext *)_context;
  self->origin    = *((vec3f *)_origin);
  self->direction = *((vec3f *)_direction);
  self->tRange    = *((box1f *)_tRange);
  self->time      = *((float *)_time);

  const ispc::SharedStructuredVolume *volume =
      (const ispc::SharedStructuredVolume *)
          self->context->super.sampler->volume;

  self->boundingBoxTRange =
      intersectBox(*((vec3f *)_origin),
                   *((vec3f *)_direction),
                   box3f{make_vec3f_rkcommon(volume->boundingBox.lower),
                         make_vec3f_rkcommon(volume->boundingBox.upper)},
                   self->tRange);

  self->intervalState.currentCellIndex = vec3i{-1, -1, -1};

#if 0
    self->intervalState.nominalDeltaT =
        reduce_min(volume->gridSpacing *
                   rcp_safe(absf(self->direction))); /* in ray space */
#else
  float x   = volume->gridSpacing.x * divide_safe(std::abs(self->direction.x));
  float y   = volume->gridSpacing.y * divide_safe(std::abs(self->direction.y));
  float z   = volume->gridSpacing.z * divide_safe(std::abs(self->direction.z));
  float min = x < y ? x : y;
  self->intervalState.nominalDeltaT = min < z ? min : z;
#endif

  self->hitState.currentCellIndex  = vec3i{-1, -1, -1};
  self->hitState.currentCellTRange = {std::numeric_limits<float>::infinity(),
                                      -std::numeric_limits<float>::infinity()};
}

// from SharedStructuredVolume.ih
inline void transformLocalToObject_uniform_structured_regular(
    const ispc::SharedStructuredVolume *self,
    const vec3f &localCoordinates,
    vec3f &objectCoordinates)
{
  objectCoordinates =
      vec3f{self->gridOrigin.x + localCoordinates.x * self->gridSpacing.x,
            self->gridOrigin.y + localCoordinates.y * self->gridSpacing.y,
            self->gridOrigin.z + localCoordinates.z * self->gridSpacing.z};
}

// from GridAccelerator.ispc
inline ispc::uint32 GridAccelerator_getCellIndex1D(
    ispc::GridAccelerator *accelerator, const vec3i &cellIndex)
{
  const vec3i brickIndex = {cellIndex.x >> BRICK_WIDTH_BITCOUNT,
                            cellIndex.y >> BRICK_WIDTH_BITCOUNT,
                            cellIndex.z >> BRICK_WIDTH_BITCOUNT};

  const ispc::uint32 brickAddress =
      brickIndex.x + accelerator->bricksPerDimension.x *
                         (brickIndex.y + accelerator->bricksPerDimension.y *
                                             (ispc::uint32)brickIndex.z);

  const vec3i cellOffset = bitwise_AND(cellIndex, BRICK_WIDTH - 1);

  return brickAddress << (3 * BRICK_WIDTH_BITCOUNT) |
         cellOffset.z << (2 * BRICK_WIDTH_BITCOUNT) |
         cellOffset.y << (BRICK_WIDTH_BITCOUNT) | cellOffset.x;
}

inline void GridAccelerator_getCellValueRange(
    ispc::GridAccelerator *accelerator,
    const vec3i &cellIndex,
    ispc::uint32 attributeIndex,
    box1f &valueRange)
{
  const ispc::uint32 cellIndex1D =
      GridAccelerator_getCellIndex1D(accelerator, cellIndex);
  auto _valueRange =
      accelerator->cellValueRanges[attributeIndex * accelerator->cellCount +
                                   cellIndex1D];
  valueRange = {_valueRange.lower, _valueRange.upper};
}

inline box3f GridAccelerator_getCellBounds(
    const ispc::GridAccelerator *accelerator, const vec3i &index)
{
  ispc::SharedStructuredVolume *volume = accelerator->volume;

  /* coordinates of the lower corner of the cell in object coordinates */
  vec3f lower;
  transformLocalToObject_uniform_structured_regular(
      volume,
      vec3f{(float)(index.x << CELL_WIDTH_BITCOUNT),
            (float)(index.y << CELL_WIDTH_BITCOUNT),
            (float)(index.z << CELL_WIDTH_BITCOUNT)},
      lower);

  /* coordinates of the upper corner of the cell in object coordinates */
  vec3f upper;
  transformLocalToObject_uniform_structured_regular(
      volume,
      vec3f{(float)((index.x + 1) << CELL_WIDTH_BITCOUNT),
            (float)((index.y + 1) << CELL_WIDTH_BITCOUNT),
            (float)((index.z + 1) << CELL_WIDTH_BITCOUNT)},
      upper);

  return (box3f{lower, upper});
}

inline bool GridAccelerator_nextCell(const ispc::GridAccelerator *accelerator,
                                     const GridAcceleratorIterator *iterator,
                                     vec3i &cellIndex,
                                     box1f &cellTRange)
{
  ispc::SharedStructuredVolume *volume = accelerator->volume;

  const bool firstCell = cellIndex.x == -1;
  box1f cellInterval{
      std::numeric_limits<float>::infinity(),
      -std::numeric_limits<float>::infinity()};  // dpcpp doesn't like
                                                 // constructor that uses
                                                 // posinf/neginf

  if (firstCell) {
    /* first iteration */
    vec3f localCoordinates;
    transformObjectToLocal_uniform_structured_regular(
        volume,
        iterator->origin +
            (iterator->boundingBoxTRange.lower) * iterator->direction,
        localCoordinates);

    cellIndex        = vec3i{(int)localCoordinates.x >> CELL_WIDTH_BITCOUNT,
                      (int)localCoordinates.y >> CELL_WIDTH_BITCOUNT,
                      (int)localCoordinates.z >> CELL_WIDTH_BITCOUNT};
    box3f cellBounds = GridAccelerator_getCellBounds(accelerator, cellIndex);

    /* clamp next cell bounds to ray iterator bounding range */
    cellInterval = intersectBox(iterator->origin,
                                iterator->direction,
                                cellBounds,
                                iterator->boundingBoxTRange);
  }

  if (!firstCell || isempty1f(cellInterval)) {
    /* subsequent iterations: only moving one cell at a time */

    /* TODO: see "A Fast Voxel Traversal Algorithm for Ray Tracing", John
       Amanatides, to see if this can be further simplified */

    /* transform object-space direction and origin to cell-space */
    const vec3f cellDirection = vec3f{
        iterator->direction.x * 1.f / volume->gridSpacing.x * RCP_CELL_WIDTH,
        iterator->direction.y * 1.f / volume->gridSpacing.y * RCP_CELL_WIDTH,
        iterator->direction.z * 1.f / volume->gridSpacing.z * RCP_CELL_WIDTH};

    const vec3f rcpCellDirection = divide_safe(cellDirection);

    vec3f cellOrigin;
    transformObjectToLocal_uniform_structured_regular(
        volume, iterator->origin, cellOrigin);
    cellOrigin = cellOrigin * RCP_CELL_WIDTH;

    /* sign of direction determines index delta (1 or -1 in each
       dimension) to far corner cell */
    const vec3i cornerDeltaCellIndex =
        vec3i{static_cast<int>(1 - 2 * (intbits(cellDirection.x) >> 31)),
              static_cast<int>(1 - 2 * (intbits(cellDirection.y) >> 31)),
              static_cast<int>(1 - 2 * (intbits(cellDirection.z) >> 31))};

    /* find exit distance within current cell */
    const vec3f t0 = (vec3f{static_cast<float>(cellIndex.x),
                            static_cast<float>(cellIndex.y),
                            static_cast<float>(cellIndex.z)} -
                      cellOrigin) *
                     rcpCellDirection;
    const vec3f t1 = (vec3f{static_cast<float>(cellIndex.x + 1),
                            static_cast<float>(cellIndex.y + 1),
                            static_cast<float>(cellIndex.z + 1)} -
                      cellOrigin) *
                     rcpCellDirection;
    const vec3f tMax = max(t0, t1);

    const float tExit = reduce_min(tMax);

    /* the next cell corresponds to the exit point (which will be a
       movement in one direction only) */
    vec3i deltaCellIndex = vec3i{tMax.x == tExit ? cornerDeltaCellIndex.x : 0,
                                 tMax.y == tExit ? cornerDeltaCellIndex.y : 0,
                                 tMax.z == tExit ? cornerDeltaCellIndex.z : 0};

    cellIndex = cellIndex + deltaCellIndex;

    box3f cellBounds = GridAccelerator_getCellBounds(accelerator, cellIndex);

    /* clamp next cell bounds to ray iterator bounding range */
    cellInterval = intersectBox(iterator->origin,
                                iterator->direction,
                                cellBounds,
                                iterator->boundingBoxTRange);
  }

  if (isempty1f(cellInterval)) {
    cellTRange = box1f{std::numeric_limits<float>::infinity(),
                       -std::numeric_limits<float>::infinity()};
    return false;
  } else {
    cellTRange = cellInterval;
    return true;
  }
}

// from GridAcceleratorIterator.ispc

inline void GridAcceleratorIterator_iterateInterval_uniform(
    const void *_self, const void *_interval, int *result)
{
  GridAcceleratorIterator *self = (GridAcceleratorIterator *)_self;
  Interval *interval            = (Interval *)_interval;

  if (isempty1f(self->boundingBoxTRange)) {
    *result = false;
    return;
  }

  const ispc::SharedStructuredVolume *volume =
      (const ispc::SharedStructuredVolume *)
          self->context->super.sampler->volume;

  while (GridAccelerator_nextCell(volume->accelerator,
                                  self,
                                  self->intervalState.currentCellIndex,
                                  interval->tRange)) {
    box1f cellValueRange{
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity()};  // dpcpp doesn't like
                                                   // constructor that uses
                                                   // posinf/neginf
    GridAccelerator_getCellValueRange(volume->accelerator,
                                      self->intervalState.currentCellIndex,
                                      self->context->super.attributeIndex,
                                      cellValueRange);
    bool returnInterval = false;

    if (valueRangesOverlap(self->context->super.valueRanges, cellValueRange)) {
      returnInterval = true;
    }

    if (returnInterval) {
      interval->valueRange    = cellValueRange;
      interval->nominalDeltaT = self->intervalState.nominalDeltaT;

      *result = true;
      return;
    }
  }

  *result = false;
}

// from Iterator.ih

/*
 * Intersect isosurfaces along the given ray using Newton-Raphson iteration.
 */
inline bool intersectSurfacesNewton(const ispc::SamplerShared *sampler,
                                    const vec3f &origin,
                                    const vec3f &direction,
                                    const box1f &tRange,
                                    const unsigned int attributeIndex,
                                    const float &time,
                                    const float step,
                                    const int numValues,
                                    const float *values,
                                    Hit &hit)
{
  /* our bracketing sample t-values will always be in multiples of `step`,
  to avoid artifacts / differences in hits between neighboring rays, or when
  moving between macrocell boundaries, for example.

  note that the current approach here takes only one Newton iteration, so
  consistent bracketing is especially important for "smooth" results. */
  const int minTIndex = floor(tRange.lower / step);
  const int maxTIndex = ceil(tRange.upper / step);

  float t0      = minTIndex * step;
  float sample0 = SharedStructuredVolume_computeSample_uniform(
      (const ispc::SharedStructuredVolume *)sampler->volume,
      origin + t0 * direction,
      sampler->filter,
      attributeIndex,
      time);

  float t;

  for (int i = minTIndex; i < maxTIndex; i++) {
    t = (i + 1) * step;

    const float sample = SharedStructuredVolume_computeSample_uniform(
        (const ispc::SharedStructuredVolume *)sampler->volume,
        origin + t * direction,
        sampler->filter,
        attributeIndex,
        time);

    float tHit    = std::numeric_limits<float>::infinity();
    float epsilon = std::numeric_limits<float>::infinity();
    float value   = std::numeric_limits<float>::infinity();

    if (!isnan(sample0 + sample) && (sample != sample0)) {
      for (int j = 0; j < numValues; j++) {
        if ((values[j] - sample0) * (values[j] - sample) <= 0.f) {
          const float rcpSamp = 1.f / (sample - sample0);
          float tIso          = std::numeric_limits<float>::infinity();
          if (!isnan(rcpSamp)) {
            tIso = t0 + (values[j] - sample0) * rcpSamp * (t - t0);
          }

          if (tIso < tHit && tIso >= tRange.lower && tIso <= tRange.upper) {
            tHit    = tIso;
            value   = values[j];
            epsilon = step * 0.125f;
          }
        }
      }

      if (tHit < std::numeric_limits<float>::infinity()) {
        hit.t       = tHit;
        hit.sample  = value;
        hit.epsilon = epsilon * length(direction); /* in object space */
        return true;
      }
    }

    t0      = t;
    sample0 = sample;
  }

  return false;
}

// from GridAcceleratorIterator.ispc

inline void GridAcceleratorIterator_iterateHit_uniform(const void *_self,
                                                       const void *_hit,
                                                       int *result)
{
  GridAcceleratorIterator *self = (GridAcceleratorIterator *)_self;

  const ispc::HitIteratorContext *hitContext =
      (const ispc::HitIteratorContext *)self->context;

  Hit *hit = (Hit *)_hit;

  if (isempty1f(self->boundingBoxTRange)) {
    *result = false;
    return;
  }

  if (hitContext->numValues == 0) {
    *result = false;
    return;
  }

  const ispc::SharedStructuredVolume *volume =
      (const ispc::SharedStructuredVolume *)
          self->context->super.sampler->volume;

  /* first iteration */
  if (self->hitState.currentCellIndex.x == -1) {
    self->hitState.activeCell =
        GridAccelerator_nextCell(volume->accelerator,
                                 self,
                                 self->hitState.currentCellIndex,
                                 self->hitState.currentCellTRange);
  }

  // reduce_min volume->gridSpacing
  const float x    = volume->gridSpacing.x;
  const float y    = volume->gridSpacing.y;
  const float z    = volume->gridSpacing.z;
  const float min  = x < y ? x : y;
  const float step = min < z ? min : z;

  while (self->hitState.activeCell) {
    box1f cellValueRange{std::numeric_limits<float>::infinity(),
                         -std::numeric_limits<float>::infinity()};
    GridAccelerator_getCellValueRange(volume->accelerator,
                                      self->hitState.currentCellIndex,
                                      self->context->super.attributeIndex,
                                      cellValueRange);

    bool cellValueRangeOverlap =
        valueRangesOverlap(self->context->super.valueRanges, cellValueRange);

    if (cellValueRangeOverlap) {
      bool foundHit =
          intersectSurfacesNewton(self->context->super.sampler,
                                  self->origin,
                                  self->direction,
                                  self->hitState.currentCellTRange,
                                  self->context->super.attributeIndex,
                                  self->time,
                                  0.5f * step,
                                  hitContext->numValues,
                                  hitContext->values,
                                  *hit);
      if (foundHit) {
        *result                                = true;
        self->hitState.currentCellTRange.lower = hit->t + hit->epsilon;

        /* move to next cell if next t passes the cell boundary */
        if (isempty1f(self->hitState.currentCellTRange)) {
          self->hitState.activeCell =
              GridAccelerator_nextCell(volume->accelerator,
                                       self,
                                       self->hitState.currentCellIndex,
                                       self->hitState.currentCellTRange);

          /* continue where we left off */
          self->hitState.currentCellTRange.lower = hit->t + hit->epsilon;
        }

        return;
      }
    }

    /* if no hits are found, move to the next cell; if a hit is found we'll
     stay in the cell to pursue other hits */
    self->hitState.activeCell =
        GridAccelerator_nextCell(volume->accelerator,
                                 self,
                                 self->hitState.currentCellIndex,
                                 self->hitState.currentCellTRange);
  }

  *result = false;
}
