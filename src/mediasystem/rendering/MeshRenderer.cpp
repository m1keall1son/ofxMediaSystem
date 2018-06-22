//
//  MeshRenderer.cpp
//  WallTest
//
//  Created by Michael Allison on 6/22/18.
//

#include "MeshRenderer.h"

namespace mediasystem {
    
    MeshRenderer::MeshRenderer( ofMesh mesh, std::shared_ptr<IMaterial> material ):
        mMesh(std::move(mesh)),
        mMaterial(std::move(material))
    {}
    
    void MeshRenderer::draw()
    {
        if(mMaterial)
            mMaterial->bind();
        mMesh.draw();
        if(mMaterial)
            mMaterial->unbind();
    }
    
}//end namespace media system
