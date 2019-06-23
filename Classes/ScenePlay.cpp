
#include "ScenePlay.h"
#include "Scenes.h"
#include "ui/ui_ext.h"
#include "library/pch.h"
#include <functional>

#define SPEED_BASE .55f
#define PHYSICSMATERIAL             PhysicsMaterial(0.5f, 1.f, 0.f)
#define PHYSICSMATERIAL_OBSTACLE    PhysicsMaterial(0.f, 1.f, 0.f)
#define PHYSICSMATERIAL_BOARD    PhysicsMaterial(0.f, 1.f, 0.f)
#define GRID_AREA Vec2(8.f, 13.f)
//#define GRID_AREA Vec2(2.f, 3.f)
#define RATIO_OBSTACLE_PER_GRID 0.6f
#define _ID_BOTTOM 0
#define _ID_BRIX_START 101
#define _ID_GIFT 100
#define BALL_INIT_POSITION_Y 7

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
void ScenePlay::PLAYER::init(ScenePlay* p, const string& name, int layerId, int hpId, int mpId, int ballId, int alertId, int labelId, int fnId) {
    this->name = name;
    pScene = p;
    this->ballId = ballId;
    
    if(p->mBrixLayerRatio == -1.f)
        layer = p->getNodeById(layerId);
    else {
        auto pLayer = p->getNodeById(layerId);
        layer = gui::inst()->createLayout(Size(pLayer->getContentSize().height * p->mBrixLayerRatio, pLayer->getContentSize().height), "", false, Color3B::MAGENTA);
        pLayer->addChild(layer);
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
    float speed = SPEED_BASE + (battleBrix::inst()->mUserData.level * 0.05f);
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
    
    std::function<void()> f[] =  {
        std::bind( &ScenePlay::PLAYER::addBrix0, this)
        , std::bind( &ScenePlay::PLAYER::addBrix1, this)
        , std::bind( &ScenePlay::PLAYER::addBrix2, this)
        , std::bind( &ScenePlay::PLAYER::addBrix3, this)
        , std::bind( &ScenePlay::PLAYER::addBrix4, this)
    };
    
    f[fnId]();
    
    //add gift 좀더 생각해 봐야겠당
    
//    auto gift = gui::inst()->addSpriteAutoDimension(3, 1, "icons8-wedding-gift-96.png", layerBrix, ALIGNMENT_CENTER, GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//    gui::inst()->setScale(gift, gridSize.width * 0.75f);
//    auto posStart = gift->getPosition();
//    pScene->setPhysicsBodyRect(gift
//                               , PHYSICSMATERIAL_OBSTACLE
//                               , false
//                               , _ID_GIFT
//                               , -1
//                               , -1 //_PHYSICS_ID_MY_BALL
//                               , ballId
//                               );
//    float d = (float)(max(10, getRandValue(20))) / 10.f;
//    auto seq = Sequence::create(
//                                Hide::create()
//                                , MoveTo::create(d, Vec2(0, layerBrix->getContentSize().height * 1.5))
//                                , DelayTime::create(3.f)
//                                , Show::create()
//                                , MoveTo::create(d, posStart)
//                                , DelayTime::create(3.f)
//                                , NULL);
//    gift->runAction(RepeatForever::create(seq));
    
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
    ball = guiExt::drawCircleForPhysics(layer, position, obstacleSize.height / 2.f, color);
    pScene->setPhysicsBodyCircle(ball, PHYSICSMATERIAL, true, ballId);
    
    ball->getPhysicsBody()->setVelocityLimit(layerBrix->getContentSize().height * 2.f);
}
//createBoard ===========================================================================
void ScenePlay::PLAYER::createBoard() {
    board = LayerColor::create(Color4B::RED);
    board->setContentSize(Size(gridSize.width * 2.f, gridSize.height / 3.f));
    Vec2 pos = gui::inst()->getPointVec2(1, 11, ALIGNMENT_CENTER, layer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    pos.x -= board->getContentSize().width / 2.f;
    pos.y -= board->getContentSize().height / 2.f;
    board->setPosition(pos);
    
    Size size = Size(board->getContentSize().width / 4.f, board->getContentSize().height);
    
    for(int n=0; n < 4; n++) {
        auto rect = guiExt::drawRectForPhysics(board
                                               , gui::inst()->getPointVec2(n, 0, ALIGNMENT_CENTER, board->getContentSize(), Vec2(4,1), Vec2::ZERO, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO)
                                               , size
                                               , ui_wizard_share::inst()->getPalette()->getColor("RED")
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
void ScenePlay::PLAYER::decreseHP(float f) {
    auto val = hp->setValueDecrese(f);
    hp->blink();
    vibrate();
    
    if(val <= 0.3f) {
        alert->setVisible(true);
        alert->runAction(RepeatForever::create(Blink::create(0.4f, 1)));
    }
}
// onContact ===========================================================================
bool ScenePlay::PLAYER::onContact(int id, bool toRight) {
    if(ball == NULL)
        return false;
    
    bool ret = false;
    
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
        
        //board에서 부터 top까지
//        vel.y -= (board->getPosition().y + board->getContentSize().height);
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
//        vel = Vec2(0, 130);
        ball->getPhysicsBody()->setVelocity(vel);
//        CCLOG("%s \t Vel =[%f, %f] Ball = [%f, %f]",this->name.c_str(), vel.x, vel.y, ball->getPhysicsBody()->getVelocity().x, ball->getPhysicsBody()->getVelocity().y);
        
    } else if(id >= _ID_BRIX_START) {
        combo++;
        
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
        label->setPosition(ballPosition);
        label->runAction( Sequence::create(ScaleBy::create(0.3, 1.5), RemoveSelf::create(), NULL) );
        
        //brix vibration
        if(!brixEffectFlagMap[id]) {
            brixEffectFlagMap[id] = true;
            guiExt::addVibrateEffect(layerBrix->getChildByTag(id), CallFunc::create([=](){ brixEffectFlagMap[id] = false; }));
        }
        //attack
        if(mp->setValueIncrese(fIncrease) >= 1.f) {
            mp->setValue(0.f);
            mp->blink();
            //pScene->getNodeById(0)
            Vec2 pos = Vec2(layer->getContentSize().width / 2.f, layer->getContentSize().height * .75f);
            guiExt::addScaleEffect(layer, "icons8-action-96.png", "ATTACK", ui_wizard_share::inst()->getPalette()->getColor("GRAY"), NULL, .4f, .4f, pos, 150);

            ret = true;
        }
    }
    
    return ret;
}
// onBomb ===========================================================================
void ScenePlay::PLAYER::onBomb() {
    Vec2 pos = Vec2(layer->getContentSize().width / 2.f, layer->getContentSize().height * .75f);
    guiExt::addScaleEffect(layer, "icons8-atomic-bomb-96.png", "BOMB", ui_wizard_share::inst()->getPalette()->getColor("GRAY"), NULL, .4f, .4f, pos, 150);
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
// addBrix0 ===========================================================================
void ScenePlay::PLAYER::addBrix0() {
    
    int brixId = _ID_BRIX_START;
    for(int n=0; n < BALL_INIT_POSITION_Y; n++)
    {
        Vec2 position = gui::inst()->getPointVec2(getRandValue(8), n, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[(int)n], true, .1f);
        pScene->setPhysicsBodyRect(rect
                                   , PHYSICSMATERIAL_OBSTACLE
                                   , false
                                   , brixId
                                   , -1
                                   , -1 //_PHYSICS_ID_MY_BALL
                                   , ballId
                                   );
//        int d = max(10, getRandValue(30));
//        float f = (float)d / 10.f;
//        auto seq = Sequence::create(MoveTo::create(f, Vec2(layerBrix->getContentSize().width - rect->getContentSize().width, rect->getPosition().y)), MoveTo::create(f, Vec2(0, rect->getPosition().y)), NULL);
//        rect->runAction(RepeatForever::create(seq));
        
        brixEffectFlagMap[brixId] = false;
        brixId++;
    }
}
// addBrix1 ===========================================================================
void ScenePlay::PLAYER::addBrix1() {
    
    int brixId = _ID_BRIX_START;
    for(int n=0; n < BALL_INIT_POSITION_Y; n = n + 2)
    {
        Vec2 position = gui::inst()->getPointVec2(0, n, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[(int)n], true, .1f);
        pScene->setPhysicsBodyRect(rect
                                   , PHYSICSMATERIAL_OBSTACLE
                                   , false
                                   , brixId
                                   , -1
                                   , -1 //_PHYSICS_ID_MY_BALL
                                   , ballId
                                   );
        int d = max(10, getRandValue(30));
        float f = (float)d / 10.f;
        auto seq = Sequence::create(MoveTo::create(f, Vec2(layerBrix->getContentSize().width - rect->getContentSize().width, rect->getPosition().y)), MoveTo::create(f, Vec2(0, rect->getPosition().y)), NULL);
        rect->runAction(RepeatForever::create(seq));
        
        brixEffectFlagMap[brixId] = false;
        brixId++;
    }
}
// addBrix2 ===========================================================================
void ScenePlay::PLAYER::addBrix2() {
    
    float yEnd = gui::inst()->getPointVec2(0, 7, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
    float yStart = gui::inst()->getPointVec2(0, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
    
    int brixId = _ID_BRIX_START;
    for(int n=0; n < 8; n = n + 2)
    {
        Vec2 position = gui::inst()->getPointVec2(n, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[(int)n], true, .1f);
        pScene->setPhysicsBodyRect(rect
                                   , PHYSICSMATERIAL_OBSTACLE
                                   , false
                                   , brixId
                                   , -1
                                   , -1 //_PHYSICS_ID_MY_BALL
                                   , ballId
                                   );
        int d = max(10, getRandValue(30));
        float f = (float)d / 10.f;
        auto seq = Sequence::create(MoveTo::create(f, Vec2(rect->getPosition().x, yEnd)), MoveTo::create(f, Vec2(rect->getPosition().x, yStart)), NULL);
        rect->runAction(RepeatForever::create(seq));
        
        brixEffectFlagMap[brixId] = false;
        brixId++;
    }
}
// addBrix3 ===========================================================================
void ScenePlay::PLAYER::addBrix3() {
    
    float yEnd = gui::inst()->getPointVec2(0, 7, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
    float yStart = gui::inst()->getPointVec2(0, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
    float xStart = gui::inst()->getPointVec2(1, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).x;
    float xEnd = gui::inst()->getPointVec2(6, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).x;
    
    
    int brixId = _ID_BRIX_START;
    int colorIdx = 0;
    
    //양쪽 side
    
    for(int n=0; n < 8; n = n + 7)
    {
        Vec2 position = gui::inst()->getPointVec2(n, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[colorIdx++], true, .1f);
        pScene->setPhysicsBodyRect(rect
                                   , PHYSICSMATERIAL_OBSTACLE
                                   , false
                                   , brixId
                                   , -1
                                   , -1 //_PHYSICS_ID_MY_BALL
                                   , ballId
                                   );
        int d = max(10, getRandValue(30));
        float f = (float)d / 10.f;
        auto seq = Sequence::create(MoveTo::create(f, Vec2(rect->getPosition().x, yEnd)), MoveTo::create(f, Vec2(rect->getPosition().x, yStart)), NULL);
        rect->runAction(RepeatForever::create(seq));
        
        brixEffectFlagMap[brixId] = false;
        brixId++;
    }
    
    for(int n=0; n <= 6; n = n + 3)
    {
        Vec2 position = gui::inst()->getPointVec2(0, n, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[colorIdx++], true, .1f);
        pScene->setPhysicsBodyRect(rect
                                   , PHYSICSMATERIAL_OBSTACLE
                                   , false
                                   , brixId
                                   , -1
                                   , -1 //_PHYSICS_ID_MY_BALL
                                   , ballId
                                   );
        int d = max(10, getRandValue(30));
        float f = (float)d / 10.f;
        auto seq = Sequence::create(MoveTo::create(f, Vec2(xEnd, rect->getPosition().y)), MoveTo::create(f, Vec2(xStart, rect->getPosition().y)), NULL);
        rect->runAction(RepeatForever::create(seq));
        
        brixEffectFlagMap[brixId] = false;
        brixId++;
    }
}
// addBrix4 ===========================================================================
void ScenePlay::PLAYER::addBrix4() {
    
    float yEnd = gui::inst()->getPointVec2(0, 7, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
    float yStart = gui::inst()->getPointVec2(0, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO).y;
    Vec2 start1 = gui::inst()->getPointVec2(1, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    Vec2 start2 = gui::inst()->getPointVec2(6, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    Vec2 end1 = gui::inst()->getPointVec2(6, 7, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    Vec2 end2 = gui::inst()->getPointVec2(1, 7, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    
    
    int brixId = _ID_BRIX_START;
    int colorIdx = 0;
    
    //양쪽 side
    
    for(int n=0; n < 8; n = n + 7)
    {
        Vec2 position = gui::inst()->getPointVec2(n, 0, ALIGNMENT_CENTER, layerBrix->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
        auto rect = guiExt::drawRectForPhysics(layerBrix, position, obstacleSize, pScene->mColors[colorIdx++], true, .1f);
        pScene->setPhysicsBodyRect(rect
                                   , PHYSICSMATERIAL_OBSTACLE
                                   , false
                                   , brixId
                                   , -1
                                   , -1 //_PHYSICS_ID_MY_BALL
                                   , ballId
                                   );
        int d = max(10, getRandValue(30));
        float f = (float)d / 10.f;
        auto seq = Sequence::create(MoveTo::create(f, Vec2(rect->getPosition().x, yEnd)), MoveTo::create(f, Vec2(rect->getPosition().x, yStart)), NULL);
        rect->runAction(RepeatForever::create(seq));
        
        brixEffectFlagMap[brixId] = false;
        brixId++;
    }
    
    for(int n=0; n < 2; n++)
    {
        Vec2 positionStart = (n==0) ? start1 : start2;
        Vec2 positionEnd = (n==0) ? end1 : end2;
        auto rect = guiExt::drawRectForPhysics(layerBrix, positionStart, obstacleSize, pScene->mColors[colorIdx++], true, .1f);
        pScene->setPhysicsBodyRect(rect
                                   , PHYSICSMATERIAL_OBSTACLE
                                   , false
                                   , brixId
                                   , -1
                                   , -1 //_PHYSICS_ID_MY_BALL
                                   , ballId
                                   );
        int d = max(10, getRandValue(30));
        float f = (float)d / 10.f;
        auto seq = Sequence::create(MoveTo::create(f, positionEnd), MoveTo::create(f, positionStart), NULL);
        rect->runAction(RepeatForever::create(seq));
        
        brixEffectFlagMap[brixId] = false;
        brixId++;
    }
}
/* ----------------------------------------------------------------------------------------------------------------
 
 
 ScenePlay

 
 ---------------------------------------------------------------------------------------------------------------- */
bool ScenePlay::init()
{
    int n = 0;
    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("PINK_LIGHT");
    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("BLUE_LIGHT");
    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("YELLOW_LIGHT");
    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("PURPLE_LIGHT");
    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("GREEN_LIGHT");
    
    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("PINK_LIGHT");
    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("BLUE_LIGHT");
    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("YELLOW_LIGHT");
    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("PURPLE_LIGHT");
    mColors[n++] = ui_wizard_share::inst()->getPalette()->getColor("GREEN_LIGHT");
    
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
    
    int fnId = getRandValue(5);
    
    PLAYER me;
    me.init(this, "ME", ID_NODE_MY_AREA, ID_NODE_MY_HP, ID_NODE_MY_MP, _BALL_ID[0], ID_NODE_MY_ALERT, ID_NODE_MY_LABEL, fnId);
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
        p.init(this, "CPU" + to_string(n+1), areaId, hpId, mpId, _BALL_ID[n+1], alertId, labelId, fnId);
        mPlayers.push_back(p);
    }
    
//    gui::inst()->drawGrid(mLayer, mLayer->getContentSize(), GRID_AREA, Size::ZERO, Size::ZERO);
    
    guiExt::addMovingEffect(this->getNodeById(0)
                            , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_DEEP")
                            , ""
                            , "MAP " + to_string(fnId)
                            , ui_wizard_share::inst()->getPalette()->getColor("ORANGE")
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
// onSkill ===========================================================================
void ScenePlay::onSkill(int idx) {
    if(mIsEnd)
        return;
    CallFunc * pFn = NULL;
    switch(idx) {
        case 0: {
            pFn = CallFunc::create([=]()
            {
                mPlayers[_PLAYER_ID_ME].hp->setValueIncrese(DECREASE_HP * 2);
                for(int n = 1; n < mPlayers.size(); n++) {
                    mPlayers[n].onBomb();
                    mPlayers[n].decreseHP(DECREASE_HP / 2.f);
                }
            });
            break;
        }
        case 1: { //potion
            pFn = CallFunc::create([=]()
            {
                mPlayers[_PLAYER_ID_ME].hp->setValueIncrese(DECREASE_HP * 4);
            });
            break;
        }
        case 2: { //bomb
            pFn = CallFunc::create([=]()
            {
                for(int n = 1; n < mPlayers.size(); n++) {
                    mPlayers[n].onBomb();
                    mPlayers[n].decreseHP();
                }
            });
            break;
        }
    }
    
    if(pFn)
        guiExt::addMovingEffect(this->getNodeById(0)
                                , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_LIGHT2")
                                , battleBrix::inst()->mItems[idx].img
                                , battleBrix::inst()->mItems[idx].name
                                , ui_wizard_share::inst()->getPalette()->getColor("YELLOW")
                                , false
                                , 1.5f
                                , pFn
                                );
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
                onSkill(n);
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
                        mPlayers[i].decreseHP();
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
                                , ui_wizard_share::inst()->getPalette()->getColor("BLACK")
                                , false
                                , 1.5f
                                , CallFunc::create([=](){ this->onFinish(); })
                                );
    }
    
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
//    int nRanking = mPlayers[_PLAYER_ID_ME].ranking;
//    string szRanking = getRankString(nRanking);
//    
//    battleBrix::rewardData reward = battleBrix::inst()->getReward(nRanking);
//    //    ID_NODE_ENDING = 90,
//    this->getNodeById(ID_NODE_ENDING)->setVisible(true);
//    
//    //    ID_NODE_ENDING_RANKING,
//    ((Label*)getNodeById(ID_NODE_ENDING_RANKING))->setString(szRanking);
//    
//    //    ID_NODE_ENDING_PROGRESSBAR,
//    ((ui_progressbar*)getNodeById(ID_NODE_ENDING_PROGRESSBAR))->setValue(battleBrix::inst()->getGrowthPercentage());
//    ((ui_progressbar*)getNodeById(ID_NODE_ENDING_PROGRESSBAR))->setText(battleBrix::inst()->getLevelString());
//    
//    //    ID_NODE_ENDING_REWARD,
//    auto pReward =((Label*)getNodeById(ID_NODE_ENDING_REWARD));
//    if(reward.growth >= 0) {
//        pReward->setString("+" + to_string(reward.growth));
//    } else {
//        pReward->setString(to_string(reward.growth));
//        pReward->setColor(ui_wizard_share::inst()->getPalette()->getColor3B("RED_DARK"));
//    }
//    
////    pReward->setColor(ui_wizard_share::inst()->getPalette()->getColor3B(szColorReward));
//    //    ID_NODE_ENDING_REWARD_POINT,
//    auto pPoint = ((ui_icon*)this->getNodeById(ID_NODE_ENDING_REWARD_POINT));
//    pPoint->setText((reward.point >= 0) ? "+" + to_string(reward.point) : to_string(reward.point));
//    
//    //    ID_NODE_ENDING_REWARD_HEART,
//    auto pHeart = ((ui_icon*)this->getNodeById(ID_NODE_ENDING_REWARD_HEART));
//    pHeart->setText((reward.heart >= 0) ? "+" + to_string(reward.heart) : to_string(reward.heart));
//    
//    //effect
//    guiExt::runFlyEffect(this->getNodeById(ID_NODE_ENDING_REWARD)
//                         , CallFunc::create([=]() {
//                                if(battleBrix::inst()->applyReward(nRanking)) {
//                                    //level up
//                                    auto levelup = this->getNodeById(ID_NODE_LEVELUP);
//                                    levelup->setVisible(true);
//                                    guiExt::runScaleEffect(levelup, NULL, 0.8f, true);
//                                    //auto bg = this->getNodeById(0);
//                                    auto particle = gui::inst()->createParticle("firework.plist", gui::inst()->getCenter());
//                                    this->addChild(particle);
//                                }
//                                ((ui_progressbar*)getNodeById(ID_NODE_ENDING_PROGRESSBAR))->setValue(battleBrix::inst()->getGrowthPercentage());
//                                ((ui_progressbar*)getNodeById(ID_NODE_ENDING_PROGRESSBAR))->setText(battleBrix::inst()->getLevelString());
//                            })
//                         , 1.f, (reward.growth < 0) ? true : false);
//    
//    if(reward.point > 0)
//        guiExt::runScaleEffect(this->getNodeById(ID_NODE_ENDING_REWARD_POINT));
//    
//    if(reward.heart > 0)
//        guiExt::runScaleEffect(this->getNodeById(ID_NODE_ENDING_REWARD_HEART));
//}

//====================================================================
bool SceneEnding::init()
{
    this->loadFromJson("ending", "ending.json");
    
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
