//
//  MemberDetection.hpp
//  Belron
//
//  Created by Michael Allison on 10/4/18.
//

#pragma once

#include <type_traits>

//use SFINAE (Substitution Failure Is Not An Error) to determine if a type has certain traits, much like reflection

//Example Usage:
/*
     GENERATE_HAS_MEMBER(att)  // Creates 'has_member_att'.
     GENERATE_HAS_MEMBER(func) // Creates 'has_member_func'.
 
     struct A
     {
         int att;
         void func ( double );
     };
 
     struct B
     {
         char att[3];
         double func ( const char* );
     };
 
     struct C : A, B { }; // It will also work with ambiguous members.
 
     int main ( )
     {
         std::cout << std::boolalpha
         << "\n" "'att' in 'C' : "
         << has_member_att<C>::value // <type_traits>-like interface.
         << "\n" "'func' in 'C' : "
         << has_member_func<C>() // Implicitly convertible to 'bool'.
         << "\n";
     }
 */

#define GENERATE_HAS_MEMBER(member)                                                     \
    template <class T>                                                                  \
    class HasMember_##member {                                                          \
    private:                                                                            \
        using Yes = char[2];                                                            \
        using No = char[1];                                                             \
                                                                                        \
        struct Fallback { int member; };                                                \
        struct Derived : T, Fallback { };                                               \
                                                                                        \
        template<class U>                                                               \
        static No& test ( decltype(U::member)* );                                       \
                                                                                        \
        template < typename U >                                                         \
        static Yes& test ( U* );                                                        \
                                                                                        \
    public:                                                                             \
        static constexpr bool RESULT = sizeof(test<Derived>(nullptr)) == sizeof(Yes);   \
    };                                                                                  \
                                                                                        \
    template <class T>                                                                  \
    struct has_member_##member : public std::integral_constant<bool, detail::HasMember_##member<T>::RESULT>{};

//Example Usage
/*
     GENERATE_HAS_MEMBER_TYPE(Foo) // Creates 'has_member_type_Foo'.
 
     struct A
     {
        struct Foo;
     };
 
     struct B
     {
        using Foo = int;
     };
 
     struct C : A, B { }; // Will also work on incomplete or ambiguous types.
 
 
     int main ( )
     {
         std::cout << std::boolalpha
         << "'Foo' in 'C' : "
         << has_member_type_Foo<C>::value
         << "\n";
     }
 */

#define GENERATE_HAS_MEMBER_TYPE(Type)                                                  \
    template <class T>                                                                  \
    class HasMemberType_##Type                                                          \
    {                                                                                   \
    private:                                                                            \
        using Yes = char[2];                                                            \
        using  No = char[1];                                                            \
                                                                                        \
        struct Fallback { struct Type { }; };                                           \
        struct Derived : T, Fallback { };                                               \
                                                                                        \
        template < class U >                                                            \
        static No& test ( typename U::Type* );                                          \
        template < typename U >                                                         \
        static Yes& test ( U* );                                                        \
                                                                                        \
    public:                                                                             \
        static constexpr bool RESULT = sizeof(test<Derived>(nullptr)) == sizeof(Yes);   \
    };                                                                                  \
    template <class T>                                                                  \
    struct has_member_type_##Type : public std::integral_constant<bool, detail::HasMemberType_##Type<T>::RESULT>{};

//Example Usage
/*
     GENERATE_HAS_MEMBER_FUNCTION_SIGNITURE(Foo) // Creates 'has_member_type_Foo'.
 
     struct A
     {
        void Foo(Bar&);
     };
 
     struct B
     {
        float Foo(const Baz*);
     };
 
     struct C
     {
        int Foo();
     };
 
     int main ( )
     {
        std::cout << std::boolalpha
        << "A has Foo with signiture void(Bar&): "
        << has_member_function_signiture_Foo<A,void(Bar&)>() << "\n"
        << "B has Foo with signiture float(const Baz*): "
        << has_member_function_signiture_Foo<B,float(const Baz*)>() << "\n"
        << "C has Foo with signiture void(): "
        << has_member_function_signiture_Foo<C,void()>()
        << "\n";
     }
 
     //prints:
     A has Foo with signiture void(Bar&): true
     B has Foo with signiture float(const Baz*): true
     C has Foo with signiture void(): false
 
     NOTE: will NOT work on static functions
 */

#define GENERATE_HAS_MEMBER_FUNCTION_SIGNITURE(function)                                \
    template<typename T, typename Ret, typename ... Args>                               \
    class _HasMemeberFunctionSigniture_##function                                       \
    {                                                                                   \
        template <typename U, Ret (U::*)(Args...)> struct Check;                        \
        template <typename U> static char func(Check<U, &U::function> *);               \
        template <typename U> static int func(...);                                     \
    public:                                                                             \
        typedef _HasMemeberFunctionSigniture_##function type;                           \
        enum { value = sizeof(func<T>(0)) == sizeof(char) };                            \
    };                                                                                  \
                                                                                        \
    template<typename T, typename>                                                      \
    struct has_member_function_signiture_##function;                                    \
                                                                                        \
    template<typename T, typename Ret, typename...Args>                                 \
    struct has_member_function_signiture_##function<T,Ret(Args...)> : public std::integral_constant<bool,_HasMemeberFunctionSigniture_##function<T,Ret,Args...>::value> {};


//specialized for call operator
namespace mediasystem {
    namespace detail {
        template<typename T, typename Ret, typename ... Args>
        class HasMemeberCallOperatorSigniture
        {
            template <typename U, Ret (U::*)(Args...)> struct Check;
            template <typename U> static char func(Check<U, &U::operator()> *);
            template <typename U> static int func(...);
        public:
            typedef HasMemeberCallOperatorSigniture type;
            enum { value = sizeof(func<T>(0)) == sizeof(char) };
        };
    }//end namespace detail
    template<typename T, typename>
    struct is_callable_with_signiture;

    template<typename T, typename Ret, typename...Args>
    struct is_callable_with_signiture<T,Ret(Args...)> : public std::integral_constant<bool,detail::HasMemeberCallOperatorSigniture<T,Ret,Args...>::value> {};
}//end namespace media system
