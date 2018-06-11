#pragma once
#include <assert.h>
#include <type_traits>

namespace mediasystem {

    template<typename T>
    class ExponentialMovingAverage {
        public:

            static_assert(std::is_same<float, T>::value || std::is_same<double, T>::value, "unsupported type! Must be double or float");
            
            ExponentialMovingAverage():
                mFilter(static_cast<T>(0.9)),
                mAvg(static_cast<T>(1.0))
            {}
            
            ExponentialMovingAverage(T filter, T init):
                mFilter(filter),
                mAvg(init)
            {
                assert((mFilter <= 1.0) && (mFilter >= 0.0));
            }

            const T filter(T sample){
                mAvg = mFilter * sample + (static_cast<T>(1.0)-mFilter) * mAvg;
                return mAvg;
            }

            const T getAverage() const { return mAvg; }

        private:
            T mAvg;
            T mFilter;
    };

}//end namespace mediasystem
