/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/cme/stop_cme/p9_cme_stop_irq_handlers.c $ */
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

#include "p9_cme_stop.h"
#include "p9_cme_stop_enter_marks.h"
#include "p9_cme_irq.h"

extern CmeStopRecord G_cme_stop_record;

// When take an Interrupt on falling edge of SPWU from a CPPM.
// 1) Read EINR to check if another one has been set
//    in the meantime from the same core.  If so abort.
// 2) Clear Special Wakeup Done to that CPPM.
// 3) Read GPMMR[1] to see if any Special Wakeup has been sent to active
//    in the meantime.  If so, set Special Wakeup Done again and abort.
// 4) Otherwise flip polarity of Special Wakeup in EISR and clear PM_EXIT
//    (note for abort cases do not Do not flip polarity of Special Wakeup in EISR.)
void
p9_cme_stop_spwu_handler(void* arg, PkIrqId irq)
{
    MARK_TRAP(STOP_SPWU_HANDLER)
    PK_TRACE_INF("SPWU Handler");
    PK_TRACE_DBG("SPWU Trigger %d", irq);

    PkMachineContext ctx;
    int      sem_post = 0;
    uint32_t raw_spwu = (in32(CME_LCL_EISR) & BITS32(14, 2)) >> SHIFT32(15);
    uint64_t scom_data = 0;

    if (raw_spwu & CME_MASK_C0)
    {
        // if failing edge == spwu drop:
        if (G_cme_stop_record.core_in_spwu & CME_MASK_C0)
        {
            PK_TRACE("Falling edge of spwu_c0, first clearing EISR");
            out32(CME_LCL_EISR_CLR, BIT32(14));

            // if spwu asserts again before we drop spwu_done, do nothing, else:
            if ((~(in32(CME_LCL_EINR))) & BIT32(14))
            {
                PK_TRACE("SPWU drop confirmed, now dropping spwu_done");
                out32(CME_LCL_SICR_CLR, BIT32(16));

                CME_GETSCOM(PPM_GPMMR, CME_MASK_C0, CME_SCOM_AND, scom_data);

                // if spwu has been re-asserted after spwu_done is dropped:
                if (scom_data & BIT64(1))
                {
                    PK_TRACE("SPWU asserts again, re-asserting spwu_done");
                    out32(CME_LCL_SICR_OR, BIT32(16));
                }
                // if spwu truly dropped:
                else
                {
                    PK_TRACE("Flip EIPR to raising edge and drop pm_exit");
                    out32(CME_LCL_EIPR_OR,  BIT32(14));
                    out32(CME_LCL_SICR_CLR, BIT32(4));

                    // Core0 is now out of spwu, allow pm_active
                    G_cme_stop_record.core_in_spwu &= ~CME_MASK_C0;
                    g_eimr_override                &= ~IRQ_VEC_STOP_C0;
                }
            }
        }
        // raising edge, Note do not clear EISR as thread will read and clear:
        else
        {
            PK_TRACE("Raising edge of spwu_c0, clear EISR later in exit thread");
            sem_post = 1;
        }
    }

    if (raw_spwu & CME_MASK_C1)
    {
        // if failing edge == spwu drop:
        if (G_cme_stop_record.core_in_spwu & CME_MASK_C1)
        {
            PK_TRACE("Falling edge of spwu_c1, first clearing EISR");
            out32(CME_LCL_EISR_CLR, BIT32(15));

            // if spwu asserts again before we drop spwu_done, do nothing, else:
            if ((~(in32(CME_LCL_EINR))) & BIT32(15))
            {
                PK_TRACE("SPWU drop confirmed, now dropping spwu_done");
                out32(CME_LCL_SICR_CLR, BIT32(17));

                CME_GETSCOM(PPM_GPMMR, CME_MASK_C1, CME_SCOM_AND, scom_data);

                // if spwu has been re-asserted after spwu_done is dropped:
                if (scom_data & BIT64(1))
                {
                    PK_TRACE("SPWU asserts again, re-asserting spwu_done");
                    out32(CME_LCL_SICR_OR, BIT32(17));
                }
                // if spwu truly dropped:
                else
                {
                    PK_TRACE("Flip EIPR to raising edge and drop pm_exit");
                    out32(CME_LCL_EIPR_OR,  BIT32(15));
                    out32(CME_LCL_SICR_CLR, BIT32(5));

                    // Core1 is now out of spwu, allow pm_active
                    G_cme_stop_record.core_in_spwu &= ~CME_MASK_C1;
                    g_eimr_override                &= ~IRQ_VEC_STOP_C1;
                }
            }

        }
        // raising edge, Note do not clear EISR as thread will read and clear:
        else
        {
            PK_TRACE("Raising edge of spwu_c1, clear EISR later in exit thread");
            sem_post = 1;
        }
    }

    if (sem_post)
    {
        out32(CME_LCL_EIMR_OR, BITS32(12, 6) | BITS32(20, 2));
        PK_TRACE_INF("Launching exit thread");
        pk_semaphore_post((PkSemaphore*)arg);
    }
    else
    {
        pk_irq_vec_restore(&ctx);
    }
}



