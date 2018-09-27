//
//  MeshRenderer.hpp
//  WallTest
//
//  Created by Michael Allison on 6/22/18.
//

#pragma once

#include "ofMain.h"
#include "mediasystem/memory/Memory.h"

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
        
        constexpr size_t MATERIAL_ALLOCATION_BLOCK_SIZE = 64;

        template<typename T>
        using MaterialAllocator = DynamicAllocator<detail::Material<T>, MATERIAL_ALLOCATION_BLOCK_SIZE>;
        
        template<typename T>
        using MaterialProxyAllocator = DynamicAllocator<detail::MaterialProxy<T>, MATERIAL_ALLOCATION_BLOCK_SIZE>;
        
        template<typename T>
        using MaterialProxyWeakAllocator = DynamicAllocator<detail::MaterialProxyWeak<T>, MATERIAL_ALLOCATION_BLOCK_SIZE>;
        
    }//end namespace detail
    
    ofMesh meshFromRect(const ofRectangle& rect, bool normalized = true);
    
    class MeshRenderer {
    public:
        
        template<typename BindableType, typename...Args>
        MeshRenderer(ofMesh mesh, Args&&...args):
            mMesh(std::move(mesh)),
            mMaterial(std::allocate_shared<detail::Material<BindableType>>(detail::MaterialAllocator<BindableType>(), std::forward<Args>(args)...))
        {}
        
        template<typename BindableType>
        MeshRenderer(ofMesh mesh, std::shared_ptr<BindableType> material) :
        mMesh(std::move(mesh)),
        mMaterial(std::allocate_shared<detail::MaterialProxy<BindableType>>(detail::MaterialProxyAllocator<BindableType>(), std::move(material)))
        {}
        
        template<typename BindableType>
        MeshRenderer(ofMesh mesh, std::weak_ptr<BindableType> material) :
            mMesh(std::move(mesh)),
            mMaterial(std::allocate_shared<detail::MaterialProxyWeak<BindableType>>(detail::MaterialProxyWeakAllocator<BindableType>(), std::move(material)))
        {}
        
        void draw();
        
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
    
}//end namespace mediasystem
