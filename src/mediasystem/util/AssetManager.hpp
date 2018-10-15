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
        Handle<T> createAsset(std::string key, Args&&...args){
            auto& manager = get_element_by_type<Manager<std::string,T>>(mManagers);
            return manager.template create<T>(std::move(key), std::forward<Args>(args)...);
        }
        
        template<typename T>
        bool removeAsset(std::string key){
            auto& manager = get_element_by_type<Manager<std::string,T>>(mManagers);
            return manager.remove(key);
        }
        
        template<typename T>
        Handle<T> retrieveAsset(std::string key){
            auto& manager = get_element_by_type<Manager<std::string,T>>(mManagers);
            return manager.retrieve(key);
        }
        
    private:
        std::tuple<Manager<std::string,AssetTypes>...> mManagers;
    };
    
}//end namespace mediasystem
