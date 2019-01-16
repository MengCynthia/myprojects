package com.example.cynthia.hw9_1;

/**
 * Created by cynthia on 2017/11/24.
 */
import android.app.Application;
import java.util.HashMap;

import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;
/*
public  class Querydata extends Application {
    private JSONObject querydata;

    private static Querydata instance;

    public static Querydata getInstance(){
        return instance;
    }

    public JSONObject getquerydata(){
        return querydata;
    }
    public void setquerydata(JSONObject s){
       querydata = s;
    }

    @Override
    public void onCreate(){
        super.onCreate();
        instance = this;
    }

}
*/
public class Querydata extends Application {
    private HashMap<String, JSONObject> map = new HashMap<String, JSONObject>();
    //发送的方法

    public void put(String key,JSONObject object){
        map.put(key, object);
    }

    //获取数据方法
    public JSONObject get(String key){
        return map.get(key);
    }
}