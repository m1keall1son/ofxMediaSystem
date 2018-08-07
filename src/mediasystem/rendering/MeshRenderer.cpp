//
//  MeshRenderer.cpp
//  WallTest
//
//  Created by Michael Allison on 6/22/18.
//

#include "MeshRenderer.h"

namespace mediasystem {
    
    ofMesh meshFromRect(const ofRectangle& rect, bool normalized)
    {
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        
        mesh.addVertex(rect.getTopLeft());
        mesh.addVertex(rect.getBottomLeft());
        mesh.addVertex(rect.getTopRight());
        
        mesh.addVertex(rect.getTopRight());
        mesh.addVertex(rect.getBottomLeft());
        mesh.addVertex(rect.getBottomRight());

        if(normalized){
            mesh.addTexCoord(glm::vec2(0,0));
            mesh.addTexCoord(glm::vec2(0,1));
            mesh.addTexCoord(glm::vec2(1,0));
            
            mesh.addTexCoord(glm::vec2(1,0));
            mesh.addTexCoord(glm::vec2(0,1));
            mesh.addTexCoord(glm::vec2(1,1));
        
        }else{
            mesh.addTexCoord(glm::vec2(rect.getTopLeft()));
            mesh.addTexCoord(glm::vec2(rect.getBottomLeft()));
            mesh.addTexCoord(glm::vec2(rect.getTopRight()));
            
            mesh.addTexCoord(glm::vec2(rect.getTopRight()));
            mesh.addTexCoord(glm::vec2(rect.getBottomLeft()));
            mesh.addTexCoord(glm::vec2(rect.getBottomRight()));
        }
        
        return mesh;
    }
    
    void MeshRenderer::draw()
    {
        if(mMaterial)
            mMaterial->bind();
        mMesh.draw();
        if(mMaterial)
            mMaterial->unbind();
    }
    
}//end namespace media system
