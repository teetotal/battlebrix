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

bool SceneMain::init()
{
    gui::inst()->initDefaultWithSpriteCache("fonts/SDSwaggerTTF.ttf");
	this->loadFromJson("main", "main.json");
    
    for(int n=0; n<3; n++) {
        battleBrix::itemData item = battleBrix::inst()->mItems[n];
        auto sprite = gui::inst()->getSprite(item.img);
        
        int id = ID_NODE_ITEM_1 + (n*1000);
        
        ((ui_checkbox*)getNodeById(id+1))->setText(item.name);
        ((Sprite*)getNodeById(id+2))->setTexture(sprite->getTexture());
        ((ui_icon*)getNodeById(id+3))->setText(numberFormat(item.price));
    }

    sumPrice();
    
//    auto bg = getNodeById(0);
//    Vec2 center = gui::inst()->getCenter();
//    
//    COLOR_RGB color, colorFont, colorBack;
//    color.set(Color3B::MAGENTA);
//    colorFont.set(Color3B::BLUE);
//    colorBack.set(Color3B::WHITE);
//
//    ui_button::create(1234
//              , 12345
//              , "TESTj"
//              , this
//              , center
//              , ALIGNMENT_CENTER
//              , Size(50, 25)
//              , ui_button::TYPE_CIRCLE
//                      , std::bind(&SceneMain::fn, this, std::placeholders::_1, std::placeholders::_2)
//              , color
//              , colorFont
//              , colorBack
//              , ""//icons8-wedding-gift-96.png"
//              , ui_button::TOUCH_TYPE_AUTO_DISABLE
//              );
 
    
//    pCheckbox = ui_checkbox::create(this, center, ALIGNMENT_CENTER, Size(100, 50), "Check box", color, colorFont);

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
            this->replaceScene(ScenePlay::create());
            break;
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
    int price = 0;
    for(int n=0; n<3; n++) {
        int id = ID_NODE_ITEM_1 + (n*1000);
        if(((ui_checkbox*)getNodeById(id+1))->isChecked())
            price += battleBrix::inst()->mItems[n].price;
    }
    
    ((Label*)getNodeById(ID_NODE_POINT_SUBTRACT))->setString("- " + numberFormat(price));
    int remain = battleBrix::inst()->mUserData.point - price;
    
    if(remain < 0) {
//        ((Label*)getNodeById(ID_NODE_POINT_REMAIN))->setString("- " + numberFormat(remain * -1));
        ((ui_icon*)getNodeById(ID_NODE_POINT_REMAIN))->setText("- " + numberFormat(remain * -1));
        ((ui_button *)getNodeById(ID_NODE_START))->setEnabled(false);
    }
    else {
//        ((Label*)getNodeById(ID_NODE_POINT_REMAIN))->setString(numberFormat(remain));
        ((ui_icon*)getNodeById(ID_NODE_POINT_REMAIN))->setText(numberFormat(remain));
        ((ui_button *)getNodeById(ID_NODE_START))->setEnabled(true);
    }
}

//====================================================================
bool SceneDaily::init()
{
    mTodayIdx = 1;
   
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
    for(int n=0; n< 8; n++) {
        float fontSize = gui::inst()->getFontSize(Size(roulette->mRadius, roulette->mRadius));
        auto node = gui::inst()->createLabel(0, 0, to_string(100 + n), fontSize, ALIGNMENT_CENTER, Color3B::BLACK);
        roulette->insertItem(node);
    }
    
    roulette->setValue(getRandValue(360), CC_CALLBACK_1(SceneDaily::callbackRoulette, this));
    
    return true;
}

void SceneDaily::callbackRoulette(Ref* pSender) {
    ui_roulette * roulette = (ui_roulette *)(pSender);
    int idx = roulette->getResultIdx();
    
    roulette->setVisible(false);
    getNodeById(3000)->setVisible(true);
    auto todayImg = getNodeById(3001);
    todayImg->runAction(Sequence::create(DelayTime::create(.2f)
                                         , ScaleBy::create(.3f, 1.2f)
                                         , ScaleBy::create(.2f, (1.f / 1.2f))
                                         , DelayTime::create(0.5f)
                                         , CallFunc::create([=]() { this->replaceScene(SceneMain::create()); })
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
    
    return true;
}

void SceneShop::callback(Ref* pSender, int from, int link) {
    this->replaceScene(SceneMain::create());
}

const string SceneShop::getText(const string& defaultString, int id) {
    switch(id) {
        default:
            return battleBrix::inst()->getText(defaultString, id);
    }
}
