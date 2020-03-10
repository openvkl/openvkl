// ======================================================================== //
// Copyright 2019-2020 Intel Corporation                                    //
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

#include "VdbIterator.h"
#include "../../common/export_util.h"
#include "../../iterator/Iterator.h"
#include "VdbIterator_ispc.h"
#include "VdbVolume.h"

namespace openvkl {
  namespace ispc_driver {

    template <int W>
    constexpr int VdbIterator<W>::ispcStorageSize;

    template <int W>
    VdbIterator<W>::VdbIterator(const vintn<W> &valid,
                                const VdbVolume<W> *volume,
                                const vvec3fn<W> &origin,
                                const vvec3fn<W> &direction,
                                const vrange1fn<W> &tRange,
                                const ValueSelector<W> *valueSelector)
        : Iterator<W>(valid, volume, origin, direction, tRange, valueSelector)
    {
      static bool oneTimeChecks = false;

      if (!oneTimeChecks) {
        int ispcSize = CALL_ISPC(VdbIterator_sizeOf);

        if (ispcSize > ispcStorageSize) {
          LogMessageStream(VKL_LOG_ERROR)
              << "VdbIterator required ISPC object size = " << ispcSize
              << ", allocated size = " << ispcStorageSize << std::endl;

          throw std::runtime_error("VdbIterator has insufficient ISPC storage");
        }

        oneTimeChecks = true;
      }

      CALL_ISPC(VdbIterator_Initialize,
                static_cast<const int *>(valid),
                &ispcStorage[0],
                volume->getGrid(),
                (void *)&origin,
                (void *)&direction,
                (void *)&tRange,
                valueSelector ? valueSelector->getISPCEquivalent() : nullptr);
    }

    template <int W>
    const Interval<W> *VdbIterator<W>::getCurrentInterval() const
    {
      return reinterpret_cast<const Interval<W> *>(
          CALL_ISPC(VdbIterator_getCurrentInterval, (void *)&ispcStorage[0]));
    }

    template <int W>
    void VdbIterator<W>::iterateInterval(const vintn<W> &valid,
                                         vintn<W> &result)
    {
      CALL_ISPC(VdbIterator_iterateInterval,
                static_cast<const int *>(valid),
                (void *)&ispcStorage[0],
                static_cast<int *>(result));
    }

    template <int W>
    const Hit<W> *VdbIterator<W>::getCurrentHit() const
    {
      return reinterpret_cast<const Hit<W> *>(
          CALL_ISPC(VdbIterator_getCurrentHit, (void *)&ispcStorage[0]));
    }

    template <int W>
    void VdbIterator<W>::iterateHit(const vintn<W> &valid, vintn<W> &result)
    {
      CALL_ISPC(VdbIterator_iterateHit,
                static_cast<const int *>(valid),
                (void *)&ispcStorage[0],
                static_cast<int *>(result));
    }

    template class VdbIterator<VKL_TARGET_WIDTH>;

  }  // namespace ispc_driver
}  // namespace openvkl