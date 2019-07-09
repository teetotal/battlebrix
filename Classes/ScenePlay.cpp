
#include "ScenePlay.h"
#include "Scenes.h"
#include "ui/ui_ext.h"
#include "library/pch.h"
#include "ui/ui_character_animal.h"
#include <functional>

#define SPEED_BASE .55f
#define PHYSICSMATERIAL             PhysicsMaterial(0.5f, 1.f, 0.f)
#define PHYSICSMATERIAL_OBSTACLE    PhysicsMaterial(0.f, 1.f, 0.f)
#define PHYSICSMATERIAL_BOARD    PhysicsMaterial(0.f, 1.f, 0.f)
//#define GRID_AREA Vec2(8.f, 13.f)
#define GRID_AREA Vec2(8.f, 11.f)
#define RATIO_OBSTACLE_PER_GRID 0.65f
#define _ID_BOTTOM 0
#define _ID_BRIX_START 101
#define _ID_WALL 50
#define _ID_GIFT_START 51
#define _ID_TRAP_START 61

#define BALL_INIT_POSITION_Y 8

enum _PLAYER_ID {
    _PLAYER_ID_ME = 0,
    _PLAYER_ID_OTHER,
};
enum _BOARD_ID {
    _BOARD_ID_L = 1,
    _BOARD_ID_LM,
    _BOARD_ID_RM,
    _BOARD_ID_R
};

const int _BALL_ID[] = {
    0x01 << 0,
    0x01 << 1,
    0x01 << 2,
    0x01 << 3,
    0x01 << 4
};

enum ID_NODE {
    ID_NODE_BG = 0,
    ID_NODE_MY = 1,
    ID_NODE_MY_HP,
    ID_NODE_MY_MP,
    ID_NODE_MY_AREA,
    ID_NODE_MY_ALERT,
    ID_NODE_MY_LABEL,
    ID_NODE_MY_LABEL_NAME,
    
    ID_NODE_OTHER = 11,
    ID_NODE_OTHER_HP,
    ID_NODE_OTHER_MP,
    ID_NODE_OTHER_AREA,
    ID_NODE_OTHER_ALERT,
    ID_NODE_OTHER_LABEL,
    ID_NODE_OTHER_LABEL_NAME,
    
    ID_NODE_CPU_1 = 21,
    ID_NODE_CPU_2 = 31,
    ID_NODE_CPU_3 = 41,
    ID_NODE_CPU_4 = 51,
    
    ID_NODE_SKILL = 100,
    ID_NODE_SKILL_1,
    ID_NODE_SKILL_2,
    ID_NODE_SKILL_3,
    
    ID_NODE_CONTROLBAR = 200,
    
    //top
    ID_NODE_TOP_TYPE = 300, // multi play or 1 on 1
    ID_NODE_TOP_GRADE,
    ID_NODE_TOP_MAP_TYPE,
    
    //for ending
    ID_NODE_ENDING = 10000,
    ID_NODE_ENDING_RANKING,
    ID_NODE_ENDING_AGAIN,
    ID_NODE_ENDING_REWARD,
    ID_NODE_ENDING_REWARD_POINT,
    ID_NODE_ENDING_REWARD_HEART,
    ID_NODE_LEVELUP = 20000,
    ID_NODE_LEVELUP_LABEL,
    ID_NODE_LEVELUP_GRADE,
};

//static COLOR_RGB colors[5] = {
//    ui_wizard_share::inst()->getPalette()->getColor("PINK_LIGHT"),
//    ui_wizard_share::inst()->getPalette()->getColor("YELLOW_LIGHT"),
//    ui_wizard_share::inst()->getPalette()->getColor("GREEN_LIGHT"),
//    ui_wizard_share::inst()->getPalette()->getColor("BLUE_LIGHT"),
//    ui_wizard_share::inst()->getPalette()->getColor("PURPLE_LIGHT")
//};
// ----------------------------------------------------------------------------------------------------------------
void ScenePlay::PLAYER::init(ScenePlay* p, int idx, const string& name, int layerId, int hpId, int mpId, int ballId, int alertId, int labelId, int fnId) {
    this->idx = idx;
    this->name = name;
    pScene = p;
    this->ballId = ballId;
    
    
    //layer 비율을 똑같이 맞출것인가
//    if(p->mBrixLayerRatio == -1.f)
//        layer = p->getNodeById(layerId);
//    else {
//        auto pLayer = p->getNodeById(layerId);
//        layer = gui::inst()->createLayout(Size(pLayer->getContentSize().height * p->mBrixLayerRatio, pLayer->getContentSize().height), "", false, Color3B::MAGENTA);
//        pLayer->addChild(layer);
//    }
    layer = p->getNodeById(layerId);
    
    //enable skill
    for(int n = 0; n < PLAY_ITEM_CNT; n++) {
        if(!battleBrix::inst()->mItemSelected.isSelected[n])
            ((ui_icon*)p->getNodeById(ID_NODE_SKILL_1 + n))->setEnabled(false);
            
    }
    
    hp = (ui_progressbar*)p->getNodeById(hpId);
    mp = (ui_progressbar*)p->getNodeById(mpId);
    alert = p->getNodeById(alertId);
    label = (Label*)p->getNodeById(labelId);
    labelName = (Label*)p->getNodeById(labelId+1);
    labelName->setString(name);
    
    //progressbar
    hp->setValue(1.f);
    mp->setValue(0.f);
    
    //speed
    float speed = battleBrix::inst()->getMyGrade().speed;
    p->initPhysicsBody(layer, PHYSICSMATERIAL_OBSTACLE, false, speed);
    
    gridSize   = gui::inst()->getGridSize(layer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO);
    obstacleSize = gridSize;
    obstacleSize.width *= RATIO_OBSTACLE_PER_GRID;
    obstacleSize.height *= RATIO_OBSTACLE_PER_GRID;
    fontSizeCombo = gui::inst()->getFontSize(gridSize) * 1.f;
    
    // create board
    createBoard();
    // create bottom
    createBottom();
    
    //create brix layer
    createLayerBrix();
    
//    std::function<void()> f[] =  {
//        std::bind( &ScenePlay::PLAYER::addBrix0, this)
//        , std::bind( &ScenePlay::PLAYER::addBrix1, this)
//        , std::bind( &ScenePlay::PLAYER::addBrix2, this)
//        , std::bind( &ScenePlay::PLAYER::addBrix3, this)
//        , std::bind( &ScenePlay::PLAYER::addBrix4, this)
//    };
    
    //f[fnId]();
    addBrix(fnId);
    //createGift();
    
}