void
p9_cme_stop_exit_handler(void* arg, PkIrqId irq)
{
    MARK_TRAP(STOP_EXIT_HANDLER)
    PK_TRACE_INF("SX Handler");
    PK_TRACE_DBG("SX Trigger %d", irq);
    out32(CME_LCL_EIMR_OR, BITS32(12, 6) | BITS32(20, 2));
    pk_semaphore_post((PkSemaphore*)arg);
}



void
p9_cme_stop_enter_handler(void* arg, PkIrqId irq)
{
    MARK_TRAP(STOP_ENTER_HANDLER)
    PK_TRACE_INF("SE Handler");
    PK_TRACE_DBG("SE Trigger %d", irq);
    out32(CME_LCL_EIMR_OR, BITS32(12, 6) | BITS32(20, 2));
    pk_semaphore_post((PkSemaphore*)arg);
}



void
p9_cme_stop_db1_handler(void* arg, PkIrqId irq)
{
    PkMachineContext ctx;
    MARK_TRAP(STOP_DB1_HANDLER)
    PK_TRACE_INF("DB1 Handler");
    PK_TRACE_DBG("DB1 Trigger %d", irq);
    pk_irq_vec_restore(&ctx);
}



void
p9_cme_stop_db2_handler(void* arg, PkIrqId irq)
{
    PkMachineContext ctx;
    cppm_cmedb2_t    db2c0 = {0};
    cppm_cmedb2_t    db2c1 = {0};

    MARK_TRAP(STOP_DB2_HANDLER)
    PK_TRACE_INF("DB2 Handler");
    PK_TRACE_DBG("DB2 Trigger %d", irq);

    CME_GETSCOM(CPPM_CMEDB2, CME_MASK_C0, CME_SCOM_AND, db2c0.value);
    CME_GETSCOM(CPPM_CMEDB2, CME_MASK_C1, CME_SCOM_AND, db2c1.value);
    CME_PUTSCOM(CPPM_CMEDB2, CME_MASK_BC, 0);
    out32(CME_LCL_EISR_CLR,  BITS32(18, 2));

    if (db2c0.fields.cme_message_numbern == DB2_BLOCK_WKUP_ENTRY)
    {
        G_cme_stop_record.core_blockwu |= CME_MASK_C0;
        g_eimr_override                |= IRQ_VEC_PCWU_C0;
    }
    else if (db2c0.fields.cme_message_numbern == DB2_BLOCK_WKUP_EXIT)
    {
        G_cme_stop_record.core_blockwu &= ~CME_MASK_C0;
        g_eimr_override                &= ~IRQ_VEC_PCWU_C0;
    }

    if (db2c1.fields.cme_message_numbern == DB2_BLOCK_WKUP_ENTRY)
    {
        G_cme_stop_record.core_blockwu |= CME_MASK_C1;
        g_eimr_override                |= IRQ_VEC_PCWU_C1;
    }
    else if (db2c1.fields.cme_message_numbern == DB2_BLOCK_WKUP_EXIT)
    {
        G_cme_stop_record.core_blockwu &= ~CME_MASK_C1;
        g_eimr_override                &= ~IRQ_VEC_PCWU_C1;
    }

    out32(CME_LCL_SICR_OR,  G_cme_stop_record.core_blockwu << SHIFT32(3));
    out32(CME_LCL_SICR_CLR,
          (~G_cme_stop_record.core_blockwu & CME_MASK_BC) << SHIFT32(3));

    out32(CME_LCL_FLAGS_OR, G_cme_stop_record.core_blockwu << SHIFT32(9));
    out32(CME_LCL_FLAGS_CLR,
          (~G_cme_stop_record.core_blockwu & CME_MASK_BC) << SHIFT32(9));

    pk_irq_vec_restore(&ctx);
}
