//
//  TupleHelpers.hpp
//  WallTest
//
//  Created by Michael Allison on 6/19/18.
//

#pragma once
#include <tuple>

#define UNUSED_VARIABLE(expr) do { (void)(expr); } while (0)

namespace mediasystem {
    
    namespace detail
    {
        template<int... Is>
        struct seq { };
        
        template<int N, int... Is>
        struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };
        
        template<int... Is>
        struct gen_seq<0, Is...> : seq<Is...> { };
        
        template<typename T, typename F, int... Is>
        void for_each(T&& t, F f, seq<Is...>)
        {
            auto l = { (f(std::get<Is>(t)), 0)... };
            UNUSED_VARIABLE(l);
        }
        template <class T, std::size_t N, class... Args>
        struct get_number_of_element_from_tuple_by_type_impl
        {
            static constexpr auto value = N;
        };
        
        template <class T, std::size_t N, class... Args>
        struct get_number_of_element_from_tuple_by_type_impl<T, N, T, Args...>
        {
            static constexpr auto value = N;
        };
        
        template <class T, std::size_t N, class U, class... Args>
        struct get_number_of_element_from_tuple_by_type_impl<T, N, U, Args...>
        {
            static constexpr auto value = get_number_of_element_from_tuple_by_type_impl<T, N + 1, Args...>::value;
        };
        
    } //end namespace detail
    
    template <class T, class... Args>
    T& get_element_by_type(std::tuple<Args...>& t)
    {
        return std::get<detail::get_number_of_element_from_tuple_by_type_impl<T, 0, Args...>::value>(t);
    }
    
    template<typename... Ts, typename F>
    void for_each_in_tuple(std::tuple<Ts...>& t, F f)
    {
        detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
    }
    
}//end namespace mediasystem
