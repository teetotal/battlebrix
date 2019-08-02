/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __SCENE_PLAY_H__
#define __SCENE_PLAY_H__

#include "ui/ui_wizard.h"
#include "ui/ui_progressbar.h"
#include "ui/ui_icon.h"
#include "battleBrix.h"

#define DECREASE_HP 0.05f
USING_NS_CC;

// ScenePlay ==================================
class ScenePlay : public ui_wizard
{
public:
    static Scene* createScene() {
        return ScenePlay::create();
    };
    virtual bool init();
    CREATE_FUNC_PHYSICS(ScenePlay);
    
    vector<COLOR_RGB> mColors;
    float mBrixLayerRatio;
    
protected:
    virtual void callback(Ref* pSender, int from, int link);
    virtual const string getText(const string& defaultString, int id);
    virtual const float getProgressValue(int id) { return 0; };
private:
    bool onTouchBegan(Touch* touch, Event* event);
    bool onTouchEnded(Touch* touch, Event* event);
    void onTouchMoved(Touch *touch, Event *event);
    bool onContactBegin(PhysicsContact &contact);
    
    void onTouchesBegan(const vector<Touch*>&touches, Event* event) {
        for(int n=0; n<touches.size(); n++) onTouchBegan(touches[n], event);
    };
    void onTouchesEnded(const vector<Touch*>&touches, Event* event) {
        for(int n=0; n<touches.size(); n++) onTouchEnded(touches[n], event);
    };
    void onTouchesMoved(const vector<Touch*>&touches, Event* event) {
        for(int n=0; n<touches.size(); n++) onTouchMoved(touches[n], event);
    };
    
    void timer(float f);
    void timerLoose(float f);
    void onFinish();
    void onEnd();
    void onSkill(int idx, int from);
    void attack(int from, int itemIdx);
    
    struct OBSTACLE {
        DrawNode * pDrawNode;
        int typeCode;
        Vec2 position;
    };
    
    struct PLAYER {
        int idx;
        string name;
        Node * layer, * layerBrix;
        Node * ball;
        Node * board;
        Node * alert;
        Label * label, * labelName;
        ui_icon * skills[PLAY_ITEM_CNT];
        int IQ;
        int ballId;
        int combo;
        Vec2 preBallPosition;
        float skillThreshold;
        
        //map<int, Vec2> obstaclesPos;
        map<int, bool> brixEffectFlagMap;
        
        ui_progressbar * hp, * mp;
        bool lockShake;
        clock_t latestCollisionWithBoard;
        ScenePlay * pScene;
        
        Size gridSize, obstacleSize;
        float fontSizeCombo;
        
        bool isEnd;
        int dangerousStatus;
        int ranking;
        
        PLAYER() {
            ranking = -1;
            skillThreshold = 0.f;
            dangerousStatus = 0;
            lockShake = false;
            latestCollisionWithBoard = 0;
            combo = 0;
            layerBrix = NULL;
            ball = NULL;
            board = NULL;
            layer = NULL;
            alert = NULL;
            label = NULL;
            isEnd = false;
            preBallPosition = Vec2(100, 100); //충분히 큰값으로 초기화
            for(int n=0; n<PLAY_ITEM_CNT; n++)  {
                skills[n] = NULL;
            }
        };
        void init(ScenePlay* p, int idx, const string& name, int layerId, int hpId, int mpId, int ballId, int alertId, int labelId, int fnId, int IQ);
        void finish();
        void vibrate();
        bool onContact(int id, bool toRight = false);
        void decreseHP(const string from, float val = DECREASE_HP);
        void createBall();
        void createBoard();
        void createBottom();
        void createSkill(int skillQuantity);
        
        void addBrix(int idx);
        Node * createBrix(brixMap::position pos, int id);
        
        Sprite * createBrixFromSprite(brixMap::position pos, int id, const string img);
        Sprite * createGiftOrTrapEffect(Vec2 pos, brixMap::TYPE type, CallFunc * fn);
        
        const float getHPValue() {
            return hp->getValue();
        };
        void skill();
        void setBackgroundStatus();
        bool onCombo(int id);
        void onTimer(float f);
        void onBomb(const string from, int itemIdx);
        void setRanking(int ranking);
        void createLayerBrix();
    };
    vector<PLAYER> mPlayers;
    vector<ui_icon*> mSkills;
    Node * mSkillParentNode; //for multi touch
    Node * mControlBar; //for check multi touch
    
    bool mIsEnd;
    mutex mLock;
};


// SceneEnding ==================================
class SceneEnding : public ui_wizard
{
public:
    static Scene* createScene() {
        return SceneEnding::create();
    };
    virtual bool init();
    CREATE_FUNC(SceneEnding);
    
protected:
    virtual void callback(Ref* pSender, int from, int link);
    virtual const string getText(const string& defaultString, int id) { return battleBrix::inst()->getText(defaultString, id); };
    virtual const float getProgressValue(int id) { return battleBrix::inst()->getProgressValue(id); };
    
private:
    void onAgain();
};
#endif // __SCENE_PLAY_H__
