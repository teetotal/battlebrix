//
//  SceneArcade.cpp
//
//  Created by Jung, DaeCheon on 19/07/2019.
//

#include "SceneArcade.h"
#include "Scenes.h"
#include "ScenePlay.h"
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
    vector<battleBrix::intPair> vec = battleBrix::inst()->getStageStatus();
    Vec2 lastPos = Vec2::ZERO;
    int current = 0;
    if(vec.size() > 0) current = vec[vec.size() - 1].k + 1;
    
    const int idxStart = 101;
    Vec2 posPreCurrent = Vec2::ZERO;
    for(int n=idxStart; n < idxStart + 14; n++) {
        const int stageId = n - idxStart;
        
        bool isLink = false;
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
        
        //이동 모션
        if(current > 0 && stageId == current -1 && battleBrix::inst()->mStageInfo.isArcadeStageCleared) {
            posPreCurrent = layer->getPosition();
        }
        if(stageId == current) {
            isLink = true;
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
            float x = (getNodeById(0)->getContentSize().width / 2) - layer->getPosition().x;
            if(x > mMoveMax) x = mMoveMax;
            else if(x < mMoveMin) x = mMoveMin;
            
            mLayer->setPosition(Vec2(x, mLayer->getPosition().y));
            
            if(posPreCurrent != Vec2::ZERO) { //현재위치 이동
                Vec2 posLast = layer->getPosition();
                layer->setPosition(posPreCurrent);
                layer->runAction(MoveTo::create(1, posLast));
            }
            
        }
        else {
            string szColor = "DARKGRAY_LIGHT";
            if(stageId < current) {
                szColor = "BLUE";
                string szStar;
                switch(vec[stageId].v) {
                    case 1: szStar = "★☆☆";
                        isLink = true;
                        break;
                    case 2: szStar = "★★☆";
                        isLink = true;
                        break;
                    default: szStar = "★★★";
                        isLink = false;
                        szColor = "BLUE_DARK";
                        break;
                        
                }
                
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
            
            ui_icon::create()->addCircle(layer
                                         , gui::inst()->getGridSize(layer->getContentSize(), Vec2(1, 1), Vec2::ZERO, Vec2::ZERO)
                                         , gui::inst()->getPointVec2(0, 0, ALIGNMENT_CENTER, layer->getContentSize(), Vec2(1, 1), Vec2::ZERO, Vec2::ZERO, Vec2::ZERO)
                                         , ALIGNMENT_CENTER
                                         , ui_wizard_share::inst()->getPalette()->getColor(szColor)
                                         , to_string(stageId + 1));
        }
        //link
        if(isLink)
            gui::inst()->addTextButtonAutoDimension(0, 0, "M", layer
                                                    , CC_CALLBACK_1(SceneArcade::callback, this, stageId, 100)
                                                    , 0
                                                    , ALIGNMENT_CENTER
                                                    , Color3B::BLACK
                                                    , Vec2(1, 1)
                                                    , Vec2::ZERO
                                                    , Vec2::ZERO
                                                    , Vec2::ZERO
                                                    , Vec2::ZERO)->setOpacity(0);
    }
    
    return true;
}

