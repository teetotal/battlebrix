//
//  battleBrix.hpp
//  battlebrix-mobile
//
//  Created by Jung, DaeCheon on 13/06/2019.
//

#ifndef battleBrix_h
#define battleBrix_h

#include <string>
#include "library/pch.h"

using namespace std;

enum _ID_NODE {
    _ID_NODE_LABEL_ID = 1,
    _ID_NODE_LABEL_POINT = 3,
    _ID_NODE_LABEL_HEART = 4,
    _ID_NODE_TIMER_HEART = 5,
    _ID_NODE_LABEL_LEVEL = 12,
    _ID_NODE_PROGRESSBAR = 13,
    
    _ID_NODE_LABEL_RANKING = 17,
};

#define PLAY_ITEM_CNT 3
//===============================================================
class brixMap {
public:
    brixMap() {};
    virtual ~brixMap() {};
    static brixMap * inst() {
        if(hInstance == NULL)
            hInstance = new brixMap;
        return hInstance;
    };
    void init();

    struct position {
        int x;
        int y;
    };
    //brixMovement
    struct brixMovement {
        int type;
        vector<position> path;
        void load(rapidjson::Value &p);
    };
    //brix brixPosition
    struct brixPosition {
        string title;
        vector<position> statics;
        vector<brixMovement> movements;
        
        void load(rapidjson::Value &p);
    };
    
    vector<brixPosition> mBrixMap;
    
    inline int getMapRandom() {
        return getRandValue((int)mBrixMap.size());
    };
    
    inline const brixPosition getMap(int idx) {
        return mBrixMap[idx];
    };
    
private:
    static brixMap * hInstance;
};

//===============================================================
class battleBrix {
public:
    battleBrix() : mLastRanking(1) {};
    virtual ~battleBrix() {};
    static battleBrix * inst() {
      if(hInstance == NULL)
          hInstance = new battleBrix;
        return hInstance;
    };
    void init();
    
    //user data
    struct userData {
        int win;
        int lose;
        int ranking;
        int grade;
        int heart;
        int heartMax;
        time_t heartTimerStart;
        int point;
        int maxGrowth;
        int growth;
        string id;
        
        int rechargeTime;
        
        userData() : win(0)
        , lose(0)
        , ranking(532340)
        , grade(5)
        , heart(5)
        , heartMax(8)
        , point(5000)
        , maxGrowth(128)
        , growth(10)
        {
            id = "teetotal";
            heartTimerStart = getNow();
        };
        
        time_t getRechargeRemainTime() {
            return rechargeTime - (getNow() - heartTimerStart);
        };
        string getRechargeRemainTimeString(){
            if(heart >= heartMax)
                return "0:00";
            
            time_t t = getRechargeRemainTime();
            int min = (int)t / 60;
            int sec = t % 60;
            
                
            return to_string(min) + ":" + ((sec < 10) ? "0" : "") + to_string(sec);
        };
        // 변화가 있으면 true
        bool recharge() {
            bool ret = false;
            time_t now = getNow();
            while(heart < heartMax) {
                if(now - heartTimerStart >= rechargeTime) {
                    heart++;
                    heartTimerStart += rechargeTime;
                    ret = true;
                } else {
                    break;
                }
            }
            
            return ret;
        };
        
    } mUserData;
    
    // reward meta
    struct rewardData {
        int growth;
        int point;
        int heart;
        
        void init(int nGrowth, int nPoint, int nHeart) {
            growth = nGrowth;
            point = nPoint;
            heart = nHeart;
        }
    };
    
    // item meta
    struct itemData {
        int price;
        string img;
        string name;
        
        void set(const string name, int price, const string img) {
            this->name = name;
            this->price = price;
            this->img = img;
        };
    };
    
    // selected
    struct itemSelected {
        bool isSelected[PLAY_ITEM_CNT];
        
        itemSelected() {
            for(int n = 0; n < PLAY_ITEM_CNT; n++) {
                isSelected[n] = true;
            }
        };
        
        int getTotalPoint() {
            int total = 0;
            for(int n = 0; n < PLAY_ITEM_CNT; n++) {
                if(isSelected[n])
                    total += battleBrix::inst()->mItems[n].price;
            }
            return total;
        };
    } mItemSelected;
    
    //grade
    struct grade {
        int grade;
        string title;
        float speed;
    };
    
    const string getText(const string& defaultString, int id);
    const float getProgressValue(int id);
    rewardData getReward(int ranking);
    bool applyReward(int ranking);
    inline float getGrowthPercentage() {
        return (float)mUserData.growth / (float)mUserData.maxGrowth;
    };
    // level up 이면 true
    bool increseGrowth(int val);
    const string getLevelString();
    //play 비용
    bool payForPlay(int point, int heart = 1);
    bool checkPayForPlay(int point, int heart = 1);
    //get my grade
    grade getMyGrade() {
        return mGrades[mUserData.grade];
    };
    
    rewardData mRewards[6];
    itemData mItems[PLAY_ITEM_CNT];
    vector<grade> mGrades;
    int mLastRanking;
    
private:
    static battleBrix * hInstance;
    int mGrowthPerLevel;
};
#endif /* battleBrix_h */
