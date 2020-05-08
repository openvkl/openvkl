// Copyright 2019-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VKLCommon.h"
#include "objectFactory.h"
#include "ospcommon/memory/RefCount.h"
#include "ospcommon/utility/ParameterizedObject.h"

using namespace ospcommon::memory;

namespace openvkl {

  struct Data;

  struct OPENVKL_CORE_INTERFACE ManagedObject
      : public ospcommon::memory::RefCount,
        public ospcommon::utility::ParameterizedObject
  {
    using VKL_PTR = ManagedObject *;

    ManagedObject() = default;

    virtual ~ManagedObject() override;

    template <typename T>
    T getParam(const char *name, T valIfNotFound = T());

    // commit the object's outstanding changes (such as changed parameters)
    virtual void commit() {}

    // common function to help printf-debugging; every derived class should
    // overrride this!
    virtual std::string toString() const;

    // subtype of this ManagedObject
    VKLDataType managedObjectType{VKL_UNKNOWN};
  };

  template <typename OPENVKL_CLASS, VKLDataType VKL_TYPE>
  inline OPENVKL_CLASS *createInstanceHelper(const std::string &type)
  {
    static_assert(std::is_base_of<ManagedObject, OPENVKL_CLASS>::value,
                  "createInstanceHelper<>() is only for VKL classes, not"
                  " generic types!");

    auto *object = objectFactory<OPENVKL_CLASS, VKL_TYPE>(type);

    // denote the subclass type in the ManagedObject base class.
    if (object) {
      object->managedObjectType = VKL_TYPE;
    }

    return object;
  }

  template <typename OPENVKL_CLASS, typename OPENVKL_HANDLE>
  OPENVKL_CLASS &referenceFromHandle(OPENVKL_HANDLE handle)
  {
    return *((OPENVKL_CLASS *)handle);
  }

  // Inlined definitions //////////////////////////////////////////////////////

  template <typename T>
  inline T ManagedObject::getParam(const char *name, T valIfNotFound)
  {
    return ParameterizedObject::getParam<T>(name, valIfNotFound);
  }

  template <>
  inline Data *ManagedObject::getParam<Data *>(const char *name,
                                               Data *valIfNotFound)
  {
    auto *obj = ParameterizedObject::getParam<ManagedObject *>(
        name, (ManagedObject *)valIfNotFound);
    if (obj && obj->managedObjectType == VKL_DATA)
      return (Data *)obj;
    else
      return valIfNotFound;
  }

}  // namespace openvkl

// Specializations for ISPCDriver /////////////////////////////////////////////

namespace ospcommon {
  namespace utility {

    template <>
    inline void ParameterizedObject::Param::set(
        const openvkl::ManagedObject::VKL_PTR &object)
    {
      using VKL_PTR = openvkl::ManagedObject::VKL_PTR;

      if (object)
        object->refInc();

      if (data.is<VKL_PTR>()) {
        auto *existingObj = data.get<VKL_PTR>();
        if (existingObj != nullptr)
          existingObj->refDec();
      }

      data = object;
    }

  }  // namespace utility
}  // namespace ospcommon
