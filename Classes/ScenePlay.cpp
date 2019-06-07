
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
    
    _PHYSICS_ID_1_BALL = 0x01 << 10,
    _PHYSICS_ID_1_BOARD = 0x01 << 11,
    _PHYSICS_ID_1_OBSTACLE_1 = 0x01 << 12,
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
// ----------------------------------------------------------------------------------------------------------------
void ScenePlay::PLAYER::init(ScenePlay* p, int layerId, int hpId, int mpId) {
    layer = p->getNodeById(layerId);
    hp = (ui_progressbar*)p->getNodeById(hpId);
    mp = (ui_progressbar*)p->getNodeById(mpId);
    hp->setValue(1.f);
    mp->setValue(0.f);
    p->initPhysicsBody(layer, PHYSICSMATERIAL_OBSTACLE, false, SEPPED);
    /*
    // add ball
    ball = p->createBall(layer);
    // add board
    board = p->createBoard(layer);
    // add bottom
    p->createBottom(layer);
    */
    pScene = p;
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
bool ScenePlay::PLAYER::onContact(int id) {
    
    const Vec2 ballPosition = ball->getPosition();
    
    if(id == _ID_BOTTOM) {
        hp->setValueDecrese(0.1);
        mp->blink();
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
        
        CCLOG("Other = %d, (%f)", id, ballPosition.y);
        
        
        switch((_BOARD_ID)id) {
            case _BOARD_ID_L:
                ball->getPhysicsBody()->setVelocity(Vec2(layer->getContentSize().width * -1.f, layer->getContentSize().height));
                break;
            case _BOARD_ID_LM:
                ball->getPhysicsBody()->setVelocity(Vec2(layer->getContentSize().width * -0.5f, layer->getContentSize().height));
                break;
            case _BOARD_ID_RM:
                ball->getPhysicsBody()->setVelocity(Vec2(layer->getContentSize().width * 0.5f, layer->getContentSize().height));
                break;
            case _BOARD_ID_R:
                ball->getPhysicsBody()->setVelocity(Vec2(layer->getContentSize()));
                break;
            default:
                break;
        }
        
    } else {
        auto label = gui::inst()->addLabelAutoDimension(0, 0, "COMBO", layer, pScene->mFontSizeCombo, ALIGNMENT_CENTER, ui_wizard_share::inst()->getPalette()->getColor3B("ORANGE"));
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
                                    , false
                                    );
            return true;
        }
    }
    return false;

}
// ----------------------------------------------------------------------------------------------------------------
bool ScenePlay::init()
{
    colors[0] = ui_wizard_share::inst()->getPalette()->getColor("PINK_LIGHT");
    colors[1] = ui_wizard_share::inst()->getPalette()->getColor("YELLOW_LIGHT");
    colors[2] = ui_wizard_share::inst()->getPalette()->getColor("GREEN_LIGHT");
    colors[3] = ui_wizard_share::inst()->getPalette()->getColor("BLUE_LIGHT");
    colors[4] = ui_wizard_share::inst()->getPalette()->getColor("PURPLE_LIGHT");
    
    this->loadFromJson("play", "play.json");
//    ((LoadingBar *)this->getNodeById(1))->setDirection(LoadingBar::Direction::RIGHT);
//    ((LoadingBar *)this->getNodeById(2))->setDirection(LoadingBar::Direction::RIGHT);
    
    TOUCH_INIT(ScenePlay);
    PHYSICS_CONTACT(ScenePlay);
    
    mPlayers[_PLAYER_ID_ME].init(this, ID_NODE_MY_AREA, ID_HP_MY, ID_MP_MY);
    mPlayers[_PLAYER_ID_OTHER].init(this, ID_NODE_OTHER_AREA, ID_HP_OTHER, ID_MP_OTHER);
    
    mGridSize   = gui::inst()->getGridSize(mPlayers[_PLAYER_ID_ME].layer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO);
    mObstacleSize = mGridSize;
    mObstacleSize.width *= RATIO_OBSTACLE_PER_GRID;
    mObstacleSize.height *= RATIO_OBSTACLE_PER_GRID;
    mFontSizeCombo = gui::inst()->getFontSize(mGridSize) * 1.f;
    
    for(int n=0; n < 15; n ++) {
        addObstacle(mPlayers[_PLAYER_ID_ME].layer, Vec2(getRandValue(8), getRandValue(5)));
        addObstacle(mPlayers[_PLAYER_ID_OTHER].layer, Vec2(getRandValue(8), getRandValue(5)));
    }
    // add ball
    mPlayers[_PLAYER_ID_ME].ball = createBall(mPlayers[_PLAYER_ID_ME].layer);
    // add board
    mPlayers[_PLAYER_ID_ME].board = createBoard(mPlayers[_PLAYER_ID_ME].layer);
    // add bottom
    createBottom(mPlayers[_PLAYER_ID_ME].layer);
    
//    gui::inst()->drawGrid(mLayer, mLayer->getContentSize(), GRID_AREA, Size::ZERO, Size::ZERO);
    
    return true;
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

//createBall ===========================================================================
Node * ScenePlay::createBall(Node * p) {
    this->getPhysicsWorld()->setAutoStep(false);
    this->getPhysicsWorld()->step(0.0f);
    COLOR_RGB color = ui_wizard_share::inst()->getPalette()->getColor("WHITE");
    
    Vec2 position = gui::inst()->getPointVec2(2, 5, ALIGNMENT_CENTER, p->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    auto ball = guiExt::drawCircleForPhysics(p, position, mObstacleSize.height / 2.f, color);
    this->setPhysicsBodyCircle(ball, PHYSICSMATERIAL, true
                               , _PHYSICS_ID_MY_BALL
                               , -1
                               , -1 // _PHYSICS_ID_MY_BOARD | _PHYSICS_ID_MY_OBSTACLE_1
                               ,  _PHYSICS_ID_MY_OBSTACLE_1
                               );
    ball->getPhysicsBody()->setVelocityLimit(p->getContentSize().width * 1.f);
    this->getPhysicsWorld()->setAutoStep(true);
   
    return ball;
}

//createBoard ===========================================================================
Node * ScenePlay::createBoard(Node * p) {
    auto layer = LayerColor::create(Color4B::RED);
    layer->setContentSize(Size(mGridSize.width * 2.f, mGridSize.height / 3.f));
    Vec2 pos = gui::inst()->getPointVec2(1, 11, ALIGNMENT_CENTER, p->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    pos.x -= layer->getContentSize().width / 2.f;
    pos.y -= layer->getContentSize().height / 2.f;
    layer->setPosition(pos);
    
    Size size = Size(layer->getContentSize().width / 4.f, layer->getContentSize().height);
   
    for(int n=0; n < 4; n++) {
        auto rect = guiExt::drawRectForPhysics(layer
                                               , gui::inst()->getPointVec2(n, 0, ALIGNMENT_CENTER, layer->getContentSize(), Vec2(4,1), Vec2::ZERO, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO)
                                               , size
                                               , ui_wizard_share::inst()->getPalette()->getColor("RED")
                                               , true
                                               , 0.f);
        this->setPhysicsBodyRect(rect
                                 , PHYSICSMATERIAL_BOARD
                                 , false
                                 , _BOARD_ID_L + n
                                 , -1
                                 , -1 //_PHYSICS_ID_MY_BALL
                                 , _PHYSICS_ID_MY_BALL
                                 );
    }
    
    
    p->addChild(layer);
    
    return layer;
}
//createBottom ===========================================================================
Node * ScenePlay::createBottom(Node * p) {
    auto rect = guiExt::drawRectForPhysics(p
                                           , Vec2(p->getContentSize().width / 2.f, mGridSize.height / 4.f)
                                           , Size(p->getContentSize().width, mGridSize.height / 4.f)
                                           , ui_wizard_share::inst()->getPalette()->getColor("BLACK")
                                           , true
                                           , .1f);
    this->setPhysicsBodyRect(rect
                             , PHYSICSMATERIAL
                             , false
                             , _ID_BOTTOM
                             , -1
                             , -1 //_PHYSICS_ID_MY_BALL
                             , _PHYSICS_ID_MY_BALL
                             );
    
    return rect;
}
// add Obstacle ===========================================================================
void ScenePlay::addObstacle(Node * layer, Vec2 pos) {
    Vec2 position = gui::inst()->getPointVec2(pos.x, pos.y, ALIGNMENT_CENTER, layer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    auto rect = guiExt::drawRectForPhysics(layer, position, mObstacleSize, colors[(int)pos.y], true, .1f);
    int id = 123;
    this->setPhysicsBodyRect(rect, PHYSICSMATERIAL_OBSTACLE, false
                             , id
                             , -1
                             , -1 //_PHYSICS_ID_MY_BALL
                             , _PHYSICS_ID_MY_BALL
                             );
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
    bitmask st = getBitmask(contact);
    if(isCollosion(contact, _PHYSICS_ID_MY_BALL, other)) {
//        CCLOG("Collision %d with %d, Category %d, %d, Collision %d, %d"
//              , _PHYSICS_ID_MY_BALL, other
//              , st.categoryA, st.categoryB
//              , st.collisionA, st.collisionB
//              );
    }
    
    if(isContact(contact, _PHYSICS_ID_MY_BALL, other)) {
        
        if(mPlayers[_PLAYER_ID_ME].onContact(other)) {
            mPlayers[_PLAYER_ID_OTHER].decreseHP();
        }
        
    }
    return true;
}
