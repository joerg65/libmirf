/*
	Model of Mirf wrapping library ported to Odroid C1 / Android

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

    system("insmod /system/lib/modules/spicc.ko");
    system("insmod /system/lib/modules/spidev.ko");

    return JNI_VERSION_1_6;
}

//mirf(uint8_t _cepin, uint32_t _freq, uint8_t _spi_channel, uint8_t _payload_size, uint8_t _mirf_CH);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfSetup(JNIEnv * env, jobject  obj, uint8_t ce, uint32_t speed, uint8_t spi_channel, uint8_t size, uint8_t mirf_channel) {
    receiver = new mirf(ce, speed, spi_channel, size, mirf_channel);
    //LOG_D("Setup");
}

//void config(void);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfConfig(JNIEnv* env, jobject obj) {
    if (receiver != NULL) receiver->config();
}

//void reconfig_rx(void);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfReConfigRx(JNIEnv* env, jobject obj) {
    if (receiver != NULL) receiver->reconfig_rx();
}

//void reconfig_tx(void);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfReConfigTx(JNIEnv* env, jobject obj) {
    if (receiver != NULL) receiver->reconfig_tx();
}

//void set_address(uint8_t pos, uint8_t* address);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfSetAddress(JNIEnv* env, jobject obj, jbyte pos, jstring address) {
    if (receiver != NULL){
        const char *nativeString = env->GetStringUTFChars(address, 0);
        receiver->set_address(pos, (uint8_t*)nativeString);
        LOG_D("SetAddress: %s", nativeString);
        env->ReleaseStringUTFChars(address, nativeString);
    }
}

//uint8_t receive_data(void* buf);
JNIEXPORT jbyteArray JNICALL
Java_path_to_your_app_MirfReceiveData(JNIEnv* env, jobject obj, jbyte size) {
    if (receiver != NULL) {
        jbyte *data=(jbyte *)malloc(size*sizeof(jbyte));
        receiver->receive_data(data);
        jbyteArray result=env->NewByteArray(size);
        env->SetByteArrayRegion(result, 0, size, data);
        delete[] data;
        return result;
    }
    return 0;
}

//uint8_t transmit_data(void* buf);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfTransmitData(JNIEnv* env, jobject obj, jbyteArray array) {
    if (receiver != NULL) {
        jbyte *buf = env->GetByteArrayElements(array, NULL);
        receiver->transmit_data(buf);
        env->ReleaseByteArrayElements(array, buf, 0);
    }
}

//uint8_t status(void);
//uint8_t max_rt_reached(void);

//uint8_t data_ready(void);
JNIEXPORT int JNICALL
Java_path_to_your_app_MirfDataReady(JNIEnv* env, jobject obj) {
    if (receiver != NULL) return receiver->data_ready();
    LOG_D("MirfDataReady:return 0");
    return 0;
}

//uint8_t read_register(uint8_t reg, uint8_t* buf, uint8_t len);
//uint8_t read_register(uint8_t reg);
//uint8_t write_register(uint8_t reg, const uint8_t* buf, uint8_t len);
//uint8_t write_register(uint8_t reg, uint8_t value);
//void config_register(uint8_t reg, uint8_t value);
//uint8_t get_data(void* buf);
//uint8_t send_data(void* buf);

//void power_up_rx(void);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfPowerUpRx(JNIEnv* env, jobject obj) {
    if (receiver != NULL) receiver->power_up_rx();
}

//void power_up_tx(void);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfPowerUpTx(JNIEnv* env, jobject obj) {
    if (receiver != NULL) receiver->power_up_tx();
}

//void power_down(void);

//uint8_t flush_rx(void);
JNIEXPORT int JNICALL
Java_path_to_your_app_MirfFlushRx(JNIEnv* env, jobject obj) {
    if (receiver != NULL) return receiver->flush_rx();
    return 0;
}

//uint8_t flush_tx(void);
JNIEXPORT int JNICALL
Java_path_to_your_app_MirfFlushTx(JNIEnv* env, jobject obj) {
    if (receiver != NULL) return receiver->flush_tx();
    return 0;
}

//void start_listening(void);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfStartListening(JNIEnv* env, jobject obj) {
    if (receiver != NULL) receiver->start_listening();
}

//void stop_listening(void);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfStopListening(JNIEnv* env, jobject obj) {
    if (receiver != NULL) receiver->stop_listening();
}

//void delay_us(unsigned int howLong);
JNIEXPORT void JNICALL
Java_path_to_your_app_MirfDelayMicroSeconds(JNIEnv* env, jobject obj, int us) {
    if (receiver != NULL) receiver->delay_us(us);
}



#ifdef __cplusplus
}
#endif
