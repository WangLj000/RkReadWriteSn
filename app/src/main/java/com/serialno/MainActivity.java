package com.serialno;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    private Button sn_read;
    private Button sn_write;
    private TextView tv_sn;
    private EditText et_input_sn;

    private Button mac_read;
    private Button mac_write;
    private TextView tv_mac;
    private EditText et_input_mac;
    private ReadWriteSerialNo readWriteSerialNo;
    static int VENDOR_SN_ID     =   1;
    static int VENDOR_WIFI_MAC_ID = 2;
    static int VENDOR_LAN_MAC_ID =  3;
    static int VENDOR_BLUETOOTH_ID = 4;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        sn_read = findViewById(R.id.serial_no_read);
        sn_write = findViewById(R.id.serial_no_write);
        tv_sn = findViewById(R.id.sn_review);
        et_input_sn = findViewById(R.id.et_input_sn);

        mac_read = findViewById(R.id.mac_read);
        mac_write = findViewById(R.id.mac_write);
        tv_mac = findViewById(R.id.mac_review);
        et_input_mac = findViewById(R.id.et_input_mac);

        readWriteSerialNo = new ReadWriteSerialNo();


        sn_read.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String sn = readWriteSerialNo.SerialRead(VENDOR_SN_ID);
                tv_sn.setText(getResources().getString(R.string.sn_name) + sn);
            }
        });

        sn_write.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String sn = et_input_sn.getText().toString();
                int ret = readWriteSerialNo.SerialWrite(VENDOR_SN_ID,sn);
                if(ret == 0){
                    Toast.makeText(getApplicationContext(),"Write SN success",Toast.LENGTH_LONG).show();
                }else{
                    Toast.makeText(getApplicationContext(),"Write SN fail",Toast.LENGTH_LONG).show();
                }
            }
        });

        mac_read.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String mac = readWriteSerialNo.SerialRead(VENDOR_LAN_MAC_ID);
                tv_mac.setText(getResources().getString(R.string.eth0_mac_name) + mac);
            }
        });

        mac_write.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String mac = et_input_mac.getText().toString();
                int ret = readWriteSerialNo.SerialWrite(VENDOR_LAN_MAC_ID,mac);
                if(ret == 0){
                    Toast.makeText(getApplicationContext(),"Write Mac success",Toast.LENGTH_LONG).show();
                }else{
                    Toast.makeText(getApplicationContext(),"Write Mac fail",Toast.LENGTH_LONG).show();
                }
            }
        });
    }

}
