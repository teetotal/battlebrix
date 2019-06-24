//
//  battleBrix.cpp
//  battlebrix-mobile
//
//  Created by Jung, DaeCheon on 13/06/2019.
//

#include "battleBrix.h"

battleBrix * battleBrix::hInstance = NULL;

void battleBrix::init() {
    
    rapidjson::Document d = getJsonValue("config.json");
    
    mUserData.rechargeTime = d["rechargeInterval"].GetInt();
    mGrowthPerLevel = d["growthPerLevel"].GetInt();
    
    //reward
    const rapidjson::Value& rewards = d["play"]["rewards"];
    for (rapidjson::SizeType i = 0; i < rewards.Size(); i++)
    {
        int growth = rewards[rapidjson::SizeType(i)]["growth"].GetInt();
        int point = rewards[rapidjson::SizeType(i)]["point"].GetInt();
        int heart = rewards[rapidjson::SizeType(i)]["heart"].GetInt();
        mRewards[i+1].init(growth, point, heart);
    }
    
    //play item
    const rapidjson::Value& playItems = d["play"]["items"];
    for (rapidjson::SizeType i = 0; i < playItems.Size(); i++) {
        const string name = playItems[rapidjson::SizeType(i)]["name"].GetString();
        const string img = playItems[rapidjson::SizeType(i)]["img"].GetString();
        int price = playItems[rapidjson::SizeType(i)]["price"].GetInt();
        mItems[i].set(name, price, img);
    }
    
    //Grade titles
    const rapidjson::Value& gradeTitles = d["gradeTitles"];
    for (rapidjson::SizeType i = 0; i < gradeTitles.Size(); i++) {
        mGradeTitles.push_back(gradeTitles[rapidjson::SizeType(i)].GetString());
    }
}

const string battleBrix::getText(const string& defaultString, int id) {
    switch(id) {
        case _ID_NODE_LABEL_ID:
            return battleBrix::inst()->mUserData.id;
            
        case _ID_NODE_LABEL_POINT:
            return numberFormat(battleBrix::inst()->mUserData.point);
            
        case _ID_NODE_LABEL_HEART:
            return to_string(battleBrix::inst()->mUserData.heart) + " / " + to_string(battleBrix::inst()->mUserData.heartMax);
            
        case _ID_NODE_TIMER_HEART:
            return mUserData.getRechargeRemainTimeString();
            
        case _ID_NODE_LABEL_LEVEL:
            return "Lv." + to_string(battleBrix::inst()->mUserData.level);
            
        case _ID_NODE_PROGRESSBAR:
            return getLevelString();
            
        case _ID_NODE_LABEL_RANKING:
            return numberFormat(battleBrix::inst()->mUserData.ranking) + "th";
            
        default:
            return defaultString;
    }
}
const float battleBrix::getProgressValue(int id) {
    switch(id) {
        case _ID_NODE_PROGRESSBAR:
            return getGrowthPercentage();
        default:
            return -1.f;
    }
}

battleBrix::rewardData battleBrix::getReward(int ranking) {
    return mRewards[ranking];
}
bool battleBrix::applyReward(int ranking) {
    rewardData reward = getReward(ranking);
    mUserData.heart += reward.heart;
    mUserData.point += reward.point;
    
    return increseGrowth(reward.growth);
}

bool battleBrix::increseGrowth(int val) {
    mUserData.growth += val;
    if(mUserData.growth >= mUserData.maxGrowth) {
        mUserData.growth = mUserData.growth - mUserData.maxGrowth;
        mUserData.level++;
        mUserData.maxGrowth = mUserData.level * mGrowthPerLevel;
        
        return true;
        
    } else if(mUserData.growth < 0) {
        bool isReset = false;
        if(mUserData.level - 1 < 1)
            isReset = true;
        
        mUserData.level = max(1,  mUserData.level - 1);
        mUserData.maxGrowth = mUserData.level * mGrowthPerLevel;
        mUserData.growth = isReset ? 0 : mUserData.maxGrowth + mUserData.growth;
    }
    return false;
}

const string battleBrix::getLevelString() {
    return mGradeTitles[mUserData.level];
}

bool battleBrix::payForPlay(int point, int heart) {
    if(!checkPayForPlay(point, heart))
        return false;
    
    if(mUserData.heart >= mUserData.heartMax)
        mUserData.heartTimerStart = getNow();
    
    mUserData.point -= point;
    mUserData.heart -= heart;
    
    return true;
}

bool battleBrix::checkPayForPlay(int point, int heart) {
    if(mUserData.point < point || mUserData.heart < heart) {
        return false;
    }
    return true;
}
