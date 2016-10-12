# libmirf
mirf library ported to Odroid C1 / Android

The intent of this library is to communicate with RF24 sensors as the nRF24L01. To use it from an Android app it need a wrapper library as the mirf_android.cpp.
To compile the library it can be stored e.g. to the sample dirctory of the Android NDK and executed the comand: 
```
"../../ndk-build -B"
```