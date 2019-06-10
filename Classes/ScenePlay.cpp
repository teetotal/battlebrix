
#include "ScenePlay.h"
#include "Scenes.h"
#include "ui_ext.h"
#include "library/util.h"

#define SEPPED 1.5f
#define PHYSICSMATERIAL             PhysicsMaterial(.1f, 1.f, 0.f)
#define PHYSICSMATERIAL_OBSTACLE    PhysicsMaterial(.1f, 1.f, 0.f)
#define PHYSICSMATERIAL_BOARD    PhysicsMaterial(.1f, 1.f, 0.f)
#define GRID_AREA Vec2(8.f, 13.f)
//#define GRID_AREA Vec2(2.f, 3.f)
#define RATIO_OBSTACLE_PER_GRID 0.6f
#define _ID_BOTTOM 0

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

enum _PHYSICS_ID {
    _PHYSICS_ID_MY_BALL = 0x01 << 0,
    _PHYSICS_ID_MY_BOARD = 0x01 << 1,
    _PHYSICS_ID_MY_OBSTACLE_1 = 0x01 << 2,
    
    _PHYSICS_ID_1_BALL = 0x01 << 3
};

enum ID_NODE {
    ID_NODE_BG = 0,
    ID_HP_MY,
    ID_MP_MY,
    ID_HP_OTHER,
    ID_MP_OTHER,
    ID_NODE_MY_AREA = 10,
    ID_NODE_OTHER_AREA
};

//static COLOR_RGB colors[5] = {
//    ui_wizard_share::inst()->getPalette()->getColor("PINK_LIGHT"),
//    ui_wizard_share::inst()->getPalette()->getColor("YELLOW_LIGHT"),
//    ui_wizard_share::inst()->getPalette()->getColor("GREEN_LIGHT"),
//    ui_wizard_share::inst()->getPalette()->getColor("BLUE_LIGHT"),
//    ui_wizard_share::inst()->getPalette()->getColor("PURPLE_LIGHT")
//};
// ----------------------------------------------------------------------------------------------------------------
void ScenePlay::PLAYER::init(ScenePlay* p, int layerId, int hpId, int mpId, int ballId) {
    pScene = p;
    this->ballId = ballId;
    layer = p->getNodeById(layerId);
    hp = (ui_progressbar*)p->getNodeById(hpId);
    mp = (ui_progressbar*)p->getNodeById(mpId);
    hp->setValue(1.f);
    mp->setValue(0.f);
    p->initPhysicsBody(layer, PHYSICSMATERIAL_OBSTACLE, false, SEPPED);
    
    gridSize   = gui::inst()->getGridSize(layer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO);
    obstacleSize = gridSize;
    obstacleSize.width *= RATIO_OBSTACLE_PER_GRID;
    obstacleSize.height *= RATIO_OBSTACLE_PER_GRID;
    fontSizeCombo = gui::inst()->getFontSize(gridSize) * 1.f;
    
    // create board
    createBoard();
    // create bottom
    createBottom();
    
    for(int n=0; n < 5; n ++) {
        addObstacle(Vec2(getRandValue(8), n), obstacleId++);
    }
}
//createBall ===========================================================================
void ScenePlay::PLAYER::createBall() {
    COLOR_RGB color = ui_wizard_share::inst()->getPalette()->getColor("WHITE");
    
    Vec2 position = gui::inst()->getPointVec2(2, 5, ALIGNMENT_CENTER, layer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    ball = guiExt::drawCircleForPhysics(layer, position, obstacleSize.height / 2.f, color);
    pScene->setPhysicsBodyCircle(ball, PHYSICSMATERIAL, true
                                 , ballId
                                 , -1
                                 , -1 // _PHYSICS_ID_MY_BOARD | _PHYSICS_ID_MY_OBSTACLE_1
                                 ,  _PHYSICS_ID_MY_OBSTACLE_1
                                 );
    ball->getPhysicsBody()->setVelocityLimit(layer->getContentSize().width * 1.f);
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
                                 , _PHYSICS_ID_MY_BALL
                                 );
    }
    
    layer->addChild(board);
}
//createBottom ===========================================================================
void ScenePlay::PLAYER::createBottom() {
    auto rect = guiExt::drawRectForPhysics(layer
                                           , Vec2(layer->getContentSize().width / 2.f, gridSize.height / 4.f)
                                           , Size(layer->getContentSize().width, gridSize.height / 4.f)
                                           , ui_wizard_share::inst()->getPalette()->getColor("BLACK")
                                           , true
                                           , 0.f);
    pScene->setPhysicsBodyRect(rect
                             , PHYSICSMATERIAL
                             , false
                             , _ID_BOTTOM
                             , -1
                             , -1 //_PHYSICS_ID_MY_BALL
                             , _PHYSICS_ID_MY_BALL
                             );
}
// add Obstacle ===========================================================================
void ScenePlay::PLAYER::addObstacle() {

    pScene->getPhysicsWorld()->setAutoStep(false);
    pScene->getPhysicsWorld()->step(0.0f);
    vector<Vec2> vec;
    //전체 삭제
    for(int n=0; n < obstacles.size(); n++) {
        layer->removeChild(obstacles[n]);
    }
    obstacles.clear();
    
    for(map<int, Vec2>::iterator it = obstaclesPos.begin(); it != obstaclesPos.end(); ++it) {
        Vec2 pos = it->second;
        if(pos.y < 6)
            vec.push_back(Vec2(pos.x, pos.y + 1));
    }
    obstaclesPos.clear();
    
    //생성
    for(int n=0; n < vec.size(); n++) {
        addObstacle(vec[n], obstacleId++);
    }
    
    vec.clear();
    
    //신규 추가
    Vec2 pos = Vec2(getRandValue(8), 0);
    addObstacle(pos, obstacleId++);
    pScene->getPhysicsWorld()->setAutoStep(true);
}

