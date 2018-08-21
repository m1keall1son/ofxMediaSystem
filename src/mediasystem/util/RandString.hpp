//
//  RandString.hpp
//  Aerialtronics
//
//  Created by Michael Allison on 8/20/18.
//

#pragma once

namespace mediasystem {


    inline std::string randString(size_t length)
    {
        auto randchar = []() -> char
        {
            const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[(int)ofRandom(max_index)];
        };
        std::string str(length, 0);
        std::generate_n(str.begin(), length, randchar);
        return str;
    }

}//end namespace mediasystem
