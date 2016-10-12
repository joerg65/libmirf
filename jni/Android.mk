LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libWiringPi
LOCAL_SRC_FILES := ../../wiringPi/libs/armeabi/libwiringPi.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -DDEBUG

LOCAL_MODULE := libmirf

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../wiringPi/jni/wiringPi

LOCAL_SRC_FILES := \
	mirf.cpp 

LOCAL_LDLIBS    := -llog

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libdl \
	libc \
	libWiringPi

LOCAL_CFLAGS += -DANDROID -Wall -Wextra

include $(BUILD_SHARED_LIBRARY)
