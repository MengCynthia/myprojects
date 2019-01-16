package com.example.cynthia.hw9_1;

import java.io.IOException;

import android.content.Context;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.ContextMenu;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.SimpleAdapter;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.ArrayAdapter;
import android.widget.Toast;
import android.content.Intent;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.BaseAdapter;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import android.app.AlertDialog;
import android.view.ViewGroup;
import java.util.Collections;
import java.util.Comparator;
import java.util.Timer;
import java.util.TimerTask;
import android.os.Handler;
import android.os.Message;

public class MainActivity extends AppCompatActivity {
    AutoCompleteTextView symbolText;
    TextView responseView;
    ProgressBar progressBar;
    int index = 0;
    boolean symorder;
    String symsort;
    ListView favlist;
    Handler h;
    Context mContext;
    List<Map<String, Object>> list;
    SimpleAdapter adapterfavlist;
    ListDataSave dataSave;
    //static final String API_KEY = "USE_YOUR_OWN_API_KEY";
    static final String API_URL = "http://shencsci571-php.us-west-1.elasticbeanstalk.com/789/";
    static final String API_URL_symbol = "http://shencsci571-php.us-west-1.elasticbeanstalk.com/456/";
    List<String> autolist = new ArrayList<String>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        symorder = true;
        mContext = getApplicationContext();

        progressBar = (ProgressBar)findViewById(R.id.progressBar1);
        final Querydata app = (Querydata) getApplication();

        favlist = (ListView) findViewById(R.id.favlist);

        list = new ArrayList<Map<String, Object>>();
       // final List<favorite> list = new ArrayList<favorite>();
        dataSave = new ListDataSave(mContext,"fav");


            if (dataSave.getDataList("favlist")==null) {
                JSONObject init = new JSONObject();
                try {
                    JSONObject inits = new JSONObject();
                    inits.put("symbol", "1");
                    inits.put("price", "2");
                    inits.put("change", "3");
                    inits.put("chaPre", "4");
                    init.put("", inits);
                } catch (JSONException e) {
                    e.printStackTrace();
                }

                dataSave.setDataList("favlist", init);
                System.out.println("datasave"+dataSave.getDataList("favlist").toString());
                //System.out.println("datasave2"+dataSave.getDataList("favlist").optJSONObject("").toString());
            }
       // System.out.println("datasave"+dataSave.getDataList("favlist").toString());
        System.out.println("datasave2"+dataSave.getDataList("favlist").optJSONObject("nameValuePairs").toString());
     /*   if (app.get("favlist")==null) {
            JSONObject init = new JSONObject();
            try {
                JSONObject inits = new JSONObject();
                inits.put("symbol", "1");
                inits.put("price", "2");
                inits.put("change", "3");
                inits.put("chaPre", "4");
                init.put("", inits);
            } catch (JSONException e) {
                e.printStackTrace();
            }

            app.put("favlist", init);
            System.out.println(app.get("favlist"));

        }*/
        if (app.get("favlist")==null) {
            List<String> keys = new ArrayList<String>();
            Iterator interit = dataSave.getDataList("favlist").optJSONObject("nameValuePairs").keys();
            while(interit.hasNext()){
                keys.add((String) interit.next());
            }
            JSONObject init = new JSONObject();
            for(int i=0;i<keys.size();i++){
                try {
                    init.put(keys.get(i),
                        dataSave.getDataList("favlist").optJSONObject("nameValuePairs").
                                getJSONObject(keys.get(i)).getJSONObject("nameValuePairs"));
                } catch (JSONException e) {
                    e.printStackTrace();
                }

            }
            //app.put("favlist",dataSave.getDataList("favlist").optJSONObject("nameValuePairs") );
            app.put("favlist",init);
            System.out.println(app.get("favlist"));
        }
        System.out.println(app.get("favlist"));

        symbolText = (AutoCompleteTextView) findViewById(R.id.auto);

