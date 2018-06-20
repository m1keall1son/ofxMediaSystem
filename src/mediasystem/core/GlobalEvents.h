//
//  GlobalEvents.h
//  WallTest
//
//  Created by Michael Allison on 6/19/18.
//

#pragma once

#include "mediasystem/events/IEvent.h"

namespace mediasystem {
    
    struct SystemInit : Event<SystemInit> {};
    struct SystemPostInit : Event<SystemPostInit> {};
    struct SystemShutdown : Event<SystemShutdown> {};
    struct SystemReset : Event<SystemReset> {};
    
}//end namespace mediasystem