//createLayerBrix ===========================================================================
void ScenePlay::PLAYER::createLayerBrix() {
    if(layerBrix)
        layer->removeChild(layerBrix);
    
    layerBrix = gui::inst()->createLayout(layer->getContentSize(), "", false, Color3B::MAGENTA);
    
    layer->addChild(layerBrix);
}
//createBall ===========================================================================
void ScenePlay::PLAYER::createBall() {
    COLOR_RGB color = ui_wizard_share::inst()->getPalette()->getColor("WHITE");
    
    Vec2 position = gui::inst()->getPointVec2(2, BALL_INIT_POSITION_Y, ALIGNMENT_CENTER, layer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    ball = guiExt::drawCircleForPhysics(layer, position, fmin(obstacleSize.width, obstacleSize.height) / 3.f, color);
    pScene->setPhysicsBodyCircle(ball, PHYSICSMATERIAL, true, ballId);
    
    ball->getPhysicsBody()->setVelocityLimit(layerBrix->getContentSize().height * 2.f);
}
//createBoard ===========================================================================
void ScenePlay::PLAYER::createBoard() {
    board = LayerColor::create(Color4B::RED);
    board->setContentSize(Size(gridSize.width * 2.f, gridSize.width * .2f));
    Vec2 pos = gui::inst()->getPointVec2(1, GRID_AREA.y - 1, ALIGNMENT_CENTER, layer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    pos.x -= board->getContentSize().width / 2.f;
    pos.y -= board->getContentSize().height / 2.f;
    board->setPosition(pos);
    
    Size size = Size(board->getContentSize().width / 4.f, board->getContentSize().height);
    
    for(int n=0; n < 4; n++) {
        auto rect = guiExt::drawRectForPhysics(board
                                               , gui::inst()->getPointVec2(n, 0, ALIGNMENT_CENTER, board->getContentSize(), Vec2(4,1), Vec2::ZERO, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO)
                                               , size
                                               , (n ==0 || n == 3 ) ? ui_wizard_share::inst()->getPalette()->getColor("PINK"): ui_wizard_share::inst()->getPalette()->getColor("PINK_LIGHT")
                                               , true
                                               , 0.f);
        pScene->setPhysicsBodyRect(rect
                                 , PHYSICSMATERIAL_BOARD
                                 , false
                                 , _BOARD_ID_L + n
                                 , -1
                                 , -1 //_PHYSICS_ID_MY_BALL
                                 , ballId//_PHYSICS_ID_MY_BALL
                                 );
    }
    
    layer->addChild(board);
}
//createBottom ===========================================================================
void ScenePlay::PLAYER::createBottom() {
    auto rect = guiExt::drawRectForPhysics(layer
                                           , Vec2(layer->getContentSize().width / 2.f, gridSize.height / 4.f)
                                           , Size(layer->getContentSize().width, gridSize.height / 4.f)
                                           , ui_wizard_share::inst()->getPalette()->getColor("TRANSPARENT")
                                           , true
                                           , 0.f);
    pScene->setPhysicsBodyRect(rect
                             , PHYSICSMATERIAL
                             , false
                             , _ID_BOTTOM
                             , -1
                             , -1 //_PHYSICS_ID_MY_BALL
                             , -1 //_PHYSICS_ID_MY_BALL
                             );
}

// vibrate ===========================================================================
void ScenePlay::PLAYER::vibrate() {
    if(lockShake)
        return;
    lockShake = true;
    guiExt::addVibrateEffect(layer, CallFunc::create([=]() { lockShake = false; }));
}
// decreseHP ===========================================================================
void ScenePlay::PLAYER::decreseHP(const string from, float f) {
    if(this->isEnd)
        return;
    
    auto val = hp->setValueDecrese(f);
    hp->blink();
    vibrate();
    
    if(from.size() > 0) {
        Vec2 pos = Vec2(layer->getContentSize().width / 2.f, layer->getContentSize().height * .75f);
        guiExt::addScaleEffect(layer, "icons8-action-96.png", from, ui_wizard_share::inst()->getPalette()->getColor("GRAY"), NULL, .4f, .4f, pos, 150);
    }
    
    if(val <= 0.3f) {
        alert->setVisible(true);
        alert->runAction(RepeatForever::create(Blink::create(0.4f, 1)));
    }
}
// onContact ===========================================================================
bool ScenePlay::PLAYER::onContact(int id, bool toRight) {
    if(ball == NULL)
        return false;
    
    const Vec2 ballPosition = ball->getPosition();
    
    if(id == _ID_BOTTOM) {
        combo = 0;
        hp->setValueDecrese(0.1);
        hp->blink();
        vibrate();
    }
    else if(id >= _BOARD_ID_L && id <= _BOARD_ID_R) {
        //동시 충돌 방지
        clock_t now = clock();
        if(now - latestCollisionWithBoard < 100) {
//            CCLOG("%s %lf concurrent collision %d",name.c_str(), (double)latestCollisionWithBoard, id);
            return false;
        }
        else
            latestCollisionWithBoard = now;
        
        //보드에 맞지도 않았는데 튀어 나가는 현상이 있음.
        if(ballPosition.y > board->getPosition().y + board->getContentSize().height * 2.f)
            return false;
        
        combo = 0;
        Vec2 vel = layer->getContentSize();
        vel.y *= 1.5f;
        switch((_BOARD_ID)id) {
            case _BOARD_ID_L:
                vel.x *= -1.f;
                break;
            case _BOARD_ID_LM:
                vel.x *= -0.2f;
                break;
            case _BOARD_ID_RM:
                vel.x *= 0.2f;
                break;
            case _BOARD_ID_R:
                vel.x *= 1.f;
                break;
            default:
                break;
        }
        ball->getPhysicsBody()->setVelocity(vel);
        
    } else if(id >= _ID_GIFT_START && id < _ID_TRAP_START) {
        auto pGift = layerBrix->getChildByTag(id);
        if(pGift == NULL)
            return false;
        Vec2 pos = pGift->getPosition();
        auto p = createGiftOrTrapEffect(pos, brixMap::TYPE_GIFT, CallFunc::create([=](){
            pScene->onSkill(getRandValue((int)battleBrix::inst()->mItems.size()), this->idx);
        }));
        //gift
        layerBrix->removeChildByTag(id);
        layerBrix->addChild(p);
    } else if(id >= _ID_TRAP_START && id < _ID_BRIX_START) {
        Vec2 pos = layerBrix->getChildByTag(id)->getPosition();
        auto p = createGiftOrTrapEffect(pos, brixMap::TYPE_TRAP, CallFunc::create([=](){
            this->decreseHP("");
        }));
        //gift
        layerBrix->removeChildByTag(id);
        layerBrix->addChild(p);
    } else if(id >= _ID_BRIX_START) {
        //구조체에서는 mutex를 사용할 수 없다.
        return onCombo(id);
    }
    
    return false;
}
Sprite * ScenePlay::PLAYER::createGiftOrTrapEffect(Vec2 pos, brixMap::TYPE type, CallFunc * fn) {
    const string img = (type == brixMap::TYPE_GIFT) ? "icons8-wedding-gift-96.png" : "icons8-bomb-96.png";
    auto p = gui::inst()->getSprite(img);
    gui::inst()->setScale(p, gridSize.width * 0.75f);
    p->setPosition(pos);
    guiExt::runScaleEffect(p, fn, 0.3f, true);
    
    return p;
}
// onCombo ===========================================================================
bool ScenePlay::PLAYER::onCombo(int id) {
    combo++;
    const Vec2 ballPosition = ball->getPosition();
    
    string sz = (combo > 1) ? to_string(combo) + "COMBO" : "Cool";
    string color;
    
    float fIncrease = 0.05;
    if(combo == 1) {
        color = "YELLOW";
    }
    else if(combo < 5) {
        fIncrease *= 2.f;
        color = "ORANGE";
    }
    else if(combo < 8) {
        fIncrease *= 3.f;
        color = "BLUE";
    }
    else {
        fIncrease *= 4.f;
        color = "GREEN";
    }
    
    auto label = gui::inst()->addLabelAutoDimension(0, 0, sz, layer, fontSizeCombo, ALIGNMENT_CENTER, ui_wizard_share::inst()->getPalette()->getColor3B(color));
    label->enableGlow(ui_wizard_share::inst()->getPalette()->getColor4B("BLACK"));
    label->setOpacity(150);
    label->setPosition(ballPosition);
    label->runAction( Sequence::create(ScaleBy::create(0.3, 1.5), RemoveSelf::create(), NULL) );
    
    //brix vibration
    if(!brixEffectFlagMap[id]) {
        brixEffectFlagMap[id] = true;
        guiExt::addVibrateEffect(layerBrix->getChildByTag(id), CallFunc::create([=](){ brixEffectFlagMap[id] = false; }));
    }
    //attack
    if(mp->setValueIncrese(fIncrease) >= 1.f) {
        auto particle = gui::inst()->createParticle("star.plist"
                                                    , Vec2(this->mp->getPosition().x + (mp->getContentSize().width * 0.9f), this->mp->getPosition().y+ (mp->getContentSize().height * 0.5f)));
        // 1: gui::inst()->mVisibleX = x : layerSize
        // x = layerSize / gui::inst()->mVisibleX
        particle->setScale(layer->getContentSize().width / gui::inst()->mVisibleX);
        layer->addChild(particle);
        mp->setValue(0.f);
        mp->blink();
        
        return true;
    }
    return false;
}
// onBomb ===========================================================================
void ScenePlay::PLAYER::onBomb(const string from) {
    if(isEnd)
        return;
    
    Vec2 pos = Vec2(layer->getContentSize().width / 2.f, layer->getContentSize().height * .75f);
    guiExt::addScaleEffect(layer, "icons8-atomic-bomb-96.png", from, ui_wizard_share::inst()->getPalette()->getColor("GRAY"), NULL, .4f, .4f, pos, 150);
}
// setAutoPlay ===========================================================================
void ScenePlay::PLAYER::onTimer(float f) {
    if(ball) {
        Vec2 pos = Vec2(ball->getPosition().x, board->getPosition().y);
        pos.x -= board->getContentSize().width / 2.f;
        if(pos.x <= 0)
            pos.x = 0;
        if(pos.x >= layer->getContentSize().width - board->getContentSize().width)
            pos.x = layer->getContentSize().width - board->getContentSize().width;

        int randVal = getRandValue(100);
        float duration = f * (0.4f + ((float)randVal / 100.f));
//        CCLOG("Moving Speed: %s - %f", name.c_str(), duration);
        board->runAction(MoveTo::create(duration, pos));
    }
}
// setRanking ===========================================================================
void ScenePlay::PLAYER::setRanking(int ranking) {
    if(isEnd)
        return;
    if(getHPValue() <= 0.f) {
        finish();
    }
    if(this->ranking != ranking) {
        this->ranking = ranking;
        label->setString(to_string(ranking));
        label->runAction(Sequence::create(ScaleTo::create(0.1, 1.5), ScaleTo::create(0.1, 1.f), NULL));
    }
    
}
// finish ===========================================================================
void ScenePlay::PLAYER::finish() {
    if(isEnd)
        return;
    
    isEnd = true;
    layer->removeChild(ball);
    ball = NULL;
    layerBrix->removeAllChildren();
    layer->removeChild(board);
    label->setColor(ui_wizard_share::inst()->getPalette()->getColor3B("BLACK"));
    
    alert->stopAllActions();
    alert->setVisible(false);
}
//gift 좀더 생각해 봐야겠당
Sprite * ScenePlay::PLAYER::createBrixFromSprite(brixMap::position pos, int id, const string img){
    auto p = gui::inst()->addSpriteAutoDimension(pos.x, pos.y, img, layerBrix, ALIGNMENT_CENTER, GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    gui::inst()->setScale(p, gridSize.width * 0.75f);
    auto posStart = p->getPosition();
    pScene->setPhysicsBodyRect(p
                               , PHYSICSMATERIAL_OBSTACLE
                               , false
                               , id
                               , -1
                               , -1 //_PHYSICS_ID_MY_BALL
                               , ballId
                               );
    
    return p;
}
//
Node * ScenePlay::PLAYER::createBrix(brixMap::position pos, int id)
{
    int x = (pos.x == -1) ? getRandValue(GRID_AREA.x) : pos.x;
    int y = (pos.y == -1) ? getRandValue(BALL_INIT_POSITION_Y) : pos.y;
    
    Vec2 position = gui::inst()->getPointVec2(x, y, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    COLOR_RGB color, colorBG;
    if(id == _ID_WALL) {
        color.set(ui_wizard_share::inst()->getPalette()->getColor("DARKGRAY"));
        colorBG.set(ui_wizard_share::inst()->getPalette()->getColor("BLACK"));
    }
    else {
        color.set(pScene->mColors[id % pScene->mColors.size()]);
        colorBG.set(pScene->mColors[(id + 1) % pScene->mColors.size()]);
    }
    
//    auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, color, true, .1f);
    
    auto rect = ui_character_animal::create(layerBrix
                                           , obstacleSize
                                           , position
                                           , ALIGNMENT_CENTER
                                           , color
                                           , ui_wizard_share::inst()->getPalette()->getColor("DARKGRAY")
                                           , ui_wizard_share::inst()->getPalette()->getColor("PINK")
                                           , colorBG
                                           );
    rect->addRectangle();
    
    pScene->setPhysicsBodyRect(rect
                               , PHYSICSMATERIAL_OBSTACLE
                               , false
                               , id
                               , -1
                               , -1 //_PHYSICS_ID_MY_BALL
                               , ballId
                               );
    
    brixEffectFlagMap[id] = false;
    
    return rect;
}
// addBrix ===========================================================================
void ScenePlay::PLAYER::addBrix(int idx) {
    brixMap::brixPosition brix = brixMap::inst()->getMap(idx);
    int brixId = _ID_BRIX_START;
    int giftId = _ID_GIFT_START;
    int trapId = _ID_TRAP_START;
    
    vector<brixMap::position> posHistory;
    //static
    for(int n=0; n < brix.statics.size(); n++)
    {
        brixMap::position posOriginal = brix.statics[n];
        brixMap::position pos;
        pos.x = posOriginal.x;
        pos.y = posOriginal.y;
        while(pos.x == -1 || pos.y == -1) {
            if(pos.x == -1) pos.x = getRandValue(GRID_AREA.x);
            if(pos.y == -1) pos.y = getRandValue(BALL_INIT_POSITION_Y);
            bool isExist = false;
            for(int i=0; i < posHistory.size(); i++) {
                if(posHistory[i].x == pos.x && posHistory[i].y == pos.y) {
                    isExist = true;
                    //원래값으로 복구
                    pos.x = posOriginal.x;
                    pos.y = posOriginal.y;
                    break;
                }
            }
            
            if(!isExist)
                break;
        }
        createBrix(pos, brixId++);
        posHistory.push_back(pos);
    }
    //movement
    for(int n=0; n < brix.movements.size(); n++)
    {
        int last = (int)brix.movements[n].path.size() - 1;
        int id;
        Node * pGiftOrTrap = NULL;
        switch(brix.movements[n].type) {
            case brixMap::TYPE_NORMAL:
                id = brixId;
                brixId++;
                break;
            case brixMap::TYPE_WALL:
                id = _ID_WALL;
                break;
                //이러면 오직 한개의 gift와 trap만 있다.
            case brixMap::TYPE_GIFT:
                pGiftOrTrap = createBrixFromSprite(brix.movements[n].path[last], giftId++, "icons8-wedding-gift-96.png");
                break;
            case brixMap::TYPE_TRAP:
                pGiftOrTrap = createBrixFromSprite(brix.movements[n].path[last], trapId++, "icons8-bomb-96.png");
                break;
            default:
                CCASSERT(false, "invalid type");
                break;
        }
        auto rect = (pGiftOrTrap) ? pGiftOrTrap : createBrix(brix.movements[n].path[last], id);
        
        //path
        Sequence * seq;
        Vector<FiniteTimeAction *> arr;
        for(int i=0; i<brix.movements[n].path.size(); i++ ){
            
            int d = max(10, getRandValue(30));
            float f = (float)d / 10.f;
            int x = (brix.movements[n].path[i].x == -1) ? getRandValue(GRID_AREA.x) : brix.movements[n].path[i].x;
            int y = (brix.movements[n].path[i].y == -1) ? getRandValue(BALL_INIT_POSITION_Y) : brix.movements[n].path[i].y;
            
            Vec2 position = gui::inst()->getPointVec2(x, y, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
            //anchor point가 0,0
            position.x -= obstacleSize.width / 2;
            position.y -= obstacleSize.height / 2;
            
            auto move = MoveTo::create(f, position);
            arr.pushBack(move);
        }
        seq = Sequence::create(arr);
        rect->runAction(RepeatForever::create(seq));
    }
}
// addBrix0 ===========================================================================
//void ScenePlay::PLAYER::addBrix0() {
//
//    int brixId = _ID_BRIX_START;
//    for(int n=0; n < BALL_INIT_POSITION_Y; n++)
//    {
//        Vec2 position = gui::inst()->getPointVec2(getRandValue(8), n, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[(int)n], true, .1f);
//        pScene->setPhysicsBodyRect(rect
//                                   , PHYSICSMATERIAL_OBSTACLE
//                                   , false
//                                   , brixId
//                                   , -1
//                                   , -1 //_PHYSICS_ID_MY_BALL
//                                   , ballId
//                                   );
//
//        brixEffectFlagMap[brixId] = false;
//        brixId++;
//    }
//
//}
//// addBrix1 ===========================================================================
//void ScenePlay::PLAYER::addBrix1() {
//
//    int brixId = _ID_BRIX_START;
//    for(int n=0; n < BALL_INIT_POSITION_Y; n = n + 2)
//    {
//        Vec2 position = gui::inst()->getPointVec2(0, n, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[(int)n], true, .1f);
//        pScene->setPhysicsBodyRect(rect
//                                   , PHYSICSMATERIAL_OBSTACLE
//                                   , false
//                                   , brixId
//                                   , -1
//                                   , -1 //_PHYSICS_ID_MY_BALL
//                                   , ballId
//                                   );
//        int d = max(10, getRandValue(30));
//        float f = (float)d / 10.f;
//        auto seq = Sequence::create(MoveTo::create(f, Vec2(layerBrix->getContentSize().width - rect->getContentSize().width, rect->getPosition().y)), MoveTo::create(f, Vec2(0, rect->getPosition().y)), NULL);
//        rect->runAction(RepeatForever::create(seq));
//
//        brixEffectFlagMap[brixId] = false;
//        brixId++;
//    }
//}
//// addBrix2 ===========================================================================
//void ScenePlay::PLAYER::addBrix2() {
//
//    float yEnd = gui::inst()->getPointVec2(0, 7, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
//    float yStart = gui::inst()->getPointVec2(0, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
//
//    int brixId = _ID_BRIX_START;
//    for(int n=0; n < 8; n = n + 2)
//    {
//        Vec2 position = gui::inst()->getPointVec2(n, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[(int)n], true, .1f);
//        pScene->setPhysicsBodyRect(rect
//                                   , PHYSICSMATERIAL_OBSTACLE
//                                   , false
//                                   , brixId
//                                   , -1
//                                   , -1 //_PHYSICS_ID_MY_BALL
//                                   , ballId
//                                   );
//        int d = max(10, getRandValue(30));
//        float f = (float)d / 10.f;
//        auto seq = Sequence::create(MoveTo::create(f, Vec2(rect->getPosition().x, yEnd)), MoveTo::create(f, Vec2(rect->getPosition().x, yStart)), NULL);
//        rect->runAction(RepeatForever::create(seq));
//
//        brixEffectFlagMap[brixId] = false;
//        brixId++;
//    }
//}
//// addBrix3 ===========================================================================
//void ScenePlay::PLAYER::addBrix3() {
//
//    float yEnd = gui::inst()->getPointVec2(0, 7, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
//    float yStart = gui::inst()->getPointVec2(0, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
//    float xStart = gui::inst()->getPointVec2(1, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).x;
//    float xEnd = gui::inst()->getPointVec2(6, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).x;
//
//
//    int brixId = _ID_BRIX_START;
//    int colorIdx = 0;
//
//    //양쪽 side
//
//    for(int n=0; n < 8; n = n + 7)
//    {
//        Vec2 position = gui::inst()->getPointVec2(n, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[colorIdx++], true, .1f);
//        pScene->setPhysicsBodyRect(rect
//                                   , PHYSICSMATERIAL_OBSTACLE
//                                   , false
//                                   , brixId
//                                   , -1
//                                   , -1 //_PHYSICS_ID_MY_BALL
//                                   , ballId
//                                   );
//        int d = max(10, getRandValue(30));
//        float f = (float)d / 10.f;
//        auto seq = Sequence::create(MoveTo::create(f, Vec2(rect->getPosition().x, yEnd)), MoveTo::create(f, Vec2(rect->getPosition().x, yStart)), NULL);
//        rect->runAction(RepeatForever::create(seq));
//
//        brixEffectFlagMap[brixId] = false;
//        brixId++;
//    }
//
//    for(int n=0; n <= 6; n = n + 3)
//    {
//        Vec2 position = gui::inst()->getPointVec2(0, n, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[colorIdx++], true, .1f);
//        pScene->setPhysicsBodyRect(rect
//                                   , PHYSICSMATERIAL_OBSTACLE
//                                   , false
//                                   , brixId
//                                   , -1
//                                   , -1 //_PHYSICS_ID_MY_BALL
//                                   , ballId
//                                   );
//        int d = max(10, getRandValue(30));
//        float f = (float)d / 10.f;
//        auto seq = Sequence::create(MoveTo::create(f, Vec2(xEnd, rect->getPosition().y)), MoveTo::create(f, Vec2(xStart, rect->getPosition().y)), NULL);
//        rect->runAction(RepeatForever::create(seq));
//
//        brixEffectFlagMap[brixId] = false;
//        brixId++;
//    }
//}
//// addBrix4 ===========================================================================
//void ScenePlay::PLAYER::addBrix4() {
//
//    float yEnd = gui::inst()->getPointVec2(0, 7, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
//    float yStart = gui::inst()->getPointVec2(0, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
//    Vec2 start1 = gui::inst()->getPointVec2(1, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//    Vec2 start2 = gui::inst()->getPointVec2(6, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//    Vec2 end1 = gui::inst()->getPointVec2(6, 7, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//    Vec2 end2 = gui::inst()->getPointVec2(1, 7, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//
//
//    int brixId = _ID_BRIX_START;
//    int colorIdx = 0;
//
//    //양쪽 side
//
//    for(int n=0; n < 8; n = n + 7)
//    {
//        Vec2 position = gui::inst()->getPointVec2(n, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[colorIdx++], true, .1f);
//        pScene->setPhysicsBodyRect(rect
//                                   , PHYSICSMATERIAL_OBSTACLE
//                                   , false
//                                   , brixId
//                                   , -1
//                                   , -1 //_PHYSICS_ID_MY_BALL
//                                   , ballId
//                                   );
//        int d = max(10, getRandValue(30));
//        float f = (float)d / 10.f;
//        auto seq = Sequence::create(MoveTo::create(f, Vec2(rect->getPosition().x, yEnd)), MoveTo::create(f, Vec2(rect->getPosition().x, yStart)), NULL);
//        rect->runAction(RepeatForever::create(seq));
//
//        brixEffectFlagMap[brixId] = false;
//        brixId++;
//    }
//
//    for(int n=0; n < 2; n++)
//    {
//        Vec2 positionStart = (n==0) ? start1 : start2;
//        Vec2 positionEnd = (n==0) ? end1 : end2;
//        auto rect = guiExt::drawRectForPhysics(layerBrix, positionStart, obstacleSize, pScene->mColors[colorIdx++], true, .1f);
//        pScene->setPhysicsBodyRect(rect
//                                   , PHYSICSMATERIAL_OBSTACLE
//                                   , false
//                                   , brixId
//                                   , -1
//                                   , -1 //_PHYSICS_ID_MY_BALL
//                                   , ballId
//                                   );
//        int d = max(10, getRandValue(30));
//        float f = (float)d / 10.f;
//        auto seq = Sequence::create(MoveTo::create(f, positionEnd), MoveTo::create(f, positionStart), NULL);
//        rect->runAction(RepeatForever::create(seq));
//
//        brixEffectFlagMap[brixId] = false;
//        brixId++;
//    }
//}
/* ----------------------------------------------------------------------------------------------------------------
 
 
 ScenePlay

 
 ---------------------------------------------------------------------------------------------------------------- */
bool ScenePlay::init()
{
//    int n = 0;
    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("YELLOW_LIGHT"));
    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("YELLOW"));
    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("YELLOW_DARK"));
    
    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("BLUE_LIGHT"));
    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("BLUE"));
    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("BLUE_DARK"));
    
    
