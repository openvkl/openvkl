// Copyright 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <functional>
#include "openvkl/openvkl.h"

namespace openvkl {

  struct Data;
  struct ManagedObject;

  // Public wide types for given widths
  template <int W>
  struct vklPublicWideTypes
  {
    using vkl_vvec3fW   = void;
    using vkl_vrange1fW = void;
  };

  template <>
  struct vklPublicWideTypes<1>
  {
    using vkl_vvec3fW   = vkl_vec3f;
    using vkl_vrange1fW = vkl_range1f;
  };

  template <>
  struct vklPublicWideTypes<4>
  {
    using vkl_vvec3fW   = vkl_vvec3f4;
    using vkl_vrange1fW = vkl_vrange1f4;
  };

  template <>
  struct vklPublicWideTypes<8>
  {
    using vkl_vvec3fW   = vkl_vvec3f8;
    using vkl_vrange1fW = vkl_vrange1f8;
  };

  template <>
  struct vklPublicWideTypes<16>
  {
    using vkl_vvec3fW   = vkl_vvec3f16;
    using vkl_vrange1fW = vkl_vrange1f16;
  };

  // Infer (compile time) VKL_DATA_TYPE from input type
  template <typename T>
  struct VKLTypeFor
  {
    static constexpr VKLDataType value = VKL_UNKNOWN;
  };

#define VKLTYPEFOR_SPECIALIZATION(type, vkl_type)  \
  template <>                                      \
  struct VKLTypeFor<type>                          \
  {                                                \
    static constexpr VKLDataType value = vkl_type; \
  };

  VKLTYPEFOR_SPECIALIZATION(VKLDevice, VKL_DEVICE);
  VKLTYPEFOR_SPECIALIZATION(openvkl::ManagedObject *, VKL_OBJECT);
  VKLTYPEFOR_SPECIALIZATION(openvkl::Data *, VKL_DATA);
  VKLTYPEFOR_SPECIALIZATION(void *, VKL_VOID_PTR);
  VKLTYPEFOR_SPECIALIZATION(bool, VKL_BOOL);
  VKLTYPEFOR_SPECIALIZATION(char *, VKL_STRING);
  VKLTYPEFOR_SPECIALIZATION(const char *, VKL_STRING);
  VKLTYPEFOR_SPECIALIZATION(const char[], VKL_STRING);
  VKLTYPEFOR_SPECIALIZATION(char, VKL_CHAR);
  VKLTYPEFOR_SPECIALIZATION(unsigned char, VKL_UCHAR);
  VKLTYPEFOR_SPECIALIZATION(vec2uc, VKL_VEC2UC);
  VKLTYPEFOR_SPECIALIZATION(vec3uc, VKL_VEC3UC);
  VKLTYPEFOR_SPECIALIZATION(vec4uc, VKL_VEC4UC);
  VKLTYPEFOR_SPECIALIZATION(short, VKL_SHORT);
  VKLTYPEFOR_SPECIALIZATION(unsigned short, VKL_USHORT);
  VKLTYPEFOR_SPECIALIZATION(int32_t, VKL_INT);
  VKLTYPEFOR_SPECIALIZATION(vec2i, VKL_VEC2I);
  VKLTYPEFOR_SPECIALIZATION(vec3i, VKL_VEC3I);
  VKLTYPEFOR_SPECIALIZATION(vec4i, VKL_VEC4I);
  VKLTYPEFOR_SPECIALIZATION(uint32_t, VKL_UINT);
  VKLTYPEFOR_SPECIALIZATION(vec2ui, VKL_VEC2UI);
  VKLTYPEFOR_SPECIALIZATION(vec3ui, VKL_VEC3UI);
  VKLTYPEFOR_SPECIALIZATION(vec4ui, VKL_VEC4UI);
  VKLTYPEFOR_SPECIALIZATION(int64_t, VKL_LONG);
  VKLTYPEFOR_SPECIALIZATION(vec2l, VKL_VEC2L);
  VKLTYPEFOR_SPECIALIZATION(vec3l, VKL_VEC3L);
  VKLTYPEFOR_SPECIALIZATION(vec4l, VKL_VEC4L);
  VKLTYPEFOR_SPECIALIZATION(uint64_t, VKL_ULONG);
  VKLTYPEFOR_SPECIALIZATION(vec2ul, VKL_VEC2UL);
  VKLTYPEFOR_SPECIALIZATION(vec3ul, VKL_VEC3UL);
  VKLTYPEFOR_SPECIALIZATION(vec4ul, VKL_VEC4UL);
  VKLTYPEFOR_SPECIALIZATION(float, VKL_FLOAT);
  VKLTYPEFOR_SPECIALIZATION(vec2f, VKL_VEC2F);
  VKLTYPEFOR_SPECIALIZATION(vec3f, VKL_VEC3F);
  VKLTYPEFOR_SPECIALIZATION(vec4f, VKL_VEC4F);
  VKLTYPEFOR_SPECIALIZATION(double, VKL_DOUBLE);
  VKLTYPEFOR_SPECIALIZATION(box1i, VKL_BOX1I);
  VKLTYPEFOR_SPECIALIZATION(box2i, VKL_BOX2I);
  VKLTYPEFOR_SPECIALIZATION(box3i, VKL_BOX3I);
  VKLTYPEFOR_SPECIALIZATION(box4i, VKL_BOX4I);
  VKLTYPEFOR_SPECIALIZATION(box1f, VKL_BOX1F);
  VKLTYPEFOR_SPECIALIZATION(box2f, VKL_BOX2F);
  VKLTYPEFOR_SPECIALIZATION(box3f, VKL_BOX3F);
  VKLTYPEFOR_SPECIALIZATION(box4f, VKL_BOX4F);
  VKLTYPEFOR_SPECIALIZATION(linear2f, VKL_LINEAR2F);
  VKLTYPEFOR_SPECIALIZATION(linear3f, VKL_LINEAR3F);
  VKLTYPEFOR_SPECIALIZATION(affine2f, VKL_AFFINE2F);
  VKLTYPEFOR_SPECIALIZATION(affine3f, VKL_AFFINE3F);

#define VKLTYPEFOR_DEFINITION(type) \
  constexpr VKLDataType VKLTypeFor<type>::value

}  // namespace openvkl
