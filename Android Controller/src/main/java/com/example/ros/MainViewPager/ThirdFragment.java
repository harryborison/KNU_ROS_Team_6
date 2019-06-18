package com.example.ros.MainViewPager;

import android.content.Context;
import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Toast;

import com.example.ros.R;
import com.example.ros.RetrofitAPI;
import com.google.gson.JsonObject;

import java.util.HashMap;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class ThirdFragment extends Fragment {

    private RetrofitAPI apiService = null;

    public ThirdFragment() {
        // Required empty public constructor
    }

    public static ThirdFragment newInstance() {

        ThirdFragment fragment = new ThirdFragment();
        Bundle args = new Bundle();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment

        View mLayout = inflater.inflate(R.layout.fragment_third, container, false);

        Button foodButton = mLayout.findViewById(R.id.third_btn_food);
        Button voiceButton = mLayout.findViewById(R.id.third_btn_voice);

        ButtonCLickListener buttonCLickListener = new ButtonCLickListener();

        foodButton.setOnClickListener(buttonCLickListener);
        voiceButton.setOnClickListener(buttonCLickListener);

        return mLayout;
    }

    public class ButtonCLickListener implements View.OnClickListener {

        HashMap<String, String> input = new HashMap<>();

        @Override
        public void onClick(View v) {

            switch (v.getId()) {
                case R.id.third_btn_food:
                    if (apiService == null) {
                        apiService = FirstFragment.apiService;
                        if (apiService == null) {
                            Toast.makeText(getContext(), "Please set IP and port number", Toast.LENGTH_SHORT).show();
                            return;
                        }
                    }

                    input.put("direction", "1");

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
                case R.id.third_btn_voice:
                    if (apiService == null) {
                        apiService = FirstFragment.apiService;
                        if (apiService == null) {
                            Toast.makeText(getContext(), "Please set IP and port number", Toast.LENGTH_SHORT).show();
                            return;
                        }
                    }

                    input.put("direction", "3");

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

}
