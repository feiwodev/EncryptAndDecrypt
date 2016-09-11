//
// Created by Zeno on 2016/9/10.
//

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>

//#include "util.h"

#define LOGE(TAG,FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,TAG,FORMAT,__VA_ARGS__)

JNIEXPORT jstring JNICALL
Java_com_zeno_encryptanddecrypt_ndk_HelloNDK_sayHelloNDK(JNIEnv *env, jclass type) {

    // TODO

    return (*env)->NewStringUTF(env, "this String come from C ");
}

/*加密文件*/
JNIEXPORT void JNICALL
Java_com_zeno_encryptanddecrypt_ndk_HelloNDK_fileEncrypt(JNIEnv *env, jclass type,
                                                         jstring normalFilePath_,
                                                         jstring encryptFilePath_,
                                                         jstring password_) {
    const char *normalFilePath = (*env)->GetStringUTFChars(env, normalFilePath_, 0);
    const char *encryptFilePath = (*env)->GetStringUTFChars(env, encryptFilePath_, 0);
    const char *password = (*env)->GetStringUTFChars(env, password_, 0);


    int passwordLen = strlen(password);

    LOGE("MainActivity","path1 == %s , path2 == %s",normalFilePath , encryptFilePath) ;

     // 读文件指针
    FILE* frp = fopen(normalFilePath,"rb") ;
    // 写文件指针
    FILE* fwp = fopen(encryptFilePath,"wb") ;

    if(frp == NULL) {
        LOGE("MainActivity","%s","文件不存在") ;
        return;
    }
    if(fwp == NULL) {
        LOGE("MainActivity","%s","没有写权限") ;
    }

    // 边读边写边加密
    int buffer ;
    int index = 0 ;
    while((buffer = fgetc(frp)) != EOF) {

        // write
        fputc(buffer ^ *(password+(index % passwordLen)),fwp) ;
        index++ ;
    }


    // 关闭文件流
    fclose(fwp);
    fclose(frp);

    LOGE("MainActivity","%s","文件加密成功") ;

    (*env)->ReleaseStringUTFChars(env, normalFilePath_, normalFilePath);
    (*env)->ReleaseStringUTFChars(env, encryptFilePath_, encryptFilePath);
    (*env)->ReleaseStringUTFChars(env, password_, password);
}

/*文件解密*/
JNIEXPORT void JNICALL
Java_com_zeno_encryptanddecrypt_ndk_HelloNDK_fileDecrypt(JNIEnv *env, jclass type,
                                                         jstring encryptFilePath_,
                                                         jstring decryptFilePath_,
                                                         jstring password_) {
    const char *encryptFilePath = (*env)->GetStringUTFChars(env, encryptFilePath_, 0);
    const char *decryptFilePath = (*env)->GetStringUTFChars(env, decryptFilePath_, 0);
    const char *password = (*env)->GetStringUTFChars(env, password_, 0);

    // 得到密码长度
    int passwordLen = strlen(password) ;

    // 读写文件
    FILE* encrypt_fp = fopen(encryptFilePath,"rb") ;
    FILE* decrypt_fp = fopen(decryptFilePath,"wb");

    if(encrypt_fp == NULL) {
        LOGE("MainActivity","%s","文件不存在，或没有文件读权限");
        return;
    }

    if(decrypt_fp == NULL) {
        LOGE("MainActivity","%s","没有文件写权限");
        return;
    }


    int buffer ;
    int index = 0 ;

    while((buffer = fgetc(encrypt_fp)) != EOF) {

        fputc(buffer ^ *(password+(index % passwordLen)),decrypt_fp);

        index++ ;
    }

    // 关闭流
    fclose(decrypt_fp) ;
    fclose(encrypt_fp);

    LOGE("MainActivity","%s","文件解密成功");

    (*env)->ReleaseStringUTFChars(env, encryptFilePath_, encryptFilePath);
    (*env)->ReleaseStringUTFChars(env, decryptFilePath_, decryptFilePath);
    (*env)->ReleaseStringUTFChars(env, password_, password);
}

