// Copyright 2019-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "AMRVolume.ih"
#include "CellRef.ih"
#include "DualCell.ih"
#include "common/export_util.h"
#include "sampler/Sampler.ih"

varying float AMR_finest(const Sampler *uniform self,
                         const varying vec3f &P,
                         const varying float &_time)
{
  const AMRVolume *uniform volume = (const AMRVolume *uniform)self->volume;
  const AMR *uniform amr          = &volume->amr;

  vec3f lP;  // local amr space
  AMRVolume_transformObjectToLocal(volume, P, lP);

  DualCell D;
  initDualCell(D, lP, *amr->finestLevel);
  findDualCell(amr, D);
  return lerp(D);
}

export void EXPORT_UNIQUE(AMR_install_finest, void *uniform _sampler)
{
  Sampler *uniform sampler       = (Sampler * uniform) _sampler;
  sampler->computeSample_varying = AMR_finest;
}
