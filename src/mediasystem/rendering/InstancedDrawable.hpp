//
//  InstancedDrawable.hpp
//  Belron
//
//  Created by Michael Allison on 10/4/18.
//

#pragma once

#include "mediasystem/util/TupleHelpers.hpp"
#include "mediasystem/util/MemberDetection.hpp"
#include "mediasystem/core/ComponentSystem.hpp"
#include "mediasystem/core/Scene.h"
#include "ofMain.h"

namespace mediasystem {
    
    template<typename T>
    struct AttribInfo;
    
    template<>
    struct AttribInfo<float> {
        constexpr static int components = 1;
        constexpr static const char* name(){ return "float"; }
    };
    
    template<>
    struct AttribInfo<glm::vec2> {
        constexpr static int components = 2;
        constexpr static const char* name(){ return "vec2"; }
    };
    
    template<>
    struct AttribInfo<glm::vec3> {
        constexpr static int components = 3;
        constexpr static const char* name(){ return "vec3"; }
    };
    
    template<>
    struct AttribInfo<glm::vec4> {
        constexpr static int components = 4;
        constexpr static const char* name(){ return "vec4"; }
    };
    
    template<>
    struct AttribInfo<glm::mat4> {
        constexpr static int components = 16;
        constexpr static const char* name(){ return "mat4"; }
    };
    
    struct AttributeCreator {
        AttributeCreator(ofBufferObject& buf, ofVboMesh& vboMesh, int startingLocation, int total_stride ):location(startingLocation), stride(total_stride), buffer(buf), mesh(vboMesh) {}
        int location{5};
        int offset{0};
        int stride{0};
        ofBufferObject& buffer;
        ofVboMesh& mesh;
        template<typename T>
        void operator()(T& type){
            static_assert( std::is_same<glm::mat4,T>::value ||  std::is_same<glm::vec2,T>::value || std::is_same<glm::vec3,T>::value || std::is_same<glm::vec4,T>::value || std::is_same<float,T>::value, "GPUTypes can only be a float, or glm::vec* or glm::mat4" );
            auto comp = AttribInfo<T>::components;
#if defined(TARGET_LINUX)
            mesh.getVbo().setAttributeBuffer(location, buffer, comp, stride, stride - (offset + sizeof(T)));
            ofLogVerbose("AttributeCreator") << "(linux- offsets are inverted?) attrib " << AttribInfo<T>::name() << " locaton: " << location << " components: " << comp << "  stride: " << stride << " offset: " << stride - (offset + sizeof(T));
#else
            mesh.getVbo().setAttributeBuffer(location, buffer, comp, stride, offset);
               ofLogVerbose("AttributeCreator") << "attrib " << AttribInfo<T>::name() << " locaton: " << location << " components: " << comp << "  stride: " << stride << " offset: " << offset;
#endif
            mesh.getVbo().setAttributeDivisor(location, 1);
            ++location;
            offset += sizeof(T);
        }
    };
    
    template<>
    inline void AttributeCreator::operator()(glm::mat4& matrix){
        for(int i =0; i<4; i++){
#if defined(TARGET_LINUX)
            mesh.getVbo().setAttributeBuffer(location, buffer, 4, stride, stride - (offset + sizeof(glm::vec4)));
            ofLogVerbose("AttributeCreator") << "(linux- offsets are inverted?) attrib mat4 locaton: " << location << " components: " << 4 << "  stride: " << stride << " offset: " << stride - (offset + sizeof(glm::vec4));
#else
            mesh.getVbo().setAttributeBuffer(location, buffer, 4, stride, offset);
            ofLogVerbose("AttributeCreator") << "attrib mat4 locaton: " << location << " components: " << 4 << "  stride: " << stride << " offset: " << offset;
#endif
            mesh.getVbo().setAttributeDivisor(location, 1);
            ++location;
            offset += sizeof(glm::vec4);
        }
    }
    
    template<typename UniformData, typename InstanceType, typename...GPUTypes>
    class InstancedDrawable {
    public:
        
        GENERATE_HAS_MEMBER_FUNCTION_SIGNITURE(bind);
        GENERATE_HAS_MEMBER_FUNCTION_SIGNITURE(unbind);
        GENERATE_HAS_MEMBER_FUNCTION_SIGNITURE(prepareShader);
        GENERATE_HAS_MEMBER_FUNCTION_SIGNITURE(populate);
       
