//
//  battleBrix.cpp
//  battlebrix-mobile
//
//  Created by Jung, DaeCheon on 13/06/2019.
//

#include "battleBrix.h"

battleBrix * battleBrix::hInstance = NULL;
brixMap * brixMap::hInstance = NULL;
//===============================================================================
void brixMap::init() {
    rapidjson::Document d = getJsonValue("brix.json");
    const rapidjson::Value& brix = d["brix"];
    for (rapidjson::SizeType i = 0; i < brix.Size(); i++)
    {
        brixPosition b;
        b.load(d["brix"][rapidjson::SizeType(i)]);
        mBrixMap.push_back(b);
    }
}
//-------------------------------------------------------------------------------
void brixMap::brixPosition::load(rapidjson::Value &p) {
    this->title = p["title"].GetString();
    this->minGrade = p["minGrade"].GetInt();
    this->maxGrade = p["maxGrade"].GetInt();
    //statics
    const rapidjson::Value& statics = p["static"];
    for (rapidjson::SizeType i = 0; i < statics.Size(); i++)
    {
        position pos;
        pos.x = statics[rapidjson::SizeType(i)][rapidjson::SizeType(0)].GetInt();
        pos.y = statics[rapidjson::SizeType(i)][rapidjson::SizeType(1)].GetInt();
        
        this->statics.push_back(pos);
    }
    //movement
    const rapidjson::Value& movements = p["movement"];
    for (rapidjson::SizeType i = 0; i < movements.Size(); i++)
    {
        brixMovement movement;
        movement.load(p["movement"][rapidjson::SizeType(i)]);
        this->movements.push_back(movement);
    }
    
}
//-------------------------------------------------------------------------------
void brixMap::brixMovement::load(rapidjson::Value &p) {
    
    this->type = p["type"].GetInt();
    
    const rapidjson::Value& path = p["path"];
    for (rapidjson::SizeType i = 0; i < path.Size(); i++)
    {
        position pos;
        pos.x = path[rapidjson::SizeType(i)][rapidjson::SizeType(0)].GetInt();
        pos.y = path[rapidjson::SizeType(i)][rapidjson::SizeType(1)].GetInt();
        
        this->path.push_back(pos);
    }
    
}
int brixMap::getMapRandom() {
    vector<int> v;
    int grade = battleBrix::inst()->mUserData.grade;
    for(int n=0; n<mBrixMap.size(); n++) {
        if(mBrixMap[n].minGrade <= grade) {
            switch(mBrixMap[n].maxGrade)
            {
                case -1:
                    v.push_back(n);
                    break;
                default:
                    if(mBrixMap[n].maxGrade >= grade)
                        v.push_back(n);
                    break;
            }
        }
    }
    CC_ASSERT(v.size() > 0);
    return v[getRandValue((int)v.size())];
};
//===============================================================================
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
    const rapidjson::Value& grades = d["grades"];
    for (rapidjson::SizeType i = 0; i < grades.Size(); i++) {
        grade g;
        g.grade = grades[rapidjson::SizeType(i)]["grade"].GetInt();
        g.speed = grades[rapidjson::SizeType(i)]["speed"].GetFloat();
        g.title = grades[rapidjson::SizeType(i)]["title"].GetString();
        
        mGrades.push_back(g);
    }
    
    //brix map
    
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
            return "GD." + to_string(battleBrix::inst()->mUserData.grade);
            
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
    
    mUserData.increaseHeart(reward.heart);
    mUserData.increasePoint(reward.point);
    
    return increseGrowth(reward.growth);
}

bool battleBrix::increseGrowth(int val) {
    mUserData.growth += val;
    if(mUserData.growth >= mUserData.maxGrowth) {
        mUserData.growth = mUserData.growth - mUserData.maxGrowth;
        mUserData.grade++;
        mUserData.maxGrowth = mUserData.grade * mGrowthPerLevel;
        
        return true;
        
    } else if(mUserData.growth < 0) {
        bool isReset = false;
        if(mUserData.grade - 1 < 1)
            isReset = true;
        
        mUserData.grade = max(1,  mUserData.grade - 1);
        mUserData.maxGrowth = mUserData.grade * mGrowthPerLevel;
        mUserData.growth = isReset ? 0 : mUserData.maxGrowth + mUserData.growth;
    }
    return false;
}

const string battleBrix::getLevelString() {
    return mGrades[mUserData.grade].title;
}

bool battleBrix::payForPlay(int point, int heart) {
    if(!checkPayForPlay(point, heart))
        return false;
    
    if(mUserData.heart >= mUserData.heartMax)
        mUserData.setHeartTimerStart(getNow());
    
    mUserData.increasePoint(point * -1);
    mUserData.increaseHeart(heart * -1);
    
    return true;
}

bool battleBrix::checkPayForPlay(int point, int heart) {
    if(mUserData.point < point || mUserData.heart < heart) {
        return false;
    }
    return true;
}
