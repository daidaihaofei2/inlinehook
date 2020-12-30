//
// Created by 111 on 2020/12/25.
//
#include <stdio.h>
#include <jni.h>
#include <string>
#include <vector>
#include <android/log.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "include/inlineHook.h"

#define LOG_TAG "InlineHook"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
static const char* className="com/example/inlinehook/MainActivity";


FILE* (*old_fopen)(const char* __path, const char* __mode) = NULL;

FILE* new_fopen(const char* __path, const char* __mode)
{
    __android_log_print(ANDROID_LOG_WARN,"Hook warning","Detecting fopen function is hooked...");
    return NULL;
}

int hook()
{
    if (registerInlineHook((uint32_t)fopen, (uint32_t) new_fopen, (uint32_t **) &old_fopen) != ELE7EN_OK) {
        return -1;
    }
    if (inlineHook((uint32_t) fopen) != ELE7EN_OK) {
        return -1;
    }

    return 0;
}

int unHook()
{
    if (inlineUnHook((uint32_t) fopen) != ELE7EN_OK) {
        return -1;
    }

    return 0;
}

int native_Hook(){
    return hook();
}

static int test_Hook(){
    char *fileName="/sdcard/testHook.txt";
    FILE *fp=NULL;
    if((fp=fopen(fileName,"w+"))==NULL){
        ALOGE("fopen FAILED");
        return 0;
    }
    unHook();
    if((fp=fopen(fileName,"w+"))==NULL){
        ALOGE("fopen FAILED");
        return 0;
    } else{
        __android_log_print(ANDROID_LOG_WARN,"Hook warning","fopen open File*");
        fclose(fp);
    }

}


static const JNINativeMethod methods[]={{"nativeHook","()V",(void*)native_Hook},
                                        {"testHook","()V",(void*)test_Hook}};

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
    JNIEnv* env=NULL;
    if(vm->GetEnv((void**)&env,JNI_VERSION_1_6)){
        return JNI_ERR;
    }
    jclass claz = env->FindClass(className);
    if(env->RegisterNatives(claz,methods,sizeof(methods) / sizeof(methods[0]))<0){
        ALOGE("RegisterNatives FAILED");
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

