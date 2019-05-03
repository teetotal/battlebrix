/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __SCENES_H__
#define __SCENES_H__

#include "ui/ui_wizard.h"
USING_NS_CC;

class SceneMain : public ui_wizard
{
public:
    static Scene* createScene() {
        return SceneMain::create();
    };
    virtual bool init();
    CREATE_FUNC(SceneMain);
    
protected:
    virtual void callback(Ref* pSender, int from, int link);
    virtual const string getText(const string& defaultString, int id);
    
private:
    enum eLINK {
        eLINK_PLAY = 0
    };
};

class ScenePlay : public ui_wizard
{
public:
    static Scene* createScene() {
        return ScenePlay::create();
    };
    virtual bool init();
    CREATE_FUNC(ScenePlay);
    
protected:
    virtual void callback(Ref* pSender, int from, int link);
    virtual const string getText(const string& defaultString, int id);
};

#endif // __SCENES_H__
