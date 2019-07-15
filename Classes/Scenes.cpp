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

#include "Scenes.h"
#include "SimpleAudioEngine.h"
#include "ScenePlay.h"
#include "ui/ui_ext.h"
#include "library/pch.h"
#include "ui/ui_character_animal.h"

#define HEART_TIMER     this->schedule([=](float f){\
if(battleBrix::inst()->mUserData.recharge()){\
    auto p = ((ui_icon*)getNodeById(_ID_NODE_LABEL_HEART));\
    p->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_HEART));\
    guiExt::runScaleEffect(p);\
}\
((ui_button*)getNodeById(_ID_NODE_TIMER_HEART))->setText(battleBrix::inst()->mUserData.getRechargeRemainTimeString());\
}, 1.f, "heartTimer");

bool SceneMain::init()
{
    gui::inst()->initDefaultWithSpriteCache("fonts/SDSwaggerTTF.ttf");
	this->loadFromJson("main", "main.json");
    
    for(int n = 0; n < PLAY_ITEM_CNT; n++) {
        battleBrix::itemData item = battleBrix::inst()->mItems[n];
        auto sprite = gui::inst()->getSprite(item.img);
        
        int id = ID_NODE_ITEM_1 + (n*1000);
        
        auto checkBox = ((ui_checkbox*)getNodeById(id+1));
        checkBox->setText(item.name);
        bool isChecked = battleBrix::inst()->mItemSelected.isSelected[n];
        checkBox->setChecked(isChecked);
        getNodeById(id+4)->setVisible(isChecked);
            
        ((Sprite*)getNodeById(id+2))->setTexture(sprite->getTexture());
        ((ui_icon*)getNodeById(id+3))->setText(numberFormat(item.price));
    }

    sumPrice();
    
    //timer
    HEART_TIMER


    return true;
}

void SceneMain::callback(Ref* pSender, int from, int link) {
    if(link >= ID_NODE_ITEM_1 && link <= ID_NODE_ITEM_3) {
        auto p = (ui_checkbox*)getNodeById(link+1);
        p->setToggle();
        ((ui_checkbox*)getNodeById(link+4))->setVisible(p->isChecked());
        
        sumPrice();
        return;
    }
    switch(link) {
        case eLINK_PLAY:
            return runPlay();
        case eLINK_SHOP:
            this->replaceScene(SceneShop::create());
            break;
        case eLINK_LEADERBOARD:
            break;
        case eLINK_FRIENDS:
            break;
        case eLINK_BAG:
            break;
        default:
            break;
    }
}

const string SceneMain::getText(const string& defaultString, int id) {
    switch(id) {
        case ID_NODE_POINT:
            return numberFormat(battleBrix::inst()->mUserData.point);
        case ID_NODE_POINT_SUBTRACT:
        case ID_NODE_POINT_REMAIN:
        default:
            return battleBrix::inst()->getText(defaultString, id);
    }
}

void SceneMain::sumPrice() {
    //맘에 안드는 코드일세
    for(int n = 0; n < PLAY_ITEM_CNT; n++) {
        battleBrix::inst()->mItemSelected.isSelected[n] = false;
        int id = ID_NODE_ITEM_1 + (n * 1000);
        if(((ui_checkbox*)getNodeById(id+1))->isChecked())
            battleBrix::inst()->mItemSelected.isSelected[n] = true;
    }
    
    int price = battleBrix::inst()->mItemSelected.getTotalPoint();
    
    ((Label*)getNodeById(ID_NODE_POINT_SUBTRACT))->setString("- " + numberFormat(price));
    int remain = battleBrix::inst()->mUserData.point - price;
    
    if(remain < 0) {
        ((ui_button *)getNodeById(ID_NODE_START))->setEnabled(false);
    }
    else {
        ((ui_button *)getNodeById(ID_NODE_START))->setEnabled(true);
    }
    ((ui_icon*)getNodeById(ID_NODE_POINT_REMAIN))->setText(numberFormat(remain));
}

