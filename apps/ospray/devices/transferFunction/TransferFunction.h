// ======================================================================== //
// Copyright 2009-2018 Intel Corporation                                    //
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

// scalar_volley_device
#include "../common/ManagedObject.h"
// ospcommon
#include <ospray/ospcommon/range.h>

namespace ospray {
  namespace scalar_volley_device {

    struct TransferFunction : public ManagedObject
    {
      TransferFunction() = default;

      void commit();
      vec4f getColor(float value) const;

     private:
      std::vector<vec4f> colors;

      range1f valueRange;
    };

  }  // namespace scalar_volley_device
}  // namespace ospray
