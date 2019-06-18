package com.example.ros.MainViewPager;


import android.content.Intent;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebResourceRequest;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.example.ros.MainActivity;
import com.example.ros.R;
import com.example.ros.RetrofitAPI;
import com.example.ros.SettingActivity;
import com.google.gson.JsonObject;

import java.util.HashMap;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;


/**
 * A simple {@link Fragment} subclass.
 */
public class FirstFragment extends Fragment {

    private ImageButton arrow_up, arrow_down, arrow_right, arrow_left;
    private ProgressBar progressBar;
    private Retrofit mRetrofit;
    public static RetrofitAPI apiService = null;
    private static final String TAG = FirstFragment.class.getSimpleName();
    public static boolean isAutonomous = false;


    public static String CONNECTION_IP="20.20.1.58", CONNECTION_PORT="5000";
    public static String targetURL = "http://20.20.1.188:8081";

    public FirstFragment() {
        // Required empty public constructor
    }

    public static FirstFragment newInstance() {

        FirstFragment fragment = new FirstFragment();
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

        View mLayout = inflater.inflate(R.layout.fragment_first, container, false);
        arrow_up = mLayout.findViewById(R.id.main_btn_up);
        arrow_down = mLayout.findViewById(R.id.main_btn_down);
        arrow_left = mLayout.findViewById(R.id.main_btn_left);
        arrow_right = mLayout.findViewById(R.id.main_btn_right);
        WebView webView = mLayout.findViewById(R.id.main_webView);
        progressBar = mLayout.findViewById(R.id.main_loading);
        progressBar.setIndeterminate(true);
        progressBar.getIndeterminateDrawable().setColorFilter(Color.parseColor("#88C3FC"), PorterDuff.Mode.MULTIPLY);

        checkURL();
        setRetroInit();

        ArrowClickListener arrowClickListener = new ArrowClickListener();

        arrow_up.setOnClickListener(arrowClickListener);
        arrow_down.setOnClickListener(arrowClickListener);
        arrow_left.setOnClickListener(arrowClickListener);
        arrow_right.setOnClickListener(arrowClickListener);


        setWebView(webView, targetURL);


        return mLayout;
    }

    private void checkURL() {

        Intent intent = new Intent(getActivity(), SettingActivity.class);

        if (CONNECTION_PORT.equals("") || CONNECTION_IP.equals("") || targetURL.equals("")) {
            startActivity(intent);
        }

    }

    private void setWebView(final WebView webView, String targetURL) {

        if (targetURL.isEmpty()) {
            Intent intent = new Intent(getActivity(), SettingActivity.class);
            startActivity(intent);
        }

        webView.setWebViewClient(new WebViewClient() {

            @Override
            public void onPageFinished(WebView view, String url) {
                super.onPageFinished(view, url);

                progressBar.setVisibility(View.GONE);
                webView.setVisibility(View.VISIBLE);
            }
        });
        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webView.loadUrl(targetURL);
        Log.d(TAG, "web camera setting finished");
    }


    private void setRetroInit() {

        Log.d(TAG, "setRetroInit : " + CONNECTION_IP + ":" + CONNECTION_PORT);

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

            HashMap<String, String> input = new HashMap<>();

            if (isAutonomous) {
                Toast.makeText(getContext(), "You can't use on Autonomous mode", Toast.LENGTH_SHORT).show();
                return;
            }

            if (apiService == null) {
                Toast.makeText(getContext(), "ApiService is null", Toast.LENGTH_SHORT).show();
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


}
