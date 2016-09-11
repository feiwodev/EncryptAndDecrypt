package com.zeno.encryptanddecrypt;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.zeno.encryptanddecrypt.ndk.HelloNDK;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getSimpleName() ;

    // SD_CARD 根路径
    private static final String SD_CARD_PATH = Environment.getExternalStorageDirectory().getAbsolutePath() ;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(HelloNDK.sayHelloNDK());


    }

    public void onClick(View view) {
        switch (view.getId()) {

            /*文件加密*/
            case R.id.btn_file_encrypt:
                fileEncrypt();
                break;

            /*文件解密*/
            case R.id.btn_file_decrypt:
                fileDecrypt();
                break;

            /*文件分割*/
            case R.id.btn_file_split:
                fileSplit();
                break;
            /*文件合并*/
            case R.id.btn_file_merge:
                fileMerge();
                break;
        }
    }

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
}
