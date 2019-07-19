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
#include "ui/ui_ext.h"
#include "library/pch.h"
#include "ui/ui_effect.h"

#include "battleBrix.h"
USING_NS_CC;

enum eLINK {
    eLINK_SHOP = 0,
    eLINK_ARCADE,
    eLINK_MULTI,
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
// macro
#define LEVELUP_EVENT(callFunc) Node * layer = this->getNodeById(30000);\
        layer->setVisible(true);\
        layer->addChild(gui::inst()->createParticle("firework.plist", gui::inst()->getCenter(layer)));\
        Label * title = ((Label*)this->getNodeById(30001));\
        title->enableBold();\
        Label * level = ((Label*)this->getNodeById(30002));\
        level->enableBold();\
        level->setString(battleBrix::inst()->getText("", _ID_NODE_LABEL_LEVEL));\
        guiExt::runScaleEffect(title, callFunc, 1.f, false);

#define HEART_TIMER     this->schedule([=](float f){\
        if(battleBrix::inst()->mUserData.recharge()){\
            auto p = ((ui_icon*)getNodeById(_ID_NODE_LABEL_HEART));\
            p->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_HEART));\
            guiExt::runScaleEffect(p);\
        }\
        ((ui_button*)getNodeById(_ID_NODE_TIMER_HEART))->setText(battleBrix::inst()->mUserData.getRechargeRemainTimeString());\
        }, 1.f, "heartTimer");

#define LINK switch(link) {\
        case eLINK_SHOP:\
            this->replaceScene(SceneShop::create());\
            break;\
        case eLINK_ARCADE:\
            this->replaceScene(SceneArcade::create());\
            break;\
        case eLINK_MULTI:\
            this->replaceScene(SceneMain::create());\
            break;\
        case eLINK_FRIENDS:\
            break;\
        case eLINK_BAG:\
            break;\
        default:\
            break;\
    }

#endif // __SCENES_H__
