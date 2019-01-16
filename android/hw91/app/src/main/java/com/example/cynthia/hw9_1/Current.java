package com.example.cynthia.hw9_1;

/**
 * Created by cynthia on 2017/11/23.
 */

import android.net.Uri;
import android.support.v4.app.Fragment;
import com.facebook.FacebookSdk;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.widget.ShareDialog;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.util.Log;

import android.webkit.ValueCallback;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.SimpleAdapter;
import android.widget.TableLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.Spinner;
import android.widget.ArrayAdapter;
import android.webkit.WebView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;
import java.util.List;
import java.util.Map;
import java.util.regex.*;
import android.webkit.JavascriptInterface;

import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.annotation.SuppressLint;

public class Current extends Fragment {
    static final String API_URL_symbol = "http://shencsci571-php.us-west-1.elasticbeanstalk.com/123/";
    JSONObject localdata;
    String symbol;
    ProgressBar progressBar;
    ProgressBar progressBar1;
    private TextView mTextView;
    JSONObject queryData;
    JSONObject Price;
    TextView test;
    ImageButton star;
    String indicator;
    ListDataSave dataSave;
    ImageButton fbshare;
    String resurl;
    ShareDialog shareDialog;
    @SuppressLint("JavascriptInterface")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.current, container, false);

        final Querydata app = (Querydata) getActivity().getApplication();
        dataSave = new ListDataSave(getActivity(),"fav");
        System.out.println("datasave2"+dataSave.getDataList("favlist").optJSONObject("nameValuePairs").toString());
        //final JSONObject fav1 = dataSave.getDataList("favlist").optJSONObject("nameValuePairs");


        progressBar = (ProgressBar) rootView.findViewById(R.id.progressBar2);
        progressBar1 = (ProgressBar) rootView.findViewById(R.id.progressBar5);
        test = (TextView) rootView.findViewById(R.id.test);
        star = (ImageButton) rootView.findViewById(R.id.star);
        fbshare = (ImageButton) rootView.findViewById(R.id.fbshare);
        fbshare.setClickable(false);
        star.setClickable(false);


        final WebView webview = (WebView) rootView.findViewById(R.id.web);

        ListView stocklist = (ListView) rootView.findViewById(R.id.stocklist);
        List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();

        final Button change = (Button) rootView.findViewById(R.id.Change);

        change.setClickable(false);
        change.setTextColor(getResources().getColor(R.color.disable));

        progressBar.setVisibility(View.VISIBLE);

        WebSettings webSetting = webview.getSettings();
        webSetting.setJavaScriptEnabled(true);
        webSetting.setBuiltInZoomControls(true);
        webSetting.setLayoutAlgorithm(WebSettings.LayoutAlgorithm.SINGLE_COLUMN);
        webview.addJavascriptInterface(new JsInteration(), "control");

       // webSetting.setUseWideViewPort(true);
       // webSetting.setLoadWithOverviewMode(true);

        webSetting.setSaveFormData(true);
        webSetting.setSupportZoom(true);
        //webSetting.sett(TextSize.SMALLER);
        webview.setWebChromeClient(new WebChromeClient() {
        });


        Bundle bundle = getActivity().getIntent().getExtras();
        //接收symbol值
        symbol = bundle.getString("symbol");

        if (app.get("favlist").has(symbol)) {
            System.out.println("Have added");
            star.setImageDrawable(getResources().getDrawable(R.drawable.filled));
        }

        else {
            System.out.println("Haven't added");
            star.setImageDrawable(getResources().getDrawable(R.drawable.empty));
        }

        List<String> ind = new ArrayList<String>();
        ind.add("Price");
        ind.add("SMA");
        ind.add("EMA");
        ind.add("STOCH");
        ind.add("ADX");
        ind.add("BBANDS");
        ind.add("CCI");
        ind.add("MACD");
        ind.add("RSI");

        // 初始化控件
        final Spinner spinner = (Spinner) rootView.findViewById(R.id.indicator);
        //spinner.setClickable(false);
        // 建立数据源
        // 建立Adapter并且绑定数据源
        ArrayAdapter<String> adapter=new ArrayAdapter<String>(getActivity(),android.R.layout.simple_spinner_item, ind);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        //绑定 Adapter到控件
        spinner .setAdapter(adapter);

        change.setClickable(false);
        change.setTextColor(getResources().getColor(R.color.disable));


        change.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                webview.setWebViewClient(new WebViewClient() {

                    @Override
                    public void onPageFinished(WebView view, String url) {
                        super.onPageFinished(view, url);
                        progressBar1.setVisibility(View.VISIBLE);
                        testMethod(webview,indicator);
                        change.setClickable(false);
                        change.setTextColor(getResources().getColor(R.color.disable));
                    }

                });
                webview.loadUrl("file:///android_asset/chart.html");

            }
        });

        if(app.get("querydata")!=null && app.get("querydata").has("price") &&
                app.get("querydata").has("INTRADAY")) {
            progressBar.setVisibility(View.GONE);
            queryData = app.get("querydata");
            System.out.println("2:" + queryData);
            Price = queryData.optJSONObject("price");
            System.out.println("p:"+Price);
            List<String> A = new ArrayList<String>();
            List<String> a = new ArrayList<String>();
            JSONObject Interday = queryData.optJSONObject("INTRADAY");
            List<String> interA = new ArrayList<String>();
            List<String> intera = new ArrayList<String>();

            if (!Price.has("Time Series (Daily)") ||
                    !Interday.has("Time Series (1min)")) {
                TextView error = (TextView) rootView.findViewById(R.id.error1);
                error.setVisibility(View.VISIBLE);
                change.setClickable(false);
                change.setTextColor(getResources().getColor(R.color.disable));
                progressBar.setVisibility(View.GONE);
            } else {
                Iterator it = Price.optJSONObject("Time Series (Daily)").keys();
                while (it.hasNext()) {
                    A.add((String) it.next());
                }

                if (A.size() < 130) {
                    for (int i = 0; i < A.size(); i++) {
                        a.add(i, A.get(i));
                    }
                } else {
                    for (int i = 0; i < 130; i++) {
                        a.add(i, A.get(i));
                    }
                }

                System.out.println("a:" + a);

                Iterator interit = Interday.optJSONObject("Time Series (1min)").keys();
                while (interit.hasNext()) {
                    interA.add((String) interit.next());
                }

                if (interA.size() < 10) {
                    for (int i = 0; i < interA.size(); i++) {
                        intera.add(i, interA.get(i));
                    }
                } else {
                    for (int i = 0; i < 10; i++) {
                        intera.add(i, interA.get(i));
                    }
                }
                System.out.println("intera:" + intera);

                final String Symbol_ = Price.optJSONObject("Meta Data").optString("2. Symbol");
                final float Close_min = Float.parseFloat(Interday.optJSONObject("Time Series (1min)").
                        optJSONObject(intera.get(0)).optString("4. close"));
                float predayClose = Float.parseFloat(Price.optJSONObject("Time Series (Daily)").
                        optJSONObject(a.get(1)).optString("4. close"));
                float Open = Float.parseFloat(Price.optJSONObject("Time Series (Daily)").
                        optJSONObject(a.get(0)).optString("1. open"));

                final float Change = (Float.parseFloat(Price.optJSONObject("Time Series (Daily)").
                        optJSONObject(a.get(0)).optString("4. close"))
                        -
                        Float.parseFloat(Price.optJSONObject("Time Series (Daily)").
                                optJSONObject(a.get(1)).optString("4. close"))
                );

                final float ChaPrecent = (Change / predayClose) * 100;

                float Daylow = Float.parseFloat(Price.optJSONObject("Time Series (Daily)").
                        optJSONObject(a.get(0)).optString("3. low"));
                float Dayhigh = Float.parseFloat(Price.optJSONObject("Time Series (Daily)").
                        optJSONObject(a.get(0)).optString("2. high"));

                String VolumeLast = Price.optJSONObject("Time Series (Daily)").
                        optJSONObject(a.get(0)).optString("5. volume");


                String Timestamp = intera.get(0) + "  EST";


                String pattern = ".*16:00:00.*";

                boolean isMatch = Pattern.matches(pattern, Timestamp);
                float PreClose;
                //判断显示的close是今天的还是昨天的
                if (isMatch) {
                    PreClose = Close_min;
                } else {
                    PreClose = predayClose;
                }

                int DateCount = a.size();


                //-------------------put data in table--------------------//


                Map<String, Object> map = new HashMap<String, Object>();
                map.put("title", "Stock Symbol");
                map.put("value", Symbol_);
                map.put("arrow", null);
                list.add(map);
                map = new HashMap<String, Object>();
                map.put("title", "Last Price");
                map.put("value", new java.text.DecimalFormat("0.00").format(Close_min));
                map.put("arrow", null);
                list.add(map);
                map = new HashMap<String, Object>();
                map.put("title", "Change");
                map.put("value", new java.text.DecimalFormat("0.00").format(Change) + "(" +
                        new java.text.DecimalFormat("0.00").format(ChaPrecent) + "%" + ")");
                if (Change > 0) {
                    map.put("arrow", R.drawable.up);
                } else if (Change < 0) {
                    map.put("arrow", R.drawable.down);
                }
                list.add(map);
                map = new HashMap<String, Object>();
                map.put("title", "Timestamp");
                map.put("value", Timestamp);
                map.put("arrow", null);
                list.add(map);
                map = new HashMap<String, Object>();
                map.put("title", "Open");
                map.put("value", new java.text.DecimalFormat("0.00").format(Open));
                map.put("arrow", null);
                list.add(map);
                map = new HashMap<String, Object>();
                map.put("title", "Close");
                map.put("value", new java.text.DecimalFormat("0.00").format(PreClose));
                map.put("arrow", null);
                list.add(map);
                map = new HashMap<String, Object>();
                map.put("title", "Day's Range");
                map.put("value", new java.text.DecimalFormat("0.00").format(Daylow) + " - " +
                        new java.text.DecimalFormat("0.00").format(Dayhigh));
                map.put("arrow", null);
                list.add(map);
                map = new HashMap<String, Object>();
                map.put("title", "Volume");
                map.put("value", VolumeLast);
                map.put("arrow", null);
                list.add(map);

                SimpleAdapter adaptertable = new SimpleAdapter(getActivity(), list, R.layout.stock,
                        new String[]{"title", "value", "arrow"},
                        new int[]{R.id.stocktitle, R.id.stockvalue, R.id.arrow});
                stocklist.setAdapter(adaptertable);


                //--------------------------------------------------------//
                //-----------------------show chart-----------------------//


                spinner.setClickable(true);

                spinner.setOnItemSelectedListener(new OnItemSelectedListener() {
                    // parent： 为控件Spinner view：显示文字的TextView position：下拉选项的位置从0开始
                    public void onItemSelected(AdapterView<?> parent, View view, final int position, long id) {
                        //获取Spinner控件的适配器
                        final ArrayAdapter<String> adapter = (ArrayAdapter<String>) parent.getAdapter();
                        // tvResult.setText(adapter.getItem(position));
                        Toast.makeText(getActivity(), adapter.getItem(position), Toast.LENGTH_LONG);
                        indicator = adapter.getItem(position);
                        change.setClickable(true);
                        change.setTextColor(getResources().getColor(R.color.enable));
                    }

                    //没有选中时的处理
                    public void onNothingSelected(AdapterView<?> parent) {
                    }
                });



                //--------------------------------------------------------//
                //-----------------------add favlist----------------------//

                star.setClickable(true);
                star.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        //如果favlist有就删掉并置空star

                        if (app.get("favlist") != null) {
                            if (app.get("favlist").has(Symbol_)) {
                                app.get("favlist").remove(Symbol_);
                                dataSave.setDataList("favlist",app.get("favlist"));
                                System.out.println(app.get("favlist").toString());
                                System.out.println(dataSave.getDataList("favlist").optJSONObject("nameValuePairs").toString());
                                star.setImageDrawable(getResources().getDrawable(R.drawable.empty));
                            }
                            //如果没有就加上并且置满star
                            else {

                                try {
                                    JSONObject addsym = new JSONObject();
                                    addsym.put("symbol", Symbol_);
                                    addsym.put("price", new java.text.DecimalFormat("0.00").format(Close_min));
                                    addsym.put("change", new java.text.DecimalFormat("0.00").format(Change));
                                    addsym.put("chaPre", "(" + new java.text.DecimalFormat("0.00").format(ChaPrecent) + "%" + ")");
                                    app.get("favlist").put(Symbol_, addsym);
                                    System.out.println(app.get("favlist").toString());
                                    //dataSave.getDataList("favlist").optJSONObject("nameValuePairs").put(Symbol_, addsym);
                                    dataSave.setDataList("favlist",app.get("favlist"));
                                    System.out.println(dataSave.getDataList("favlist").optJSONObject("nameValuePairs").toString());
                                    star.setImageDrawable(getResources().getDrawable(R.drawable.filled));
                                } catch (JSONException e) {
                                    e.printStackTrace();
                                }
                            }

                        } else {

                            Toast.makeText(getActivity(), "null", Toast.LENGTH_LONG).show();

                        }
                    }
                });


                //--------------------------------------------------------//
                // -----------------------facebook share----------------------//

                fbshare.setClickable(false);
                fbshare.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        if(resurl!=null) {
                            //Toast.makeText(getActivity(),"share",Toast.LENGTH_SHORT).show();
                            ShareLinkContent content = new ShareLinkContent.Builder()
                                    .setContentUrl(Uri.parse(resurl))
                                    .build();
                            shareDialog = new ShareDialog(getActivity());
                            shareDialog.show(content, ShareDialog.Mode.AUTOMATIC);
                            // connection.close()
                        }else {
                            Toast.makeText(getActivity(),"Please load chart first",Toast.LENGTH_SHORT).show();
                        }
                    }
                });

                //--------------------------------------------------------//
            }
        }



        return rootView;
    }



    public class JsInteration {
        @JavascriptInterface
        public void getrul(String message) {
            resurl = message;
            System.out.println(resurl);
            fbshare.setClickable(true);
        }
    }

    private void testMethod(WebView webview, String ind) {

        String data = queryData.toString();
        String call = "javascript:jsshowchar("+data+",\"" + ind + "\")";

        webview.evaluateJavascript("jsshowchar("+data+",\"" + ind + "\")", new ValueCallback<String>() {
            @Override
            public void onReceiveValue(String s) {
                progressBar1.setVisibility(View.GONE);
                //resurl = s;
                //System.out.println(resurl);
            }
        });

        // webview.loadUrl(call);
    }


}

