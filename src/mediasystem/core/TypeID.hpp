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
    
    template<typename T, typename U>
    std::weak_ptr<T> convertHandle( const std::weak_ptr<U>& generic ){
        if(auto locked = generic.lock()){
            return std::static_pointer_cast<T>(locked);
        }
        return std::weak_ptr<T>();
    }
}//end namespace media system
