// ======================================================================== //
// Copyright 2018 Intel Corporation                                         //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "../common/ManagedObject.h"
#include "../common/objectFactory.h"
#include "../iterator/RayIterator.h"
#include "openvkl/openvkl.h"
#include "ospcommon/math/box.h"

using namespace ospcommon;

namespace openvkl {
  namespace ispc_driver {

    template <int W>
    struct Volume : public ManagedObject
    {
      Volume()                   = default;
      virtual ~Volume() override = default;

      static Volume *createInstance(const std::string &type)
      {
        return createInstanceHelper<Volume<W>, VKL_VOLUME>(type);
      }

      virtual void commit() override
      {
        ManagedObject::commit();
      }

      // volumes must provide their own ray iterator implementations based on
      // their internal acceleration structures.

      // initialize a new rayIterator for the given input rays (specified by
      // origin, direction and tRange) and optional samplesMask indicating
      // volume sample values of interest. if no samplesMask is provided, all
      // intervals intersecting the volume should be (iteratively) returned by
      // iterateIntervalV(), and no surfaces should be returned by
      // iterateSurfaceV().
      //
      // the rayIterator object can be casted to volume-specific iterator types,
      // and may maintain internal state as desired, e.g. for current state
      // within an acceleration structure, etc.
      virtual void initRayIteratorV(vVKLRayIteratorN<W> &rayIterator,
                                    const vvec3fn<W> &origin,
                                    const vvec3fn<W> &direction,
                                    const vrange1fn<W> &tRange,
                                    const SamplesMask *samplesMask)
      {
        throw std::runtime_error(
            "newRayIteratorV() not implemented in this volume!");
      }

      // for each active lane / ray (indicated by valid), iterate once for the
      // given rayIterator and return the next interval (if any) satisfying the
      // ray iterator's samplesMask in interval. result (0 or 1) should
      // indicate if a new interval was found for each active lane.
      //
      // rayIterator may be modified to track any internal state as desired.
      virtual void iterateIntervalV(const int *valid,
                                    vVKLRayIteratorN<W> &rayIterator,
                                    vVKLIntervalN<W> &interval,
                                    vintn<W> &result)
      {
        throw std::runtime_error(
            "iterateIntervalV() not implemented in this volume!");
      }

      // for each active lane / ray (indicated by valid), iterate once for the
      // given rayIterator and return the next surface hit (if any) satisfying
      // the ray iterator's samplesMask in hit. result (0 or 1) should
      // indicate if a new surface hit was found for each active lane.
      //
      // rayIterator may be modified to track any internal state as desired.
      virtual void iterateSurfaceV(const int *valid,
                                   vVKLRayIteratorN<W> &rayIterator,
                                   vVKLHitN<W> &hit,
                                   vintn<W> &result)
      {
        throw std::runtime_error(
            "iterateSurfaceV() not implemented in this volume!");
      }

      virtual SamplesMask *newSamplesMask()
      {
        throw std::runtime_error(
            "newSamplesMask() not implemented in this volume!");
      }

      virtual void computeSampleV(const int *valid,
                                  const vvec3fn<W> &objectCoordinates,
                                  vfloatn<W> &samples) const = 0;

      virtual vec3f computeGradient(const vec3f &objectCoordinates) const = 0;
      virtual box3f getBoundingBox() const                                = 0;
    };

#define VKL_REGISTER_VOLUME(InternalClass, external_name) \
  VKL_REGISTER_OBJECT(                                    \
      ::openvkl::ManagedObject, volume, InternalClass, external_name)

  }  // namespace ispc_driver
}  // namespace openvkl