void SceneMain::runPlay() {
    int totalPoint = battleBrix::inst()->mItemSelected.getTotalPoint();
    // check validation
    if(battleBrix::inst()->mUserData.heart < 1) {
        //alert
    }
    
//    if(battleBrix::inst()->mUserData.point < totalPoint) {
//    }
    
    // pay and change scene
    if(battleBrix::inst()->payForPlay(totalPoint))
    {
        CallFunc * next;
        if(battleBrix::inst()->mUserData.increseExp()) { //levelup
            next = CallFunc::create([=](){
                showLevelUp();
            });
        } else {
            next = CallFunc::create([=]() { // go to playscene
                this->replaceScene(ScenePlay::create());
            });
        }
        // 효과
        ((ui_icon*)getNodeById(_ID_NODE_LABEL_POINT))->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_POINT));
        guiExt::runScaleEffect(getNodeById(_ID_NODE_LABEL_POINT));
        
        ((ui_icon*)getNodeById(_ID_NODE_LABEL_HEART))->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_HEART));
        guiExt::runScaleEffect(getNodeById(_ID_NODE_LABEL_HEART), next);
    }
}

void SceneMain::showLevelUp() {
    
    CallFunc * p = CallFunc::create([=]() { this->replaceScene(ScenePlay::create()); });
    LEVELUP_EVENT(p)
}
//====================================================================
bool SceneDaily::init()
{
    //animal
//    ui_character_animal * ani = ui_character_animal::create();
//    ani->addRectangle(Size(300, 200)
//                   , ui_wizard_share::inst()->getPalette()->getColor3B("BLUE")
//                   , ui_wizard_share::inst()->getPalette()->getColor4F("YELLOW")
//                   , ui_wizard_share::inst()->getPalette()->getColor4F("DARKGRAY")
//                   , ui_wizard_share::inst()->getPalette()->getColor4F("PINK")
//                   );
//    ani->addCircle(Size(125, 125)
//                   , ui_wizard_share::inst()->getPalette()->getColor3B("BLUE")
//                   , ui_wizard_share::inst()->getPalette()->getColor4F("YELLOW")
//                   , ui_wizard_share::inst()->getPalette()->getColor4F("DARKGRAY")
//                   , ui_wizard_share::inst()->getPalette()->getColor4F("PINK")
//                   );
//    ani->setAnchorPoint(Vec2(0.5, 0.5));
//    ani->setPosition(gui::inst()->getCenter());
//    this->addChild(ani);
//    return true;
    
    mTodayIdx = 2;
   
    gui::inst()->initDefaultWithSpriteCache("fonts/SDSwaggerTTF.ttf");
    this->loadFromJson("daily", "daily.json");
    //disable
    for(int n = 1; n < mTodayIdx; n++) {
        auto disable = getNodeById(100 + n);
        disable->setVisible(true);
    }
    //today
    Node * todayTitle = *getNodePointerById(mTodayIdx * 10 + 1);
    todayTitle->setColor(ui_wizard_share::inst()->getPalette()->getColor3B("YELLOW"));
    todayTitle->runAction(Sequence::create(DelayTime::create(0.2f)
                                           , ScaleBy::create(.3f, 1.2f)
                                           , ScaleBy::create(.2f, (1.f / 1.2f))
                                           , DelayTime::create(0.2f)
                                           , NULL));
    getNodeById(mTodayIdx)->setVisible(true);
    
    //roulette
    mRoulette = (ui_roulette*)getNodeById(2001);
    
    ui_roulette * roulette = (ui_roulette*)(mRoulette);
    Size size = Size(roulette->mRadius, roulette->mRadius);
    
    mRewards[0] = 100;
    mRewards[1] = -1;
    mRewards[2] = 100;
    mRewards[3] = -1;
    mRewards[4] = 150;
    mRewards[5] = -2;
    mRewards[6] = 350;
    mRewards[7] = -3;
    
    for(int n=0; n< 8; n++) {
        //float fontSize = gui::inst()->getFontSize(Size(roulette->mRadius, roulette->mRadius));
        //auto node = gui::inst()->createLabel(0, 0, to_string(100 + n), fontSize, ALIGNMENT_CENTER, Color3B::BLACK);
        auto node = ui_icon::create();
        node->setGlow(false);
        if(mRewards[n] > 0) {
            node->addCircle(NULL
                            , size
                            , Vec2::ZERO
                            , ALIGNMENT_CENTER
                            , ui_wizard_share::inst()->getPalette()->getColor("YELLOW")
                            , "P"
                            , ""
                            , to_string(mRewards[n])
                            , ui_wizard_share::inst()->getPalette()->getColor("DARKGRAY"));
        } else {
            node->addHeart(NULL, size, Vec2::ZERO, ALIGNMENT_CENTER, "x" + to_string(mRewards[n] * -1), ui_wizard_share::inst()->getPalette()->getColor("DARKGRAY"), -3);
        }
        
        
        roulette->insertItem(node);
    }
    
    roulette->setValue(getRandValue(360), CC_CALLBACK_1(SceneDaily::callbackRoulette, this));
    
    return true;
}

