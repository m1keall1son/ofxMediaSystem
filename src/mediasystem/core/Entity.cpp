//
//  Entity.cpp
//  WallTest
//
//  Created by Michael Allison on 6/14/18.
//

#include "Entity.h"

namespace mediasystem {
    
    std::map<mediasystem::type_id_t, size_t> mediasystem::Entity::sComponentIds = {};
    size_t mediasystem::Entity::sNextComponentId = 0;
    
    Entity::Entity(Scene& scene, uint64_t id):
        mScene(scene),
        mId(id)
    {
        mComponents.reset();
    }
    
    bool Entity::destroy(){
        if(isValid()){
            for(size_t i = 0; i < mComponents.size(); i++){
                if(mComponents[i]){
                    if(auto compType = getType(i)){
                        mScene.destroyComponent(compType, mId);
                    }
                }
            }
            mComponents.reset();
            mScene.destroyEntity(mId);
            mId = std::numeric_limits<size_t>::max();
            return true;
        }
        return false;
    }
}//end namespace mediasystem
