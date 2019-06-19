//
//  battleBrix.hpp
//  battlebrix-mobile
//
//  Created by Jung, DaeCheon on 13/06/2019.
//

#ifndef battleBrix_h
#define battleBrix_h

#include <string>
using namespace std;

enum _ID_NODE {
    _ID_NODE_LABEL_ID = 1,
    _ID_NODE_LABEL_POINT = 3,
    _ID_NODE_LABEL_HEART = 4,
    _ID_NODE_LABEL_LEVEL = 12,
    _ID_NODE_PROGRESSBAR = 13,
    _ID_NODE_PROGRESSBAR_LABEL_WINNING_RATE = 14,
    _ID_NODE_LABEL_WINNING_RATE = 15,
    _ID_NODE_LABEL_RANKING = 17,
};

class battleBrix {
public:
    battleBrix() {};
    virtual ~battleBrix() {};
    static battleBrix * inst() {
      if(hInstance == NULL)
          hInstance = new battleBrix;
        return hInstance;
    };
    void init();
    
    struct userData {
        int win;
        int lose;
        int ranking;
        int level;
        int grade;
        int heart;
        int heartMax;
        int point;
        
        int maxGrowth;
        int growth;
        string id;
        
        userData() : win(0), lose(0), ranking(532340), level(1), grade(0), heart(0), heartMax(5), point(1000), maxGrowth(128), growth(10) {
            id = "teetotal";
        };
        
    } mUserData;
    
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
    
private:
    static battleBrix * hInstance;
    rewardData mRewards[6];
};
#endif /* battleBrix_h */
