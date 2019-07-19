//
//  SceneArcade.cpp
//
//  Created by Jung, DaeCheon on 19/07/2019.
//

#include "SceneArcade.h"
#include "Scenes.h"
#include "ui/ui_ext.h"
#include "ui/ui_icon.h"

bool SceneArcade::init()
{
    this->loadFromJson("arcade", "arcade.json");
    
    TOUCH_INIT(SceneArcade);
    
    //bottom disable
    getNodeById(_ID_NODE_BOTTOM_ARCADE)->setVisible(false);
    getNodeById(_ID_NODE_BOTTOM_SELECTED_ARCADE)->setVisible(true);
    //timer
    HEART_TIMER
    //
    mLayer = getNodeById(100);
    mMoveMin = (mLayer->getContentSize().width * -1) + (gui::inst()->mVisibleSize.width / 2.f);
    mMoveMax = gui::inst()->mVisibleSize.width / 2.f;
    return true;
}

void SceneArcade::callback(Ref* pSender, int from, int link) {
    LINK
}

const string SceneArcade::getText(const string& defaultString, int id) {
    switch(id) {
        default:
            return battleBrix::inst()->getText(defaultString, id);
    }
}

// touch ===========================================================================
bool SceneArcade::onTouchBegan(Touch* touch, Event* event) {
    mTouchStart = touch->getLocation();
    return true;
}
bool SceneArcade::onTouchEnded(Touch* touch, Event* event) {
    return false;
}

void SceneArcade::onTouchMoved(Touch *touch, Event *event) {
    float moved = mTouchStart.x - touch->getLocation().x;
    Vec2 from = mLayer->getPosition();
    Vec2 to = Vec2(from.x - moved, from.y);
    if(to.x < mMoveMin)
        to.x = mMoveMin;
    if(to.x > mMoveMax)
        to.x = mMoveMax;
    mLayer->setPosition(to);
    
    mTouchStart = touch->getLocation();
}