void ScenePlay::PLAYER::addObstacle(Vec2 pos, int id) {
    
    Vec2 position = gui::inst()->getPointVec2(pos.x, pos.y, ALIGNMENT_CENTER, layer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    auto rect = guiExt::drawRectForPhysics(layer, position, obstacleSize, pScene->mColors[(int)pos.y], true, .1f);
    pScene->setPhysicsBodyRect(rect
                               , PHYSICSMATERIAL_OBSTACLE
                               , false
                               , id
                               , -1
                               , -1 //_PHYSICS_ID_MY_BALL
                               , ballId
                               );
    //map 추가
//    OBSTACLE obs;
//    obs.pDrawNode = rect;
//    obs.position = pos;
//    mapObstacle[id] = obs;
    obstaclesPos[id] = pos;
    obstacles.push_back(rect);
}
// delete Obstacle ===========================================================================
void ScenePlay::PLAYER::deleteObstacle(int id) {
    layer->removeChildByTag(id);
    obstaclesPos.erase(id);
//    CCASSERT(p, "INVALID Obstacle");
    
}
// vibrate ===========================================================================
void ScenePlay::PLAYER::vibrate() {
    if(lockShake)
        return;
    lockShake = true;
    
    Vec2 pos = layer->getPosition();
    float duration = 0.1f;
    float width = 5.f;
    layer->runAction(Sequence::create( MoveTo::create(duration, Vec2(pos.x - width, pos.y - width))
                                      , MoveTo::create(duration, Vec2(pos.x + width, pos.y + width))
                                      , MoveTo::create(duration, Vec2(pos.x - width, pos.y))
                                      , MoveTo::create(duration, Vec2(pos.x + width, pos.y))
                                      , MoveTo::create(duration, pos)
                                      , CallFunc::create([=]() { lockShake = false; })
                                      , NULL));
}
// decreseHP ===========================================================================
void ScenePlay::PLAYER::decreseHP() {
    hp->setValueDecrese(0.1);
    hp->blink();
    vibrate();
}
// onContact ===========================================================================
bool ScenePlay::PLAYER::onContact(int id, bool toRight) {
    if(ball == NULL)
        return false;
    
    CCLOG("id %d", id);
    bool ret = false;
    
    const Vec2 ballPosition = ball->getPosition();
    
    if(id == _ID_BOTTOM) {
        hp->setValueDecrese(0.1);
        hp->blink();
        vibrate();
    }
    else if(id >= _BOARD_ID_L && id <= _BOARD_ID_R) {
        //동시 충돌 방지
        clock_t now = clock();
        if(now - latestCollisionWithBoard < 10) {
            CCLOG("%lf concurrent collision %d", latestCollisionWithBoard, id);
            return false;
        }
        else
            latestCollisionWithBoard = now;
        
        //보드에 맞지도 않았는데 튀어 나가는 현상이 있음.
        if(ballPosition.y > board->getPosition().y + board->getContentSize().height * 2.f)
            return false;
        
//        CCLOG("Other = %d, (%f)", id, ballPosition.y);
        
        switch((_BOARD_ID)id) {
            case _BOARD_ID_L:
                ball->getPhysicsBody()->setVelocity(Vec2(layer->getContentSize().width * -1.f, layer->getContentSize().height));
                break;
            case _BOARD_ID_LM:
                ball->getPhysicsBody()->setVelocity(Vec2(layer->getContentSize().width * -0.2f, layer->getContentSize().height));
                break;
            case _BOARD_ID_RM:
                ball->getPhysicsBody()->setVelocity(Vec2(layer->getContentSize().width * 0.2f, layer->getContentSize().height));
                break;
            case _BOARD_ID_R:
                ball->getPhysicsBody()->setVelocity(Vec2(layer->getContentSize()));
                break;
            default:
                break;
        }
    } else if(id >= 100) {
        deleteObstacle(id);
        auto label = gui::inst()->addLabelAutoDimension(0, 0, "COMBO", layer, fontSizeCombo, ALIGNMENT_CENTER, ui_wizard_share::inst()->getPalette()->getColor3B("ORANGE"));
        label->enableGlow(ui_wizard_share::inst()->getPalette()->getColor4B("BLACK"));
        label->setPosition(ballPosition);
        label->runAction( Sequence::create(ScaleBy::create(0.3, 1.5), RemoveSelf::create(), NULL) );
        
        if(mp->setValueIncrese(0.05f) >= 1.f) {
            mp->setValue(0.f);
            mp->blink();
            guiExt::addMovingEffect(pScene->getNodeById(0)
                                    , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_DEEP")
                                    , "icons8-action-96.png"
                                    , "ATTACK"
                                    , ui_wizard_share::inst()->getPalette()->getColor("GRAY")
                                    , toRight
                                    );
            ret = true;
        }
    }
    
    return ret;
}

/* ----------------------------------------------------------------------------------------------------------------
 
 ScenePlay
 
 ---------------------------------------------------------------------------------------------------------------- */
bool ScenePlay::init()
{
    mColors[0] = ui_wizard_share::inst()->getPalette()->getColor("PINK_LIGHT");
    mColors[1] = ui_wizard_share::inst()->getPalette()->getColor("YELLOW_LIGHT");
    mColors[2] = ui_wizard_share::inst()->getPalette()->getColor("GREEN_LIGHT");
    mColors[3] = ui_wizard_share::inst()->getPalette()->getColor("BLUE_LIGHT");
    mColors[4] = ui_wizard_share::inst()->getPalette()->getColor("PURPLE_LIGHT");
    
    this->loadFromJson("play", "play.json");
    
    TOUCH_INIT(ScenePlay);
    PHYSICS_CONTACT(ScenePlay);
    
    mPlayers[_PLAYER_ID_ME].init(this, ID_NODE_MY_AREA, ID_HP_MY, ID_MP_MY, _PHYSICS_ID_MY_BALL);
    mPlayers[_PLAYER_ID_OTHER].init(this, ID_NODE_OTHER_AREA, ID_HP_OTHER, ID_MP_OTHER, _PHYSICS_ID_1_BALL);
    
//    gui::inst()->drawGrid(mLayer, mLayer->getContentSize(), GRID_AREA, Size::ZERO, Size::ZERO);
    
    guiExt::addMovingEffect(this->getNodeById(0)
                            , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_DEEP")
                            , ""
                            , "CPU vs TEETOTAL"
                            , ui_wizard_share::inst()->getPalette()->getColor("ORANGE")
                            , false
                            , 1.5f
                            , CallFunc::create([=]()
    {
        mPlayers[_PLAYER_ID_ME].createBall();
        mPlayers[_PLAYER_ID_OTHER].createBall();
        
        this->schedule(schedule_selector(ScenePlay::timerAddObstacle), 1.f);
        this->schedule(schedule_selector(ScenePlay::timer), .1f);
    })
                            );
   
    return true;
}
void ScenePlay::timerAddObstacle(float f) {
    mPlayers[_PLAYER_ID_ME].addObstacle();
    mPlayers[_PLAYER_ID_OTHER].addObstacle();
}
void ScenePlay::timer(float f) {
    mPlayers[_PLAYER_ID_OTHER].board->setPosition(Vec2(mPlayers[_PLAYER_ID_OTHER].ball->getPosition().x, mPlayers[_PLAYER_ID_OTHER].board->getPosition().y));
}

void ScenePlay::callback(Ref* pSender, int from, int link) {
    switch(link) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 999:
            this->replaceScene(SceneMain::create());
            break;
        default:
            break;
    }
}

