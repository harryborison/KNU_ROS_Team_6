package com.example.ros;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.google.gson.JsonObject;

import java.util.HashMap;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class MainActivity extends AppCompatActivity {


    private Button arrow_up, arrow_down, arrow_right, arrow_left, button_set, button_edit;
    private EditText edit_ip, edit_port;
    private static String CONNECTION_IP, CONNECTION_PORT;
    private Retrofit mRetrofit;
    private RetrofitAPI apiService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        arrow_up = findViewById(R.id.main_btn_up);
        arrow_down = findViewById(R.id.main_btn_down);
        arrow_left = findViewById(R.id.main_btn_left);
        arrow_right = findViewById(R.id.main_btn_right);
        button_set = findViewById(R.id.main_btn_set);
        button_edit = findViewById(R.id.main_btn_edit);
        edit_ip = findViewById(R.id.main_edit_ip);
        edit_port = findViewById(R.id.main_edit_port);

        ArrowClickListener arrowClickListener = new ArrowClickListener();
        ManageButtonClickListener manageButtonClickListener = new ManageButtonClickListener();


        arrow_up.setOnClickListener(arrowClickListener);
        arrow_down.setOnClickListener(arrowClickListener);
        arrow_left.setOnClickListener(arrowClickListener);
        arrow_right.setOnClickListener(arrowClickListener);

        button_edit.setOnClickListener(manageButtonClickListener);
        button_set.setOnClickListener(manageButtonClickListener);

    }

    private void setRetroInit() {
        mRetrofit = new Retrofit.Builder()
                .baseUrl("http://" + CONNECTION_IP + ":" + CONNECTION_PORT)
                .addConverterFactory(GsonConverterFactory.create())
                .build();
        apiService = mRetrofit.create(RetrofitAPI.class);
    }


    public class ArrowClickListener implements View.OnClickListener {


        public ArrowClickListener() {


        }

        @Override
        public void onClick(View v) {

            HashMap<String, String> input = new HashMap<>();;

            if (apiService == null) {
                return;
            }

            switch (v.getId()) {
                case R.id.main_btn_up:

                    input.put("direction", "8");

                    apiService.sendDirection(input).enqueue(new Callback<JsonObject>() {
                        @Override
                        public void onResponse(Call<JsonObject> call, Response<JsonObject> response) {
                            if (response.isSuccessful()) {
                                JsonObject body = response.body();
                                if (body != null) {
                                    Log.e("postData end", "===========");
                                }
                            }
                        }

                        @Override
                        public void onFailure(Call<JsonObject> call, Throwable t) {
                            Log.e("postData error", t.getMessage());
                        }
                    });
                    break;
                case R.id.main_btn_down:


                    input.put("direction", "2");

                    apiService.sendDirection(input).enqueue(new Callback<JsonObject>() {
                        @Override
                        public void onResponse(Call<JsonObject> call, Response<JsonObject> response) {
                            if (response.isSuccessful()) {
                                JsonObject body = response.body();
                                if (body != null) {
                                    Log.e("postData end", "===========");
                                }
                            }
                        }

                        @Override
                        public void onFailure(Call<JsonObject> call, Throwable t) {
                            Log.e("postData error", t.getMessage());
                        }
                    });
                    break;

                case R.id.main_btn_left:

                    input.put("direction", "4");

                    apiService.sendDirection(input).enqueue(new Callback<JsonObject>() {
                        @Override
                        public void onResponse(Call<JsonObject> call, Response<JsonObject> response) {
                            if (response.isSuccessful()) {
                                JsonObject body = response.body();
                                if (body != null) {
                                    Log.e("postData end", "===========");
                                }
                            }
                        }

                        @Override
                        public void onFailure(Call<JsonObject> call, Throwable t) {
                            Log.e("postData error", t.getMessage());
                        }
                    });
                    break;

                case R.id.main_btn_right:

                    input.put("direction", "6");

                    apiService.sendDirection(input).enqueue(new Callback<JsonObject>() {
                        @Override
                        public void onResponse(Call<JsonObject> call, Response<JsonObject> response) {
                            if (response.isSuccessful()) {
                                JsonObject body = response.body();
                                if (body != null) {
                                    Log.e("postData end", "===========");
                                }
                            }
                        }

                        @Override
                        public void onFailure(Call<JsonObject> call, Throwable t) {
                            Log.e("postData error", t.getMessage());
                        }
                    });
                    break;
            }
        }
    }

    public class ManageButtonClickListener implements View.OnClickListener {
        public ManageButtonClickListener() {
        }


        @Override
        public void onClick(View v) {
            switch (v.getId()) {

                case R.id.main_btn_set:
                    CONNECTION_IP = edit_ip.getText().toString();
                    CONNECTION_PORT = edit_port.getText().toString();
                    edit_ip.setEnabled(false);
                    edit_port.setEnabled(false);
                    setRetroInit();
                    Toast.makeText(getApplicationContext(), CONNECTION_IP + " " + CONNECTION_PORT, Toast.LENGTH_SHORT).show();
                    break;

                case R.id.main_btn_edit:
                    edit_port.setEnabled(true);
                    edit_ip.setEnabled(true);
                    break;
            }
        }
    }
}