//    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("GREEN_LIGHT"));
//    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("GREEN"));
//    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("GREEN_DARK"));
    
//    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("PURPLE_LIGHT"));
//    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("PURPLE"));
//    mColors.push_back(ui_wizard_share::inst()->getPalette()->getColor("PURPLE_DARK"));
    
//    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("PINK_LIGHT");
//    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("PINK");
//    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("PINK_DARK");
//    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("YELLOW_LIGHT");
    //mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("GREEN_LIGHT");
    
    this->loadFromJson("play", "play.json");
    
    MULTITOUCH_INIT(ScenePlay);
    PHYSICS_CONTACT(ScenePlay);
    
    //skills
    mSkillParentNode = getNodeById(ID_NODE_SKILL);
    mSkills.push_back((ui_icon*)getNodeById(ID_NODE_SKILL_1));
    mSkills.push_back((ui_icon*)getNodeById(ID_NODE_SKILL_2));
    mSkills.push_back((ui_icon*)getNodeById(ID_NODE_SKILL_3));
    mControlBar = getNodeById(ID_NODE_CONTROLBAR);
    
    mIsEnd = false;
    mBrixLayerRatio = -1;
    
    int fnId = brixMap::inst()->getMapRandom();
    
    // grade
    ((Label*)getNodeById(ID_NODE_TOP_GRADE))->setString(battleBrix::inst()->getLevelString());
    // map type
    ((MenuItemLabel*)getNodeById(ID_NODE_TOP_MAP_TYPE))->setString(brixMap::inst()->getMap(fnId).title);
    
    PLAYER me;
    me.init(this, _PLAYER_ID_ME, battleBrix::inst()->mUserData.id, ID_NODE_MY_AREA, ID_NODE_MY_HP, ID_NODE_MY_MP, _BALL_ID[0], ID_NODE_MY_ALERT, ID_NODE_MY_LABEL, fnId);
    mPlayers.push_back(me);
    
    Size s1 = mPlayers[_PLAYER_ID_ME].layer->getContentSize(); //193.3333, 208.247
    //s1.width : s1.height = x : s2.height
    // = s1.width * s2.height / s1.height
    // = s1.width / s1.height * s2.height
    mBrixLayerRatio = s1.width / s1.height;
    
    
    for(int n=0; n<4; n++) {
        PLAYER p;
        int id = ((n  + 1) * 10) + 11;
        getNodeById(id++)->setVisible(true);
        
        int hpId = id++;
        int mpId = id++;
        int areaId = id++;
        int alertId = id++;
        int labelId = id;
        p.init(this, n+1, "CPU" + to_string(n+1), areaId, hpId, mpId, _BALL_ID[n+1], alertId, labelId, fnId);
        mPlayers.push_back(p);
    }
    
