//
// Created by Hasee on 2019/11/1.
//

#ifndef JAVA_TEST_JVM_HELPER_H
#define JAVA_TEST_JVM_HELPER_H

#include "jni.h"

#define NEW_STRING(size) (char*)malloc((size))


#ifdef __cplusplus
extern "C" {
#endif

JavaVM *init_jvm(const char *class_path);

int destroy_jvm();

JNIEnv* get_jni_env();


#ifdef __cplusplus
}
#endif

#endif //JAVA_TEST_JVM_HELPER_H
