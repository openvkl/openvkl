// Copyright 2019 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef OPENVKL_TESTING_CPU
#define HALF_FLOAT_SUPPORT 1
#define DOUBLE_SUPPORT 1
#endif

#ifdef OPENVKL_TESTING_GPU
#define HALF_FLOAT_SUPPORT 0
#define DOUBLE_SUPPORT 0
#endif

#include "apps/AppInit.h"
#include "volume/OpenVdbVolume.h"
#include "volume/ProceduralParticleVolume.h"
#include "volume/ProceduralShellsAMRVolume.h"
#include "volume/ProceduralStructuredRegularVolume.h"
#include "volume/ProceduralStructuredSphericalVolume.h"
#include "volume/ProceduralStructuredVolume.h"
#include "volume/ProceduralUnstructuredVolume.h"
#include "volume/ProceduralVdbVolume.h"
#include "volume/ProceduralVdbVolumeMulti.h"
#include "volume/RawFileStructuredVolume.h"
#include "volume/RawHFileStructuredVolume.h"
#include "volume/TestingStructuredVolumeMulti.h"
#include "volume/TestingUnstructuredMixedSimple.h"
#include "volume/TestingVdbTorusVolume.h"
