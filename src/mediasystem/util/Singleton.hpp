#pragma once
#include <memory>

namespace mediasystem {

	template<typename T>
	class Singleton {
	public:

		static T* getPtr() {
			if (!sInstance) {
				init();
			}
			return sInstance.get();
		}

        static T& get() {
            if (!sInstance) {
                init();
            }
            return *sInstance;
        }
        
		static void destroy() {
			sInstance.reset();
		}

		template<typename...Args>
		static void init(Args&&...args) {
			sInstance.reset(new T(std::forward<Args>(args)...));
		}

	protected:
		static std::unique_ptr<T> sInstance;
	};

}//end namespace mediasystem

template<typename T>
std::unique_ptr<T> mediasystem::Singleton<T>::sInstance = nullptr;
