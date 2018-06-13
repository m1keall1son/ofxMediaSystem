//
//  DefaultPresenter.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#pragma once

#include "mediasystem/rendering/IPresenter.h"

namespace mediasystem {
    
    class DefaultPresenter : public IPresenter {
    public:
        
        DefaultPresenter() = default;
        ~DefaultPresenter() = default;
        
        void begin()override;
        void end()override;
        void present()override{}
        
    };
    
}//end namespace mediasystem
