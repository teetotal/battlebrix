
#include "ScenePlay.h"
#include "Scenes.h"
#include "ui_ext.h"
#include "library/util.h"

#define SEPPED 1
#define PHYSICSMATERIAL             PhysicsMaterial(.1f, 1.f, 0.f)
#define PHYSICSMATERIAL_OBSTACLE    PhysicsMaterial(.1f, 1.f, 0.f)
#define GRID_AREA Vec2(8.f, 13.f)
//#define GRID_AREA Vec2(2.f, 3.f)
#define RATIO_OBSTACLE_PER_GRID 0.6f

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
    this->loadFromJson("play", "play.json");
//    ((LoadingBar *)this->getNodeById(1))->setDirection(LoadingBar::Direction::RIGHT);
//    ((LoadingBar *)this->getNodeById(2))->setDirection(LoadingBar::Direction::RIGHT);
    
    TOUCH_INIT(ScenePlay);
    PHYSICS_CONTACT(ScenePlay);
    
    mLayer      = getNodeById(ID_NODE_MY_AREA);
    mLayerOther = getNodeById(ID_NODE_OTHER_AREA);
    mGridSize   = gui::inst()->getGridSize(mLayer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO);
    mGridSize.width *= RATIO_OBSTACLE_PER_GRID;
    mGridSize.height *= RATIO_OBSTACLE_PER_GRID;
    mFontSizeCombo = gui::inst()->getFontSize(mGridSize) * 1.5f;
    
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
//    auto rect1 = getNodeById(500);
//    rect1->setVisible(false);
//    auto rect = getNodeById(501);
//    rect->setVisible(true);
//    rect->runAction(Blink::create(10, 30));
//    this->schedule(schedule_selector(ScenePlay::timer), 1.0f);
    return true;
}

void ScenePlay::callback(Ref* pSender, int from, int link) {
    switch(link) {
        case 1:
            setVibrate(mLayer);
            break;
        case 2: {
            
            break;
        }
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

void ScenePlay::timer(float f) {
    createBall();
}

//createBall ===========================================================================
Node * ScenePlay::createBall() {
    this->getPhysicsWorld()->setAutoStep(false);
    this->getPhysicsWorld()->step(0.0f);
    COLOR_RGB color = ui_wizard_share::inst()->getPalette()->getColor("WHITE");
    
    Vec2 position = gui::inst()->getPointVec2(2, 2, ALIGNMENT_CENTER, mLayer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
    auto ball = guiExt::drawCircleForPhysics(mLayer, position, mGridSize.height / 2.f, color);
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

// setVibrate ===========================================================================
void ScenePlay::setVibrate(Node * layer) {
    
    Vec2 pos = layer->getPosition();
    float duration = 0.1f;
    float width = 5.f;
    layer->runAction(Sequence::create( MoveTo::create(duration, Vec2(pos.x - width, pos.y - width))
                                       , MoveTo::create(duration, Vec2(pos.x + width, pos.y + width))
                                       , MoveTo::create(duration, Vec2(pos.x - width, pos.y))
                                       , MoveTo::create(duration, Vec2(pos.x + width, pos.y))
                                       , MoveTo::create(duration, pos)
                                       , NULL));
}

// add Obstacle ===========================================================================
void ScenePlay::addObstacle(Node * layer, Vec2 pos) {
    COLOR_RGB colors[] = {
        ui_wizard_share::inst()->getPalette()->getColor("RED_LIGHT"),
        ui_wizard_share::inst()->getPalette()->getColor("YELLOW_LIGHT"),
        ui_wizard_share::inst()->getPalette()->getColor("GREEN_LIGHT"),
        ui_wizard_share::inst()->getPalette()->getColor("BLUE_LIGHT"),
        ui_wizard_share::inst()->getPalette()->getColor("PURPLE_LIGHT")
    };
//    for(int n=1; n <= 4; n++){
//        string sz = "O" + to_string(n);
//        colors[n-1] = ui_wizard_share::inst()->getPalette()->getColor4F(sz);
//    }
    Vec2 position = gui::inst()->getPointVec2(pos.x, pos.y, ALIGNMENT_CENTER, layer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
//    auto rect = guiExt::drawRectForPhysics(layer, position, mGridSize, colors[getRandValue(sizeof(colors) / sizeof(colors[0]))], true, .1f);
    auto rect = guiExt::drawRectForPhysics(layer, position, mGridSize, colors[(int)pos.y], true, .1f);
//    auto rect = guiExt::drawRectForPhysics(layer, position, mGridSize, ui_wizard_share::inst()->getPalette()->getColor4F("PINK"), true, .1);
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
    
}
bool ScenePlay::onContactBegin(PhysicsContact &contact) {
    int other;
    bitmask st = getBitmask(contact);
    if(isCollosion(contact, _PHYSICS_ID_MY_BALL, other)) {
        CCLOG("Collision %d with %d, Category %d, %d, Collision %d, %d"
              , _PHYSICS_ID_MY_BALL, other
              , st.categoryA, st.categoryB
              , st.collisionA, st.collisionB
              );
//
//                CCLOG("Category %d, %d", st.categoryA, st.categoryB);
//                CCLOG("Collision %d, %d", st.collisionA, st.collisionB);
//                CCLOG("Contact %d, %d", st.contactA, st.contactB);
    }
    
    if(isContact(contact, _PHYSICS_ID_MY_BALL, other)) {
        const Vec2 pos = mBall->getPosition();
        
        CCLOG("Contact %d, with %d, Category %d, %d, Contact %d, %d, (%f, %f)"
              , _PHYSICS_ID_MY_BALL, other
              , st.categoryA, st.categoryB
              , st.contactA, st.contactB
              , pos.x, pos.y
              );
        
        auto label = gui::inst()->addLabelAutoDimension(0, 0, "COMBO", mLayer, mFontSizeCombo, ALIGNMENT_CENTER, ui_wizard_share::inst()->getPalette()->getColor3B("ORANGE"));
        label->enableGlow(ui_wizard_share::inst()->getPalette()->getColor4B("BLACK"));
        label->setPosition(pos);
        label->runAction( Sequence::create(ScaleBy::create(0.3, 1.5), RemoveSelf::create(), NULL) );
        
        if(mProgressbarMyMP->setValueIncrese(0.05f) >= 1.f) {
            mProgressbarMyMP->setValue(0.f);
            
            mProgressbarMyMP->runAction(Blink::create(0.5, 2));
            
            guiExt::addMovingEffect(getNodeById(0)
                                    , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_DEEP")
                                    , "icons8-action-96.png"
                                    , "ATTACK"
                                    , ui_wizard_share::inst()->getPalette()->getColor("GRAY")
                                    , false
                                    );
            
            mProgressbarOtherHP->setValueDecrese(0.1);
            setVibrate(mLayerOther);
        }
        
    }
    return true;
}
