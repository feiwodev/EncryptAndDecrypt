#include <jni.h>
#include <string.h>

JNIEXPORT jstring JNICALL
Java_com_zeno_encryptanddecrypt_ndk_HelloNDK_sayHelloNDK(JNIEnv *env, jclass type) {

    // TODO


    return (*env)->NewStringUTF(env, returnValue);
}
