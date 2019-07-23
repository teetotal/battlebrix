//
//  battleBrix.cpp
//  battlebrix-mobile
//
//  Created by Jung, DaeCheon on 13/06/2019.
//

#include "battleBrix.h"
#define SQL_FILE_NAME "battleBrix.s3db"

battleBrix * battleBrix::hInstance = NULL;
brixMap * brixMap::hInstance = NULL;
//===============================================================================
void brixMap::init() {
    rapidjson::Document d = getJsonValue("brix.json");
    const rapidjson::Value& brix = d["brix"];
    for (rapidjson::SizeType i = 0; i < brix.Size(); i++)
    {
        brixStage b;
        b.load(d["brix"][rapidjson::SizeType(i)]);
        mBrixMap.push_back(b);
    }
}
//-------------------------------------------------------------------------------
void brixMap::brixStage::load(rapidjson::Value &p) {
    this->title = p["title"].GetString();
    this->minGrade = p["minGrade"].GetInt();
    this->maxGrade = p["maxGrade"].GetInt();
    //mission
    this->mission.message = p["mission"]["message"].GetString();
    this->mission.ranking = p["mission"]["ranking"].GetInt();
    
    //prize
    this->prize.point = p["prize"]["point"].GetInt();
    this->prize.heart = p["prize"]["heart"].GetInt();
    this->prize.item = p["prize"]["item"].GetInt();
    //enableItem
    const rapidjson::Value& enableItemIdx = p["enableItemIdx"];
    for (rapidjson::SizeType i = 0; i < enableItemIdx.Size(); i++)
    {
        this->enableItemIdx.push_back(enableItemIdx[rapidjson::SizeType(i)].GetInt());
    }
    //statics
    const rapidjson::Value& statics = p["static"];
    for (rapidjson::SizeType i = 0; i < statics.Size(); i++)
    {
        position pos;
        pos.x = statics[rapidjson::SizeType(i)][rapidjson::SizeType(0)].GetInt();
        pos.y = statics[rapidjson::SizeType(i)][rapidjson::SizeType(1)].GetInt();
        
        this->statics.push_back(pos);
    }
    //movement
    const rapidjson::Value& movements = p["movement"];
    for (rapidjson::SizeType i = 0; i < movements.Size(); i++)
    {
        brixMovement movement;
        movement.load(p["movement"][rapidjson::SizeType(i)]);
        this->movements.push_back(movement);
    }
    
}
//-------------------------------------------------------------------------------
void brixMap::brixMovement::load(rapidjson::Value &p) {
    
    this->type = (TYPE)p["type"].GetInt();
    
    const rapidjson::Value& path = p["path"];
    for (rapidjson::SizeType i = 0; i < path.Size(); i++)
    {
        position pos;
        pos.x = path[rapidjson::SizeType(i)][rapidjson::SizeType(0)].GetInt();
        pos.y = path[rapidjson::SizeType(i)][rapidjson::SizeType(1)].GetInt();
        
        this->path.push_back(pos);
    }
    
}
int brixMap::getMapRandom() {
    vector<int> v;
    int grade = battleBrix::inst()->mUserData.grade;
    for(int n=0; n<mBrixMap.size(); n++) {
        if(mBrixMap[n].minGrade <= grade) {
            switch(mBrixMap[n].maxGrade)
            {
                case -1:
                    v.push_back(n);
                    break;
                default:
                    if(mBrixMap[n].maxGrade >= grade)
                        v.push_back(n);
                    break;
            }
        }
    }
    CC_ASSERT(v.size() > 0);
    return v[getRandValue((int)v.size())];
};
//======================================================================
// UserData
//======================================================================
void battleBrix::userData::increaseHeart(int n) {
    lock.lock();
    heart += n;
    int r = Sql::inst()->exec("UPDATE userData SET heart = " + to_string(heart));
    CCASSERT((r == 0), "sql failure");
    setCRC();
    lock.unlock();
};

void battleBrix::userData::increasePoint(int n) {
    lock.lock();
    point += n;
    int r = Sql::inst()->exec("UPDATE userData SET point = " + to_string(point));
    CCASSERT((r == 0), "sql failure");
    setCRC();
    lock.unlock();
};

void battleBrix::userData::setHeartTimerStart(time_t t) {
    lock.lock();
    heartTimerStart = t;
    int r = Sql::inst()->exec("UPDATE userData SET heartTimerStart = " + to_string(heartTimerStart));
    CCASSERT((r == 0), "sql failure");
    setCRC();
    lock.unlock();
};

