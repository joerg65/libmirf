# libmirf
mirf library ported to Odroid C1 / Android

This is part of my Alarm Central written on Odroid C1 / Android  
The intent of this library is to communicate with RF24 sensors as the nRF24L01. To use it from an Android app it need a wrapper library as the mirf_android.cpp.
To compile the library it can be stored e.g. to the sample directory of the Android NDK and executed the comand: 
```
"../../ndk-build -B"
```
As a dependency, there must be coexist in the tree the project wiringPi for Odroid C1. in Android.mk, there are some relative paths pointing to.
