package com.example.test1;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.app.AlertDialog;
import java.io.*;
import android.os.*;
import okhttp3.*;
import android.widget.Button;
import android.content.res.ColorStateList;
public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivityLog";
    private int ins=1;
    private String uid="****";
    private String topic="ESP32HomeRFLight";

    @SuppressLint("HandlerLeak")
    Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 0:
                    MessageBox("成功","发送信号成功");
                    break;
                case -1:
                    MessageBox("错误","发送信号失败");

                    break;
                case -2:
                    ins=0;
                    MessageBox("网络连接失败","请检查网络");

                    break;
                case 2:
                    Set_Button_EN(false);
                    break;
                case 3:
                    Set_Button_EN(true);
                    break;

                case 4:
                    Button lookdoc ;
                    lookdoc = findViewById(R.id.button);
                    lookdoc.setBackgroundTintList(ColorStateList.valueOf(0xffff0000));
                    lookdoc.setText("已连接");
                    Set_Button_EN(true);

                    Log.d(TAG,"4");
                    break;
                case 5:
                    lookdoc = findViewById(R.id.button);
                    lookdoc.setBackgroundTintList(ColorStateList.valueOf(0xff000000));
                    lookdoc.setText("未连接");
                    Set_Button_EN(false);
                    Log.d(TAG,"5");
                    break;
                default:
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        new Thread(new Runnable() {
            public void run() {
                while (ins!=0) {
                    try {
                        bemfa_Get_Online();
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }).start();



        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


    }

    public void Set_Button_EN(boolean zt)
    {
        Button lookdoc ;
        lookdoc = findViewById(R.id.H_ON);
        lookdoc.setEnabled(zt);
        lookdoc = findViewById(R.id.H_OFF);
        lookdoc.setEnabled(zt);
        lookdoc = findViewById(R.id.S_ON);
        lookdoc.setEnabled(zt);
        lookdoc = findViewById(R.id.S_OFF);
        lookdoc.setEnabled(zt);
        lookdoc = findViewById(R.id.N_NO);
        lookdoc.setEnabled(zt);
        lookdoc = findViewById(R.id.N_OFF);
        lookdoc.setEnabled(zt);
        lookdoc = findViewById(R.id.ALL_NO);
        lookdoc.setEnabled(zt);
        lookdoc = findViewById(R.id.ALL_OFF);
        lookdoc.setEnabled(zt);
        lookdoc = findViewById(R.id.W_NO);
        lookdoc.setEnabled(zt);
        lookdoc = findViewById(R.id.W_OFF);
        lookdoc.setEnabled(zt);
    }

    public void MessageBox(String tital,String msg)
    {
        new AlertDialog.Builder(this)
                .setTitle(tital)
                .setMessage(msg)
                .setPositiveButton("确定", null)
                .show();


    }

    public void bemfa_Send_msg(String data)
    {

        String url="https://apis.bemfa.com/va/postmsg";
        OkHttpClient okHttpClient = new OkHttpClient();
        FormBody formBody = new FormBody.Builder()
                .add("uid",uid)
                .add("topic",topic)
                .add("type","1")
                .add("msg",data)
                .build();
        Request request = new Request.Builder()
                .url(url)
                .post(formBody)
                .build();
        Call call=okHttpClient.newCall(request);
        call.enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                mHandler.sendEmptyMessage(-2);
                mHandler.sendEmptyMessage(3);//开按钮
            }
            @Override
            public void onResponse(Call call, Response response) throws IOException {
                String str =response.body().string();
                if(str.equals("{\"code\":0,\"message\":\"OK\",\"data\":0}"))
                {
                    mHandler.sendEmptyMessage(0);
                    mHandler.sendEmptyMessage(3);//开按钮
                }
                else
                {
                    mHandler.sendEmptyMessage(-1);
                    mHandler.sendEmptyMessage(3);//开按钮
                }
            }
        });
    }
    public void bemfa_Get_Online()
    {
        Log.d(TAG,"1");
        OkHttpClient  client = new OkHttpClient();
        Request request = new Request.Builder()
                .url("https://apis.bemfa.com/va/online?uid="+uid+"&&topic="+topic+"&&type=1")   //设置目标网络地址
			    .get()    //默认为GET请求，可以省略
                .build();
        Call  call = client.newCall(request);
        //通过Call对象的enqueue(Callback)方法来提交异步请求，异步发起的请求会被加入到队列中通过线程池来执行。最后通过接口回调的onResponse()方法来接收服务器返回的数据。
        call.enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
//                Log.d(TAG, "onFailure: ");
                mHandler.sendEmptyMessage(-2);
                mHandler.sendEmptyMessage(5);
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                String  responseData = response.body().string();
                Log.d(TAG, "onResponse: " + responseData);

                if(responseData.equals("{\"code\":0,\"message\":\"OK\",\"data\":true}"))
                    mHandler.sendEmptyMessage(4);
                else
                    mHandler.sendEmptyMessage(5);
            }

        });


    }
    public void zt_ButtonCallBack(View view) {
        ins=1;
        bemfa_Get_Online();
    }
    public void S_ON_ButtonCallBack(View view) {
        mHandler.sendEmptyMessage(2);//关按钮
        bemfa_Send_msg("S_ON");
    }
    public void S_OFF_ButtonCallBack(View view) {
        mHandler.sendEmptyMessage(2);//关按钮
        bemfa_Send_msg("S_OFF");
    }
    public void N_ON_ButtonCallBack(View view) {
        mHandler.sendEmptyMessage(2);//关按钮
        bemfa_Send_msg("N_ON");
    }
    public void N_OFF_ButtonCallBack(View view) {
        mHandler.sendEmptyMessage(2);//关按钮
        bemfa_Send_msg("N_OFF");
    }
    public void H_ON_ButtonCallBack(View view) {
        mHandler.sendEmptyMessage(2);//关按钮
        bemfa_Send_msg("H_ON");
    }
    public void H_OFF_ButtonCallBack(View view) {
        mHandler.sendEmptyMessage(2);//关按钮
        bemfa_Send_msg("H_OFF");
    }
    public void ALL_ON_ButtonCallBack(View view) {
        mHandler.sendEmptyMessage(2);//关按钮
        bemfa_Send_msg("ALL_ON");
    }
    public void ALL_OFF_ButtonCallBack(View view) {
        mHandler.sendEmptyMessage(2);//关按钮
        bemfa_Send_msg("ALL_OFF");
    }
    public void W_ON_ButtonCallBack(View view) {
        mHandler.sendEmptyMessage(2);//关按钮
        bemfa_Send_msg("W_ON");
    }
    public void W_OFF_ButtonCallBack(View view) {
        mHandler.sendEmptyMessage(2);//关按钮
        bemfa_Send_msg("W_OFF");
    }
}