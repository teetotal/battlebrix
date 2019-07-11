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
#include "library/sql.h"

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
    enum TYPE {
        TYPE_NORMAL = 0,
        TYPE_WALL,
        TYPE_GIFT,
        TYPE_TRAP
    };
    
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
        TYPE type;
        vector<position> path;
        void load(rapidjson::Value &p);
    };
    //brix brixPosition
    struct brixPosition {
        int minGrade;
        int maxGrade;
        string title;
        vector<position> statics;
        vector<brixMovement> movements;
        
        void load(rapidjson::Value &p);
    };
    
    vector<brixPosition> mBrixMap;
    
    int getMapRandom();
    
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
    bool init();
    
    //user data
    struct userData {
        string id;
        int ranking;
        int grade;
        int heart;
        int heartMax;
        time_t heartTimerStart;
        int point;
        int maxGrowth;
        int growth;
        
        
        int growthPerLevel;
        int rechargeTime;
        std::mutex lock;
        
        userData() :
                     ranking(532340)
                    , grade(1)
                    , heart(8)
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
                    increaseHeart();
                    setHeartTimerStart(heartTimerStart + rechargeTime);
                    ret = true;
                } else {
                    break;
                }
            }
            
            return ret;
        };
        
        void increaseHeart(int n=1);
        
        void increasePoint(int n);
        
        void setHeartTimerStart(time_t t);
        
        bool increseGrowth(int val);

        
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
    
    struct itemProperty {
        float hpAttack; //공격 HP
        vector<int> attackTarget; //공격 대상
        float hpRecharge; // 충전 HP
        
        void set(float attack, float recharge) {
            hpAttack = attack;
            hpRecharge = recharge;
        };
    };
    
    // item meta
    struct itemData {
        int price;
        string img;
        string name;
        
        //property
        itemProperty property;
        
        void set(const string name, int price, const string img) {
            this->name = name;
            this->price = price;
            this->img = img;
        };
    };
    
    // selected
    struct itemSelected {
        vector<bool> isSelected;
        
        void set(int cnt) {
            for(int n = 0; n < cnt; n++) {
                isSelected.push_back(true);
            }
        };
        
        int getTotalPoint() {
            int total = 0;
            for(int n = 0; n < isSelected.size(); n++) {
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
        float delay;
        int IQ;
        int skillQuantity;
    };
    
    const string getText(const string& defaultString, int id);
    const float getProgressValue(int id);
    rewardData getReward(int ranking);
    bool applyReward(int ranking);
    inline float getGrowthPercentage() {
        return (float)mUserData.growth / (float)mUserData.maxGrowth;
    };
    // level up 이면 true
//    bool increseGrowth(int val);
    const string getLevelString();
    //play 비용
    bool payForPlay(int point, int heart = 1);
    bool checkPayForPlay(int point, int heart = 1);
    //get my grade
    grade getMyGrade() {
        return mGrades[mUserData.grade];
    };
    
    rewardData mRewards[6];
    vector<itemData> mItems;
    vector<grade> mGrades;
    int mLastRanking;
    
private:
    static battleBrix * hInstance;
};
#endif /* battleBrix_h */