//    gui::inst()->drawGrid(mLayer, mLayer->getContentSize(), GRID_AREA, Size::ZERO, Size::ZERO);
    //Start
    guiExt::addMovingEffect(this->getNodeById(0)
                            , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_DEEP")
                            , ""
                            , "START"
                            , ui_wizard_share::inst()->getPalette()->getColor("DARKGRAY")
                            , false
                            , 1.5f
                            , CallFunc::create([=]()
    {
        for(int n=0; n < mPlayers.size(); n++)
            mPlayers[n].createBall();
        
        this->schedule(schedule_selector(ScenePlay::timer), 0.1f);
//        mPlayers[_PLAYER_ID_OTHER].createBall();
    }));
   
    return true;
}
// timer ===========================================================================
void ScenePlay::timer(float f) {
    for(int n = 1; n < mPlayers.size(); n++) {
        if(!mPlayers[n].isEnd)
            mPlayers[n].onTimer(f);
    }
}
// callback ===========================================================================
void ScenePlay::callback(Ref* pSender, int from, int link) {
    switch(link) {
        case 999:
            this->replaceScene(SceneMain::create());
            break;
        default:
            break;
    }
}
void ScenePlay::bomb(int id, int val) {
    for(int n = 0; n < mPlayers.size(); n++) {
        if(n == id)
            continue;
        mPlayers[n].onBomb(mPlayers[id].name);
        mPlayers[n].decreseHP("", val);
    }
}
// onSkill ===========================================================================
void ScenePlay::onSkill(int idx, int from) {
    if(mIsEnd)
        return;
    
    float recharge = battleBrix::inst()->mItems[idx].property.hpRecharge;
    float attack = battleBrix::inst()->mItems[idx].property.hpAttack;
    
    //자신일 경우만
    if(from == _PLAYER_ID_ME) {
        CallFunc * pFn = CallFunc::create([=]()
                                          {
                                              if(recharge > 0.f && !mPlayers[from].isEnd)
                                                  mPlayers[from].hp->setValueIncrese(recharge);
                                              if(attack > 0.f && !mPlayers[from].isEnd)
                                                  bomb(from, attack);
                                          });
        guiExt::addMovingEffect(this->getNodeById(0)
                                , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_LIGHT2")
                                , battleBrix::inst()->mItems[idx].img
                                , battleBrix::inst()->mItems[idx].name
                                , ui_wizard_share::inst()->getPalette()->getColor("YELLOW")
                                , false
                                , 1.5f
                                , NULL
                                , pFn
                                );
    }
    else {
        if(recharge > 0.f)
            mPlayers[from].hp->setValueIncrese(recharge);
        if(attack > 0.f)
            bomb(from, attack);
    }
}
// getText ===========================================================================
const string ScenePlay::getText(const string& defaultString, int id) {
    return defaultString;
}

