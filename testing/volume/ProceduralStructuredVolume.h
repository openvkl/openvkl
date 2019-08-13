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

#include <vector>
#include "TestingStructuredVolume.h"
#include "openvkl/openvkl.h"
#include "ospcommon/math/vec.h"

using namespace ospcommon;

namespace openvkl {
  namespace testing {

    template <typename VOXEL_TYPE,
              VOXEL_TYPE volumeSamplingFunction(const vec3f &)>
    struct ProceduralStructuredVolume : public TestingStructuredVolume
    {
      ProceduralStructuredVolume(const vec3i &dimensions,
                                 const vec3f &gridOrigin,
                                 const vec3f &gridSpacing)
          : TestingStructuredVolume("structured_regular",
                                    dimensions,
                                    gridOrigin,
                                    gridSpacing,
                                    getVKLDataType<VOXEL_TYPE>())
      {
      }

      inline VOXEL_TYPE computeProceduralValue(const vec3f &objectCoordinates)
      {
        return volumeSamplingFunction(objectCoordinates);
      }

      std::vector<unsigned char> generateVoxels() override
      {
        {
          std::vector<unsigned char> voxels(longProduct(this->dimensions) *
                                            sizeof(VOXEL_TYPE));

          VOXEL_TYPE *voxelsTyped = (VOXEL_TYPE *)voxels.data();

          auto transformLocalToObject = [&](const vec3f &localCoordinates) {
            return this->gridOrigin + localCoordinates * this->gridSpacing;
          };

          for (size_t z = 0; z < this->dimensions.z; z++) {
            for (size_t y = 0; y < this->dimensions.y; y++) {
              for (size_t x = 0; x < this->dimensions.x; x++) {
                size_t index = z * this->dimensions.y * this->dimensions.x +
                               y * this->dimensions.x + x;
                vec3f objectCoordinates =
                    transformLocalToObject(vec3f(x, y, z));
                voxelsTyped[index] = volumeSamplingFunction(objectCoordinates);
              }
            }
          }

          return voxels;
        }
      }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Procedural volume types ////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    template <typename VOXEL_TYPE>
    inline VOXEL_TYPE getWaveletValue(const vec3f &objectCoordinates)
    {
      // wavelet parameters
      constexpr double M  = 1.f;
      constexpr double G  = 1.f;
      constexpr double XM = 1.f;
      constexpr double YM = 1.f;
      constexpr double ZM = 1.f;
      constexpr double XF = 3.f;
      constexpr double YF = 3.f;
      constexpr double ZF = 3.f;

      double value = M * G *
                     (XM * ::sin(XF * objectCoordinates.x) +
                      YM * ::sin(YF * objectCoordinates.y) +
                      ZM * ::cos(ZF * objectCoordinates.z));

      if (std::is_unsigned<VOXEL_TYPE>::value) {
        value = fabs(value);
      }

      value = clamp(value,
                    double(std::numeric_limits<VOXEL_TYPE>::min()),
                    double(std::numeric_limits<VOXEL_TYPE>::max()));

      return VOXEL_TYPE(value);
    }

    inline float getZValue(const vec3f &objectCoordinates)
    {
      return objectCoordinates.z;
    }

    using WaveletProceduralVolume =
        ProceduralStructuredVolume<float, getWaveletValue<float>>;

    using WaveletProceduralVolumeUchar =
        ProceduralStructuredVolume<unsigned char,
                                   getWaveletValue<unsigned char>>;

    using WaveletProceduralVolumeShort =
        ProceduralStructuredVolume<short, getWaveletValue<short>>;

    using WaveletProceduralVolumeUshort =
        ProceduralStructuredVolume<unsigned short,
                                   getWaveletValue<unsigned short>>;

    using WaveletProceduralVolumeFloat =
        ProceduralStructuredVolume<float, getWaveletValue<float>>;

    using WaveletProceduralVolumeDouble =
        ProceduralStructuredVolume<double, getWaveletValue<double>>;

    using ZProceduralVolume = ProceduralStructuredVolume<float, getZValue>;

  }  // namespace testing
}  // namespace openvkl
