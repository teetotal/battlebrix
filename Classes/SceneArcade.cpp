//
//  SceneArcade.cpp
//
//  Created by Jung, DaeCheon on 19/07/2019.
//

#include "SceneArcade.h"
#include "Scenes.h"
#include "ui/ui_ext.h"
#include "ui/ui_icon.h"
#include "ui/ui_character_animal.h"

bool SceneArcade::init()
{
    gui::inst()->initDefaultWithSpriteCache("fonts/SDSwaggerTTF.ttf");
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
    
    //init map
    for(int n=101; n <= 120; n++) {
        Layer * layer = (Layer*)getNodeById(n);
        
        if(n == 105) {
            ui_character_animal * animal = ui_character_animal::create(layer
                                                                       , layer->getContentSize()
                                                                       , Vec2::ZERO
                                                                       , ALIGNMENT_LEFT_BOTTOM
                                                                       , ui_wizard_share::inst()->getPalette()->getColor("YELLOW")
                                                                       , ui_wizard_share::inst()->getPalette()->getColor("DARKGRAY")
                                                                       , ui_wizard_share::inst()->getPalette()->getColor("PINK")
                                                                       , ui_wizard_share::inst()->getPalette()->getColor("TRANSPARENT"));
            animal->addCircle();
            animal->runAction(RepeatForever::create(
                                                    Sequence::create(ScaleTo::create(0.5, 1.2), ScaleTo::create(0.5, 1), NULL)
                              ));
        }
        else {
            const string szColor = (n > 105) ? "GRAY" : "YELLOW";
            ui_icon::create()->addCircle(layer
                                         , gui::inst()->getGridSize(layer->getContentSize(), Vec2(1, 1), Vec2::ZERO, Vec2::ZERO)
                                         , gui::inst()->getPointVec2(0, 0, ALIGNMENT_CENTER, layer->getContentSize(), Vec2(1, 1), Vec2::ZERO, Vec2::ZERO)
                                         , ALIGNMENT_CENTER
                                         , ui_wizard_share::inst()->getPalette()->getColor(szColor)
                                         , to_string(n - 100));
        }
    }
    
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
