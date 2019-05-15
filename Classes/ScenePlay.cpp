
#include "ScenePlay.h"
#include "Scenes.h"

#define SEPPED 1
#define PHYSICSMATERIAL PhysicsMaterial(0.5, 1, 0.5)

bool ScenePlay::init()
{
    this->loadFromJson("play", "play.json");
	((LoadingBar *)this->getNodeById(1))->setDirection(LoadingBar::Direction::RIGHT);
	((LoadingBar *)this->getNodeById(2))->setDirection(LoadingBar::Direction::RIGHT);
    
    TOUCH_INIT(ScenePlay);
    PHYSICS_CONTACT(ScenePlay);
    mLayer = getNodeById(ID_NODE_MY_AREA);
    initPhysicsBody(mLayer, PHYSICSMATERIAL, false, SEPPED);
    
//    float radius = 10.f;
    mBall = createBall();
//    auto ball = gui::inst()->drawCircle(mLayer, Vec2(100, 100), radius, Color4F::BLUE);
    
//    auto ball = DrawNode::create();
//    ball->drawDot(Vec2(radius, radius), radius, Color4F::BLUE);
//    ball->setContentSize(Size(radius * 2, radius * 2));
//    auto body = PhysicsBody::createCircle(radius, PHYSICSMATERIAL);
//    ball->setPosition(Vec2(100, 100));
//    Vec2 pos = ball->getPosition();
//    Size size = ball->getContentSize();
//    ball->setPhysicsBody(body);
//    mLayer->addChild(ball);
//
//    gui::inst()->drawCircle(mLayer, Vec2(110, 110), 10.f, Color4F::GREEN);
    
    
    return true;
}

void ScenePlay::callback(Ref* pSender, int from, int link) {
	this->replaceScene(SceneMain::create());
}

const string ScenePlay::getText(const string& defaultString, int id) {
    return defaultString;
}

//createBall
Node * ScenePlay::createBall() {
    this->getPhysicsWorld()->setAutoStep(false);
    this->getPhysicsWorld()->step(0.0f);
    if (mBall != NULL) {
        mLayer->removeChild(mBall);
    }
    Color4F color = Color4F( 240 / 255.f, 208 / 255.f, 75 / 255.f, 1.f);
    auto ball = gui::inst()->drawCircleForPhysics(mLayer, Vec2(100, 100), 5.f, color);
    /*
    auto ball = gui::inst()->addSpriteAutoDimension(2, 5, "circle.png", mLayer, ALIGNMENT_CENTER, mGridSize, Size::ZERO, Size::ZERO);
    Vec2 pos = gui::inst()->getPointVec2(getRandValue(mGridSize.width), 5, ALIGNMENT_CENTER, mLayer->getContentSize(), mGridSize, Size::ZERO, Size::ZERO);
    ball->setPosition(pos);
    gui::inst()->setScale(ball, mRadius);
     */
    this->setPhysicsBodyCircle(ball, PHYSICSMATERIAL, true, 1, 0x1 << 0, 0x1 << 1 | 0x1 << 2, 0x1 << 1 | 0x1 << 2);
//    ball->getPhysicsBody()->setVelocityLimit(mDefaultVelocity * 1.f);
    
    this->getPhysicsWorld()->setAutoStep(true);
   
    return ball;
}

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
