//
//  battleBrix.cpp
//  battlebrix-mobile
//
//  Created by Jung, DaeCheon on 13/06/2019.
//

#include "battleBrix.h"

battleBrix * battleBrix::hInstance = NULL;

#define GROWTH_PER_LEVEL 128

void battleBrix::init() {
    mRewards[1].init(GROWTH_PER_LEVEL / 2, 500, 2);
    mRewards[2].init(GROWTH_PER_LEVEL / 4, 350, 2);
    mRewards[3].init(GROWTH_PER_LEVEL / 8, 350, 1);
    mRewards[4].init(0, 0, 0);
    mRewards[5].init(GROWTH_PER_LEVEL / 4 * -1, 0, 0);
    
    mItems[0].set("Combo", 150, "icons8-snail-80.png");
    mItems[1].set("Potion", 100, "icons8-hyper-potion-96.png");
    mItems[2].set("Bomb", 100, "icons8-atomic-bomb-96.png");
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
        mUserData.maxGrowth = mUserData.level * GROWTH_PER_LEVEL;
        
        return true;
        
    } else if(mUserData.growth < 0) {
        bool isReset = false;
        if(mUserData.level - 1 < 1)
            isReset = true;
        
        mUserData.level = max(1,  mUserData.level - 1);
        mUserData.maxGrowth = mUserData.level * GROWTH_PER_LEVEL;
        mUserData.growth = isReset ? 0 : mUserData.maxGrowth + mUserData.growth;
    }
    return false;
}

const string battleBrix::getLevelString() {
    switch(mUserData.level) {
        case 1:
            return "BEGINNER";
        case 2:
            return "BRONZE";
        case 3:
            return "SILVER";
        case 4:
            return "GOLD";
        case 5:
            return "ROYAL";
        default:
            return "GOD";
    }
}

bool battleBrix::payForPlay(int point, int heart) {
    if(!checkPayForPlay(point, heart))
        return false;
    
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
