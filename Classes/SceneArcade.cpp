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
    mMoveMin = (mLayer->getContentSize().width * -1) + (gui::inst()->mVisibleSize.width);
    mMoveMax = 0;//gui::inst()->mVisibleSize.width / 2;
    
    auto layerLine = getNodeById(99);
    //init map
    Vec2 lastPos = Vec2::ZERO;
    for(int n=101; n <= 114; n++) {
        Layer * layer = (Layer*)getNodeById(n);
        Vec2 pos = layer->getPosition();
        pos.x += layer->getContentSize().width / 2;
        pos.y -= layer->getContentSize().height / 2;
        if(lastPos != Vec2::ZERO) {
            gui::inst()->drawLine(layerLine
                                  , lastPos
                                  , pos
                                  , ui_wizard_share::inst()->getPalette()->getColor4F("WHITE_OPACITY_DEEP")
                                  , 4.f
                                  );
        }
        lastPos = pos;
        
        if(n == 105) {
            ui_character_animal * animal = ui_character_animal::create(layer
                                                                       , layer->getContentSize()
                                                                       , gui::inst()->getCenter(layer)
                                                                       , ALIGNMENT_CENTER
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
            const string szColor = (n > 105) ? "DARKGRAY_LIGHT" : "BLUE";
            const string szStar = "★☆☆";
            ui_icon::create()->addCircle(layer
                                         , gui::inst()->getGridSize(layer->getContentSize(), Vec2(1, 1), Vec2::ZERO, Vec2::ZERO)
                                         , gui::inst()->getPointVec2(0, 0, ALIGNMENT_CENTER, layer->getContentSize(), Vec2(1, 1), Vec2::ZERO, Vec2::ZERO, Vec2::ZERO)
                                         , ALIGNMENT_CENTER
                                         , ui_wizard_share::inst()->getPalette()->getColor(szColor)
                                         , to_string(n - 100));
            if(n < 105) {
                gui::inst()->addLabelAutoDimension(0
                                                   , -1
                                                   , szStar
                                                   , layer
                                                   , -1
                                                   , ALIGNMENT_CENTER_BOTTOM
                                                   , ui_wizard_share::inst()->getPalette()->getColor3B("YELLOW")
                                                   , Vec2(1, 2), Vec2::ZERO, Vec2::ZERO, Vec2::ZERO
                                                   );
            }
            
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