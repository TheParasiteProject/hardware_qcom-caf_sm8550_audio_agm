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

#define LOG_TAG "agm_death_notifier"

#include <stdlib.h>
#include <utils/RefBase.h>
#include <utils/Log.h>
#include <binder/TextOutput.h>
#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryDealer.h>
#include <pthread.h>
#include <cutils/list.h>
#include <signal.h>
#include "ipc_interface.h"
#include "agm_death_notifier.h"

using namespace android;

struct listnode g_client_list;
pthread_mutex_t g_client_list_lock;
bool g_client_list_init = false;
//initialize mutex

client_death_notifier::client_death_notifier(void) {
    ALOGV("%s: %d", __func__, __LINE__);
    sp<ProcessState> proc(ProcessState::self());
    proc->startThreadPool();
}
sp<client_death_notifier> Client_death_notifier = NULL;

client_info *get_client_handle_from_list(pid_t pid) {
    struct listnode *node = NULL;
    client_info *handle = NULL;

    pthread_mutex_lock(&g_client_list_lock);
    list_for_each(node, &g_client_list) {
        handle = node_to_item(node, client_info, list);
        ALOGD("%s: pid %d, handle->pid %d", __func__, pid, handle->pid);
         if (handle != NULL && handle->pid == pid) {
             ALOGD("%s: Found handle %p", __func__, handle);
             pthread_mutex_unlock(&g_client_list_lock);
             return handle;
        }
    }
    pthread_mutex_unlock(&g_client_list_lock);
    return NULL;
}


void agm_register_client(sp<IBinder> binder){
    client_info *client_handle = NULL;
	pid_t pid = IPCThreadState::self()->getCallingPid();
    android::sp<IAGMClient> client_binder = android::interface_cast<IAGMClient>(binder);
    Client_death_notifier = new client_death_notifier();
    IInterface::asBinder(client_binder)->linkToDeath(Client_death_notifier);
	ALOGE("%s: Client registered and death notifier linked to AGM", __func__);
    if (g_client_list_init == false) {
        pthread_mutex_init(&g_client_list_lock, (const pthread_mutexattr_t *) NULL);
        list_init(&g_client_list);
        g_client_list_init = true;
    }
	client_handle = (client_info *)calloc(1, sizeof(client_info));
    if (client_handle == NULL) {
       ALOGE("%s: Cannot allocate memory for client handle", __func__);
    }
    pthread_mutex_lock(&g_client_list_lock);
    client_handle->binder = client_binder;
	client_handle->pid = pid;
    client_handle->Client_death_notifier = Client_death_notifier;
    list_add_tail(&g_client_list, &client_handle->list);
	list_init(&client_handle->agm_client_hndl_list);
    pthread_mutex_unlock(&g_client_list_lock);
    
}

void agm_add_session_obj_handle( struct session_obj *handle){
    client_info *client_handle = NULL;
    struct listnode *node = NULL;
    agm_client_session_handle *hndl = NULL;
    struct listnode *tempnode = NULL;

    client_handle = get_client_handle_from_list(IPCThreadState::self()->getCallingPid());
    if (client_handle == NULL) {
        ALOGE("%s: Could not find client handle", __func__);
        goto exit;
    }

    pthread_mutex_lock(&g_client_list_lock);
    hndl = (agm_client_session_handle *)calloc(1, sizeof(agm_client_session_handle));
    if (hndl == NULL) {
        ALOGE("%s: Cannot allocate memory to store agm session handle", __func__);
        goto exit;
    }
    hndl->handle = handle;
    list_add_tail(&client_handle->agm_client_hndl_list, &hndl->list);
    ALOGD("%s: Added hndl %p to the list", __func__, hndl);

exit:
    pthread_mutex_unlock(&g_client_list_lock);
}

void agm_remove_session_obj_handle( struct session_obj *handle){
    client_info *client_handle = NULL;
    struct listnode *node = NULL;
    agm_client_session_handle *hndl = NULL;
    struct listnode *tempnode = NULL;

    client_handle = get_client_handle_from_list(IPCThreadState::self()->getCallingPid());
    if (client_handle == NULL) {
        ALOGE("%s: Could not find client handle", __func__);
        return;
    }

    pthread_mutex_lock(&g_client_list_lock);
    list_for_each_safe(node, tempnode, &client_handle->agm_client_hndl_list) {
        hndl = node_to_item(node, agm_client_session_handle, list);
        if ((hndl != NULL) && (hndl->handle == handle)) {
            ALOGD("%s: Removed handle %p", __func__, handle);
            list_remove(node);
            free(hndl);
            break;
        }
    }
    pthread_mutex_unlock(&g_client_list_lock);
}

