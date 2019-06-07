
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

bool ScenePlay::init()
{
    colors[0] = ui_wizard_share::inst()->getPalette()->getColor("PINK_LIGHT");
    colors[1] = ui_wizard_share::inst()->getPalette()->getColor("YELLOW_LIGHT");
    colors[2] = ui_wizard_share::inst()->getPalette()->getColor("GREEN_LIGHT");
    colors[3] = ui_wizard_share::inst()->getPalette()->getColor("BLUE_LIGHT");
    colors[4] = ui_wizard_share::inst()->getPalette()->getColor("PURPLE_LIGHT");
    
    mLockShakeMy = false;
    
    this->loadFromJson("play", "play.json");
//    ((LoadingBar *)this->getNodeById(1))->setDirection(LoadingBar::Direction::RIGHT);
//    ((LoadingBar *)this->getNodeById(2))->setDirection(LoadingBar::Direction::RIGHT);
    
    TOUCH_INIT(ScenePlay);
    PHYSICS_CONTACT(ScenePlay);
    
    mLayer      = getNodeById(ID_NODE_MY_AREA);
    mLayerOther = getNodeById(ID_NODE_OTHER_AREA);
    mGridSize   = gui::inst()->getGridSize(mLayer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO);
    mObstacleSize = mGridSize;
    mObstacleSize.width *= RATIO_OBSTACLE_PER_GRID;
    mObstacleSize.height *= RATIO_OBSTACLE_PER_GRID;
    mFontSizeCombo = gui::inst()->getFontSize(mGridSize) * 1.f;
    
    mProgressbarMyHP = (ui_progressbar*)getNodeById(ID_HP_MY);
    mProgressbarMyMP = (ui_progressbar*)getNodeById(ID_MP_MY);
    
    mProgressbarOtherHP = (ui_progressbar*)getNodeById(ID_HP_OTHER);
    mProgressbarOtherMP = (ui_progressbar*)getNodeById(ID_MP_OTHER);
    
    mProgressbarMyHP->setValue(1.f);
    mProgressbarMyMP->setValue(0.f);
    
    mProgressbarOtherHP->setValue(1.f);
    mProgressbarOtherMP->setValue(0.f);
    
    initPhysicsBody(mLayer, PHYSICSMATERIAL_OBSTACLE, false, SEPPED);
    initPhysicsBody(mLayerOther, PHYSICSMATERIAL_OBSTACLE, false, SEPPED);
    
    //------------------------------------------------//
//    gui::inst()->drawGrid(mLayer, mLayer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO);
    mBall = createBall();
    
    for(int n=0; n < 15; n ++) {
        addObstacle(mLayer, Vec2(getRandValue(8), getRandValue(5)));
        addObstacle(mLayerOther, Vec2(getRandValue(8), getRandValue(5)));
    }

    // add board
    mBoardMy = createBoard(mLayer);
    // add bottom
    createBottom(mLayer);
    
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
            mBall->getPhysicsBody()->setVelocity(Vec2(mLayer->getContentSize()));
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
Node * ScenePlay::createBall() {
    this->getPhysicsWorld()->setAutoStep(false);
    this->getPhysicsWorld()->step(0.0f);
    COLOR_RGB color = ui_wizard_share::inst()->getPalette()->getColor("WHITE");
    
    Vec2 position = gui::inst()->getPointVec2(2, 5, ALIGNMENT_CENTER, mLayer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    auto ball = guiExt::drawCircleForPhysics(mLayer, position, mObstacleSize.height / 2.f, color);
    this->setPhysicsBodyCircle(ball, PHYSICSMATERIAL, true
                               , _PHYSICS_ID_MY_BALL
                               , -1
                               , -1 // _PHYSICS_ID_MY_BOARD | _PHYSICS_ID_MY_OBSTACLE_1
                               ,  _PHYSICS_ID_MY_OBSTACLE_1
                               );
    ball->getPhysicsBody()->setVelocityLimit(mLayer->getContentSize().width * 1.f);
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
    
    mLatestCollisionWithBoard = 0;
    
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
// setVibrate ===========================================================================
void ScenePlay::setVibrate(Node * layer) {
    CallFunc * cf;
    if(layer == mLayer) {
        if(mLockShakeMy)
            return;
        
        mLockShakeMy = true;
        cf = CallFunc::create([=]() { mLockShakeMy = false; });
    }
    if(layer == mLayerOther) {
        if(mLockShakeOther)
            return;
        
        mLockShakeOther = true;
        cf = CallFunc::create([=]() { mLockShakeOther = false; });
    }
    
    Vec2 pos = layer->getPosition();
    float duration = 0.1f;
    float width = 5.f;
    layer->runAction(Sequence::create( MoveTo::create(duration, Vec2(pos.x - width, pos.y - width))
                                      , MoveTo::create(duration, Vec2(pos.x + width, pos.y + width))
                                      , MoveTo::create(duration, Vec2(pos.x - width, pos.y))
                                      , MoveTo::create(duration, Vec2(pos.x + width, pos.y))
                                      , MoveTo::create(duration, pos)
                                      , cf
                                      , NULL));
    
    
}

// touch ===========================================================================
bool ScenePlay::onTouchBegan(Touch* touch, Event* event) {
    return true;
}
bool ScenePlay::onTouchEnded(Touch* touch, Event* event) {
    return true;
}
void ScenePlay::onTouchMoved(Touch *touch, Event *event) {
    Vec2 pos = Vec2(touch->getLocation().x - mLayer->getPosition().x, mBoardMy->getPosition().y);
    pos.x -= mBoardMy->getContentSize().width / 2.f;
    
    if(pos.x <= 0)
        pos.x = 0;
    if(pos.x >= mLayer->getContentSize().width - mBoardMy->getContentSize().width)
        pos.x = mLayer->getContentSize().width - mBoardMy->getContentSize().width;
    
    mBoardMy->setPosition(pos);
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
        if(other == _ID_BOTTOM) {
            mProgressbarMyHP->setValueDecrese(0.1);
            mProgressbarMyHP->blink();
            setVibrate(mLayer);
            return true;
        }
        else if(other >= _BOARD_ID_L && other <= _BOARD_ID_R) {
            //동시 충돌 방지
            clock_t now = clock();
            if(now - mLatestCollisionWithBoard < 10) {
                CCLOG("%lf concurrent collision %d", mLatestCollisionWithBoard, other);
                return true;
            }
            else
                mLatestCollisionWithBoard = now;
            
            if(mBall->getPosition().y > mBoardMy->getPosition().y + mBoardMy->getContentSize().height * 2.f)
                return false;
            
            CCLOG("Other = %d, (%f)", other, mBall->getPosition().y);
            
            
            switch((_BOARD_ID)other) {
                case _BOARD_ID_L:
                    mBall->getPhysicsBody()->setVelocity(Vec2(mLayer->getContentSize().width * -1.f, mLayer->getContentSize().height));
                    break;
                case _BOARD_ID_LM:
                    mBall->getPhysicsBody()->setVelocity(Vec2(mLayer->getContentSize().width * -0.5f, mLayer->getContentSize().height));
                    break;
                case _BOARD_ID_RM:
                    mBall->getPhysicsBody()->setVelocity(Vec2(mLayer->getContentSize().width * 0.5f, mLayer->getContentSize().height));
                    break;
                case _BOARD_ID_R:
                    mBall->getPhysicsBody()->setVelocity(Vec2(mLayer->getContentSize()));
                    break;
                default:
                    break;
            }
            return true;
        }
        const Vec2 pos = mBall->getPosition();
        
//        CCLOG("Contact %d, with %d, Category %d, %d, Contact %d, %d, (%f, %f)"
//              , _PHYSICS_ID_MY_BALL, other
//              , st.categoryA, st.categoryB
//              , st.contactA, st.contactB
//              , pos.x, pos.y
//              );
        
        auto label = gui::inst()->addLabelAutoDimension(0, 0, "COMBO", mLayer, mFontSizeCombo, ALIGNMENT_CENTER, ui_wizard_share::inst()->getPalette()->getColor3B("ORANGE"));
        label->enableGlow(ui_wizard_share::inst()->getPalette()->getColor4B("BLACK"));
        label->setPosition(pos);
        label->runAction( Sequence::create(ScaleBy::create(0.3, 1.5), RemoveSelf::create(), NULL) );
        
        if(mProgressbarMyMP->setValueIncrese(0.05f) >= 1.f) {
            mProgressbarMyMP->setValue(0.f);
            
            mProgressbarMyMP->blink();
            
            guiExt::addMovingEffect(getNodeById(0)
                                    , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_DEEP")
                                    , "icons8-action-96.png"
                                    , "ATTACK"
                                    , ui_wizard_share::inst()->getPalette()->getColor("GRAY")
                                    , false
                                    );
            
            mProgressbarOtherHP->setValueDecrese(0.1);
            mProgressbarOtherHP->blink();
            setVibrate(mLayerOther);
        }
        
    }
    return true;
}