/*获取文件大小*/
long getFileSize(char* filePath) {

    FILE* fp = fopen(filePath,"rb");
    if(fp == NULL) {
        LOGE("MainActivity","%s","文件不存在，或没有读文件权限");
    }
    fseek(fp,0,SEEK_END);
    return ftell(fp);
}



/*文件分割*/
JNIEXPORT void JNICALL
Java_com_zeno_encryptanddecrypt_ndk_HelloNDK_fileSplit(JNIEnv *env, jclass type,
                                                       jstring splitFilePath_,
                                                       jstring suffix_,
                                                       jint fileNum) {
    const char *splitFilePath = (*env)->GetStringUTFChars(env, splitFilePath_, 0);
    const char *suffix = (*env)->GetStringUTFChars(env, suffix_, 0);

    // 要分割文件 ， 首先要得到分割文件的路径列表 ,申请动态内存存储路径列表
    char** split_path_list = (char**)malloc(sizeof(char*) * fileNum);

    // 得到文件大小
    long file_size = getFileSize(splitFilePath);

    // 得到路径字符长度
    int file_path_str_len = strlen(splitFilePath);

    // 组合路径
    char file_path[file_path_str_len + 5] ;
    strcpy(file_path,splitFilePath);
    strtok(file_path,".");
    strcat(file_path,"_%d");
    strcat(file_path,suffix);

    int i=0 ;
    for (; i < fileNum; ++i) {

        // 申请单个文件的路径动态内存存储
        split_path_list[i] = (char*)malloc(sizeof(char) * 128);
        // 组合单个路径
        sprintf(split_path_list[i],file_path,(i+1)) ;

        LOGE("MainActivity","%s",split_path_list[i]);
    }



    LOGE("MainActivity","文件大小 == %ld",file_size);
    LOGE("MainActivity","文件路径 == %s",splitFilePath);
    // 读文件
    FILE* fp = fopen(splitFilePath,"rb");
    if(fp == NULL) {
        LOGE("MainActivity","%s","文件不存在，或文件不可读");
        return;
    }

    // 整除 ， 说明各个文件划分大小一致
    if (file_size % fileNum) {
        // 单个文件大小
        int part_file_size = file_size/fileNum ;
        LOGE("MainActivity","单个文件大小 == %d",part_file_size);
        int i = 0 ;
        // 分割多少个文件就分段读多少次
        for (; i < fileNum; i++) {
            // 写文件
            FILE* fwp = fopen(split_path_list[i],"wb");
            if(fwp == NULL) {
                LOGE("MainActivity","%s","没有文件写入权限");
                return;
            }
            int j = 0 ;
            // 单个文件有多大 ， 就读写多少次
            for (; j < part_file_size; j++) {
                // 边读边写
                fputc(fgetc(fp),fwp) ;
            }

            // 关闭文件流
            fclose(fwp);
        }
    }
    /*文件大小不整除*/
    else{
        // 不整除
        int part_file_size = file_size / (fileNum -1 ) ;
        LOGE("MainActivity","单个文件大小 == %d",part_file_size);
        int i = 0 ;
        for (; i < (fileNum - 1); i++) {
            // 写文件
            FILE* fwp = fopen(split_path_list[i],"wb");

            if(fwp == NULL) {
                LOGE("MainActivity","%s","没有文件写入权限") ;
                return;
            }

            int j = 0 ;
            for (; j < part_file_size; j++) {
                // 边读边写
                fputc(fgetc(fp),fwp);
            }

            // 关闭流
            fclose(fwp);
        }

        // 剩余部分
        FILE* last_fwp = fopen(split_path_list[fileNum - 1],"wb") ;
        i= 0 ;
        for (; i < file_size % (fileNum -1); i++) {
                fputc(fgetc(fp),last_fwp) ;
        }

        // 关闭流
        fclose(last_fwp);

    }


    // 关闭文件流
    fclose(fp);

    // 释放动态内存
    i= 0 ;
    for (; i < fileNum ; i++) {
        free(split_path_list[i]) ;
    }

    free(split_path_list);

    (*env)->ReleaseStringUTFChars(env, splitFilePath_, splitFilePath);
    (*env)->ReleaseStringUTFChars(env, suffix_, suffix);
}

