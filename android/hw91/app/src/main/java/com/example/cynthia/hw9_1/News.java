package com.example.cynthia.hw9_1;

/**
 * Created by cynthia on 2017/11/23.
 */

import android.net.Uri;
import android.os.AsyncTask;
import android.support.design.widget.TabLayout;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.content.Intent;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;
import android.content.Context;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.lang.reflect.Array;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.regex.Pattern;
import java.util.regex.Matcher;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;
import android.widget.AdapterView;
import java.util.Map;
import java.util.HashMap;
import android.widget.SimpleAdapter;
public class News extends Fragment {
    static final String API_URL_symbol = "http://shencsci571-php.us-west-1.elasticbeanstalk.com/123/";
    JSONObject localdata;
    String symbol;
    ProgressBar progressBar;
    TextView test;
    JSONObject queryData;
    ListView newslist;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.news, container, false);
        //Bundle bundle = this.getActivity().getIntent().getExtras();
        progressBar = (ProgressBar) rootView.findViewById(R.id.progressBar4);
        test = (TextView) rootView.findViewById(R.id.t3);
        Bundle bundle = getArguments();
        progressBar.setVisibility(View.VISIBLE);
        List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();

        Querydata app = (Querydata) getActivity().getApplication();


        if(app.get("querydata")!= null && app.get("querydata").has("news")){
            progressBar.setVisibility(View.GONE);
            queryData = app.get("querydata");
            JSONObject News = queryData.optJSONObject("news");
            final List<ArrayList> articlenews = new ArrayList<ArrayList>();
           try {
               if(News.has("rss")&&News.getJSONObject("rss").has("channel")&&
                       News.getJSONObject("rss").getJSONArray("channel").length()!=0 &&
                       News.getJSONObject("rss").getJSONArray("channel").
                               getJSONObject(0).has("item")){
                if(News.getJSONObject("rss").getJSONArray("channel").
                        getJSONObject(0).getJSONArray("item").length()!=0) {

                    JSONArray news = News.getJSONObject("rss").getJSONArray("channel").
                            getJSONObject(0).getJSONArray("item");
                    System.out.println(news);

                    System.out.println(news.getJSONObject(0).getJSONArray("link").getString(0));
                    String p = "(https:\\/\\/seekingalpha.com\\/article\\/)";
                    System.out.println(p);
                    Pattern pattern = Pattern.compile(p);


                    int urlcount = 0;
                    for (int i = 0; i < news.length() && urlcount < 5; i++) {
                        Matcher m = pattern.matcher(news.getJSONObject(i).getJSONArray("link").getString(0));

                        if (m.find()) {
                            ArrayList<String> artnews = new ArrayList<String>();

                            artnews.add(0, news.getJSONObject(i).getJSONArray("title").getString(0));
                            artnews.add(1, news.getJSONObject(i).getJSONArray("link").getString(0));
                            artnews.add(2, news.getJSONObject(i).getJSONArray("sa:author_name").getString(0));
                            artnews.add(3, news.getJSONObject(i).getJSONArray("pubDate").getString(0).
                                    replace("-0500", "EST"));

                            articlenews.add(artnews);
                            urlcount++;


                            Map<String, Object> map = new HashMap<String, Object>();
                            map.put("title", artnews.get(0));
                            map.put("author", "Author: "+artnews.get(2));
                            map.put("date", "Date: "+artnews.get(3));
                            list.add(map);

                        }

                    }
                    System.out.println(list.toString());

                    if (urlcount==0) {
                        //newsecceed = 0;
                    }
                    else {

                        SimpleAdapter adapter = new SimpleAdapter(getActivity(),list,R.layout.list_single,
                                new String[]{"title","author","date"},
                                new int[]{R.id.newstitle,R.id.newsauthor,R.id.newsdate});
                        newslist=(ListView) rootView.findViewById(R.id.newslist);
                        newslist.setAdapter(adapter);
                        newslist.setOnItemClickListener(new AdapterView.OnItemClickListener() {

                            @Override
                            public void onItemClick(AdapterView<?> parent, View view,
                                                    int position, long id) {
                               // Toast.makeText(getActivity(), "You Clicked at ", Toast.LENGTH_SHORT).show();
                                Uri uri = Uri.parse(articlenews.get(position).get(1).toString());
                                Intent intent = new Intent(Intent.ACTION_VIEW, uri);
                                startActivity(intent);

                            }


                        });

                    }

                }
                else{
                    TextView error = (TextView) rootView.findViewById(R.id.error4);
                    error.setVisibility(View.VISIBLE);
                    progressBar.setVisibility(View.GONE);
                }
               }else
               {
                   TextView error = (TextView) rootView.findViewById(R.id.error3);
                   error.setVisibility(View.VISIBLE);
                   progressBar.setVisibility(View.GONE);
               }
            }catch (JSONException e) {
                e.printStackTrace();
            }

        }
        return rootView;
    }


}
