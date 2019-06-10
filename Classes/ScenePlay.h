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
    
    COLOR_RGB mColors[5];
    
protected:
    virtual void callback(Ref* pSender, int from, int link);
    virtual const string getText(const string& defaultString, int id);
    
private:
    bool onTouchBegan(Touch* touch, Event* event) ;
    bool onTouchEnded(Touch* touch, Event* event);
    void onTouchMoved(Touch *touch, Event *event);
    bool onContactBegin(PhysicsContact &contact);
    
    //Node * mLayer, * mLayerOther, * mBall;
    
    struct PLAYER {
        Node * layer;
        Node * ball;
        Node * board;
        Node * obstacles[8][8];
        ui_progressbar * hp, * mp;
        bool lockShake;
        clock_t latestCollisionWithBoard;
        ScenePlay * pScene;
        
        Size gridSize, obstacleSize;
        float fontSizeCombo;
        
        PLAYER(){
            lockShake = false;
            latestCollisionWithBoard = 0;
        };
        void init(ScenePlay* p, int layerId, int hpId, int mpId, int ballId);
        void vibrate();
        bool onContact(int id, bool toRight = false);
        void decreseHP();
        void createBall(int ballId);
        void createBoard();
        void createBottom();
        void addObstacle(Vec2 pos);
        
    };
    
    PLAYER mPlayers[2];
    
    
};
#endif // __SCENE_PLAY_H__
