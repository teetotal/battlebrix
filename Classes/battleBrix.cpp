//
//  battleBrix.cpp
//  battlebrix-mobile
//
//  Created by Jung, DaeCheon on 13/06/2019.
//

#include "battleBrix.h"
#include "library/pch.h"

battleBrix * battleBrix::hInstance = NULL;

const string battleBrix::getText(const string& defaultString, int id) {
    switch(id) {
        case _ID_NODE_LABEL_ID:
            return battleBrix::inst()->mUserData.id;
        case _ID_NODE_LABEL_POINT:
            return numberFormat(battleBrix::inst()->mUserData.point);
        case _ID_NODE_LABEL_HEART:
            return to_string(battleBrix::inst()->mUserData.heart) + " / " + to_string(battleBrix::inst()->mUserData.heartMax);
        case _ID_NODE_LABEL_LEVEL:
            return "Lv." + to_string(battleBrix::inst()->mUserData.level);
        case _ID_NODE_PROGRESSBAR_LABEL_WINNING_RATE:
            return to_string(battleBrix::inst()->mUserData.win) + " / " + to_string(battleBrix::inst()->mUserData.win + battleBrix::inst()->mUserData.lose);
        case _ID_NODE_LABEL_WINNING_RATE:
            return to_string(battleBrix::inst()->mUserData.win) + " Win / " + to_string(battleBrix::inst()->mUserData.lose) + " Lose";
        case _ID_NODE_LABEL_RANKING:
            return numberFormat(battleBrix::inst()->mUserData.ranking) + "th";
        default:
            return defaultString;
    }
}
