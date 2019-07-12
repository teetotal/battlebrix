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

#ifndef __SCENES_H__
#define __SCENES_H__

#include "ui/ui_wizard.h"
#include "ui/ui_roulette.h"
#include "ui/ui_button.h"
#include "battleBrix.h"
USING_NS_CC;

enum eLINK {
    eLINK_PLAY = 0,
    eLINK_SHOP,
    eLINK_LEADERBOARD,
    eLINK_FRIENDS,
    eLINK_BAG
};

class SceneMain : public ui_wizard
{
public:
    static Scene* createScene() {
        return SceneMain::create();
    };
    virtual bool init();
    CREATE_FUNC(SceneMain);
    
protected:
    virtual void callback(Ref* pSender, int from, int link);
    virtual const string getText(const string& defaultString, int id);
    virtual const float getProgressValue(int id) { return battleBrix::inst()->getProgressValue(id); };
private:
    enum ID_NODE {
        ID_NODE_POINT = 101,
        ID_NODE_POINT_SUBTRACT,
        ID_NODE_POINT_REMAIN,
        
        ID_NODE_START,
        
        ID_NODE_ITEM_1 = 10000,
        ID_NODE_ITEM_1_CHECKBOX,
        ID_NODE_ITEM_1_SPRITE,
        ID_NODE_ITEM_1_PRICE,
        ID_NODE_ITEM_1_OUTLINE,
        
        ID_NODE_ITEM_2 = 11000,
        ID_NODE_ITEM_2_CHECKBOX,
        ID_NODE_ITEM_2_SPRITE,
        
        ID_NODE_ITEM_3 = 12000,
        ID_NODE_ITEM_3_CHECKBOX,
        ID_NODE_ITEM_3_SPRITE,
    };
    
    void sumPrice();
    void runPlay();
    void showLevelUp();
};
// SceneDaily ==================================
class SceneDaily : public ui_wizard
{
public:
    static Scene* createScene() {
        return SceneDaily::create();
    };
    virtual bool init();
    CREATE_FUNC(SceneDaily);
    
protected:
    virtual void callback(Ref* pSender, int from, int link);
    virtual const string getText(const string& defaultString, int id);
    virtual const float getProgressValue(int id) { return 0.f; };
private:
    void actionFinished();
    int mTodayIdx;
    ui_roulette * mRoulette;
    void callbackRoulette(Ref* pSender);
    int mRewards[8];
};
// SceneShop ==================================
class SceneShop : public ui_wizard
{
    public:
    static Scene* createScene() {
        return SceneShop::create();
    };
    virtual bool init();
    CREATE_FUNC(SceneShop);
    
    protected:
    virtual void callback(Ref* pSender, int from, int link);
    virtual const string getText(const string& defaultString, int id);
    virtual const float getProgressValue(int id) { return battleBrix::inst()->getProgressValue(id); };
};
#endif // __SCENES_H__
