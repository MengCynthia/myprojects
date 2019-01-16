package com.example.cynthia.hw9_1;

/**
 * Created by cynthia on 2017/11/30.
 */
import android.content.SharedPreferences;
import android.content.Context;

import com.google.gson.Gson;
import com.google.gson.JsonParser;

import org.json.JSONObject;


public class ListDataSave {
    private SharedPreferences preferences;
    private SharedPreferences.Editor editor;

    public ListDataSave(Context mContext, String preferenceName) {
        preferences = mContext.getSharedPreferences(preferenceName, Context.MODE_PRIVATE);
        editor = preferences.edit();
    }

    /**
     * 保存List
     * @param tag
     * @param Jsonob
     */
    public  void setDataList(String tag, JSONObject Jsonob) {
        if (null == Jsonob || Jsonob.length() <= 0)
            return;
        Gson gson = new Gson();
        String strJson = gson.toJson(Jsonob);
        editor.clear();
        editor.putString(tag, strJson);
        editor.commit();

    }

    /**
     * 获取List
     * @param tag
     * @return
     */
    public JSONObject getDataList(String tag) {
        JSONObject Jsonob;
        String strJson = preferences.getString(tag,null);
        //System.out.println("1"+strJson);
        if (null == strJson) {
            return null;
        }

        Gson gson = new Gson();
        JsonParser parser =new JsonParser();
        //JSONObject Jsonob =  parser.parse(strJson).getAsJsonObject();
        try {
            Jsonob = new JSONObject(strJson);
           // System.out.println("2"+Jsonob);
            return Jsonob;
        } catch (Exception e) {
            e.printStackTrace();
        }


        return null;

    }
}