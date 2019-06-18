package com.example.ros.MainViewPager;


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
import android.widget.Toast;

import com.example.ros.R;

/**
 * A simple {@link Fragment} subclass.
 */
public class SecondFragment extends Fragment {

    private static final String TAG = SecondFragment.class.getSimpleName();
    private static String targetURL = "http://20.20.1.188:8081";
    private Button start, stop;
    private WebView webView;


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

        btnClickListener autoClickListener = new btnClickListener();
        start.setOnClickListener(autoClickListener);
        stop.setOnClickListener(autoClickListener);

        return mLayout;
    }

    private void setWebView(String url) {
        webView.setVisibility(View.VISIBLE);
        webView.setWebViewClient(new WebViewClient());
        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webView.loadUrl(url);
        Log.d(TAG, "web camera setting finished");
    }

    private void stopWebView() {
        webView.stopLoading();
        webView.setVisibility(View.INVISIBLE);
    }

    private class btnClickListener implements View.OnClickListener {

        @Override
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.second_btn_auto:
                    FirstFragment.isAutonomous = true;
                    Toast.makeText(getContext(), "autonomous", Toast.LENGTH_SHORT).show();
                    setWebView(targetURL);
                    break;
                case R.id.second_btn_hand:
                    FirstFragment.isAutonomous = false;
                    Toast.makeText(getContext(), "hand operating", Toast.LENGTH_SHORT).show();
                    stopWebView();
                    break;
            }
        }
    }

}
