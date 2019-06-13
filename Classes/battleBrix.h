//
//  battleBrix.hpp
//  battlebrix-mobile
//
//  Created by Jung, DaeCheon on 13/06/2019.
//

#ifndef battleBrix_h
#define battleBrix_h

#include <string>
using namespace std;

class battleBrix {
public:
    battleBrix() {};
    virtual ~battleBrix() {};
    static battleBrix * inst() {
      if(hInstance == NULL)
          hInstance = new battleBrix;
        return hInstance;
    };
    struct userData {
        int win;
        int lose;
        int ranking;
        int level;
        int grade;
        int heart;
        int heartMax;
        int point;
        string id;
        
        userData() : win(0), lose(0), ranking(5000000), level(1), grade(0), heart(0), heartMax(5), point(1000) {
            id = "teetotal";
        };
        
    } mUserData;
private:
    static battleBrix * hInstance;
};
#endif /* battleBrix_h */
