/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/cme/stop_cme/p9_cme_copy_scan_ring.c $ */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2015,2017                                                    */
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
#include "p9_cme_copy_scan_ring.h"
#include "p9_cme_stop.h"
#include "cmehw_common.h"
#include "gpehw_common.h"
#include "p9_cme_img_layout.h"
#include "p9_hcode_image_defines.H"

/// @brief local constants.
enum
{
    SPR_NUM_PIR             =   286,
    CME_IMAGE_BASE_ADDR     =   SRAM_START,
    CME_INST_ID_MASK        =   0x0000001E,
    COPY_DEF_CME_ADDR       =   0x00000000,
    CME_PAGE_RD_SIZE        =   0x20,
    CME_IMG_HDR_ADDR        =   CME_IMAGE_BASE_ADDR + CME_HEADER_OFFSET,
    CME_FLAG_EX_ID_BIT      =   26,
    EX_ID_SHIFT_POS         =   5,
};

void instance_scan_init( )
{
    uint32_t l_cmePir = 0;

    uint32_t l_bcLength = 0;
    uint32_t l_bceMbase = CME_INST_SPEC_RING_START;
    uint32_t l_exId = ((in32(CME_LCL_FLAGS) & BITS32(CME_FLAG_EX_ID_BIT, 1)) >> EX_ID_SHIFT_POS);
    asm volatile ( "mfspr %0, %1 \n\t" : "=r" (l_cmePir) : "i" (SPR_NUM_PIR));

    //CME's PIR gives only quad id. To determine the correct CME instance, follow the steps below:
    //(1). Read CME PIR's CME instance bit field (bit 27 -bit 31)
    //(2). Bitwise shift left by one bit position.
    //(3). OR to LSB of CME PIR (bit 31), bit 26 of CME Flag Register

    l_cmePir = ((( l_cmePir << 1 ) & CME_INST_ID_MASK) | l_exId);     // get CME instance number

    cmeHeader_t* pCmeImgHdr = (cmeHeader_t*)(CME_IMG_HDR_ADDR);

    //calculate start address of block copy and length of block copy
    l_bcLength = pCmeImgHdr->g_cme_max_spec_ring_length; // integral multiple of 32.
    //let us find out HOMER address where core specific scan rings reside.
    l_bceMbase = l_bceMbase + ( l_cmePir * l_bcLength );
    l_bceMbase = (l_bceMbase >> 5 );

    // calculate the CME SRAM destination block number(SBASE)
    // Offset below is wrt start of CME's SRAM. It is an integral
    // multiple of 32 and is populated by Hcode Image build while
    // building HOMER.
    uint32_t cmeSbase = pCmeImgHdr->g_cme_core_spec_ring_offset;
    PK_TRACE( "Start second block copy MBASE 0x%08x SBSE 0x%08x Len 0x%08x  CME Ist %d",
              l_bceMbase, cmeSbase, l_bcLength, l_cmePir );

    startCmeBlockCopy( cmeSbase, l_bcLength, l_cmePir, PLAT_CME, BAR_INDEX_1, l_bceMbase );

    PK_TRACE(" Done startCmeBlockCopy(instance_scan_init).");
}


BceReturnCode_t isScanRingCopyDone( )
{
    BceReturnCode_t l_rc;
    uint32_t l_cmePir = 0;
    uint32_t l_exId = ((in32(CME_LCL_FLAGS) & BITS32(CME_FLAG_EX_ID_BIT, 1)) >> EX_ID_SHIFT_POS);
    asm volatile ( "mfspr %0, %1 \n\t" : "=r" (l_cmePir) : "i" (SPR_NUM_PIR));

    //CME's PIR gives only quad id. To determine the correct CME instance, follow the steps below:
    //(1). Read CME PIR's CME instance bit field (bit 27 -bit 31)
    //(2). Bitwise shift left by one bit position.
    //(3). OR to LSB of CME PIR (bit 31), bit 26 of CME Flag Register

    l_cmePir = ((( l_cmePir << 1 ) & CME_INST_ID_MASK) | l_exId);     // get CME instance number

    while(1)
    {
        l_rc = checkCmeBlockCopyStatus( l_cmePir, PLAT_CME );

        if( BLOCK_COPY_SUCCESS == l_rc )
        {
            break;
        }

        if( BLOCK_COPY_FAILED == l_rc )
        {
            PK_TRACE( "failed to copy instance specific scan ring on cme %d",
                      l_cmePir );
            break;
        }
    }

    return l_rc;
}
