package com.serialno;

import android.util.Log;

public class ReadWriteSerialNo {

    // Load library
    static {
        System.loadLibrary("serial_no");
    }

//    public String SerialRead() {
//        String sn = read(type);
//        Log.d("serial_no", "java sn == " + sn);
//        return sn;
//    }

    public String SerialRead(int type) {
        String sn = read(type);
        Log.d("serial_no", "java sn == " + sn);
        return sn;
    }

    public int SerialWrite(int type,String sn) {
        return write(type,sn);
    }

    // JNI
    public native String read(int type);

    public native int write(int type ,String s);

    public native String readMac();

    public native int writeMac(String s);

}