void SceneDaily::callbackRoulette(Ref* pSender) {
    ui_roulette * roulette = (ui_roulette *)(pSender);
    
    roulette->setVisible(false);
    getNodeById(3000)->setVisible(true);
    int val = mRewards[roulette->getResultIdx()];
    ui_icon * p;
    if(val > 0) {
        p = (ui_icon*)getNodeById(3001);
        battleBrix::inst()->mUserData.increasePoint(val);
        p->setText(to_string(val));
    } else {
        p = (ui_icon*)getNodeById(3002);
        val = val * -1;
        battleBrix::inst()->mUserData.increaseHeart(val);
        p->setText("x" + to_string(val));
    }
    
    p->setVisible(true);
    
    p->runAction(Sequence::create(DelayTime::create(.2f)
                                 , ScaleBy::create(.3f, 1.2f)
                                 , ScaleBy::create(.2f, (1.f / 1.2f))
                                 , DelayTime::create(0.5f)
                                 , CallFunc::create([=]() {
        this->replaceScene(SceneMain::create());
    })
                                         , NULL));
}

void SceneDaily::callback(Ref* pSender, int from, int link) {
    getNodeById(1000)->setVisible(true);
    auto roulette = (ui_roulette*)getNodeById(2000);
    roulette->setVisible(true);
    
    gui::inst()->setModal(roulette);
    
    
    
    /*
    auto todayImg = getNodeById(1001);
//    std::function<void()> callFn = [=]() {
//        this->replaceScene(SceneMain::create());
//    };
    std::function<void()> callFn = std::bind(&SceneDaily::actionFinished, this);
    todayImg->runAction(Sequence::create(DelayTime::create(0.2f)
                                         , ScaleBy::create(.3f, 1.2f)
                                         , ScaleBy::create(.2f, (1.f / 1.2f))
                                         , DelayTime::create(0.2f)
                                         , CallFunc::create(callFn)
                                         , NULL));
     */
}

const string SceneDaily::getText(const string& defaultString, int id) {
//    if(id == mTodayId) {
//        return "TODAY";
//    } else if(id < mTodayId && id < 10 && id > 0) {
//        return "Done";
//    }
    if(id >= 1000)
        return defaultString;
    
    int nDay = id / 10;
    int nPart = id % 10;
    switch (nPart) {
        case 1: //날짜
        {
            if(mTodayIdx == nDay)
                return "TODAY";
            return "DAY" + to_string(nDay);
        }
        case 2: //이미지
        case 3: //quantity
        default:
            break;
    }
    
    return defaultString;
}

void SceneDaily::actionFinished() {
    this->replaceScene(SceneMain::create());
}
//====================================================================
bool SceneShop::init()
{
    this->loadFromJson("shop", "shop.json");
    //timer
    HEART_TIMER
    return true;
}

void SceneShop::callback(Ref* pSender, int from, int link) {
    CCLOG("from = %d, link = %d", from, link);
    this->replaceScene(SceneMain::create());
}

const string SceneShop::getText(const string& defaultString, int id) {
    switch(id) {
        default:
            return battleBrix::inst()->getText(defaultString, id);
    }
}
