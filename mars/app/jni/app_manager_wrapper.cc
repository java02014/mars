//
// Created by Cpan on 2022/9/7.
//

#include <android/log.h>
#include <jni.h>

#include "comm/jni/jnicat/jnicat_core.h"
#include "comm/jni/jnicat/jnicat_object_wrapper.h"
#include "mars/app/app.h"
#include "mars/app/app_manager.h"
#include "mars/app/base_app_manager.h"
#include "mars/app/jni/app_manager_callback_wrapper.h"

namespace mars {
namespace app {

class JniAppManager {
 public:
    static void JniCreateAppManagerFromHandle(JNIEnv* env, jobject instance, jlong handle) {
        __android_log_print(ANDROID_LOG_INFO, "NEWT", "JniCreateAppManagerFromHandle start. env:%lu handle:%ld", (uint64_t)env, handle);

        auto app_manager_cpp = (BaseAppManager*)j2c_cast(handle);
        auto appManagerWrapper = new jnicat::JniObjectWrapper<BaseAppManager>(app_manager_cpp);
        appManagerWrapper->instantiate(env, instance);

        __android_log_print(ANDROID_LOG_INFO, "NEWT", "JniCreateAppManagerFromHandle cpp %lu.", (uint64_t)app_manager_cpp);
        __android_log_print(ANDROID_LOG_INFO, "NEWT", "JniCreateAppManagerFromHandle end.");
    }

    static void JniOnDestroyAppManager(JNIEnv* env, jobject instance) {
        auto app_manager_cpp = jnicat::JniObjectWrapper<BaseAppManager>::object(env, instance);
        jnicat::JniObjectWrapper<Callback>::dispose(env, instance, "callbackHandle");
        app_manager_cpp->SetCallback(nullptr);
        jnicat::JniObjectWrapper<BaseAppManager>::dispose(env, instance);
    }

    static void JniSetCallback(JNIEnv* env, jobject instance, jobject callback) {
        __android_log_print(ANDROID_LOG_INFO, "NEWT", "JniSetCallback start. env:%lu", (uint64_t)env);
        auto app_manager_cpp = jnicat::JniObjectWrapper<BaseAppManager>::object(env, instance);

        __android_log_print(ANDROID_LOG_INFO, "NEWT", "JniSetCallback cpp %lu", (uint64_t)app_manager_cpp);
        __android_log_print(ANDROID_LOG_INFO, "NEWT", "JniSetCallback new callback.");

        auto appManagerJniCallback = new AppManagerJniCallback(env, callback);
        auto appManagerCallbackWrapper = new jnicat::JniObjectWrapper<AppManagerJniCallback>(appManagerJniCallback);
        appManagerCallbackWrapper->instantiate(env, instance, "callbackHandle");

        __android_log_print(ANDROID_LOG_INFO, "NEWT", "JniSetCallback set callback.");
        if(app_manager_cpp){
            __android_log_print(ANDROID_LOG_INFO, "NEWT", "JniSetCallback app_manager_cpp is not empty.");
        }else{
            __android_log_print(ANDROID_LOG_INFO, "NEWT", "JniSetCallback app_manager_cpp is empty.");
        }
        app_manager_cpp->SetCallback(appManagerJniCallback);

        __android_log_print(ANDROID_LOG_INFO, "NEWT", "JniSetCallback end.");
    }
};

static const JNINativeMethod kAppManagerJniMethods[] = {
    {"OnJniCreateAppManagerFromHandle", "(J)V", (void*)&mars::app::JniAppManager::JniCreateAppManagerFromHandle},
    {"OnJniDestroyAppManager", "()V", (void*)&mars::app::JniAppManager::JniOnDestroyAppManager},
    {"OnJniSetCallback", "(Ljava/lang/Object;)V", (void*)&mars::app::JniAppManager::JniSetCallback},
};

static const size_t kAppManagerJniMethodsCount = sizeof(kAppManagerJniMethods) / sizeof(JNINativeMethod);
JNICAT_DEFINE_JNI_METHOD(kAppManagerInterface, "com/tencent/mars/app/AppManager", kAppManagerJniMethods, kAppManagerJniMethodsCount)

}  // namespace app
}  // namespace mars

void ExportAppManager() {
}
