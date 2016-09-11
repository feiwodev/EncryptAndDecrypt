package com.zeno.encryptanddecrypt.ndk;

/**
 * Created by Zeno on 2016/9/10.
 *
 * NDK Demo
 */

public class HelloNDK {

    static {
        System.loadLibrary("HelloNDK");
    }

    public static native String sayHelloNDK() ;

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
}