void SceneArcade::callback(Ref* pSender, int from, int link) {
    if(link == 100) {
        CCLOG("from %d", from);
        battleBrix::inst()->mStageInfo.setArcade(from);
        replaceScene(SceneArcadeDetail::create());
    }
    
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

//=============================================================================
bool SceneArcadeDetail::init()
{
    this->loadFromJson("arcadeDetail", "arcade_detail.json");
    int stage = battleBrix::inst()->mStageInfo.arcadeStage;
    brixMap::brixStage info = brixMap::inst()->getMap(stage);
    
    ((Label*)getNodeById(40))->setString(info.title);
    //50 -> prize
    auto layerPrize = getNodeById(50);
    int n = 0;
    Vec2 gridSize = Vec2(2, 2);
    Vec2 innerMargin = Vec2(0, 0);
//    if(info.prize.point > 0) gridSize.y += 1;
//    if(info.prize.heart > 0) gridSize.y += 1;
//    if(info.prize.item >= 0) gridSize.y += 1;
    
    if(info.prize.point > 0){
        auto p = ui_icon::create();
        p->addCircle(layerPrize
                     , gui::inst()->getGridSize(layerPrize->getContentSize(), gridSize, Vec2::ZERO, innerMargin)
                     , gui::inst()->getPointVec2(n++, 0, ALIGNMENT_CENTER, layerPrize->getContentSize(), gridSize, Vec2::ZERO, Vec2::ZERO, innerMargin)
                     , ALIGNMENT_CENTER
                     , ui_wizard_share::inst()->getPalette()->getColor("YELLOW")
                     , "P"
                     , ""
                     , to_string(info.prize.point)
                     , ui_wizard_share::inst()->getPalette()->getColor("GRAY")
                     , -1
                     );
    }
    if(info.prize.heart > 0) {
        auto p = ui_icon::create();
        p->addHeart(layerPrize
                    , gui::inst()->getGridSize(layerPrize->getContentSize(), gridSize, Vec2::ZERO, innerMargin)
                    , gui::inst()->getPointVec2(n++, 0, ALIGNMENT_CENTER, layerPrize->getContentSize(), gridSize, Vec2::ZERO, Vec2::ZERO, innerMargin)
                    , ALIGNMENT_CENTER
                    , to_string(info.prize.heart)
                    , ui_wizard_share::inst()->getPalette()->getColor("GRAY")
                    , -1
                    );
    }
    if(info.prize.item >= 0) {
        gui::inst()->addLabelAutoDimension(0, 1, battleBrix::inst()->mItems[info.prize.item].name, layerPrize, -1, ALIGNMENT_CENTER
                                           , ui_wizard_share::inst()->getPalette()->getColor3B("GRAY")
                                           , gridSize, Vec2::ZERO, Vec2::ZERO, innerMargin, Vec2(1, 0)
                                           );
    }
    
    //51 -> mission
    ((Label*)getNodeById(51))->setString(info.mission.message);
    
    
    int checkedCnt = 0;
    for(int n = 0; n < battleBrix::inst()->mItems.size(); n++) {
        battleBrix::itemData item = battleBrix::inst()->mItems[n];
        auto sprite = gui::inst()->getSprite(item.img);
        
        int id = 100 + (n*10);
        
        auto checkBox = ((ui_checkbox*)getNodeById(id+1));
        checkBox->setText(item.name);
        battleBrix::inst()->mStageInfo.isSelected[n] = false;
        checkBox->setChecked(false);
        getNodeById(id+4)->setVisible(false); //select rect
        getNodeById(id+8)->setVisible(false); //link
        getNodeById(id+9)->setVisible(true); //disable
        
        ((Sprite*)getNodeById(id+2))->setTexture(sprite->getTexture());
        ((ui_icon*)getNodeById(id+3))->setText(numberFormat(item.price));
        
        //enableItems
        for(int i=0; i < info.enableItemIdx.size(); i++) {
            if(info.enableItemIdx[i] == n) {
                if(checkedCnt < PLAY_ITEM_CNT) {
                    battleBrix::inst()->mStageInfo.isSelected[n] = true;
                    checkBox->setChecked(true);
                    getNodeById(id+4)->setVisible(true);
                    checkedCnt++;
                }
                getNodeById(id+8)->setVisible(true);
                getNodeById(id+9)->setVisible(false);
                
            }
        }
    }
    displayPointChanged();
    
    //timer
    HEART_TIMER
    //
    return true;
}

void SceneArcadeDetail::callback(Ref* pSender, int from, int link) {
    if(link >= 100) {
        int cnt = 0;
        for(int n=0; n < battleBrix::inst()->mStageInfo.isSelected.size(); n++) {
            if(battleBrix::inst()->mStageInfo.isSelected[n])
                cnt++;
        }
        auto checkBox = ((ui_checkbox*)getNodeById(link+1));
        if(!checkBox->isChecked() && cnt >= PLAY_ITEM_CNT) {
            //alert
            guiExt::runScaleEffect(getNodeById(52));
        }
        else {
            checkBox->setToggle();
            getNodeById(link+4)->setVisible(checkBox->isChecked());
            int idx = (link-100) / 10;
            battleBrix::inst()->mStageInfo.isSelected[idx] = checkBox->isChecked();
            
            //point effect
            displayPointChanged();
        }
    } else if(link == 10) {
        int totalPoint = battleBrix::inst()->mStageInfo.getTotalPoint();
        // check validation
        if(battleBrix::inst()->mUserData.heart < 1) {
            //alert
        }
        
        // pay and change scene
        if(battleBrix::inst()->payForPlay(totalPoint))
        {
            CallFunc * next;
            if(battleBrix::inst()->mUserData.increseExp()) { //levelup
                next = CallFunc::create([=](){
                    CallFunc * p = CallFunc::create([=]() { this->replaceScene(ScenePlay::create()); });
                    LEVELUP_EVENT(p)
                });
            } else {
                next = CallFunc::create([=]() { // go to playscene
                    this->replaceScene(ScenePlay::create());
                });
            }
            // 효과
//            ((ui_icon*)getNodeById(_ID_NODE_LABEL_POINT))->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_POINT));
//            guiExt::runScaleEffect(getNodeById(_ID_NODE_LABEL_POINT));
            
            ((ui_icon*)getNodeById(_ID_NODE_LABEL_HEART))->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_HEART));
            guiExt::runScaleEffect(getNodeById(_ID_NODE_LABEL_HEART), next);
        } else {
            // not enough point
        }
    }
    
    LINK
}

const string SceneArcadeDetail::getText(const string& defaultString, int id) {
    switch(id) {
        default:
            return battleBrix::inst()->getText(defaultString, id);
    }
}

void SceneArcadeDetail::displayPointChanged() {
    
    auto p = ((ui_icon*)getNodeById(_ID_NODE_LABEL_POINT));
    
    guiExt::runScaleEffect(p, CallFunc::create([=]() {
        p->setText(numberFormat(battleBrix::inst()->mUserData.point - getPrice()));
                                                   }));
}

int SceneArcadeDetail::getPrice() {
    int price = 0;
    for(int n=0; n < battleBrix::inst()->mStageInfo.isSelected.size(); n++) {
        if(battleBrix::inst()->mStageInfo.isSelected[n]) {
            price += battleBrix::inst()->mItems[n].price;
        }
    }
    return price;
}
