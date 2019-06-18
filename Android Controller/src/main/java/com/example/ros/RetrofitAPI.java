package com.example.ros;

import com.google.gson.JsonObject;

import java.util.HashMap;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.FieldMap;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.POST;

public interface RetrofitAPI {

    @FormUrlEncoded
    @POST("/direction")
    Call<JsonObject> sendDirection(@FieldMap HashMap<String, String> param);
    //Call<String> sendDirection(@Body String param);
}

