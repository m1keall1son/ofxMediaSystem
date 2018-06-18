//
//  DefaultPresenter.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#include "DefaultPresenter.h"
#include "ofMain.h"

namespace mediasystem {
    
    void DefaultPresenter::begin()
    {
        ofPushStyle();
        ofEnableAlphaBlending();
    }
    
    void DefaultPresenter::end()
    {
        ofPopStyle();
    }
    
}//end namespace mediasystem
