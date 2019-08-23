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

#ifdef __cplusplus
#include <cstdint>
#include <cstdlib>
#else
#include <stdint.h>
#include <stdlib.h>
#endif

#include "common.h"

#ifdef __cplusplus
struct Volume : public ManagedObject
{
};
#else
typedef ManagedObject Volume;
#endif

typedef Volume *VKLVolume;

#ifdef __cplusplus
extern "C" {
#endif

VKLVolume vklNewVolume(const char *type);

float vklComputeSample(VKLVolume volume, const vkl_vec3f *objectCoordinates);

void vklComputeSample4(const int *valid,
                       VKLVolume volume,
                       const vkl_vvec3f4 *objectCoordinates,
                       float *samples);

void vklComputeSample8(const int *valid,
                       VKLVolume volume,
                       const vkl_vvec3f8 *objectCoordinates,
                       float *samples);

void vklComputeSample16(const int *valid,
                        VKLVolume volume,
                        const vkl_vvec3f16 *objectCoordinates,
                        float *samples);

vkl_vec3f vklComputeGradient(VKLVolume volume,
                             const vkl_vec3f *objectCoordinates);

vkl_box3f vklGetBoundingBox(VKLVolume volume);

#ifdef __cplusplus
}  // extern "C"
#endif
