package com.example.cynthia.hw9_1;

/**
 * Created by cynthia on 2017/11/26.
 */
import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.AbstractList;
import java.util.ArrayList;
import java.util.List;

public class CustomList extends ArrayAdapter<ArrayList>{

    private final Activity context;
    private final List<ArrayList> artlist;
    public CustomList(Activity context,
                      List<ArrayList> artlist) {
        super(context, R.layout.list_single);
        this.context = context;
        this.artlist = artlist;
    }
    @Override
    public View getView(int position, View view, ViewGroup parent) {
        LayoutInflater inflater = context.getLayoutInflater();
        View rowView= inflater.inflate(R.layout.list_single, null, true);
        TextView newsTitle = (TextView) rowView.findViewById(R.id.newstitle);
        TextView newsAuthor = (TextView) rowView.findViewById(R.id.newsauthor);
        TextView newsDate = (TextView) rowView.findViewById(R.id.newsdate);
        newsTitle.setText(artlist.get(position).get(0).toString());
        newsAuthor.setText(artlist.get(position).get(2).toString());
        newsDate.setText(artlist.get(position).get(3).toString());
        return rowView;
    }
}