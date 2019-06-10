/*
** Copyright (c) 2019, The Linux Foundation. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above
**     copyright notice, this list of conditions and the following
**     disclaimer in the documentation and/or other materials provided
**     with the distribution.
**   * Neither the name of The Linux Foundation nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
** ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
** OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
** IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#ifndef __UTILS_H__
#include "casa_osal_log.h"
#include "casa_osal_error.h"

#define AGM_LOGE(...) CASA_LOG_ERR(LOGTAG, __VA_ARGS__)
#define AGM_LOGD(...) CASA_LOG_ERR(LOGTAG, __VA_ARGS__)
#define AGM_LOGI(...) CASA_LOG_INFO(LOGTAG, __VA_ARGS__)
#define AGM_LOGV(...) CASA_LOG_VERBOSE(LOGTAG, __VA_ARGS__)

static inline print_kv_pairs(struct agm_key_vector *gkv)
{
   int i = 0;
   AGM_LOGE("No of kv pairs %d", gkv->num_kvs);
   for (i = 0; i < gkv->num_kvs; i++)
      AGM_LOGE("key: %x value %x", gkv->kv[i].key, gkv->kv[i].value);
}

#endif /*__UTILS_H*/
