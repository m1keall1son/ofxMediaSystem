//
//  IBehavior.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#pragma once
#include <memory>
#include <vector>

namespace mediasystem {
    
    class SceneBase;
    
    class IBehavior;
    
    using BehaviorRef = std::shared_ptr<IBehavior>;
    using BehaviorHandle = std::weak_ptr<IBehavior>;
    using BehaviorSet = std::vector<BehaviorRef>;
    
    class IBehavior {
    public:
        
        explicit IBehavior(SceneBase* scene):mScene(scene){}
        virtual ~IBehavior()=default;
    
        virtual void init() = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void update() = 0;
        virtual void draw() = 0;
        virtual void debugDraw() = 0;

        inline virtual SceneBase* getScene(){return mScene;}
        inline virtual void enable(){mEnabled = true;}
        inline virtual void disable(){mEnabled = false;}
        inline virtual bool isEnabled()const{ return mEnabled;}
        inline virtual void enableDebugDraw(){mDrawDebug = true;}
        inline virtual void disableDebugDraw(){mDrawDebug = false;}
        inline virtual bool isDebugDrawEnabled()const{ return mDrawDebug;}
        inline virtual bool isInit() const{return mInit;}
        
        //internal use
        inline void _init(){ init(); mInit = true; }
        
    private:
        
        SceneBase* mScene;
        bool mInit{false};
        bool mEnabled{true};
        bool mDrawDebug{false};

    };
    
}//end namespace media system
