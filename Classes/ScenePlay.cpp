
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
    
    initPhysicsBody(mLayer, PHYSICSMATERIAL_OBSTACLE, false, SEPPED);
    initPhysicsBody(mLayerOther, PHYSICSMATERIAL_OBSTACLE, false, SEPPED);
    
    //------------------------------------------------//
//    gui::inst()->drawGrid(mLayer, mLayer->getContentSize(), GRID_AREA, Vec2::ZERO, Vec2::ZERO);
    mBall = createBall();
    
    for(int n=0; n < 10; n ++) {
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
            guiExt::addMovingEffect(getNodeById(0)
                                    , ui_wizard_share::inst()->getPalette()->getColor("WHITE_OPACITY_DEEP")
                                    , "icons8-synchronize-480.png"
                                    , false
                                    );
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
    this->setPhysicsBodyCircle(ball, PHYSICSMATERIAL, true);
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
    this->setPhysicsBodyRect(rect, PHYSICSMATERIAL_OBSTACLE, false);
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
    return true;
}
