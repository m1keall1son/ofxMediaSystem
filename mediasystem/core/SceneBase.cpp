#include "SceneBase.h"
#include "mediasystem/core/MediaController.h"
#include "mediasystem/behaviors/ViewBase.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {

    SceneBase::SceneBase(MediaController* context, const std::string & name) :mController(context), mName(name)
    {
    }

    void SceneBase::_init()
    {
        init();
        mIsInit = true;
    }
    
    void SceneBase::initBehaviors()
    {
        auto it = mBehaviors.begin();
        auto end = mBehaviors.end();
        while (it != end) {
            if (!(*it)->isInit()) {
                (*it)->_init();
            }
            ++it;
        }
    }
    
    void SceneBase::enableAllDebugDraw()
    {
        auto it = mBehaviors.begin();
        auto end = mBehaviors.end();
        while (it != end) {
            (*it++)->enableDebugDraw();
        }
    }
    
    void SceneBase::disableAllDebugDraw()
    {
        auto it = mBehaviors.begin();
        auto end = mBehaviors.end();
        while (it != end) {
            (*it++)->disableDebugDraw();
        }
    }

    void SceneBase::_start()
    {
        initBehaviors();
        placeBehaviors();
        start();
    }
    
    void SceneBase::placeBehavior(const std::shared_ptr<IBehavior>& behavior)
    {
        auto handle = BehaviorHandle(behavior);
        
        mController->addToUpdateQueue(handle);
        
        auto view = std::dynamic_pointer_cast<ViewBase>(behavior);
        if(view){
            mController->addToRenderQueue( ViewBaseHandle(view) );
            //todo this wouldn't happen if input was allowed later, views should place themselves on queues when significant updates occur
            if(view->allowsInput()){
                mController->registerInputComponent(view->getInputComponent());
            }
        }
    }
    
    void SceneBase::placeBehaviors()
    {
        for(auto & b : mUnplacedBehaviors){
            placeBehavior(b);
        }
        mUnplacedBehaviors.clear();
    }
    
}//end namespace mediasystem