// touch ===========================================================================
bool ScenePlay::onTouchBegan(Touch* touch, Event* event) {
    return true;
}
bool ScenePlay::onTouchEnded(Touch* touch, Event* event) {
    for(int n=0; n<mSkills.size(); n++) {
        if(mSkills[n]->getBoundingBox().containsPoint(mSkillParentNode->convertTouchToNodeSpace(touch))) {
            if(mSkills[n]->isEnabled()) {
                mSkills[n]->runScaleAndDisable();
                onSkill(n, _PLAYER_ID_ME);
            }
            return false;
        }
    }
    return false;
}

void ScenePlay::onTouchMoved(Touch *touch, Event *event) {
    if(mIsEnd ||
//       mControlBar->convertTouchToNodeSpace(touch).y > mControlBar->getContentSize().height
       mControlBar->convertTouchToNodeSpace(touch).x < 0)
        return;
    
    auto posLayer = mPlayers[_PLAYER_ID_ME].layer->getParent()->getPosition();
    Vec2 pos = Vec2(touch->getLocation().x - posLayer.x, mPlayers[_PLAYER_ID_ME].board->getPosition().y);
    pos.x -= mPlayers[_PLAYER_ID_ME].board->getContentSize().width / 2.f;
    
    if(pos.x <= 0)
        pos.x = 0;
    if(pos.x >= mPlayers[_PLAYER_ID_ME].layer->getContentSize().width - mPlayers[_PLAYER_ID_ME].board->getContentSize().width)
        pos.x = mPlayers[_PLAYER_ID_ME].layer->getContentSize().width - mPlayers[_PLAYER_ID_ME].board->getContentSize().width;

    mPlayers[_PLAYER_ID_ME].board->setPosition(pos);
}

