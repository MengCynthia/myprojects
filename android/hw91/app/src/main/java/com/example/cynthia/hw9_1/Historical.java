package com.example.cynthia.hw9_1;

/**
 * Created by cynthia on 2017/11/23.
 */

import android.os.AsyncTask;
import android.support.design.widget.TabLayout;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONObject;
import org.json.JSONTokener;


public class Historical extends Fragment {
    static final String API_URL_symbol = "http://shencsci571-php.us-west-1.elasticbeanstalk.com/123/";
    JSONObject localdata;
    String symbol;
    ProgressBar progressBar;
    TextView test;
    JSONObject queryData;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.historical, container, false);
        //Bundle bundle = this.getActivity().getIntent().getExtras();
        progressBar = (ProgressBar) rootView.findViewById(R.id.progressBar3);
        test = (TextView) rootView.findViewById(R.id.t2);

        Querydata app = (Querydata) getActivity().getApplication();
        progressBar.setVisibility(View.VISIBLE);
        final WebView webview = (WebView) rootView.findViewById(R.id.web2);

        WebSettings webSetting = webview.getSettings();
        webSetting.setJavaScriptEnabled(true);
        webSetting.setBuiltInZoomControls(true);
        webSetting.setLayoutAlgorithm(WebSettings.LayoutAlgorithm.NARROW_COLUMNS);
       // webSetting.setUseWideViewPort(true);
        //webSetting.setLoadWithOverviewMode(true);
        webSetting.setSaveFormData(true);
        webSetting.setJavaScriptEnabled(true);


        if(app.get("querydata")!= null && app.get("querydata").has("price")) {

            queryData = app.get("querydata");
            System.out.println("3:" + queryData);
            if (!queryData.optJSONObject("price").has("Time Series (Daily)")) {
                TextView error = (TextView) rootView.findViewById(R.id.error2);
                error.setVisibility(View.VISIBLE);
                progressBar.setVisibility(View.GONE);
            } else {
                webview.setWebChromeClient(new WebChromeClient() {
                });
                webview.setWebViewClient(new WebViewClient() {

                    @Override
                    public void onPageFinished(WebView view, String url) {
                        super.onPageFinished(view, url);
                       // progressBar.setVisibility(View.GONE);
                        testMethod(webview);
                    }

                });
                webview.loadUrl("file:///android_asset/stock.html");


            }
        }
        return rootView;
    }

    private void testMethod(WebView webview) {

        String data = queryData.optJSONObject("price").toString();
        String call = "javascript:jsshowstock("+data+")";

        webview.evaluateJavascript("jsshowstock(" + data + ")", new ValueCallback<String>() {
            @Override
            public void onReceiveValue(String s) {
                progressBar.setVisibility(View.GONE);
            }
        });

       // webview.loadUrl(call);

    }
}
