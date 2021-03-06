//
//  Event.hpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#pragma once
#include <stdint.h>
#include <memory>
#include "mediasystem/util/TypeID.hpp"

namespace mediasystem {

    using IEventRef = std::shared_ptr<struct IEvent>;
    
    struct IEvent {
        virtual ~IEvent() = default;
        virtual type_id_t getType() const = 0;
    };
    
    template<typename EventType>
    struct Event : IEvent {
        type_id_t getType() const override { return type_id<EventType>; }
    };
    
}//end namespace mediasystem

