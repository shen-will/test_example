//
// Created by Hasee on 2019/11/1.
//

#include "jvm_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jni.h"
#include <dlfcn.h>
#include <unistd.h>

#define MAX_OP_COUNT 5

static JavaVMOption options[MAX_OP_COUNT] = {0};
static unsigned int op_count = 0;

static JavaVM *vm = nullptr;

void *libjvm = nullptr;

typedef  jint (*CreateJavaVM)(JavaVM **pvm, void **penv, void *args);

CreateJavaVM  createJavaVm = nullptr;


static bool load_libjvm(){

    if(libjvm == nullptr){
        const char *path = "/home/shen/openjdk/build/linux-x86-normal-"
                           "server-slowdebug/jdk/lib/i386/server/libjvm.so";

       libjvm = dlopen(path,RTLD_NOW);
    }

    if(libjvm == nullptr)
        return false;

    createJavaVm = (CreateJavaVM)dlsym(libjvm,"JNI_CreateJavaVM");

    return createJavaVm != nullptr;
}

static bool init_default_ops(const char *class_path){

    pid_t pid = getpid();
    char *op_pid = NEW_STRING(256);
    sprintf(op_pid,"-Dsun.java.launcher.pid=%d",pid);

    options[op_count++].optionString = "-Djava.class.path=.";
    if(class_path != nullptr && strcmp(class_path,".") != 0){
        char *op_class_path = NEW_STRING(strlen(class_path)+50);
        sprintf(op_class_path,"-Djava.class.path=%s",class_path);
        options[op_count++].optionString = op_class_path;
    }
    options[op_count++].optionString = "-Dsun.java.command=test_jvm";
    options[op_count++].optionString = "-Dsun.java.launcher=SUN_STANDARD";
    options[op_count++].optionString = op_pid;

    return true;
}

static bool create_java_vm(){

    JavaVMInitArgs args;

    memset(&args, 0, sizeof(args));
    args.version  = JNI_VERSION_1_2;
    args.nOptions = op_count;
    args.options  = options;
    args.ignoreUnrecognized = JNI_FALSE;

    JNIEnv *env = nullptr;
    return createJavaVm(&vm, (void **)&env, &args) == JNI_OK;
}

JavaVM* init_jvm(const char *class_path){

    if(!load_libjvm() || !init_default_ops(class_path) || !create_java_vm())
        return nullptr;

    return vm;
}


int destroy_jvm(){

    if(vm == nullptr)
        return -1;

    if(vm->DetachCurrentThread() != JNI_OK || vm->DestroyJavaVM() != JNI_OK){
        return -1;
    }

    vm = nullptr;
    op_count = 0;
    memset(&options,0, sizeof(JavaVMOption) * MAX_OP_COUNT);

    if(libjvm != nullptr)
        dlclose(libjvm);
}

JNIEnv* get_jni_env(){

    if(vm == nullptr)
        return nullptr;

    JNIEnv *env = nullptr;
    return  vm->GetEnv((void**)&env,JNI_VERSION_1_2) == JNI_OK ? env : nullptr;
}


