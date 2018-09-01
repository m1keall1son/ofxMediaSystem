//
//  AssetManager.hpp
//  ofxMediaSystem
//
//  Created by Michael Allison on 8/17/18.
//

#pragma once

#include "Manager.hpp"
#include "TupleHelpers.hpp"

namespace mediasystem {

    template<typename...AssetTypes>
    class AssetManager {
    public:
        
        template<typename T, typename...Args>
        std::weak_ptr<T> createAsset(ManagedResourceKey key, Args&&...args){
            auto& manager = get_element_by_type<Manager<T>>(mManagers);
            return manager.template create<T>(std::move(key), std::forward<Args>(args)...);
        }
        
        template<typename T>
        bool removeAsset(ManagedResourceKey resource){
            auto& manager = get_element_by_type<Manager<T>>(mManagers);
            return manager.remove(resource);
        }
        
        template<typename T>
        std::weak_ptr<T> retrieveAsset(ManagedResourceKey resource){
            auto& manager = get_element_by_type<Manager<T>>(mManagers);
            return manager.retrieve(resource);
        }
        
    private:
        std::tuple<Manager<AssetTypes>...> mManagers;
    };
    
}//end namespace mediasystem
