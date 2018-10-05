//
//  Sort.h
//  Belron
//
//  Created by Michael Allison on 10/5/18.
//

#pragma once
#include <limits>
#include <algorithm>
#include <vector>
#include <cstddef>

namespace mediasystem {
    
    template<size_t MaxBits = std::numeric_limits<size_t>::digits>
    struct MSBInPlaceRadixQuickSort {
        using radix = typename std::bitset<MaxBits>;
        void operator()(typename std::vector<radix>::iterator begin, typename std::vector<radix>::iterator end, std::size_t current_radix, std::size_t until_radix = 0){
            if( begin != end )
            {
                // partition into two bins (1s and 0s) based on bit value at current radix
                const auto end_of_zeroes_bin = std::partition( begin, end, [current_radix]( radix value ) { return !value[current_radix]; } );
                
                if( current_radix > until_radix )
                {
                    MSBInPlaceRadixQuickSort::operator()( begin, end_of_zeroes_bin, current_radix - 1 ); // sort the zeroes bin on the next bit position
                    MSBInPlaceRadixQuickSort::operator()( end_of_zeroes_bin, end, current_radix - 1 ); // sort the ones bin on the next bit position
                }
            }
        }
    };
    
}//end namespace mediasystem
