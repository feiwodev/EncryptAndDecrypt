# EncryptAndDecrypt
EncryptAndDecrypt , 文件加密解密与分割合并

接续上篇[NDK开发基础①使用Android Studio编写NDK](http://www.jianshu.com/p/f1b8b97d2ef8)

### 前情提要
随着Android Studio的越来越完善 ， 我们编写NDK就会越来越方便，使用Android Studio 2.2 RC2 ， 编写NDK的时候 ， 不需要使用`javah`命令来生成头文件 ，  创建一个`native`方法 ， 使用`alt + enter`会提示要你创建一个JNI函数 ， C/C++语法提示也相对比较完善了， 减少了一些重复代码的编写，相信使用Android Studio 2.2编写NDK， 不会让你失望 。

###Part 1 ， -- 文件的加密解密
在[C语言基础系列](http://www.jianshu.com/p/4701cd1e1914)第八章[文件IO](http://www.jianshu.com/p/eb5d58800a63)，介绍了文件的加密解密 ， 开发工具使用的是VS ， 在上篇[NDK开发基础①使用Android Studio编写NDK](http://www.jianshu.com/p/f1b8b97d2ef8)简单介绍了Android Studio来搭建NDK开发环境，今天我们使用Android Studio来玩一把 。

> 界面 与Project目录结构

![UI](http://upload-images.jianshu.io/upload_images/643851-788b88eea0671dba.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
就几个按钮 ， 布局文件就不贴代码了 ， 读者也可以整个输入框，可以输入加密的密码 。


![project directory](http://upload-images.jianshu.io/upload_images/643851-d44584946c363713.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

从目录上可以看出`ndk`包下面的 ， 就是一个`native`方法的类， `cpp`目录下的就是JNI `C`文件 ， CMakeLists.txt配置我们的编译流程，以及生成库与添加库的配置。

> Java native code

```java
    /**
     * 带密码的文件加密
     * @param normalFilePath 要加密的文件路径
     * @param encryptFilePath 加密之后的文件路径
     * @param password 加密密码
     */
    public static native void fileEncrypt(String normalFilePath,String encryptFilePath,String password) ;

    /**
     * 带密码的文件解密
     * @param encryptFilePath 要解密的文件路径
     * @param encryptFilePath 解密之后的文件路径
     * @param password 加密密码
     */
    public static native void fileDecrypt(String encryptFilePath,String decryptFilePath,String password) ;
```
简要提示：文件的加密解密 ， 实质上在复制文件的时候进行`^`运算，在计算机中 ， 所有的文件都是以二进制存储的 ， 所有可以进行运算来进行文件的加密解密 ， 当然 ， 密码的算法是可逆的 ， 也就是可解密的 。

> use java native code

```java
    // SD_CARD 根路径
    private static final String SD_CARD_PATH = Environment.getExternalStorageDirectory().getAbsolutePath() ;

    /*文件加密*/
    private void fileEncrypt() {
        String normalFilePath = SD_CARD_PATH+ File.separatorChar+"neck.jpg" ;
        String encryptFilePath = SD_CARD_PATH+File.separatorChar+"neck_encrypt.jpg" ;
        Log.e(TAG, "fileEncrypt: normalFilePath = "+ normalFilePath + " encryptFilePath = "+encryptFilePath);

        try{
            HelloNDK.fileEncrypt(normalFilePath,encryptFilePath,"xj");
        }catch (Exception e) {
            e.printStackTrace();
        }

    }

    /*文件解密*/
    private void fileDecrypt() {

        String encryptFilePath = SD_CARD_PATH+File.separatorChar+"neck_encrypt.jpg" ;
        String decryptFilePath = SD_CARD_PATH+File.separatorChar+"neck_decrypt.jpg" ;

        try{
            HelloNDK.fileDecrypt(encryptFilePath,decryptFilePath,"xj");
        }catch (Exception e) {
            e.printStackTrace();
        }
    }
```

> 文件加密解密实现

```c
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
```
简要提示：加密解密算法，在[C语言：文件IO](http://www.jianshu.com/p/eb5d58800a63)里面就介绍了 ， 这里就不加赘述了，加密与解密共用的是一套算法， 解密的就不贴代码了 。在使用`*password`指针的时候需要注意，`password+1`可以得到下一个字符，但是`password+0`的时候不会得到第一个字符，会输出整个字符指针，所有在使用字符指针的时候一个一个取字符 ， 需要使用`*(password+0) ,*(password+1) `取第一个第二字符 。如果您对C语言以及JNI开发都不了解的话 ， 可以关注的我的专题[Android NDK开发之旅](http://www.jianshu.com/collection/a25bf14495d7)

### Part 2 ， 文件分割与合并

__使用场景：__大文件上传 ， 如视频文件 ， 进行文件拆分 ， 分批上传 。

> Java native code

```java
	/**
     * 文件分割
     * @param splitFilePath 要分割文件的路径
     * @param suffix 分割文件拓展名
     * @param fileNum 分割文件的数量
     */
    public static native void fileSplit(String splitFilePath,String suffix,int fileNum);

    /**
     * 文件合并
     * @param splitFilePath 分割文件的路径
     * @param splitSuffix 分割文件拓展名
     * @param mergeSuffix 合并文件的拓展名
     * @param fileNum 分割文件的数量
     */
    public static native void fileMerge(String splitFilePath,String splitSuffix,String mergeSuffix,int fileNum);
```
> use java native code

```java
    /*分割文件*/
    private void fileSplit() {
        String splitFilePath = SD_CARD_PATH+File.separatorChar+"neck.jpg" ;
        String suffix = ".b";
        try{
            HelloNDK.fileSplit(splitFilePath,suffix,3);
        }catch (Exception e) {
            e.printStackTrace();
        }

    }

    /*文件合并*/
    private void fileMerge() {
        String splitFilePath = SD_CARD_PATH+File.separatorChar+"neck.jpg" ;
        String splitSuffix = ".b";
        String mergeSuffix = ".jpeg";
        try{
            HelloNDK.fileMerge(splitFilePath,splitSuffix,mergeSuffix,3);
        }catch (Exception e) {
            e.printStackTrace();
        }
    }
```

> 文件分割实现

算法分析图：

![file split](http://upload-images.jianshu.io/upload_images/643851-3348bb0d653afe62.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

伪代码：
```
// 平均分配文件大小
if(fileSize % fileNum) {
	int i= 0 ; 
	for(; i < fileNum ; i++) {
		// 创建分割文件
		int j= 0 ;
		for(;j < partFileSize ; j++) {
			// 读写数据
		}
		
	}
}
// 不可平均分配大小
else {
	// 前面的文件平分文件大小 ， 将剩下的大小全部给最后一个文件
	if(fileSize % (fileNum-1)){
		int i= 0 ; 
		for(; i < (fileNum-1) ; i++) {
			// 创建分割文件
			int j= 0 ;
			for(;j < partFileSize ; j++) {
				// 读写数据
			}
	}
	
	}else {
		// 将剩余大小fileSize % (fileNum-1)写入到最后一个文件中 
		int j = 0 ;
		for(;j < fileSize % (fileNum-1)){
			// 读写数据
		}
	
	}
}
```
文件分割算法就分析到这里 ， 下面我们来看看具体的实现：

> java native code

```java
    /*分割文件*/
    private void fileSplit() {
        String splitFilePath = SD_CARD_PATH+File.separatorChar+"neck.jpg" ;
        String suffix = ".b";
        try{
            HelloNDK.fileSplit(splitFilePath,suffix,3);
        }catch (Exception e) {
            e.printStackTrace();
        }

    }

    /*文件合并*/
    private void fileMerge() {
        String splitFilePath = SD_CARD_PATH+File.separatorChar+"neck.jpg" ;
        String splitSuffix = ".b";
        String mergeSuffix = ".jpeg";
        try{
            HelloNDK.fileMerge(splitFilePath,splitSuffix,mergeSuffix,3);
        }catch (Exception e) {
            e.printStackTrace();
        }
    }
```

> 具体实现：

```c
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
        // 组合分割的单个文件路径
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
```
简要提示：组合路径的时候，使用了`strtok`来进行文件路径的分割，将`.jpg`去掉，用`strcat`拼接了`_%d`，最后拼接了传入的文件拓展名，这样设计是为了让分割的文件 ， 不那么见名知意 。
`LOGE`定义的一个预处理函数，在C语言基础系列[C语言基础及指针⑩预编译及jni.h分析](http://www.jianshu.com/p/569f968bcdce)有着详细说明。

```c
#define LOGE(TAG,FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,TAG,FORMAT,__VA_ARGS__)
```

> 文件合并

文件的分割 ， 是件一个文件分割成多个子文件 。文件的合并 ， 则是将多个子文件合并成一个文件 。

算法分析图：

![merge file](http://upload-images.jianshu.io/upload_images/643851-fa3ebc68a92c3958.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

> 具体实现

```c


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
```
简要提示：在编写文件分割与合并的时候 ， 文件的路径千万不要弄混了，笔者有有时候也会因为变量的提示没细看 ， 而将分割的子文件路径与目标文件路径弄混 ，在命名的时候 ， 尽量规范 ， 见名知意 。在发生错误的时候 ， 一步一步的调试 ， 首先猜测哪里发生异常的可能性比较大 ， 再进行log输出查看 。

### 结语
文件的加密解密 ， 可以引申为对数据的加密解密 ， 可以使用C/C++进行加密解密， 这样对反编译APK增加了难度 ， 也多了一层防范 ， 因为java实在是太容易反编译了， 即使不反编译成java代码， 通过smail文件也可以看个大概 ， 还是很危险的 。

文件的分割与合并 ， 对文件数据传输有很大益处 。

> 如果想了解更多关于Android NDK的知识 ， 欢迎关注我的专题[Android NDK 开发之旅](http://www.jianshu.com/collection/a25bf14495d7) ， 从零开始 ， 带你一步一步进入NDK开发的世界 。

### 参考
[Standard C 语言标准函数库速查](http://ganquan.info/standard-c/function/tmpnam)
