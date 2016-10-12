/*
	Mirf library ported to Odroid C1 / Android 5.1.1
 
	Copyright (C) <2016>  <Jörg Wolff>
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <android/log.h>
#include <mirf.h>

#ifdef __cplusplus
extern "C" {
#endif


#define LOG_TAG "com.jw.mirf"

#define LOG_D(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOG_F(fn_name) __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "Called : " fn_name )

static JavaVM *java_vm;
mirf* receiver;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    // Get jclass with env->FindClass.
    // Register methods with env->RegisterNatives.

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_com_jw_alarm_MirfService_Setup(JNIEnv * env, jobject  obj, unsigned char ce, unsigned int speed, unsigned char spi_channel, unsigned char size, unsigned char mirf_channel) {
    receiver = new mirf(ce, speed, spi_channel, size, mirf_channel);
}
#ifdef __cplusplus
}
#endif
