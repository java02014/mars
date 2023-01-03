// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

/*
 * sdt_logic.cc
 *
 *  Created on: 2016年3月18日
 *      Author: caoshaokun
 */

#include "mars/sdt/sdt_logic.h"

#include "mars/baseevent/baseevent.h"
#include "mars/baseevent/baseprjevent.h"
#include "mars/comm/xlogger/xlogger.h"
#include "mars/comm/bootrun.h"
#include "mars/sdt/constants.h"

#include "sdt/src/sdt_core.h"
#include "sdt_manager.h"
#include "mars/boot/base_context.h"

namespace mars {
namespace sdt {

//static Callback* sg_callback = NULL;

static const std::string kLibName = "sdt";

/*
#define SDT_WEAK_CALL(func) \
    boost::shared_ptr<SdtCore> sdt_ptr = SdtCore::Singleton::Instance_Weak().lock();\
    if (!sdt_ptr) {\
        xwarn2(TSF"sdt uncreate");\
        return;\
    }\
	sdt_ptr->func
*/

static void onCreate() {
    xinfo2(TSF"sdt oncreate");
    //SdtCore::Singleton::Instance();
    SdtManager* sg_sdt_manager = (SdtManager*)mars::boot::CreateContext("default")->GetManager("Sdt");
    sg_sdt_manager->OnCreate();
}

static void onDestroy() {
    xinfo2(TSF"sdt onDestroy");
    //SdtCore::Singleton::AsyncRelease();
    SdtManager* sg_sdt_manager = (SdtManager*)mars::boot::CreateContext("default")->GetManager("Sdt");
    sg_sdt_manager->OnDestroy();
}

static void __initbind_baseprjevent() {

	GetSignalOnCreate().connect(&onCreate);
	GetSignalOnDestroy().connect(5, &onDestroy);
}

BOOT_RUN_STARTUP(__initbind_baseprjevent);

//active netcheck interface
void StartActiveCheck(CheckIPPorts& _longlink_check_items, CheckIPPorts& _shortlink_check_items, int _mode, int _timeout) {
	//SDT_WEAK_CALL(StartCheck(_longlink_check_items, _shortlink_check_items, _mode, _timeout));
        SdtManager* sg_sdt_manager = (SdtManager*)mars::boot::CreateContext("default")->GetManager("Sdt");
        sg_sdt_manager->StartActiveCheck(_longlink_check_items, _shortlink_check_items, _mode, _timeout);
}

void CancelActiveCheck() {
	//SDT_WEAK_CALL(CancelCheck());
        SdtManager* sg_sdt_manager = (SdtManager*)mars::boot::CreateContext("default")->GetManager("Sdt");
        sg_sdt_manager->CancelActiveCheck();
}

void SetCallBack(Callback* const callback) {
	//sg_callback = callback;
        SdtManager* sg_sdt_manager = (SdtManager*)mars::boot::CreateContext("default")->GetManager("Sdt");
        sg_sdt_manager->SetCallBack(callback);
}

#ifdef NATIVE_CALLBACK
void SetSdtNativeCallback(std::shared_ptr<SdtNativeCallback> _cb) {
    SdtManager* sg_sdt_manager = (SdtManager*)mars::boot::CreateContext("default")->GetManager("Sdt");
    sg_sdt_manager->SetSdtNativeCallback(_cb);
}
#endif

//#if !defined(ANDROID) || defined (CPP_CALL_BACK)
void (*ReportNetCheckResult)(const std::vector<CheckResultProfile>& _check_results)
= [](const std::vector<CheckResultProfile>& _check_results) {
    SdtManager* sg_sdt_manager = (SdtManager*)mars::boot::CreateContext("default")->GetManager("Sdt");
    sg_sdt_manager->ReportNetCheckResult(_check_results);
};
//#endif

}}
