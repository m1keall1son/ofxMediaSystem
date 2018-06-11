#pragma once

#include "mediasystem/behavior/IBehavior.h"
#include <functional>

namespace mediasystem {

	template<typename DataPolicy, typename BehaviorType>
	class Behavior : public DataPolicy, public BehaviorType {

        static_assert( std::is_base_of<IBehavior,BehaviorType>::value, "Behavior type must derive from IBehavior" );
        
	public:

		using BehaviorInitfn = std::function<void(Behavior<DataPolicy>*)>;
		using BehaviorDrawfn = std::function<void(Behavior<DataPolicy>*)>;
		using BehaviorUpdatefn = std::function<void(Behavior<DataPolicy>*)> ;

		template<typename...Args>
		Behavior(IScene* scene, Args&&...args):BehaviorType(scene),DataPolicy(std::forward<Args>(args)...) {
		}

		~Behavior() = default;
		
		inline virtual void init() override {
			if (mInitFn)
				mInitFn(this);
		}

		inline virtual void start() override {
			if (mStartFn)
				mStartFn(this);
		}

		inline virtual void stop() override {
			if (mStopFn)
				mStopFn(this);
		}

		inline virtual void update() override {
			if (mUpdateFn)
				mUpdateFn(this);
		}

		inline virtual void draw() override {
			if (mDrawFn)
				mDrawFn(this);
		}

		inline void setInitFn(const BehaviorInitfn& fn) { mInitFn = fn; }
		inline void setStartFn(const BehaviorUpdatefn& fn) { mStartFn = fn; }
		inline void setStopFn(const BehaviorUpdatefn& fn) { mStopFn = fn; }
		inline void setUpdateFn(const BehaviorUpdatefn& fn) { mUpdate = fn; }
		inline void setDrawFn(const BehaviorDrawfn& fn) { mDrawFn = fn; }

	private:

		BehaviorInitfn mInitFn;
		BehaviorUpdatefn mStartFn;
		BehaviorUpdatefn mStopFn;
		BehaviorUpdatefn mUpdateFn;
		BehaviorDrawfn mDrawFn;

	};

}// end namespace mediasystem
