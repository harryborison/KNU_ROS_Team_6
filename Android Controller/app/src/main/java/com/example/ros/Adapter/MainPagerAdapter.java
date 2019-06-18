package com.example.ros.Adapter;

import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentStatePagerAdapter;

import com.example.ros.MainViewPager.FirstFragment;
import com.example.ros.MainViewPager.SecondFragment;
import com.example.ros.MainViewPager.ThirdFragment;

public class MainPagerAdapter extends FragmentStatePagerAdapter {

    private static final String TAG = MainPagerAdapter.class.getSimpleName();
    private static final int NUM_PAGES = 3;
    private String[] tabTitles = new String[]{"수동", "자동", "명령하기"};

    public MainPagerAdapter(FragmentManager fm) {
        super(fm);
    }


    @Nullable
    @Override
    public CharSequence getPageTitle(int position) {
        return tabTitles[position];
    }

    @Override
    public Fragment getItem(int pagePosition) {

        switch (pagePosition) {
            case 0:
                return FirstFragment.newInstance();
            case 1:
                return SecondFragment.newInstance();
            case 2:
                return ThirdFragment.newInstance();
        }

        //error
        return new Fragment();
    }

    @Override
    public int getCount() {
        return NUM_PAGES;
    }
}
