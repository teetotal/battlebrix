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
    
    COLOR_RGB mColors[10];
    float mBrixLayerRatio;
    
protected:
    virtual void callback(Ref* pSender, int from, int link);
    virtual const string getText(const string& defaultString, int id);
    virtual const float getProgressValue(int id) { return 0; };
private:
    bool onTouchBegan(Touch* touch, Event* event) ;
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
    void onFinish();
    void onEnd();
    void onSkill(int idx);
    
    struct OBSTACLE {
        DrawNode * pDrawNode;
        int typeCode;
        Vec2 position;
    };
    
    struct PLAYER {
        string name;
        Node * layer, * layerBrix;
        Node * ball;
        Node * board;
        Node * alert;
        Label * label, * labelName;
        
        int ballId;
        int combo;
        
        //map<int, Vec2> obstaclesPos;
        map<int, bool> brixEffectFlagMap;
        
        ui_progressbar * hp, * mp;
        bool lockShake;
        clock_t latestCollisionWithBoard;
        ScenePlay * pScene;
        
        Size gridSize, obstacleSize;
        float fontSizeCombo;
        
        bool isEnd;
        int ranking;
        
        PLAYER(){
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
        };
        void init(ScenePlay* p, const string& name, int layerId, int hpId, int mpId, int ballId, int alertId, int labelId, int fnId);
        void finish();
        void vibrate();
        bool onContact(int id, bool toRight = false);
        void decreseHP(float val = DECREASE_HP);
        void createBall();
        void createBoard();
        void createBottom();
        
        void addBrix0();
        void addBrix1();
        void addBrix2();
        void addBrix3();
        void addBrix4();
        
        const float getHPValue() {
            return hp->getValue();
        };
        void onTimer(float f);
        void onBomb();
        void setRanking(int ranking);
        void createLayerBrix();
    };
    vector<PLAYER> mPlayers;
    vector<ui_icon*> mSkills;
    Node * mSkillParentNode; //for multi touch
    Node * mControlBar; //for check multi touch
    
    bool mIsEnd;
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
    virtual const string getText(const string& defaultString, int id) { return defaultString; };
    virtual const float getProgressValue(int id) { return battleBrix::inst()->getProgressValue(id); };
};
#endif // __SCENE_PLAY_H__
