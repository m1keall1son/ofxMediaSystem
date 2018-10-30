//
//  MeshRenderer.hpp
//  WallTest
//
//  Created by Michael Allison on 6/22/18.
//

#pragma once

#include "ofMain.h"
#include "mediasystem/memory/Memory.h"
#include "mediasystem/core/Scene.h"

namespace mediasystem {
    
    namespace detail {
        
        class IMaterial {
        public:
            ~IMaterial() = default;
            virtual void bind() = 0;
            virtual void unbind() = 0;
            virtual bool isProxy(){ return false; }
        };
        
        template<typename BinableType>
        class Material : public BinableType, public IMaterial {
        public:
            
            template<typename...Args>
            Material(Args&&...args):BinableType(std::forward<Args>(args)...){}
            
            virtual void bind() override {
                BinableType::bind();
            }
            
            virtual void unbind() override {
                BinableType::unbind();
            }
        };
        
        template<typename BinableType>
        class MaterialProxyWeak : public IMaterial {
        public:

            MaterialProxyWeak(std::weak_ptr<BinableType> material):mMaterial(std::move(material)){}
            
            virtual void bind() override {
                if(auto mat = mMaterial.lock()){
                    mat->bind();
                }
            }
            
            virtual void unbind() override {
                if(auto mat = mMaterial.lock()){
                    mat->unbind();
                }
            }
            
            std::shared_ptr<BinableType> getBindable(){
                return mMaterial.lock();
            }
            
            bool isProxy() override { return true; }
            
        private:
            std::weak_ptr<BinableType> mMaterial;
        };
        
        template<typename BinableType>
        class MaterialProxy : public IMaterial {
        public:
            
            MaterialProxy(std::shared_ptr<BinableType> material):mMaterial(std::move(material)){}
            
            virtual void bind() override {
                mMaterial->bind();
            }
            
            virtual void unbind() override {
                mMaterial->unbind();
            }
            
            std::shared_ptr<BinableType> getBindable(){
                return mMaterial;
            }
            
            bool isProxy() override { return true; }
            
        private:
            std::shared_ptr<BinableType> mMaterial;
        };
        
    }//end namespace detail
        
    class MeshRenderer {
    public:
        
        template<typename BindableType, typename...Args>
        MeshRenderer(Scene& scene, ofMesh mesh, Args&&...args):
            mMesh(std::move(mesh)),
            mMaterial(std::allocate_shared<detail::Material<BindableType>>(scene.getAllocator<detail::Material<BindableType>>(), std::forward<Args>(args)...))
        {}
        
        template<typename BindableType>
        MeshRenderer(Scene& scene, ofMesh mesh, std::shared_ptr<BindableType> material) :
        mMesh(std::move(mesh)),
        mMaterial(std::allocate_shared<detail::MaterialProxy<BindableType>>(scene.getAllocator<detail::MaterialProxy<BindableType>>(), std::move(material)))
        {}
        
        template<typename BindableType>
        MeshRenderer(Scene& scene, ofMesh mesh, std::weak_ptr<BindableType> material) :
            mMesh(std::move(mesh)),
            mMaterial(std::allocate_shared<detail::MaterialProxyWeak<BindableType>>(scene.getAllocator<detail::MaterialProxyWeak<BindableType>>(), std::move(material)))
        {}
        
        inline void draw()
        {
            if(mMaterial)
                mMaterial->bind();
            mMesh.draw();
            if(mMaterial)
                mMaterial->unbind();
        }
        
        template<typename MaterialType>
        std::shared_ptr<MaterialType> getMaterial(){
            if(mMaterial){
                if(mMaterial->isProxy()){
                    auto proxy = std::dynamic_pointer_cast<detail::MaterialProxy<MaterialType>>(mMaterial);
                    return proxy->getBindable();
                }else{
                    return std::dynamic_pointer_cast<MaterialType>(mMaterial);
                }
            }
            return nullptr;
        }
    private:
        ofMesh mMesh;
        std::shared_ptr<detail::IMaterial> mMaterial;
    };
    
    inline ofMesh meshFromRect(const ofRectangle& rect, bool normalized = true)
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
    
}//end namespace mediasystem