bool battleBrix::userData::increseGrowth(int val) {
    bool ret = false;
    growth += val;
    if(growth >= maxGrowth) {
        growth = growth - maxGrowth;
        grade++;
        maxGrowth = grade * growthPerGrade;
        
        ret = true;
        
    } else if(growth < 0) {
        bool isReset = false;
        if(grade - 1 < 1)
            isReset = true;
        
        grade = max(1,  grade - 1);
        maxGrowth = grade * growthPerGrade;
        growth = isReset ? 0 : maxGrowth + growth;
    }
    int r = Sql::inst()->exec("UPDATE userData SET growth = " + to_string(growth) + " , maxGrowth = " + to_string(maxGrowth) + " ,grade = " + to_string(grade));
    CCASSERT((r == 0), "sql failure");
    setCRC();
    return ret;
}
void battleBrix::userData::setCRC() {
    int crc = 128;
    int r = Sql::inst()->exec("UPDATE userData SET crc = (heartTimerStart - (point * heartMax) + (heart * heartMax) - growth - (maxGrowth / grade) - ranking - levelGrowth - (levelMaxGrowth / level)) % " + to_string(crc));
    CCASSERT((r == 0), "sql failure");
}

bool battleBrix::userData::increseExp(int val) {
    bool ret = false;
    if(levelMaxGrowth <= levelGrowth + val) { //level up
        level++;
        levelGrowth = (levelGrowth + val) - levelMaxGrowth;
        levelMaxGrowth = level * growthPerLevel;
        
        //heart
        if(heartMax > heart) {
            increaseHeart(heartMax - heart);
        }
        
        ret = true;
    } else {
        levelGrowth += val;
    }
    
    int r = Sql::inst()->exec("UPDATE userData SET level = " + to_string(level) + " , levelGrowth = " + to_string(levelGrowth) + " ,levelMaxGrowth = " + to_string(levelMaxGrowth));
    CCASSERT((r == 0), "sql failure");
    setCRC();
    return ret;
}
//===============================================================================
bool battleBrix::init() {
    
    //sql
    string sqliteFullPath = cocos2d::FileUtils::getInstance()->getWritablePath() + SQL_FILE_NAME;
    if (!cocos2d::FileUtils::getInstance()->isFileExist(sqliteFullPath)) {
        //writable 경로에 파일 복사
        cocos2d::FileUtils::getInstance()->writeDataToFile(cocos2d::FileUtils::getInstance()->getDataFromFile(SQL_FILE_NAME), sqliteFullPath);
    }
    if(!Sql::inst()->init(sqliteFullPath)) {
        CCLOG("SQL FILE INIT FAILURE");
        return false;
    }
    Sql::inst()->select("select (heartTimerStart - (point * heartMax) + (heart * heartMax) - growth - (maxGrowth / grade) - ranking - levelGrowth - (levelMaxGrowth / level))  % 128 from userData");
    
    sqlite3_stmt * stmt = Sql::inst()->select("SELECT id, grade, heart, heartMax, heartTimerStart, point, growth, maxGrowth, ranking, level, levelGrowth, levelMaxGrowth, crc FROM userData");
    if (stmt == NULL)
        return false;
    
    int result = sqlite3_step(stmt);
    
    if (result == SQLITE_ROW) {
        int idx = 0;
        mUserData.id = (const char*)sqlite3_column_text(stmt, idx++);
        mUserData.grade = sqlite3_column_int(stmt, idx++);
        mUserData.heart = sqlite3_column_int(stmt, idx++);
        mUserData.heartMax = sqlite3_column_int(stmt, idx++);
        mUserData.heartTimerStart = sqlite3_column_int64(stmt, idx++);
        mUserData.point = sqlite3_column_int(stmt, idx++);
        mUserData.growth = sqlite3_column_int(stmt, idx++);
        mUserData.maxGrowth = sqlite3_column_int(stmt, idx++);
        mUserData.ranking = sqlite3_column_int(stmt, idx++);
        mUserData.level = sqlite3_column_int(stmt, idx++);
        mUserData.levelGrowth = sqlite3_column_int(stmt, idx++);
        mUserData.levelMaxGrowth = sqlite3_column_int(stmt, idx++);
        
        int crc = sqlite3_column_int(stmt, idx++);
        long crcCheck = (mUserData.heartTimerStart - (mUserData.point * mUserData.heartMax) + (mUserData.heart * mUserData.heartMax) - mUserData.growth - (mUserData.maxGrowth / mUserData.grade) - mUserData.ranking - mUserData.levelGrowth - (mUserData.levelMaxGrowth / mUserData.level));
        crcCheck = (int)(crcCheck % 128);
        CCLOG("CRC %d - %d", crc, (int)crcCheck);
        if(crc != crcCheck) {
            CCLOG("CRC ERROR");
            return false;
        }
    }
    else
        return false;
    
    //config
    rapidjson::Document d = getJsonValue("config.json");
    
    mUserData.rechargeTime = d["rechargeInterval"].GetInt();
    mUserData.growthPerGrade = d["growthPerGrade"].GetInt();
    mUserData.growthPerLevel = d["growthPerLevel"].GetInt();
    
    //reward
    const rapidjson::Value& rewards = d["play"]["rewards"];
    for (rapidjson::SizeType i = 0; i < rewards.Size(); i++)
    {
        int growth = rewards[rapidjson::SizeType(i)]["growth"].GetInt();
        int point = rewards[rapidjson::SizeType(i)]["point"].GetInt();
        int heart = rewards[rapidjson::SizeType(i)]["heart"].GetInt();
        mRewards[i+1].init(growth, point, heart);
    }
    
    //play item
    const rapidjson::Value& playItems = d["play"]["items"];
    for (rapidjson::SizeType i = 0; i < playItems.Size(); i++) {
        const string name = playItems[rapidjson::SizeType(i)]["name"].GetString();
        const string img = playItems[rapidjson::SizeType(i)]["img"].GetString();
        int price = playItems[rapidjson::SizeType(i)]["price"].GetInt();
        itemData item;
        item.set(name, price, img);
        //property
        float attack = playItems[rapidjson::SizeType(i)]["property"]["hpAttack"].GetFloat();
        float recharge = playItems[rapidjson::SizeType(i)]["property"]["hpRecharge"].GetFloat();
        float shieldTime = playItems[rapidjson::SizeType(i)]["property"]["shieldTime"].GetFloat();
        bool isRevenge = playItems[rapidjson::SizeType(i)]["property"]["revenge"].GetBool();
        item.property.set(attack, recharge, shieldTime, isRevenge);
        
        const rapidjson::Value& targets = playItems[rapidjson::SizeType(i)]["property"]["attackTarget"];
         for (rapidjson::SizeType j = 0; j < targets.Size(); j++) {
             item.property.attackTarget.push_back(targets[rapidjson::SizeType(j)].GetInt());
         }
        
        mItems.push_back(item);
    }
    
    mItemSelected.set((int)mItems.size());
    
    //Grade titles
    const rapidjson::Value& grades = d["grades"];
    for (rapidjson::SizeType i = 0; i < grades.Size(); i++) {
        grade g;
        g.grade = grades[rapidjson::SizeType(i)]["grade"].GetInt();
        g.speed = grades[rapidjson::SizeType(i)]["speed"].GetFloat();
        g.title = grades[rapidjson::SizeType(i)]["title"].GetString();
        
        g.delay = grades[rapidjson::SizeType(i)]["AI"]["delay"].GetFloat();
        g.IQ = grades[rapidjson::SizeType(i)]["AI"]["IQ"].GetFloat();
        g.skillQuantity = grades[rapidjson::SizeType(i)]["AI"]["skills"].GetFloat();
        mGrades.push_back(g);
    }
    
    return true;
}

