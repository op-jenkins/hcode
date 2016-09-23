# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: import/chips/p9/procedures/ppe_closed/cme/stop_cme/p9_cme_edit.mk $
#
# OpenPOWER HCODE Project
#
# COPYRIGHT 2016,2017
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG
EXE=cmeImgEdit

$(EXE)_COMMONFLAGS+= -D__PPE_PLAT

$(call ADD_EXE_INCDIR, $(EXE), \
   $(CME_SRCDIR) \
   $(PK_SRCDIR)/kernel \
   $(HCODE_COMMON_LIBDIR) \
   $(HCODE_LIBDIR) \
    ) 


IMAGE_DEPS+=cmeImgEdit
OBJS=p9_cme_img_edit.o
$(call BUILD_EXE)

EXE=cpmr_headerImgEdit

$(EXE)_COMMONFLAGS+= -D__PPE_PLAT

$(call ADD_EXE_INCDIR, $(EXE), \
   $(CME_SRCDIR) \
   $(PK_SRCDIR)/kernel \
   $(HCODE_COMMON_LIBDIR) \
   $(HCODE_LIBDIR) \
    ) 


IMAGE_DEPS+=cpmr_headerImgEdit
OBJS=p9_cme_img_edit.o
$(call BUILD_EXE)