void agm_unregister_client(sp<IBinder> binder){
    android::sp<IAGMClient> client_binder = android::interface_cast<IAGMClient>(binder);
	client_info *handle = NULL;
    struct listnode *tempnode = NULL;
    struct listnode *node = NULL;

    ALOGV("%s: enter", __func__);
    pthread_mutex_lock(&g_client_list_lock);
    list_for_each_safe(node, tempnode, &g_client_list) {
        handle = node_to_item(node, client_info, list);
        if (handle != NULL && handle->pid ==
            IPCThreadState::self()->getCallingPid()) {
            if (handle->Client_death_notifier != NULL) {
                IInterface::asBinder(client_binder)->unlinkToDeath(handle->Client_death_notifier);
                handle->Client_death_notifier.clear();
                ALOGV("%s: unlink to death %d", __func__, handle->pid);
            }
            list_remove(node);
            free(handle);
        }
    }
    ALOGV("%s: exit", __func__);
    pthread_mutex_unlock(&g_client_list_lock);
}

void client_death_notifier::binderDied(const wp<IBinder>& who) {
    client_info *handle = NULL;
    struct listnode *node = NULL;
    struct listnode *tempnode = NULL;
	agm_client_session_handle *hndl = NULL;
    struct listnode *sess_node = NULL;
    struct listnode *sess_tempnode = NULL;

    ALOGD("%s: Client Died, who %p", __func__, who.unsafe_get());
    pthread_mutex_lock(&g_client_list_lock);
    list_for_each_safe(node, tempnode, &g_client_list) {
        handle = node_to_item(node, client_info, list);
        if (handle != NULL && (IInterface::asBinder(handle->binder).get() == who.unsafe_get())) {
            list_for_each_safe(sess_node, sess_tempnode, &handle->agm_client_hndl_list) {
                hndl = node_to_item(sess_node, agm_client_session_handle, list);
                if (hndl != NULL && (hndl->handle != NULL)){
				    agm_session_close(hndl->handle);
				    list_remove(sess_node);
                    free(hndl);
			    }
			}
			list_remove(node);
            free(handle);
        }
    }
    pthread_mutex_unlock(&g_client_list_lock);
    ALOGD("%s: exit", __func__);

}

const android::String16 IAGMClient::descriptor("IAGMClient");
const android::String16& IAGMClient::getInterfaceDescriptor() const {
    return IAGMClient::descriptor;
}

class BpClient: public ::android:: BpInterface<IAGMClient> {
public:
    BpClient(const sp<IBinder>& impl) :
        BpInterface<IAGMClient>(impl) {
        ALOGD("BpClient::BpClient()");
    }
};

android::sp<IAGMClient> IAGMClient::asInterface
    (const android::sp<android::IBinder>& obj) {
    ALOGV("IAGMClient::asInterface()");
    android::sp<IAGMClient> intr;
    if (obj != NULL) {
        intr = static_cast<IAGMClient*>(obj->queryLocalInterface
                        (IAGMClient::descriptor).get());
        ALOGD("IAGMClient::asInterface() interface %s",
            ((intr == 0)?"zero":"non zero"));
        if (intr == NULL)
            intr = new BpClient(obj);
    }
    return intr;
}
IAGMClient :: IAGMClient() { ALOGE("IAGMClient::IAGMClient()"); }
IAGMClient :: ~IAGMClient() { ALOGE("IAGMClient::~IAGMClient()"); }

int32_t DummyBnClient::onTransact(uint32_t code,
                                   const Parcel& data,
                                   Parcel* reply,
                                   uint32_t flags) {
    int status = 0;
    ALOGV("DummyBnClient::onTransact(%i) %i", code, flags);
    //data.checkInterface(this); //Alternate option to check interface
	CHECK_INTERFACE(IAGMClient, data, reply);
    switch(code) {
        default:
            return BBinder::onTransact(code, data, reply, flags);
            break;
    }
    return status;
}