const string battleBrix::getText(const string& defaultString, int id) {
    switch(id) {
        case _ID_NODE_LABEL_ID:
            return mUserData.id;
            
        case _ID_NODE_LABEL_POINT:
            return numberFormat(mUserData.point);
            
        case _ID_NODE_LABEL_HEART:
            mUserData.recharge();
            return to_string(mUserData.heart) + " / " + to_string(mUserData.heartMax);
            
        case _ID_NODE_TIMER_HEART:
            return mUserData.getRechargeRemainTimeString();
            
        case _ID_NODE_LABEL_LEVEL:
            return "LV." + to_string(mUserData.level);
            
        case _ID_NODE_PROGRESSBAR:
        case _ID_NODE_LABEL_GRADE:
            return getGradeString();
            
        case _ID_NODE_LABEL_RANKING:
            return numberFormat(mUserData.ranking) + "th";
            
        default:
            return defaultString;
    }
}
const float battleBrix::getProgressValue(int id) {
    switch(id) {
        case _ID_NODE_PROGRESSBAR:
            return getGrowthPercentage();
        default:
            return -1.f;
    }
}

battleBrix::rewardData battleBrix::getReward(int ranking) {
    return mRewards[ranking];
}
bool battleBrix::applyReward(int ranking) {
    rewardData reward = getReward(ranking);
    
    mUserData.increaseHeart(reward.heart);
    mUserData.increasePoint(reward.point);
    
    return mUserData.increseGrowth(reward.growth);
}

const string battleBrix::getGradeString() {
    return mGrades[mUserData.grade].title;
}

bool battleBrix::payForPlay(int point, int heart) {
    if(!checkPayForPlay(point, heart))
        return false;
    
    if(mUserData.heart >= mUserData.heartMax)
        mUserData.setHeartTimerStart(getNow());
    
    mUserData.increasePoint(point * -1);
    mUserData.increaseHeart(heart * -1);
    
    return true;
}

bool battleBrix::checkPayForPlay(int point, int heart) {
    if(mUserData.point < point || mUserData.heart < heart) {
        return false;
    }
    return true;
}
