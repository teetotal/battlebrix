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

bool SceneMain::init()
{
	this->loadFromJson("main", "main.json", "palette.json");
    auto bg = getNodeById(0);
    
//    guiExt::drawCircleForPhysics(bg, Vec2(50, 50), 100, Color4F(0.5, 0.5, 0.5, 0.5))
    gui::inst()->drawCircle(bg, Vec2::ZERO, 100, Color4F(1, 1, 1, 0.5))->runAction(
       RepeatForever::create(
                             Sequence::create(
                                              MoveTo::create(30, Vec2(bg->getContentSize().width, bg->getContentSize().height))
                                              , MoveTo::create(30, Vec2::ZERO)
                                              , NULL)
       ));
    
    this->pushScene(SceneDaily::create());
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
    mTodayId = 2;
   
    gui::inst()->initDefaultWithSpriteCache("fonts/SDSwaggerTTF.ttf");
    this->loadFromJson("daily", "daily.json", "palette.json");
    //today
    auto todayTitle = getNodeById(mTodayId);
    todayTitle->setColor(Color3B(240, 208, 75));
    todayTitle->runAction(Sequence::create(DelayTime::create(0.2f)
                                           , ScaleBy::create(.3f, 1.2f)
                                           , ScaleBy::create(.2f, (1.f / 1.2f))
                                           , DelayTime::create(0.2f)
                                           , NULL));
    getNodeById(10+mTodayId)->setVisible(true);
    
    return true;
}

void SceneDaily::callback(Ref* pSender, int from, int link) {
    
    auto today = getNodeById(100);
    today->setVisible(true);
    
    gui::inst()->setModal(today);
    
    auto todayImg = getNodeById(101);
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
}

const string SceneDaily::getText(const string& defaultString, int id) {
    if(id == mTodayId) {
        return "TODAY";
    } else if(id < mTodayId && id < 10 && id > 0) {
        return "Done";
    }
    
    return defaultString;
}

void SceneDaily::actionFinished() {
    this->replaceScene(SceneMain::create());
}

//====================================================================
bool SceneShop::init()
{
    this->loadFromJson("shop", "shop.json", "palette.json");
    return true;
}

void SceneShop::callback(Ref* pSender, int from, int link) {
    this->replaceScene(SceneMain::create());
}

const string SceneShop::getText(const string& defaultString, int id) {
    
    return defaultString;
}
