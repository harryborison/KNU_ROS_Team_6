package com.example.ros.MainViewPager;


import android.graphics.Color;
import android.graphics.PorterDuff;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.example.ros.R;
import com.google.gson.JsonObject;

import java.util.HashMap;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

import static com.example.ros.MainViewPager.FirstFragment.apiService;

/**
 * A simple {@link Fragment} subclass.
 */
public class SecondFragment extends Fragment {

    private static final String TAG = SecondFragment.class.getSimpleName();
    private static String targetURL = "http://20.20.1.188:8081";
    private Button start, stop;
    private WebView webView;
    private ProgressBar progressBar;


    public SecondFragment() {
        // Required empty public constructor
    }

    public static SecondFragment newInstance() {
        SecondFragment fragment = new SecondFragment();
        Bundle args = new Bundle();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onResume() {
        super.onResume();
        setWebView(FirstFragment.targetURL);
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View mLayout = inflater.inflate(R.layout.fragment_second, container, false);

        start = mLayout.findViewById(R.id.second_btn_auto);
        stop = mLayout.findViewById(R.id.second_btn_hand);
        webView = mLayout.findViewById(R.id.second_webview);
        progressBar = mLayout.findViewById(R.id.second_loading);
        progressBar.setIndeterminate(true);
        progressBar.getIndeterminateDrawable().setColorFilter(Color.parseColor("#88C3FC"), PorterDuff.Mode.MULTIPLY);

        btnClickListener autoClickListener = new btnClickListener();
        start.setOnClickListener(autoClickListener);
        stop.setOnClickListener(autoClickListener);

        return mLayout;
    }

    private void setWebView(String url) {

        progressBar.setVisibility(View.VISIBLE);
        webView.setVisibility(View.GONE);

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
        webView.loadUrl(url);
        Log.d(TAG, "web camera setting finished");
    }


    private class btnClickListener implements View.OnClickListener {

        HashMap<String, String> input = new HashMap<>();

        @Override
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.second_btn_auto:
                    FirstFragment.isAutonomous = true;

                    if (apiService == null) {
                        apiService = apiService;
                        if (apiService == null) {
                            Toast.makeText(getContext(), "Please set IP and port number", Toast.LENGTH_SHORT).show();
                            return;
                        }
                    }

                    input.put("direction", "102");

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

                case R.id.second_btn_hand:
                    FirstFragment.isAutonomous = false;

                    if (apiService == null) {
                        apiService = apiService;
                        if (apiService == null) {
                            Toast.makeText(getContext(), "Please set IP and port number", Toast.LENGTH_SHORT).show();
                            return;
                        }
                    }

                    input.put("direction", "101");

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
