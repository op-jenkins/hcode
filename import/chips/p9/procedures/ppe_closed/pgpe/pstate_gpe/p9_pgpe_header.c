/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/pgpe/pstate_gpe/p9_pgpe_header.c $ */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2016,2017                                                    */
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
#include "p9_pgpe_header.h"
#include "pstate_pgpe_occ_api.h"

//OCC Shared SRAM starts at bottom 2K of PGPE OCC SRAM space
#define OCC_PGPE_SRAM_ADDR_START (0xfff20000)
#define OCC_SHARED_SRAM_ADDR_START (OCC_PGPE_SRAM_ADDR_START + 0x10000 - 0x800)

PgpeHeader_t* G_pgpe_header_data;
extern PgpeHeader_t* _PGPE_IMG_HEADER __attribute__ ((section (".pgpe_image_header")));

//
//Set the pgpe_header_data struct to point to PGPE HEADER in SRAM
//
//
void p9_pgpe_header_init()
{
    G_pgpe_header_data = (PgpeHeader_t*)&_PGPE_IMG_HEADER;
}

void p9_pgpe_header_fill()
{
    PK_TRACE_DBG("> p9_pgpe_header_fill");
    HcodeOCCSharedData_t* occ_shared_data = (HcodeOCCSharedData_t*)
                                            OCC_SHARED_SRAM_ADDR_START; //Bottom 2K of PGPE OCC Sram Space
    G_pgpe_header_data->g_pgpe_occ_pstables_sram_addr = (uint32_t*)&occ_shared_data->pstate_table;//OCC Pstate table address
    G_pgpe_header_data->g_pgpe_occ_pstables_len  = sizeof(OCCPstateTable_t);//OCC Pstate table length
    G_pgpe_header_data->g_pgpe_beacon_addr = (uint32_t*)&occ_shared_data->pgpe_beacon;
    G_pgpe_header_data->g_quad_status_addr = (uint32_t*)&occ_shared_data->quad_pstate_0;

    // @todo: change to value from header
    G_pgpe_header_data->g_pgpe_gppb_sram_addr  = (uint32_t*)0xfff27000;//GPPB Sram Offset
    /*
     *
     *     //GPPB Sram Offset
     *     G_pgpe_header_data->g_pgpe_gppb_sram_addr = (uint32_t*)(OCC_PGPE_SRAM_ADDR_START +
     *             G_pgpe_header_data->g_pgpe_hcode_length)
     */


    G_pgpe_header_data->g_pgpe_wof_state_addr = (uint32_t*)&occ_shared_data->pgpe_wof_state;
    G_pgpe_header_data->g_req_active_quad_addr = (uint32_t*)&occ_shared_data->req_active_quads;

    PK_TRACE_DBG("< p9_pgpe_header_fill");
}
