package com.example.ros;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.example.ros.MainViewPager.FirstFragment;

public class SettingActivity extends Activity {

    private Button button_set, button_cancel;
    private EditText edit_ip, edit_port;
    public static final String TAG = SettingActivity.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);


        button_set = findViewById(R.id.main_btn_set);
        button_cancel = findViewById(R.id.main_btn_cancel);
        edit_ip = findViewById(R.id.main_edit_ip);
        edit_port = findViewById(R.id.main_edit_port);

        ManageButtonClickListener manageButtonClickListener = new ManageButtonClickListener();

        button_cancel.setOnClickListener(manageButtonClickListener);
        button_set.setOnClickListener(manageButtonClickListener);
    }

    public class ManageButtonClickListener implements View.OnClickListener {
        public ManageButtonClickListener() {
        }


        @Override
        public void onClick(View v) {
            switch (v.getId()) {

                case R.id.main_btn_set:
                    Log.d(TAG, "edit_ip : " + edit_ip.getText().toString());
                    Log.d(TAG, "edit_ip : " + edit_port.getText().toString());


                    FirstFragment.CONNECTION_IP = edit_ip.getText().toString();
                    FirstFragment.CONNECTION_PORT = edit_port.getText().toString();
                    edit_ip.setEnabled(false);
                    edit_port.setEnabled(false);


                    button_set.setVisibility(View.GONE);
                    edit_ip.setVisibility(View.GONE);
                    edit_port.setVisibility(View.GONE);

                    break;

                case R.id.main_btn_cancel:
                    edit_ip.setVisibility(View.VISIBLE);
                    edit_port.setVisibility(View.VISIBLE);
                    edit_port.setEnabled(true);
                    edit_ip.setEnabled(true);
                    button_set.setVisibility(View.VISIBLE);
                    break;
            }
        }
    }
}
