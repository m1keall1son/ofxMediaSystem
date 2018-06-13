#pragma once

namespace mediasystem {

	class IPresenter {
	public:

        IPresenter() = default;
        virtual ~IPresenter() = default;

        IPresenter(const IPresenter&) = delete;
        IPresenter& operator=(const IPresenter&) = delete;
        
        virtual void begin() = 0;
        virtual void end() = 0;
        virtual void present() = 0;

	};

}//end namespace mediasystem
