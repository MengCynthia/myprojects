package com.example.cynthia.hw9_1;

import android.content.Intent;
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

import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

public class stockActivity extends AppCompatActivity {

    /**
     * The {@link android.support.v4.view.PagerAdapter} that will provide
     * fragments for each of the sections. We use a
     * {@link FragmentPagerAdapter} derivative, which will keep every
     * loaded fragment in memory. If this becomes too memory intensive, it
     * may be best to switch to a
     * {@link android.support.v4.app.FragmentStatePagerAdapter}.
     */
    private SectionsPagerAdapter mSectionsPagerAdapter;

    private interface isLoadDataListener {
        public void loadComplete();
    }
    private isLoadDataListener loadLisneter;

    /**
     * The {@link ViewPager} that will host the section contents.
     */
    private ViewPager mViewPager;
    String symbol;
    ProgressBar progressBar;
    TextView responseView;
    static final String API_URL_symbol = "http://shencsci571-php.us-west-1.elasticbeanstalk.com/123/";
    JSONObject localdata;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_stock);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        // Create the adapter that will return a fragment for each of the three
        // primary sections of the activity.
        mSectionsPagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager());

        // Set up the ViewPager with the sections adapter.
        mViewPager = (ViewPager) findViewById(R.id.container);
        mViewPager.setAdapter(mSectionsPagerAdapter);

        TabLayout tabLayout = (TabLayout) findViewById(R.id.tabs);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        Toolbar mToolbarTb = (Toolbar) findViewById(R.id.toolbar);
        mToolbarTb.setNavigationOnClickListener(new View.OnClickListener()

        {
            @Override
            public void onClick (View v){

                Intent intent = new Intent(stockActivity.this,MainActivity.class);
                startActivity(intent);
                finish();
            }
        });

        mViewPager.addOnPageChangeListener(new TabLayout.TabLayoutOnPageChangeListener(tabLayout));
        tabLayout.addOnTabSelectedListener(new TabLayout.ViewPagerOnTabSelectedListener(mViewPager));


        Bundle bundle = this.getIntent().getExtras();
        //接收symbol值
        symbol = bundle.getString("symbol");
        mToolbarTb.setTitle(symbol);
        Toast toast = Toast.makeText(
                this, symbol, Toast.LENGTH_LONG);
        toast.show();


        setLoadDataComplete(new isLoadDataListener() {

            @Override
            public void loadComplete() {
               // progressBar.setVisibility(View.GONE);
                Toast toast1 = Toast.makeText(
                        stockActivity.this, "got query", Toast.LENGTH_LONG);
                toast1.show();


  /*             new Thread(new Runnable() {
                    public void run() {
                        System.out.println("3");
                        News ne = new News();

                        Bundle bundle3= new Bundle();

                        bundle3.putString("Data","ne");

                        ne.setArguments(bundle3);

                        getSupportFragmentManager().beginTransaction().replace(R.id.newslay,ne).commit();


                    }
                }).start();
*/
                new Thread(new Runnable() {
                    public void run() {
                        System.out.println("1");
                        Current cur = new Current();

                        Bundle bundle1= new Bundle();


                        bundle1.putString("symbol",symbol);

                        cur.setArguments(bundle1);

                        getSupportFragmentManager().beginTransaction().replace(R.id.constraintLayout,cur).commit();

                    }
                }).start();

                new Thread(new Runnable() {
                    public void run() {

                        System.out.println("2");
                        Historical his = new Historical();

                        Bundle bundle2= new Bundle();

                        bundle2.putString("Data","his");

                        his.setArguments(bundle2);

                        getSupportFragmentManager().beginTransaction().replace(R.id.c2,his).commit();


                    }
                }).start();


               //Querydata.getInstance().setquerydata(localdata.toString());
               // System.out.println("1:"+Querydata.getInstance().getquerydata());

                Querydata app = (Querydata) getApplication();
                String value = app.get("querydata").toString();//value接收的信息
                System.out.println("1:"+ value);
                System.out.println("fav:"+app.get("favlist").toString());





                // TODO Auto-generated method stub
                //这里执行你要的操作，当UI更新完成后会自动调用这里面的代码
            }
        });
       new getQuery1().execute();


    }



    class getQuery1 extends AsyncTask<Void, Void, String> {

       private Exception exception;

       protected void onPreExecute() {
        //progressBar.setVisibility();
        //responseView.setText("");
       }

       protected String doInBackground(Void... urls) {
        //String symbol = symbolText.getText().toString().trim().split(" ")[0];

        // Do some validation here
        //ajax1.params = {"symbol":symbollist};
        try {

            URL url = new URL(API_URL_symbol + "?symbol=" + symbol);
            //System.out.println();
            System.out.println(url+" symbol: "+symbol);
            HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
            try {
                BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(urlConnection.getInputStream()));
                StringBuilder stringBuilder = new StringBuilder();
                String line;
                while ((line = bufferedReader.readLine()) != null) {
                    stringBuilder.append(line).append("\n");
                }
                bufferedReader.close();
                return stringBuilder.toString();
            }
            finally{
                urlConnection.disconnect();
            }
        }
        catch(Exception e) {
            Log.e("ERROR", e.getMessage(), e);
            return null;
        }
    }

      protected void onPostExecute(String response) {
        if(response == null) {
            response = "THERE WAS AN ERROR";
        }
       // progressBar.setVisibility(View.GONE);
        Log.i("INFO", response);
        //responseView.setText(response);
        // TODO: check this.exception
        // TODO: do something with the feed

            try {

                localdata = (JSONObject) new JSONTokener(response).nextValue();
                Querydata app = (Querydata) getApplication();
                app.put("querydata", localdata);//要传递的信息

               // Querydata.getInstance().setquerydata(localdata);
               //System.out.println("price:"+localdata.getJSONObject("price"));
                if (loadLisneter != null) {
                    loadLisneter.loadComplete();
                }

            } catch (JSONException e) {
                e.printStackTrace();
            }
      }
}



    public void setLoadDataComplete(isLoadDataListener dataComplete) {
        this.loadLisneter = dataComplete;
    }


    /**
     * A {@link FragmentPagerAdapter} that returns a fragment corresponding to
     * one of the sections/tabs/pages.
     */
    public class SectionsPagerAdapter extends FragmentPagerAdapter {

        public SectionsPagerAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public Fragment getItem(int position) {
            switch (position) {
                case 0:
                    Current currenttab = new Current();
                    return currenttab;
                case 1:
                    Historical historicaltab = new Historical();
                    return historicaltab;
                case 2:
                    News newstab = new News();
                    return newstab;
                default:
                    return null;
            }

        }

        @Override
        public int getCount() {
            // Show 3 total pages.
            return 3;
        }
    }
}
