#pragma once

#include <chrono>

namespace mediasystem {

    template<typename Clock>
    class BasicTimer {
        public:

            BasicTimer(bool startOnConstruction = false){
                if(startOnConstruction)
                    start();
            }
         
            void start(){ mStartTime = Clock::now(); mRunning = true; }
         
            void stop(){ mRunning = false; mEndTime = Clock::now(); }
         
            const bool isRunning()const{ return mRunning; }
         
            const double getSeconds(){ 
                return static_cast<double>(getNanos())/1000000000.0;
            }
        
            const double getMilliseconds(){
                return static_cast<double>(getNanos())/1000000.0;
            }

            const long getNanos(){
                std::chrono::time_point<Clock> end;
                if(mRunning){
                    end  = Clock::now();
                }else{
                    end = mEndTime;
                }
                return std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStartTime).count();
            }

        private:

        bool mRunning{false};
        std::chrono::time_point<Clock> mStartTime;
        std::chrono::time_point<Clock> mEndTime;
        
    };

    using Timer = BasicTimer<std::chrono::high_resolution_clock>;

}//end namespace mediasystem