bool ScenePlay::onContactBegin(PhysicsContact &contact) {
    mLock.lock();
    if(mIsEnd) {
        mLock.unlock();
        return true;
    }
    
    
    int other;
//    bitmask st = getBitmask(contact);
//    if(isCollosion(contact, _PHYSICS_ID_MY_BALL, other)) {
//        CCLOG("Collision %d with %d, Category %d, %d, Collision %d, %d"
//              , _PHYSICS_ID_MY_BALL, other
//              , st.categoryA, st.categoryB
//              , st.collisionA, st.collisionB
//              );
//    }
    
//    bool isChangedRanking = false;
    for(int n = 0; n < mPlayers.size(); n++) {
        if(isContact(contact, mPlayers[n].ballId, other)) {
            if(mPlayers[n].onContact(other)) {
                
                for(int i=0; i< mPlayers.size(); i++) {
                    if(i != n)
                        mPlayers[i].decreseHP(mPlayers[n].name);
                }
            }
        }
    }
    //순위
    for(int n = 0; n < mPlayers.size(); n++) {
        int ranking = 1;
        float val = mPlayers[n].getHPValue();
        for(int i = 0; i < mPlayers.size(); i++) {
            if(n != i && val < mPlayers[i].getHPValue()) {
                ranking ++;
            }
        }
        mPlayers[n].setRanking(ranking);
    }
    //내가 끝나거나 다른게 모두 끝났거나
    bool isEndAll = true;
    for(int n=1; n<mPlayers.size(); n++){
        if(!mPlayers[n].isEnd) {
            isEndAll = false;
            break;
        }
    }
    
    if(isEndAll || mPlayers[_PLAYER_ID_ME].isEnd)
        mIsEnd = true;
    
    if(mIsEnd) {
        //각 player에서 해야됨.
        for(int n=0; n < mPlayers.size(); n++) {
            mPlayers[n].finish();
        }
//        this->getPhysicsWorld()->setAutoStep(false);
//        this->getPhysicsWorld()->step(0.0f);
        this->getNodeById(99)->setVisible(true);
        
        guiExt::addMovingEffect(this->getNodeById(0)
                                , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_DEEP")
                                , ""
                                , "FINISH"
                                , ui_wizard_share::inst()->getPalette()->getColor("DARKGRAY")
                                , false
                                , 1.5f
                                , CallFunc::create([=](){ this->onFinish(); })
                                );
    }
    mLock.unlock();
    return true;
}

