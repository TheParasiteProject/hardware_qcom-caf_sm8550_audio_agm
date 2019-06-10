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

#ifndef __AGM_SERVER_INTERFACE_H__
#define __AGM_SERVER_INTERFACE_H__

#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <agm/agm_api.h>
typedef void shmem_handle_t;

// can be shared by both server and client.

class IAgmService: public ::android::IInterface {
    public:
        DECLARE_META_INTERFACE(AgmService);

        virtual int ipc_agm_init()= 0;
        virtual int ipc_agm_audio_intf_set_metadata(uint32_t audio_intf, struct agm_meta_data *metadata)= 0;
        virtual int ipc_agm_session_set_metadata(uint32_t session_id, struct agm_meta_data *metadata)= 0;
        virtual int ipc_agm_session_audio_inf_set_metadata(uint32_t session_id, uint32_t audio_intf, struct agm_meta_data *metadata)= 0;
        virtual int ipc_agm_session_close(struct session_obj *handle)= 0;
        virtual int ipc_agm_audio_intf_set_media_config(uint32_t audio_intf, struct agm_media_config *media_config)= 0;
        virtual int ipc_agm_session_prepare(struct session_obj *session_handle)= 0;
        virtual int ipc_agm_session_start(struct session_obj *session_handle)= 0;
        virtual int ipc_agm_session_stop(struct session_obj *session_handle)= 0;
        virtual int ipc_agm_session_pause(struct session_obj *session_handle)= 0;
        virtual int ipc_agm_session_resume(struct session_obj *session_handle)= 0;
        virtual int ipc_agm_session_read(struct session_obj *session_handle, void *buff, size_t count)= 0;
        virtual int ipc_agm_session_write(struct session_obj *session_handle, void *buff, size_t count)= 0;
        virtual int ipc_agm_session_audio_inf_connect(uint32_t session_id, uint32_t audio_intf, bool state) = 0;
        virtual int ipc_agm_session_set_loopback(uint32_t capture_session_id, uint32_t playback_session_id, bool state) = 0;
		virtual size_t ipc_agm_get_hw_processed_buff_cnt(struct session_obj *handle, enum direction dir) = 0;
		virtual int ipc_agm_get_aif_info_list(struct aif_info *aif_list, size_t *num_aif_info) = 0;
        virtual int ipc_agm_session_open(uint32_t session_id, struct session_obj **handle) = 0;
        virtual int ipc_agm_session_set_config(struct session_obj *handle,
        			struct agm_session_config *session_config,
        			struct agm_media_config *media_config,
        			struct agm_buffer_config *buffer_config) = 0;
};

class BnAgmService : public ::android::BnInterface<IAgmService> {
    android::status_t onTransact(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags);
};
#endif