/*合并文件*/
JNIEXPORT void JNICALL
Java_com_zeno_encryptanddecrypt_ndk_HelloNDK_fileMerge(JNIEnv *env, jclass type,
                                                       jstring splitFilePath_,
                                                       jstring splitSuffix_,
                                                       jstring mergeSuffix_, jint fileNum) {
    const char *splitFilePath = (*env)->GetStringUTFChars(env, splitFilePath_, 0);
    const char *splitSuffix = (*env)->GetStringUTFChars(env, splitSuffix_, 0);
    const char *mergeSuffix = (*env)->GetStringUTFChars(env, mergeSuffix_, 0);

    // 1. 申请split文件路径列表动态内存
    char** split_path_list = (char**)malloc(sizeof(char*) * fileNum) ;


    // 2. 组装split文件路径
    int split_file_path_len = strlen(splitFilePath) ;
    int split_file_path_suffix_len = strlen(splitSuffix);
    char split_file_path[split_file_path_len + split_file_path_suffix_len] ;
    strcpy(split_file_path,splitFilePath);
    strtok(split_file_path,".");
    strcat(split_file_path,"_%d");
    strcat(split_file_path,splitSuffix);

    // 3. 组装merge文件路径
    int merge_file_path_len = strlen(mergeSuffix);
    char merge_file_path[split_file_path_len + merge_file_path_len] ;
    strcpy(merge_file_path,splitFilePath);
    strtok(merge_file_path,".");
    strcat(merge_file_path,mergeSuffix);

    LOGE("MainActivity","merge 文件路径 = %s",merge_file_path) ;

    // 4. 循环得到split文件路径列表
    int file_path_str_len = strlen(split_file_path);
    int i= 0;
    for (; i < fileNum; i++) {
        split_path_list[i] = (char*)malloc(sizeof(char) * file_path_str_len) ;

        sprintf(split_path_list[i],split_file_path,(i+1)) ;

        LOGE("MainActivity","split文件路径列表 = %s",split_path_list[i]) ;
    }

    // 5. 创建并打开 merge file
    FILE* merge_fwp = fopen(merge_file_path,"wb") ;

    // 6. 边读边写 ， 读多个文件，写入一个文件
    i = 0 ;
    for (; i < fileNum ; i++) {

        FILE* split_frp = fopen(split_path_list[i],"rb") ;
        if(split_frp == NULL) {
            LOGE("MainActivity","%s","文件不存在，或没有读文件权限");
            return;
        }
        long part_split_file_size = getFileSize(split_path_list[i]);
        int j = 0 ;
        for (; j < part_split_file_size; j++) {
            fputc(fgetc(split_frp),merge_fwp);
        }

        // 关闭流
        fclose(split_frp) ;

        // 每合并一个文件 ，就删除它
        remove(split_path_list[i]) ;
    }

    // 关闭文件流
    fclose(merge_fwp);

    // 释放动态内存
    i = 0 ;
    for (; i < fileNum; i++) {
        free(split_path_list[i]) ;
    }

    free(split_path_list);

    LOGE("MainActivity","%s","文件合并完成") ;

    (*env)->ReleaseStringUTFChars(env, splitFilePath_, splitFilePath);
    (*env)->ReleaseStringUTFChars(env, splitSuffix_, splitSuffix);
    (*env)->ReleaseStringUTFChars(env, mergeSuffix_, mergeSuffix);
}