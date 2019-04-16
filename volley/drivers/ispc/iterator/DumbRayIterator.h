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

#include "RayIterator.h"

namespace volley {
  namespace ispc_driver {

    struct Volume;

    template <int W>
    struct DumbRayIterator : public RayIterator<W>
    {
      DumbRayIterator(const Volume *volume,
                      const vvec3fn<W> &origin,
                      const vvec3fn<W> &direction,
                      const vrange1fn<W> &tRange,
                      const SamplesMask *samplesMask);

      ~DumbRayIterator();

      const RayInterval<W> *getCurrentRayInterval() const override;
      void iterateInterval(const int *valid, vintn<W> &result) override;

      const SurfaceHit<W> *getCurrentSurfaceHit() const override
      {
        throw std::runtime_error(
            "surface iteration not implemented for this iterator type");
      }

      void iterateSurface(const int *valid, vintn<W> &result) override
      {
        throw std::runtime_error(
            "surface iteration not implemented for this iterator type");
      }

     protected:
      void *ispcEquivalent{nullptr};
    };

  }  // namespace ispc_driver
}  // namespace volley
