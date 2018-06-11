#pragma once
#include <string>
#include <type_traits>
#include "mediasystem/behaviors/IBehavior.h"

namespace mediasystem {

	class MediaController;

	class SceneBase {
	public:

		SceneBase(MediaController* context, const std::string& name);
		virtual ~SceneBase() = default;

		virtual void init() = 0;
        virtual void setup() = 0;
		virtual void update() = 0;
		virtual void start() = 0;
		virtual void stop() = 0;
        
        virtual void enableAllDebugDraw();
        virtual void disableAllDebugDraw();

        inline const std::string& getName() const { return mName; }
        inline bool isInit() const { return mIsInit; }
        
		template<typename Behavior, typename...Args>
        inline Behavior* createBehavior(Args&&...args)
		{
            static_assert(std::is_base_of<IBehavior,Behavior>::value, "Behavior must inherit from IBehavior or TransformBehavior");
            
            auto behavior = std::make_shared<Behavior>(this, std::forward<Args>(args)...);
			mBehaviors.push_back(behavior);
            mUnplacedBehaviors.push_back(behavior);
            
			return behavior.get();
		}

        virtual void initBehaviors();
		void _start();
        void _init();

	protected:
        void placeBehaviors();
        void placeBehavior(const std::shared_ptr<IBehavior>& behavior);

		std::string		mName;
		BehaviorSet		mBehaviors;
        BehaviorSet     mUnplacedBehaviors;

	private:

        bool mIsInit{false};
		MediaController* mController;

	};

}//end namespace mediasystem
