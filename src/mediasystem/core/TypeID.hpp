//
//  TypeID.hpp
//  WallTest
//
//  Created by Michael Allison on 6/13/18.
//

#pragma once
#include <memory>

namespace mediasystem {
    
    template<typename>
    void type_id(){}
    using type_id_t = void(*)();
    
}//end namespace media system
