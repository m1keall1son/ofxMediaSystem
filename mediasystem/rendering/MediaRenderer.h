//
//  MediaRenderer.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#pragma once

#include <stdint.h>
#include <map>
#include <list>
#include <memory>
#include "mediasystem/rendering/IPresenter.h"

namespace mediasystem {
    
    class ViewBase;
    
    using DrawQueue = std::map<float, std::list<std::weak_ptr<ViewBase>>>;

    class MediaRenderer {
    public:
        
        MediaRenderer();
        ~MediaRenderer();
        
        void draw();
        void clear();
        
        void insert(float layer, const std::weak_ptr<ViewBase>& handle);
        
        template<typename Presenter, typename...Args>
        Presenter* createPresenter( Args&&...args ){
            static_assert(std::is_base_of<IPresenter,Presenter>::value, "Presenter must derive from IPresenter.");
            if(mPresenter)
                mPresenter.reset();
            mPresenter.reset(new Presenter(std::forward<Args>(args)...));
            return static_cast<Presenter*>(mPresenter.get());
        }
        
        bool isDebugDrawing()const{return mDebug;}
        void setDebugDraw(bool debug){ mDebug = debug; }
        
    private:
        
        bool mDebug{false};
        DrawQueue mDrawQueue;
        std::unique_ptr<IPresenter> mPresenter;
        
    };
    
}//end namespace mediasystem