void ScenePlay::onFinish() {
    int nRanking = mPlayers[_PLAYER_ID_ME].ranking;
    auto bg = this->getNodeById(0);
    
    if(nRanking == 1) {
        auto particle = gui::inst()->createParticle("firework.plist", gui::inst()->getCenter(bg));
        bg->addChild(particle);
        guiExt::addScaleEffect(bg
                               , "icons8-crown-480.png"
                               , getRankString(nRanking)
                               , ui_wizard_share::inst()->getPalette()->getColor("RED")
                               , CallFunc::create([=]() { onEnd(); })
                               , 0.8
                               , 0.5
                           );
    } else {
        onEnd();
    }
}

void ScenePlay::onEnd()
{
    battleBrix::inst()->mLastRanking = mPlayers[_PLAYER_ID_ME].ranking;
    this->replaceScene(SceneEnding::create());
}
//====================================================================
bool SceneEnding::init()
{
    this->loadFromJson("ending", "ending.json");
    //결과 화면에서도 타이머
    this->schedule([=](float f){
        if(battleBrix::inst()->mUserData.recharge()){
            auto p = ((ui_icon*)getNodeById(_ID_NODE_LABEL_HEART));
            p->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_HEART));
            guiExt::runScaleEffect(p);
        }
    }, 1.f, "heartTimer");
    
    //init bold
    ((Label*)this->getNodeById(ID_NODE_LEVELUP_LABEL))->enableBold();
    ((Label*)this->getNodeById(ID_NODE_LEVELUP_GRADE))->enableBold();
    
    int nRanking = battleBrix::inst()->mLastRanking;
    string szRanking = getRankString(nRanking);
    battleBrix::rewardData reward = battleBrix::inst()->getReward(nRanking);
    
    //    ID_NODE_ENDING_RANKING,
    ((Label*)getNodeById(ID_NODE_ENDING_RANKING))->setString(szRanking);
    
    //    ID_NODE_ENDING_REWARD,
    auto pReward =((Label*)getNodeById(ID_NODE_ENDING_REWARD));
    if(reward.growth >= 0) {
        pReward->setString("+" + to_string(reward.growth));
    } else {
        pReward->setString(to_string(reward.growth));
        pReward->setColor(ui_wizard_share::inst()->getPalette()->getColor3B("RED_DARK"));
    }
    
    //    pReward->setColor(ui_wizard_share::inst()->getPalette()->getColor3B(szColorReward));
    
    //    ID_NODE_ENDING_REWARD_POINT,
    auto pPoint = ((Label*)this->getNodeById(ID_NODE_ENDING_REWARD_POINT));
    pPoint->setString((reward.point >= 0) ? "+" + to_string(reward.point) : to_string(reward.point));
    
    //    ID_NODE_ENDING_REWARD_HEART,
    auto pHeart = ((Label*)this->getNodeById(ID_NODE_ENDING_REWARD_HEART));
    pHeart->setString((reward.heart >= 0) ? "+" + to_string(reward.heart) : to_string(reward.heart));
    
    bool isLevelup = battleBrix::inst()->applyReward(nRanking);
    //다시하기 활성화 체크
    if(!battleBrix::inst()->checkPayForPlay(battleBrix::inst()->mItemSelected.getTotalPoint())) {
        ((ui_button*)getNodeById(ID_NODE_ENDING_AGAIN))->setEnabled(false);
    }
    
    //effect
    guiExt::runFlyEffect(this->getNodeById(ID_NODE_ENDING_REWARD)
                         , CallFunc::create([=]()
                                            {
                                                if(isLevelup) {
                                                    //level up
                                                    this->getNodeById(ID_NODE_ENDING)->setVisible(false);
                                                    this->getNodeById(0)->addChild(gui::inst()->createParticle("firework.plist", gui::inst()->getCenter()));
                                                    this->getNodeById(ID_NODE_LEVELUP)->setVisible(true);
                                                    auto levelGrade = (Label *)this->getNodeById(ID_NODE_LEVELUP_GRADE);
                                                    levelGrade->setString(battleBrix::inst()->getLevelString());
                                                    guiExt::runScaleEffect(this->getNodeById(ID_NODE_LEVELUP_LABEL), CallFunc::create([=]() {
                                                        guiExt::runScaleEffect(levelGrade, CallFunc::create([=]() {
                                                            this->getNodeById(ID_NODE_LEVELUP)->setVisible(false);
                                                            this->getNodeById(ID_NODE_ENDING)->setVisible(true);
                                                        }), .6f, false);
                                                    }), .3f, false);
                                                }
                                                // progressbar
                                                ((ui_progressbar*)getNodeById(_ID_NODE_PROGRESSBAR))->setValue(battleBrix::inst()->getGrowthPercentage());
                                                ((ui_progressbar*)getNodeById(_ID_NODE_PROGRESSBAR))->setText(battleBrix::inst()->getLevelString());
                                            })
                         , 1.f, (reward.growth < 0) ? true : false);
    
    if(reward.point > 0) {
        this->getNodeById(ID_NODE_ENDING_REWARD_POINT)->setVisible(true);
        guiExt::runFlyEffect(this->getNodeById(ID_NODE_ENDING_REWARD_POINT)
                             , CallFunc::create([=]() {
            ((ui_icon*)this->getNodeById(_ID_NODE_LABEL_POINT))->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_POINT));
        }), 1.2f);
    }
    
    if(reward.heart > 0) {
        this->getNodeById(ID_NODE_ENDING_REWARD_HEART)->setVisible(true);
        guiExt::runFlyEffect(this->getNodeById(ID_NODE_ENDING_REWARD_HEART)
                             , CallFunc::create([=]() {
            ((ui_icon*)this->getNodeById(_ID_NODE_LABEL_HEART))->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_HEART));
        }), 1.2f);
    }
    return true;
}

void SceneEnding::callback(Ref* pSender, int from, int link) {
    switch(link) {
        case 0:
            this->replaceScene(SceneMain::create());
            break;
        case 1:
            onAgain();
            break;
    }
}

void SceneEnding::onAgain() {
    ((ui_icon*)this->getNodeById(_ID_NODE_LABEL_HEART))->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_HEART));
    if(battleBrix::inst()->payForPlay(battleBrix::inst()->mItemSelected.getTotalPoint())) {
        ((ui_icon*)this->getNodeById(_ID_NODE_LABEL_POINT))->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_POINT));
        ((ui_icon*)this->getNodeById(_ID_NODE_LABEL_HEART))->setText(battleBrix::inst()->getText("", _ID_NODE_LABEL_HEART));
        guiExt::runScaleEffect(this->getNodeById(_ID_NODE_LABEL_POINT));
        guiExt::runScaleEffect(this->getNodeById(_ID_NODE_LABEL_HEART), CallFunc::create([=](){
            this->replaceScene(ScenePlay::create());
        }));
    } else {
        //alert
    }
    
}
