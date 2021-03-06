/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe/hwpf/include/plat/plat_target_utils.H $ */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2012,2017                                                    */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/**
 * @file plat_target_util.H
 * @brief platform utility definitions for fapi2 targets
 */

#ifndef __FAPI2_PLAT_TARGET_UTIL__
#define __FAPI2_PLAT_TARGET_UTIL__

//
// Platform Utility functions..
//
namespace fapi2
{

/// @brief Function to initialize the G_targets vector based on partial good
///      attributes
ReturnCode plat_TargetsInit();

/// @brief Function to apply any gard records set (via
//  ATTR_EQ_GARD/ATTR_EC_GARD) to mark corresponding targets non functional
ReturnCode plat_ApplyGards();

/// @brief Function to initialize the G_targets vector based on partial good
///      attributes
Target<TARGET_TYPE_PROC_CHIP> plat_getChipTarget();

/// @brief Function to return a platform target handle, given the chiplet
//         number
//  @param i_chipletNumber The chiplet number of the target
//  @return Platform handle
//  @note The caller can use the platform handle to construct a Target of
//  it's choice. Ex:
//  fapi2::Target<fapi2::TARGET_TYPE_CORE>
//  l_core(plat_getTargetHandleByChipletNumber(0x20);
plat_target_handle_t plat_getTargetHandleByChipletNumber(
    const uint8_t i_chipletNumber);
}
#endif