        symbolText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                //autolist.clear();
            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void afterTextChanged(Editable editable) {
                //System.out.println(autolist);
                System.out.println("a:"+symbolText.getText().toString());
                //System.out.println("contains:"+autolist.contains(symbolText.getText().toString()));
                if(!autolist.contains(symbolText.getText().toString()) && symbolText.getText().toString().trim().length()!=0){
                    autolist.clear();
                    new RetrieveFeedTask().execute();
                    System.out.println("change");
                }


            }
        });

        List<String> symkeys = new ArrayList<String>();
        Iterator interit = app.get("favlist").keys();

        while(interit.hasNext()){
            symkeys.add((String) interit.next());
        }
        for(int i=0;i<symkeys.size();i++){
            System.out.println("1"+symkeys.get(i));
           if(symkeys.get(i).length()!=0){
                Map<String, Object> map = new HashMap<String, Object>();
                map.put("symbol", app.get("favlist").optJSONObject(symkeys.get(i)).optString("symbol"));
                map.put("price", app.get("favlist").optJSONObject(symkeys.get(i)).optString("price"));
                map.put("change", app.get("favlist").optJSONObject(symkeys.get(i)).optString("change"));
                map.put("chaPre", app.get("favlist").optJSONObject(symkeys.get(i)).optString("chaPre"));
                map.put("index",i);
                list.add(map);

            }
        }
        //System.out.println(list.toString());

        adapterfavlist = new SimpleAdapter(this,list,R.layout.favlist_single,
                new String[]{"symbol","price","change","chaPre"},
                new int[]{R.id.symbol,R.id.price,R.id.change,R.id.chapre});

        favlist.setAdapter(adapterfavlist);
        favlist.setOnCreateContextMenuListener(this);

        favlist.setOnItemLongClickListener(new OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
                index = arg2;
                AlertDialog.Builder alert = new AlertDialog.Builder(
                        MainActivity.this,AlertDialog.THEME_HOLO_LIGHT);
               alert.setTitle("Remove from Favorites?");
              //  alert.setMessage("Remove from Favorites?")
                alert.setItems(R.array.delete, new OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        //do your work here

                        String[] PK = getResources()
                                .getStringArray(
                                        R.array.delete);

                        if(PK[which].equals("YES")) {
                            String deletesymbol = list.get(index).get("symbol").toString();
                            list.remove(index);
                            app.get("favlist").remove(deletesymbol);
                            dataSave.setDataList("favlist",app.get("favlist"));
                            adapterfavlist.notifyDataSetChanged();
                            dialog.dismiss();
                        }
                        else {
                            dialog.dismiss();
                        }
                    }
                });

                alert.show();
                return true;
            }

        });

        favlist.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                String symbol = list.get(i).get("symbol").toString();
                System.out.println(symbol);
                app.put("querydata",null);
                Intent intent = new Intent(MainActivity.this, stockActivity.class);//显示intent
                Bundle bundle=new Bundle();
                //传递name参数为tinyphp
                bundle.putString("symbol", symbol);
                intent.putExtras(bundle);
                startActivity(intent);
                finish();
            }
        });



        final List<String> sort = new ArrayList<String>();
        sort.add("Sort by");
        sort.add("Default");
        sort.add("Symbol");
        sort.add("Price");
        sort.add("Change");
        sort.add("Change(%)");
        final List<String> order = new ArrayList<String>();
        order.add("Order");
        order.add("Ascending");
        order.add("Descending");

        // 初始化控件
        Spinner sortspinner = (Spinner) findViewById(R.id.sort);
        Spinner orderspinner = (Spinner) findViewById(R.id.order);
        // 建立数据源

        // 建立Adapter并且绑定数据源
        ArrayAdapter<String> sortadapter=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item, sort);
        sortadapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        ArrayAdapter<String> orderadapter=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item, order);
        orderadapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        //绑定 Adapter到控件
        sortspinner .setAdapter(sortadapter);
        sortspinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            // parent： 为控件Spinner view：显示文字的TextView position：下拉选项的位置从0开始
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                //获取Spinner控件的适配器
                ArrayAdapter<String> adapter = (ArrayAdapter<String>) parent.getAdapter();
                //Toast.makeText(MainActivity.this, adapter.getItem(position), Toast.LENGTH_LONG).show();
                if(position!=0) {
                    if(symorder) {
                        switch (position) {
                            case 1:
                                symsort = "index";
                                if (!list.isEmpty()) {
                                    Collections.sort(list, new Comparator<Map<String, Object>>() {
                                        @Override
                                        public int compare(Map<String, Object> object1,
                                                           Map<String, Object> object2) {
                                            //根据文本排序
                                            return ((int) object1.get(symsort))-((int) object2.get(symsort));
                                        }
                                    });
                                }
                                adapterfavlist.notifyDataSetChanged();
                                break;
                            case 2:
                                symsort = "symbol";
                                if (!list.isEmpty()) {
                                    Collections.sort(list, new Comparator<Map<String, Object>>() {
                                        @Override
                                        public int compare(Map<String, Object> object1,
                                                           Map<String, Object> object2) {
                                            //根据文本排序
                                            return ((String) object1.get(symsort)).compareTo((String) object2.get(symsort));
                                        }
                                    });
                                }
                                adapterfavlist.notifyDataSetChanged();
                                break;
                            case 3:
                                symsort = "price";
                                if (!list.isEmpty()) {
                                    Collections.sort(list, new Comparator<Map<String, Object>>() {
                                        @Override
                                        public int compare(Map<String, Object> object1,
                                                           Map<String, Object> object2) {
                                            //根据文本排序
                                            return ((int) Float.parseFloat(object1.get(symsort).toString()))
                                                    -((int) Float.parseFloat(object2.get(symsort).toString()));
                                        }
                                    });
                                }
                                adapterfavlist.notifyDataSetChanged();
                                break;
                            case 4:
                                symsort = "change";
                                if (!list.isEmpty()) {
                                    Collections.sort(list, new Comparator<Map<String, Object>>() {
                                        @Override
                                        public int compare(Map<String, Object> object1,
                                                           Map<String, Object> object2) {
                                            //根据文本排序
                                            return ((int) Float.parseFloat(object1.get(symsort).toString()))
                                                    -((int) Float.parseFloat(object2.get(symsort).toString()));
                                        }
                                    });
                                }
                                adapterfavlist.notifyDataSetChanged();
                                break;
                            case 5:
                                symsort = "chaPre";
                                if (!list.isEmpty()) {
                                    Collections.sort(list, new Comparator<Map<String, Object>>() {
                                        @Override
                                        public int compare(Map<String, Object> object1,
                                                           Map<String, Object> object2) {
                                            //根据文本排序
                                            return ((int) getchaPre(object1.get(symsort).toString()))
                                                    -((int) getchaPre(object2.get(symsort).toString()));
                                        }
                                    });
                                }
                                adapterfavlist.notifyDataSetChanged();
                                break;
                            default:
                                symsort = "symbol";
                                break;
                        }

                    }
                    else {
                        switch (position) {
                            case 1:
                                symsort = "index";
                                if (!list.isEmpty()) {
                                    Collections.sort(list, new Comparator<Map<String, Object>>() {
                                        @Override
                                        public int compare(Map<String, Object> object1,
                                                           Map<String, Object> object2) {
                                            //根据文本排序
                                            return ((int) object2.get(symsort))-((int) object1.get(symsort));
                                        }
                                    });
                                }
                                adapterfavlist.notifyDataSetChanged();
                                break;
                            case 2:
                                symsort = "symbol";
                                if (!list.isEmpty()) {
                                    Collections.sort(list, new Comparator<Map<String, Object>>() {
                                        @Override
                                        public int compare(Map<String, Object> object1,
                                                           Map<String, Object> object2) {
                                            //根据文本排序
                                            return ((String) object2.get(symsort)).compareTo((String) object1.get(symsort));
                                        }
                                    });
                                }
                                adapterfavlist.notifyDataSetChanged();
                                break;
                            case 3:
                                symsort = "price";
                                if (!list.isEmpty()) {
                                    Collections.sort(list, new Comparator<Map<String, Object>>() {
                                        @Override
                                        public int compare(Map<String, Object> object1,
                                                           Map<String, Object> object2) {
                                            //根据文本排序
                                            return ((int) Float.parseFloat(object2.get(symsort).toString()))
                                                    -((int) Float.parseFloat(object1.get(symsort).toString()));
                                        }
                                    });
                                }
                                adapterfavlist.notifyDataSetChanged();
                                break;
                            case 4:
                                symsort = "change";
                                if (!list.isEmpty()) {
                                    Collections.sort(list, new Comparator<Map<String, Object>>() {
                                        @Override
                                        public int compare(Map<String, Object> object1,
                                                           Map<String, Object> object2) {
                                            //根据文本排序
                                            return ((int) Float.parseFloat(object2.get(symsort).toString()))
                                                    -((int) Float.parseFloat(object1.get(symsort).toString()));
                                        }
                                    });
                                }
                                adapterfavlist.notifyDataSetChanged();
                                break;
                            case 5:
                                symsort = "chaPre";
                                if (!list.isEmpty()) {
                                    Collections.sort(list, new Comparator<Map<String, Object>>() {
                                        @Override
                                        public int compare(Map<String, Object> object1,
                                                           Map<String, Object> object2) {
                                            //根据文本排序
                                            return ((int) getchaPre(object2.get(symsort).toString()))
                                                    -((int) getchaPre(object1.get(symsort).toString()));
                                        }
                                    });
                                }
                                adapterfavlist.notifyDataSetChanged();
                                break;
                            default:
                                symsort = "symbol";
                                break;
                        }
                    }

                }

            }
            //没有选中时的处理
            public void onNothingSelected(AdapterView<?> parent) {
            }
        });



        orderspinner .setAdapter(orderadapter);
        orderspinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            // parent： 为控件Spinner view：显示文字的TextView position：下拉选项的位置从0开始
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                //获取Spinner控件的适配器
                ArrayAdapter<String> adapter = (ArrayAdapter<String>) parent.getAdapter();
                //Toast.makeText(MainActivity.this,adapter.getItem(position),Toast.LENGTH_LONG).show();
                switch (position){
                    case 0:
                        break;
                    case 1:
                        symorder = true;
                        break;
                    case 2:
                        symorder = false;
                        break;
                    default:
                        break;
                }
            }
            //没有选中时的处理
            public void onNothingSelected(AdapterView<?> parent) {
            }
        });

        final List<String> symlist = new ArrayList<String>();
        for(int i=0; i<list.size(); i++){
            symlist.add(list.get(i).get("symbol").toString());
        }
        final ImageButton refresh = (ImageButton) findViewById(R.id.refresh);

        refresh.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //new ScheduleRun(5);
                if(!list.isEmpty()) {
                    progressBar.setVisibility(View.VISIBLE);
                    int count = 0;
                    for (int i = 0; i < symlist.size(); i++) {
                        refresh Ref = new refresh();
                        Ref.setsymbol(symlist.get(count), count);
                        //System.out.println(symlist.get(count));
                        Ref.execute();
                        adapterfavlist.notifyDataSetChanged();
                        count++;
                    }

                   // progressBar.setVisibility(View.GONE);
                }
                else {
                    Toast.makeText(MainActivity.this,"NO Favorites",Toast.LENGTH_SHORT).show();
                }
            }
        });

        Switch autorefresh = (Switch)findViewById(R.id.autorefresh);

        h = new Handler() {
            public void handleMessage(Message msg) {
                progressBar.setVisibility(View.VISIBLE);
                adapterfavlist.notifyDataSetChanged();
               // Toast.makeText(MainActivity.this,"auto",Toast.LENGTH_SHORT).show(); //this is the textview
            }
        };
        autorefresh.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                ScheduleRun scheduleRun =  new ScheduleRun();

                if(!list.isEmpty()){
                    //new ScheduleRun(5);
                    if(b) {
                        //选中
                        scheduleRun.Run(5);
                        progressBar.setVisibility(View.GONE);
                    }
                    else {
                        //未选中
                        scheduleRun.stop();

                    }
                }else{

                    Toast.makeText(MainActivity.this,"NO Favorites",Toast.LENGTH_SHORT).show();
                }
            }
        });

        Button getquery = (Button) findViewById(R.id.getquery);
        Button clear = (Button) findViewById(R.id.clear);
        clear.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                symbolText.setText("");
            }
        });
        getquery.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                if(symbolText.getText().toString().trim().length()==0) {
                    Toast toast= Toast.makeText(
                            MainActivity.this,"Please enter a stock name or symbol",Toast.LENGTH_SHORT);
                    toast.show();
                }else {
                //new getQuery().execute();
                   // Querydata.getInstance().setquerydata(null);
                    app.put("querydata",null);
                    Intent intent = new Intent(MainActivity.this, stockActivity.class);//显示intent
                    Bundle bundle=new Bundle();
                    //传递name参数为tinyphp
                    bundle.putString("symbol", symbolText.getText().toString().trim().split(" ")[0]);
                    intent.putExtras(bundle);
                    startActivity(intent);
                    finish();
                }
            }
        });


    }
    public float getchaPre (String chaPre){

        float cp = Float.parseFloat(chaPre.replaceAll("\\(","").
                                           replaceAll("%","").
                                           replaceAll("\\)","").trim());
        return cp;
    }

    public class ScheduleRun {
        Timer timer;

        public ScheduleRun(){
            timer = new Timer();
        }

        public void Run(int delaytime) {

            timer.schedule(new ScheduleRunTask(), 0, delaytime * 1000);   //推迟0秒执行, 间隔delaytime秒重复运行.
            //timer.schedule(new ScheduleRunTask(),delaytime  * 1000);     //推迟delaytime  秒后执行(只执行一次)
        }


        public void stop() {
            System.out.println("Task Finish!");
            timer.cancel();
        }

        class ScheduleRunTask extends TimerTask {
            int numRunnings = 3;

            public void runbat(int timeFortmat) {
                String cmd = "cmd /c start D:/ScheduleRun/data/" + timeFortmat + ".bat";
                try {
                    Process ps = Runtime.getRuntime().exec(cmd);
                    System.out.println(ps.getInputStream());
                } catch (IOException ioe) {
                    ioe.printStackTrace();
                }
            }

            public void run() {
                if (numRunnings > 0) {
                    System.out.println("running.....");
                   // Toast.makeText(MainActivity.this,"auto",Toast.LENGTH_SHORT);
                   h.obtainMessage(1,list);
                    List<String> symlist = new ArrayList<String>();
                    for(int i=0; i<list.size(); i++){
                        symlist.add(list.get(i).get("symbol").toString());
                    }

                    int count = 0;
                    for (int i = 0; i < symlist.size(); i++) {
                        refresh Ref = new refresh();
                        Ref.setsymbol(symlist.get(count), count);
                        //System.out.println(symlist.get(count));
                        Ref.execute();
                        count++;
                    }
                    h.obtainMessage(1,list).sendToTarget();
                    runbat(1340);
                    numRunnings--;
                } else {
                    System.out.println("Task Finish!");
                    timer.cancel(); //Stop timer and we also can call System.exit
                    //System.exit(0);   //Stops everything
                }
            }
        }
    }
    class refresh extends AsyncTask<Void, Void, String> {

        private Exception exception;
        private String symbol;
        int ind;
        final Querydata app = (Querydata) getApplication();
        public void setsymbol(String symbol,int ind){
            this.symbol = symbol;
            this.ind = ind;
        }
        protected void onPreExecute() {
//            Toast.makeText(MainActivity.this,"refresh "+symbol,Toast.LENGTH_SHORT).show();
//            progressBar.setVisibility(View.VISIBLE);
        }

        protected String doInBackground(Void... urls) {


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
            progressBar.setVisibility(View.GONE);
            Log.i("INFO", response);
             //responseView.setText(response);

            // TODO: check this.exception
            // TODO: do something with the feed



            try {

                JSONObject Interday = (JSONObject) new JSONTokener(response).nextValue();
                List<String> interA = new ArrayList<String>();
                List<String> intera = new ArrayList<String>();
                if(Interday.has("Time Series (1min)")) {
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

                    final String Symbol_ = Interday.optJSONObject("Meta Data").optString("2. Symbol");
                    final float last_min = Float.parseFloat(Interday.optJSONObject("Time Series (1min)").
                            optJSONObject(intera.get(0)).optString("4. close"));
                    float prelast_min = Float.parseFloat(Interday.optJSONObject("Time Series (1min)").
                            optJSONObject(intera.get(1)).optString("4. close"));

                    final float Change = (Float.parseFloat(Interday.optJSONObject("Time Series (1min)").
                            optJSONObject(intera.get(0)).optString("4. close"))
                            -
                            Float.parseFloat(Interday.optJSONObject("Time Series (1min)").
                                    optJSONObject(intera.get(1)).optString("4. close"))
                    );

                    final float ChaPrecent = (Change / prelast_min) * 100;

                    JSONObject addsym = new JSONObject();
                    addsym.put("symbol", Symbol_);
                    addsym.put("price", new java.text.DecimalFormat("0.00").format(last_min));
                    addsym.put("change", new java.text.DecimalFormat("0.00").format(Change));
                    addsym.put("chaPre", "(" + new java.text.DecimalFormat("0.00").format(ChaPrecent) + "%" + ")");
                    app.get("favlist").put(Symbol_, addsym);
                    dataSave.setDataList("favlist",app.get("favlist"));
                    System.out.println(app.get("favlist").toString());
                    Map<String, Object> map = new HashMap<String, Object>();
                    map.put("symbol", Symbol_);
                    map.put("price", new java.text.DecimalFormat("0.00").format(last_min));
                    map.put("change", new java.text.DecimalFormat("0.00").format(Change));
                    map.put("chaPre", "(" + new java.text.DecimalFormat("0.00").format(ChaPrecent) + "%" + ")");
                    map.put("index", ind);
                    list.remove(ind);
                    list.add(ind, map);
                    System.out.println(list.toString());
                    //adapterfavlist.notifyDataSetChanged();
                }
                else {
                    Toast.makeText(MainActivity.this,"Fail get Data! Try Again",Toast.LENGTH_LONG).show();
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }

        }
    }

    class RetrieveFeedTask extends AsyncTask<Void, Void, String> {

        private Exception exception;

        protected void onPreExecute() {
            autolist.clear();
            progressBar.setVisibility(View.VISIBLE);
        }

        protected String doInBackground(Void... urls) {
            String symbol = symbolText.getText().toString().replaceAll("\\s*", "");;

            // Do some validation here

            try {
                URL url = new URL(API_URL + "?symbol=" + symbol);
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

            Log.i("INFO", response);
           // responseView.setText(response);
            // TODO: check this.exception
            // TODO: do something with the feed

            try {
                JSONArray object = (JSONArray) new JSONTokener(response).nextValue();

                for(int i=0; i<object.length()&&i<5;i++){
                    JSONObject r = object.getJSONObject(i);
                    String autores = r.getString("Symbol")+" "+"-"+" "+
                                     r.getString("Name")+" "+"("+r.getString("Exchange")+")";
                    //System.out.println(autores);
                    autolist.add(autores);
                }
                System.out.println("1:"+autolist);
                if(autolist.isEmpty()){
                    String autores = "No Match Symbol";
                    autolist.add(autores);
                }
                System.out.println("2:"+autolist);
                ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this,
                        android.R.layout.simple_dropdown_item_1line, autolist);

                progressBar.setVisibility(View.GONE);
                symbolText.setAdapter(adapter);
                symbolText.setThreshold(1);


            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    }


}



