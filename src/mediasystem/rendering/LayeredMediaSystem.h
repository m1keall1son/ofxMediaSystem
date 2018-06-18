//
//  RenderSystem.hpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#pragma once

#include <unordered_map>
#include <list>
#include "mediasystem/events/IEvent.h"
#include "mediasystem/rendering/LayeredMediaComponent.h"
#include "mediasystem/rendering/IPresenter.h"

namespace mediasystem {
    
    class Scene;
    
    class LayeredMediaSystem {
    public:
        
        LayeredMediaSystem(Scene& context);
    
        template<typename Presenter, typename...Args>
        Presenter* createPresenter( Args&&...args ){
            static_assert(std::is_base_of<IPresenter,Presenter>::value, "Presenter must derive from IPresenter.");
            if(mPresenter)
                mPresenter.reset();
            mPresenter.reset(new Presenter(std::forward<Args>(args)...));
            return static_cast<Presenter*>(mPresenter.get());
        }
        
        inline void enableDebugDraw(){ mDebugDraw = true; }
        inline void disableDebugDraw(){ mDebugDraw = false; }
        inline bool isDebugDrawEnabled() const { return mDebugDraw; }
        
        void draw();
        void clear();
        
        inline void setGlobalAlpha(float alpha){ mGlobalAlpha = alpha; }
        inline float getGlobalAlpha()const{ return mGlobalAlpha; }

    private:
        
        EventStatus onDrawEvent(const IEventRef& event);
        EventStatus onNewComponentEvent(const IEventRef& event);

        void insertIntoLayer(float layer, LayeredMediaComponentHandle&& handle);
        
        std::unordered_map<float, std::list<LayeredMediaComponentHandle>> mComponentsByLayer;
        bool mDebugDraw{false};
        //todo this could be targets + materials etc.
        std::unique_ptr<IPresenter> mPresenter;
        float mGlobalAlpha{1.f};
    };
    
}//end namespace mediasystem