        static_assert(sizeof...(GPUTypes) > 0, "GPU representation cannot be empty. use a regular material.");
        
        using GPURep = typename std::conditional<sizeof...(GPUTypes) == 1, typename std::tuple_element<0, std::tuple<GPUTypes...>>::type, std::tuple<GPUTypes...>>::type;
        
        static_assert( sizeof(GPURep) == sizeof(std::tuple<GPUTypes...>), "Ensuring sizes are equal" );
        
        static_assert(has_member_function_signiture_populate<InstanceType,void(GPUTypes&...)>(), "InstanceType must have member with signiture `void populate(GPUTypes...&)`");
        static_assert(has_member_function_signiture_prepareShader<UniformData,void(ofShaderSettings&)>(), "UniformData must have member `void UniformData::prepareShader(ofShaderSettings&)`");
        static_assert(has_member_function_signiture_bind<UniformData,void(ofShader&)>(), "UniformData must have member `void UniformData::bind(ofShader&)`");
        static_assert(has_member_function_signiture_unbind<UniformData,void(ofShader&)>(), "UniformData must have member `void UniformData::unbind(ofShader&)`");

        InstancedDrawable(Scene& scene, UniformData uniforms, ofMesh baseMesh, size_t initialSize = 0):
            mMesh(std::move(baseMesh)),
            mUniformData(std::move(uniforms))
        {
            ofShaderSettings settings;
            mUniformData.prepareShader(settings);
            mShader.setup(settings);
            mInstances = scene.getComponents<InstanceType>();
            if(initialSize > 0){
                resize(initialSize);
            }
        }
        
        InstancedDrawable(Scene& scene, ofMesh baseMesh, size_t initialSize = 0):mMesh(std::move(baseMesh)){
            ofShaderSettings settings;
            mUniformData.prepareShader(settings);
            mShader.setup(settings);
            mInstances = scene.getComponents<InstanceType>();
            if(initialSize > 0){
                resize(initialSize);
            }
        }
        
        void resize(size_t size){
            mLocalBuffer.resize(size);
            mGpuBuffer.allocate(sizeof(GPURep) * mLocalBuffer.size(), GL_STREAM_DRAW);
            auto dummy = std::tuple<GPUTypes...>(); //force a tuple even if there's just one type to use the attribute createor
            for_each_in_tuple(dummy, AttributeCreator(mGpuBuffer, mMesh, 5, sizeof(GPURep)));
        }
        
        void draw(){
            auto iter = mInstances.iter();
            if(mInstances.size() > mLocalBuffer.size()){
                resize(mInstances.size());
            }
            auto dataPtr = mLocalBuffer.begin();
            while(auto it = iter.next()){
                callPopulate(*it, *dataPtr++, gen_seq<sizeof...(GPUTypes)>(), BoolType<(is_greater<sizeof...(GPUTypes),1>())>());
            }
            mGpuBuffer.updateData(sizeof(GPURep) * mInstances.size(), mLocalBuffer.data());
            
            mShader.begin();
            mUniformData.bind(mShader);
            mMesh.drawInstanced(OF_MESH_FILL, mInstances.size());
            mUniformData.unbind(mShader);
            mShader.end();
        }
        
        UniformData& getUniforms(){ return mUniformData; }
        ofShader& getShader(){ return mShader; }
        ofVboMesh& getMesh(){ return mMesh; }
        ofBufferObject& getBuffer(){ return mGpuBuffer; }

    private:
        
        template<size_t one, size_t two>
        struct is_greater : public std::integral_constant<bool,(one > two)>{};
        
        template<bool val>
        struct BoolType : public std::integral_constant<bool, val>{};
        
        template<int ...S>
        void callPopulate(InstanceType& instance, GPURep& gpu, seq<S...>, BoolType<false>) {
            instance.populate(gpu);
        }
        
        template<int ...S>
        void callPopulate(InstanceType& instance, std::tuple<GPUTypes...>& gpu, seq<S...>, BoolType<true>) {
            instance.populate(std::get<S>(gpu)...);
        }
        
        ComponentMap<InstanceType> mInstances;
        std::vector<GPURep> mLocalBuffer;
        ofBufferObject mGpuBuffer;
        ofShader mShader;
        ofVboMesh mMesh;
        UniformData mUniformData;
    };
    
}//end namespace mediasystem
