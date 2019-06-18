package com.example.ros;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.example.ros.MainViewPager.FirstFragment;

public class SettingActivity extends AppCompatActivity {

    private Button button_set, button_cancel;
    private EditText edit_ip, edit_port, edit_url;
    private boolean isEditable = true;
    public static final String TAG = SettingActivity.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);


        button_set = findViewById(R.id.main_btn_set);
        button_cancel = findViewById(R.id.main_btn_cancel);
        edit_ip = findViewById(R.id.main_edit_ip);
        edit_port = findViewById(R.id.main_edit_port);
        edit_url = findViewById(R.id.main_edit_url);

        modeCheck();

        ManageButtonClickListener manageButtonClickListener = new ManageButtonClickListener();

        button_cancel.setOnClickListener(manageButtonClickListener);
        button_set.setOnClickListener(manageButtonClickListener);

    }

    @Override
    public void finish() {
        super.finish();
    }

    public void modeCheck() {
        if (isEditable) {

            edit_port.setVisibility(View.VISIBLE);
            edit_ip.setVisibility(View.VISIBLE);
            edit_url.setVisibility(View.VISIBLE);

            edit_ip.setText(FirstFragment.CONNECTION_IP);
            edit_port.setText(FirstFragment.CONNECTION_PORT);
            edit_url.setText(FirstFragment.targetURL);
            button_set.setText("set");
        } else {
            edit_port.setVisibility(View.GONE);
            edit_ip.setVisibility(View.GONE);
            edit_url.setVisibility(View.GONE);

            edit_ip.setText(FirstFragment.CONNECTION_IP);
            edit_port.setText(FirstFragment.CONNECTION_PORT);
            edit_url.setText(FirstFragment.targetURL);

            button_set.setText("edit");
        }
    }

    public class ManageButtonClickListener implements View.OnClickListener {
        public ManageButtonClickListener() {
        }


        @Override
        public void onClick(View v) {
            switch (v.getId()) {

                case R.id.main_btn_set:

                    if (isEditable) {
                        FirstFragment.CONNECTION_IP = edit_ip.getText().toString();
                        FirstFragment.CONNECTION_PORT = edit_port.getText().toString();
                        FirstFragment.targetURL = edit_url.getText().toString();
                        Toast.makeText(SettingActivity.this, FirstFragment.CONNECTION_IP + "," + FirstFragment.CONNECTION_PORT + "," + FirstFragment.targetURL, Toast.LENGTH_SHORT).show();

                        SettingActivity.this.finish();
                    }

                    isEditable = !isEditable;
                    modeCheck();

                    break;

                case R.id.main_btn_cancel:

                    SettingActivity.this.finish();
                    break;
            }
        }
    }
}
