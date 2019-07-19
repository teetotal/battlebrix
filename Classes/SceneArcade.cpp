//
//  SceneArcade.cpp
//
//  Created by Jung, DaeCheon on 19/07/2019.
//

#include "SceneArcade.h"
#include "Scenes.h"
#include "ui/ui_ext.h"
#include "ui/ui_icon.h"

bool SceneArcade::init()
{
    this->loadFromJson("arcade", "arcade.json");
    //bottom disable
    getNodeById(_ID_NODE_BOTTOM_ARCADE)->setVisible(false);
    getNodeById(_ID_NODE_BOTTOM_SELECTED_ARCADE)->setVisible(true);
    //timer
    HEART_TIMER
    return true;
}

void SceneArcade::callback(Ref* pSender, int from, int link) {
    LINK
}

const string SceneArcade::getText(const string& defaultString, int id) {
    switch(id) {
        default:
            return battleBrix::inst()->getText(defaultString, id);
    }
}
