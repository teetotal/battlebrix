//
//  SceneArcade.h
//
//  Created by Jung, DaeCheon on 19/07/2019.
//

#ifndef SceneArcade_h
#define SceneArcade_h

#include "ui/ui_wizard.h"
#include "battleBrix.h"
USING_NS_CC;

class SceneArcade : public ui_wizard
{
public:
    static Scene* createScene() {
        return SceneArcade::create();
    };
    virtual bool init();
    CREATE_FUNC(SceneArcade);
    
protected:
    virtual void callback(Ref* pSender, int from, int link);
    virtual const string getText(const string& defaultString, int id);
    virtual const float getProgressValue(int id) { return battleBrix::inst()->getProgressValue(id); };
};

#endif /* SceneArcade_h */
