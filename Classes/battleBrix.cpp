//
//  battleBrix.cpp
//  battlebrix-mobile
//
//  Created by Jung, DaeCheon on 13/06/2019.
//

#include "battleBrix.h"
#include "library/pch.h"

battleBrix * battleBrix::hInstance = NULL;

#define GROWTH_PER_LEVEL 128

void battleBrix::init() {
    mRewards[1].init(GROWTH_PER_LEVEL / 2, 100, 1);
    mRewards[2].init(GROWTH_PER_LEVEL / 4, 50, 1);
    mRewards[3].init(GROWTH_PER_LEVEL / 8, 0, 0);
    mRewards[4].init(0, 0, 0);
    mRewards[5].init(GROWTH_PER_LEVEL / 4 * -1, 0, 0);
}

const string battleBrix::getText(const string& defaultString, int id) {
    switch(id) {
        case _ID_NODE_LABEL_ID:
            return battleBrix::inst()->mUserData.id;
            
        case _ID_NODE_LABEL_POINT:
            return numberFormat(battleBrix::inst()->mUserData.point);
            
        case _ID_NODE_LABEL_HEART:
            return to_string(battleBrix::inst()->mUserData.heart) + " / " + to_string(battleBrix::inst()->mUserData.heartMax);
            
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
        mUserData.level = max(1,  mUserData.level - 1);
        mUserData.growth = 0;
        mUserData.maxGrowth = mUserData.level * GROWTH_PER_LEVEL;
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
