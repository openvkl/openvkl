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

#include "ISPCDriver.h"
#include "../samples_mask/SamplesMask.h"
#include "../volume/Volume.h"
#include "common/Data.h"

namespace volley {
  namespace ispc_driver {

    template <int W>
    bool ISPCDriver<W>::supportsWidth(int width)
    {
      return W >= width;
    }

    template <int W>
    void ISPCDriver<W>::commit()
    {
      Driver::commit();
    }

    template <int W>
    void ISPCDriver<W>::commit(VLYObject object)
    {
      ManagedObject *managedObject = (ManagedObject *)object;
      managedObject->commit();
    }

    template <int W>
    void ISPCDriver<W>::release(VLYObject object)
    {
      ManagedObject *managedObject = (ManagedObject *)object;
      managedObject->refDec();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Data ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    template <int W>
    VLYData ISPCDriver<W>::newData(size_t numItems,
                                   VLYDataType dataType,
                                   const void *source,
                                   VLYDataCreationFlags dataCreationFlags)
    {
      Data *data = new Data(numItems, dataType, source, dataCreationFlags);
      return (VLYData)data;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Iterator ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    template <int W>
    VLYRayIterator ISPCDriver<W>::newRayIterator(VLYVolume volume,
                                                 const vec3f &origin,
                                                 const vec3f &direction,
                                                 const range1f &tRange,
                                                 VLYSamplesMask samplesMask)
    {
      auto &volumeObject = referenceFromHandle<Volume>(volume);
      return (VLYRayIterator)volumeObject.newRayIterator(
          origin,
          direction,
          tRange,
          reinterpret_cast<const SamplesMask *>(samplesMask));
    }

    template <int W>
    VLYRayIterator ISPCDriver<W>::newRayIterator8(const int *valid,
                                                  VLYVolume volume,
                                                  const vvec3fn<8> &origin,
                                                  const vvec3fn<8> &direction,
                                                  const vrange1fn<8> &tRange,
                                                  VLYSamplesMask samplesMask)
    {
      auto &volumeObject = referenceFromHandle<Volume>(volume);
      return (VLYRayIterator)volumeObject.newRayIterator8(
          origin,
          direction,
          tRange,
          reinterpret_cast<const SamplesMask *>(samplesMask));
    }

    template <int W>
    bool ISPCDriver<W>::iterateInterval(VLYRayIterator rayIterator,
                                        VLYRayInterval &rayInterval)
    {
      throw std::runtime_error(
          "iterateInterval() not implemented on this driver!");
    }

    template <int W>
    void ISPCDriver<W>::iterateInterval8(const int *valid,
                                         VLYRayIterator rayIterator,
                                         VLYRayInterval8 &rayInterval,
                                         vintn<8> &result)
    {
      auto &rayIteratorObject =
          referenceFromHandle<RayIterator<8>>(rayIterator);
      rayIteratorObject.iterateInterval(valid, result);
      rayInterval = *reinterpret_cast<const VLYRayInterval8 *>(
          rayIteratorObject.getCurrentRayInterval());
    }

    template <int W>
    void ISPCDriver<W>::iterateSurface8(const int *valid,
                                        VLYRayIterator rayIterator,
                                        VLYSurfaceHit8 &surfaceHit,
                                        vintn<8> &result)
    {
      auto &rayIteratorObject =
          referenceFromHandle<RayIterator<8>>(rayIterator);
      rayIteratorObject.iterateSurface(valid, result);
      surfaceHit = *reinterpret_cast<const VLYSurfaceHit8 *>(
          rayIteratorObject.getCurrentSurfaceHit());
    }

    ///////////////////////////////////////////////////////////////////////////
    // Module /////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    template <int W>
    VLYError ISPCDriver<W>::loadModule(const char *moduleName)
    {
      return volley::loadLocalModule(moduleName);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Parameters /////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    template <int W>
    void ISPCDriver<W>::set1f(VLYObject object, const char *name, const float x)
    {
      ManagedObject *managedObject = (ManagedObject *)object;
      managedObject->setParam(name, x);
    }

    template <int W>
    void ISPCDriver<W>::set1i(VLYObject object, const char *name, const int x)
    {
      ManagedObject *managedObject = (ManagedObject *)object;
      managedObject->setParam(name, x);
    }

    template <int W>
    void ISPCDriver<W>::setVec3f(VLYObject object,
                                 const char *name,
                                 const vec3f &v)
    {
      ManagedObject *managedObject = (ManagedObject *)object;
      managedObject->setParam(name, v);
    }

    template <int W>
    void ISPCDriver<W>::setVec3i(VLYObject object,
                                 const char *name,
                                 const vec3i &v)
    {
      ManagedObject *managedObject = (ManagedObject *)object;
      managedObject->setParam(name, v);
    }

    template <int W>
    void ISPCDriver<W>::setObject(VLYObject object,
                                  const char *name,
                                  VLYObject setObject)
    {
      ManagedObject *target = (ManagedObject *)object;
      ManagedObject *value  = (ManagedObject *)setObject;
      target->setParam(name, value);
    }

    template <int W>
    void ISPCDriver<W>::setString(VLYObject object,
                                  const char *name,
                                  const std::string &s)
    {
      ManagedObject *managedObject = (ManagedObject *)object;
      managedObject->setParam(name, s);
    }

    template <int W>
    void ISPCDriver<W>::setVoidPtr(VLYObject object, const char *name, void *v)
    {
      ManagedObject *managedObject = (ManagedObject *)object;
      managedObject->setParam(name, v);
    }

    /////////////////////////////////////////////////////////////////////////
    // Samples mask /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////

    template <int W>
    VLYSamplesMask ISPCDriver<W>::newSamplesMask(VLYVolume volume)
    {
      auto &volumeObject = referenceFromHandle<Volume>(volume);
      return (VLYSamplesMask)volumeObject.newSamplesMask();
    }

    template <int W>
    void ISPCDriver<W>::samplesMaskSetRanges(
        VLYSamplesMask samplesMask,
        const utility::ArrayView<const range1f> &ranges)
    {
      auto &samplesMaskObject = referenceFromHandle<SamplesMask>(samplesMask);
      samplesMaskObject.setRanges(ranges);
    }

    template <int W>
    void ISPCDriver<W>::samplesMaskSetValues(
        VLYSamplesMask samplesMask,
        const utility::ArrayView<const float> &values)
    {
      auto &samplesMaskObject = referenceFromHandle<SamplesMask>(samplesMask);
      samplesMaskObject.setValues(values);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Volume /////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    template <int W>
    VLYVolume ISPCDriver<W>::newVolume(const char *type)
    {
      return (VLYVolume)Volume::createInstance(type);
    }

    template <int W>
    float ISPCDriver<W>::computeSample(VLYVolume volume,
                                       const vec3f &objectCoordinates)
    {
      auto &volumeObject = referenceFromHandle<Volume>(volume);
      return volumeObject.computeSample(objectCoordinates);
    }

#define __define_computeSampleN(WIDTH)                                        \
  template <int W>                                                            \
  void ISPCDriver<W>::computeSample##WIDTH(                                   \
      const int *valid,                                                       \
      VLYVolume volume,                                                       \
      const vvec3fn<WIDTH> &objectCoordinates,                                \
      vfloatn<WIDTH> &samples)                                                \
  {                                                                           \
    auto &volumeObject = referenceFromHandle<Volume>(volume);                 \
    if (WIDTH != W) {                                                         \
      switch (W) {                                                            \
      case 4:                                                                 \
        break;                                                                \
      case 8: {                                                               \
        vvec3fn<8> oc8 = static_cast<vvec3fn<8>>(objectCoordinates);          \
        vintn<8> valid8;                                                      \
        for (int i = 0; i < 8; i++)                                           \
          valid8[i] = i < WIDTH ? valid[i] : 0;                               \
        vfloatn<8> samples8;                                                  \
        volumeObject.computeSample8((const int *)&valid8, oc8, samples8);     \
        for (int i = 0; i < WIDTH; i++)                                       \
          samples[i] = samples8[i];                                           \
        break;                                                                \
      }                                                                       \
      case 16: {                                                              \
        vvec3fn<16> oc16 = static_cast<vvec3fn<16>>(objectCoordinates);       \
        vintn<16> valid16;                                                    \
        for (int i = 0; i < 16; i++)                                          \
          valid16[i] = i < WIDTH ? valid[i] : 0;                              \
        vfloatn<16> samples16;                                                \
        volumeObject.computeSample16((const int *)&valid16, oc16, samples16); \
        for (int i = 0; i < WIDTH; i++)                                       \
          samples[i] = samples16[i];                                          \
        break;                                                                \
      }                                                                       \
      }                                                                       \
    } else {                                                                  \
      volumeObject.computeSample##WIDTH(valid, objectCoordinates, samples);   \
    }                                                                         \
  }

    __define_computeSampleN(4);
    __define_computeSampleN(8);
    __define_computeSampleN(16);

#undef __define_computeSampleN

    template <int W>
    vec3f ISPCDriver<W>::computeGradient(VLYVolume volume,
                                         const vec3f &objectCoordinates)
    {
      auto &volumeObject = referenceFromHandle<Volume>(volume);
      return volumeObject.computeGradient(objectCoordinates);
    }

    template <int W>
    box3f ISPCDriver<W>::getBoundingBox(VLYVolume volume)
    {
      auto &volumeObject = referenceFromHandle<Volume>(volume);
      return volumeObject.getBoundingBox();
    }

    VLY_REGISTER_DRIVER(ISPCDriver<4>, ispc_driver_4)
    VLY_REGISTER_DRIVER(ISPCDriver<8>, ispc_driver_8)
    VLY_REGISTER_DRIVER(ISPCDriver<16>, ispc_driver_16)

    template class ISPCDriver<4>;
    template class ISPCDriver<8>;
    template class ISPCDriver<16>;

  }  // namespace ispc_driver
}  // namespace volley

extern "C" VOLLEY_DLLEXPORT void volley_init_module_ispc_driver() {}
