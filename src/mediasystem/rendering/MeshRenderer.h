//
//  MeshRenderer.hpp
//  WallTest
//
//  Created by Michael Allison on 6/22/18.
//

#pragma once

#include "ofMain.h"

namespace mediasystem {
    
    class IMaterial {
    public:
        ~IMaterial() = default;
        virtual void bind() = 0;
        virtual void unbind() = 0;
    };
    
    class MeshRenderer {
    public:
        MeshRenderer(ofMesh mesh, std::shared_ptr<IMaterial> material);
        void draw();
        template<typename MaterialType>
        std::shared_ptr<MaterialType> getMaterial(){
            static_assert(std::is_base_of<IMaterial,MaterialType>::value, "MaterialType must inherit IMaterial");
            return std::static_pointer_cast<MaterialType>(mMaterial);
        }
    private:
        ofMesh mMesh;
        std::shared_ptr<IMaterial> mMaterial;
    };
    
}//end namespace mediasystem