const string ScenePlay::getText(const string& defaultString, int id) {
    return defaultString;
}

// touch ===========================================================================
bool ScenePlay::onTouchBegan(Touch* touch, Event* event) {
    return true;
}
bool ScenePlay::onTouchEnded(Touch* touch, Event* event) {
    return true;
}

void ScenePlay::onTouchMoved(Touch *touch, Event *event) {
    Vec2 pos = Vec2(touch->getLocation().x - mPlayers[_PLAYER_ID_ME].layer->getPosition().x, mPlayers[_PLAYER_ID_ME].board->getPosition().y);
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
    if(isCollosion(contact, _PHYSICS_ID_MY_BALL, other)) {
//        CCLOG("Collision %d with %d, Category %d, %d, Collision %d, %d"
//              , _PHYSICS_ID_MY_BALL, other
//              , st.categoryA, st.categoryB
//              , st.collisionA, st.collisionB
//              );
    }
    
    if(isContact(contact, _PHYSICS_ID_MY_BALL, other))
    {
        if(mPlayers[_PLAYER_ID_ME].onContact(other)) {
            mPlayers[_PLAYER_ID_OTHER].decreseHP();
        }
    }
    
    if(isContact(contact, _PHYSICS_ID_1_BALL, other))
    {
        if(mPlayers[_PLAYER_ID_OTHER].onContact(other, true)) {
            mPlayers[_PLAYER_ID_ME].decreseHP();
        }
    }
    
    bool isEnd = false;
    string szEnd, szColor;
    if(mPlayers[_PLAYER_ID_OTHER].getHPValue() <= 0.f) {
        //WIN
        szEnd = "WIN";
        szColor = "ORANGE";
        isEnd = true;
    } else if(mPlayers[_PLAYER_ID_ME].getHPValue() <= 0.f) {
        //Lose
        szEnd = "LOSE";
        szColor = "BLACK";
        isEnd = true;
    }
    
    if(isEnd) {
        this->getPhysicsWorld()->setAutoStep(false);
        this->getPhysicsWorld()->step(0.0f);
        
        guiExt::addMovingEffect(this->getNodeById(0)
                                , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_DEEP")
                                , ""
                                , szEnd
                                , ui_wizard_share::inst()->getPalette()->getColor(szColor)
                                , false
                                , 1.5f
                                , CallFunc::create([=](){ this->replaceScene(SceneMain::create()); })
                                );
    }
    
    return true;
}
