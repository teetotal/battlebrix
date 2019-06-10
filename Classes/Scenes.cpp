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
#include "library/util.h"

bool SceneMain::init()
{
    gui::inst()->initDefaultWithSpriteCache("fonts/SDSwaggerTTF.ttf");
	this->loadFromJson("main", "main.json");
//    auto bg = getNodeById(0);
    
//    guiExt::drawCircleForPhysics(bg, Vec2(50, 50), 100, Color4F(0.5, 0.5, 0.5, 0.5))
//    gui::inst()->drawCircle(bg, Vec2::ZERO, 100, Color4F(1, 1, 1, 0.5))->runAction(
//       RepeatForever::create(
//                             Sequence::create(
//                                              MoveTo::create(30, Vec2(bg->getContentSize().width, bg->getContentSize().height))
//                                              , MoveTo::create(30, Vec2::ZERO)
//                                              , NULL)
//       ));
//
    return true;
}

void SceneMain::callback(Ref* pSender, int from, int link) {
    switch((eLINK)link) {
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
    return defaultString;
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
    
    return defaultString;
}
