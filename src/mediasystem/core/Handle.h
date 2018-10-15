//
//  Handle.h
//  Belron
//
//  Created by Michael Allison on 10/11/18.
//

#pragma once

#include <memory>

//std:: shared & weak ptr

namespace mediasystem {
    
    template<typename T, typename U>
    std::weak_ptr<T> static_weak_pointer_cast( const std::weak_ptr<U>& generic ){
        if(auto locked = generic.lock()){
            return std::static_pointer_cast<T>(locked);
        }
        return std::weak_ptr<T>();
    }
    
    template<typename T, typename U>
    std::weak_ptr<T> dynamic_weak_pointer_cast( const std::weak_ptr<U>& generic ){
        if(auto locked = generic.lock()){
            return std::dynamic_pointer_cast<T>(locked);
        }
        return std::weak_ptr<T>();
    }
    
    template<typename T, typename U>
    std::weak_ptr<T> const_weak_pointer_cast( const std::weak_ptr<U>& generic ){
        if(auto locked = generic.lock()){
            return std::const_pointer_cast<T>(locked);
        }
        return std::weak_ptr<T>();
    }
    
    template< class T, class U >
    std::shared_ptr<T> reinterpret_pointer_cast( const std::shared_ptr<U>& r ) noexcept
    {
        auto p = reinterpret_cast<typename std::shared_ptr<T>::element_type*>(r.get());
        return std::shared_ptr<T>(r, p);
    }
    
    template<typename T, typename U>
    std::weak_ptr<T> reinterpret_weak_pointer_cast( const std::weak_ptr<U>& generic ){
        if(auto locked = generic.lock()){
            return reinterpret_pointer_cast<T>(locked);
        }
        return std::weak_ptr<T>();
    }
    
    template<typename T>
    struct weak_ptr_handle_adapter : public std::weak_ptr<T>{
        
        weak_ptr_handle_adapter() = default;
        weak_ptr_handle_adapter( const std::shared_ptr<T>& ptr ) : std::weak_ptr<T>(ptr){}
        weak_ptr_handle_adapter& operator=(const std::shared_ptr<T>& ptr) {
            std::weak_ptr<T>::operator=(ptr);
            return *this;
        }
     
        //de-reference access to a strong
        std::shared_ptr<T> operator*() {
            if( auto strong = std::weak_ptr<T>::lock() ){
                return strong;
            }else{
                return nullptr;
            }
        }
        
        //convertable to bool
        operator bool() const {
            return !std::weak_ptr<T>::expired();
        }
    };
    
    /////STRONGS AND HANDLES
    
    //needs to be implicitly convertable to bool and keep the pointed to object alive in a ref counted way
    template<typename T>
    using StrongHandle = std::shared_ptr<T>;
    
    //dereference to get an unowned strong ptr to the type, should be compareable
    //an must be convertable to bool
    template<typename T>
    using Handle = weak_ptr_handle_adapter<T>;
    
    namespace detail {
    
        template<typename T, typename U, template <typename> class ptr>
        struct cast {
            static ptr<T> dynamicCast( const ptr<U>& r ) noexcept;
            static ptr<T> staticCast( const ptr<U>& r ) noexcept;
            static ptr<T> constCast( const ptr<U>& r ) noexcept;
            static ptr<T> reinterpretCast( const ptr<U>& r ) noexcept;
        };
       
        template<typename T, typename U>
        struct cast<T,U,StrongHandle> {
            static StrongHandle<T> dynamicCast( const StrongHandle<U>& r ) noexcept;
            static StrongHandle<T> staticCast( const StrongHandle<U>& r ) noexcept;
            static StrongHandle<T> constCast( const StrongHandle<U>& r ) noexcept;
            static StrongHandle<T> reinterpretCast( const StrongHandle<U>& r ) noexcept;
        };
        
        template<typename T, typename U>
        struct cast<T,U,Handle> {
            static Handle<T> dynamicCast( const Handle<U>& r ) noexcept;
            static Handle<T> staticCast( const Handle<U>& r ) noexcept;
            static Handle<T> constCast( const Handle<U>& r ) noexcept;
            static Handle<T> reinterpretCast( const Handle<U>& r ) noexcept;
        };
        
        template<typename T, typename U>
        StrongHandle<T> cast<T,U,StrongHandle>::dynamicCast( const StrongHandle<U>& r ) noexcept {
            return std::dynamic_pointer_cast<T>(r);
        }
        
        template<typename T, typename U>
        StrongHandle<T> cast<T,U,StrongHandle>::staticCast( const StrongHandle<U>& r ) noexcept {
            return std::static_pointer_cast<T>(r);
        }
        
        template<typename T, typename U>
        StrongHandle<T> cast<T,U,StrongHandle>::constCast( const StrongHandle<U>& r ) noexcept {
            return std::const_pointer_cast<T>(r);
        }
        
        template<typename T, typename U>
        StrongHandle<T> cast<T,U,StrongHandle>::reinterpretCast( const StrongHandle<U>& r ) noexcept {
            return reinterpret_pointer_cast<T>(r);
        }
        
        template<typename T, typename U>
        Handle<T> cast<T,U,Handle>::dynamicCast( const Handle<U>& r ) noexcept {
            return dynamic_weak_pointer_cast<T>(r);
        }
        
        template<typename T, typename U>
        Handle<T> cast<T,U,Handle>::staticCast( const Handle<U>& r ) noexcept {
            return static_weak_pointer_cast<T>(r);
        }
        
        template<typename T, typename U>
        Handle<T> cast<T,U,Handle>::constCast( const Handle<U>& r ) noexcept {
            return const_weak_pointer_cast<T>(r);
        }
        
        template<typename T, typename U>
        Handle<T> cast<T,U,Handle>::reinterpretCast( const Handle<U>& r ) noexcept {
            return reinterpret_weak_pointer_cast<T>(r);
        }
       
    }//end namspace detail
    
    
    template<typename T, typename...Args>
    inline StrongHandle<T> makeStrongHandle( Args&&...args ){
        return std::make_shared<T>( std::forward<Args>(args)... );
    }
    
    template<typename T, typename Alloc, typename...Args>
    inline StrongHandle<T> allocateStrongHandle( Alloc&& alloc, Args&&...args ){
        return std::allocate_shared<T>( std::forward<Alloc>(alloc), std::forward<Args>(args)... );
    }
    
    template<typename T, typename U>
    inline StrongHandle<T> staticCast( const StrongHandle<U>& ptr ) noexcept {
        return detail::cast<T,U,StrongHandle>::staticCast(ptr);
    }
    
    template<typename T, typename U>
    inline StrongHandle<T> dynamicCast( const StrongHandle<U>& ptr ) noexcept {
        return detail::cast<T,U,StrongHandle>::dynamicCast(ptr);
    }
    
    template<typename T, typename U>
    inline StrongHandle<T> constCast( const StrongHandle<U>& ptr ) noexcept {
        return detail::cast<T,U,StrongHandle>::constCast(ptr);
    }
    
    template<typename T, typename U>
    inline StrongHandle<T> reinterpretCast( const StrongHandle<U>& ptr ) noexcept {
        return detail::cast<T,U,StrongHandle>::reinterpretCast(ptr);
    }
    
    template<typename T, typename U>
    inline Handle<T> staticCast( const Handle<U>& handle ) noexcept {
        return detail::cast<T,U,Handle>::staticCast(handle);
    }
    
    template<typename T, typename U>
    inline Handle<T> dynamicCast( const Handle<U>& handle ) noexcept {
        return detail::cast<T,U,Handle>::dynamicCast(handle);
    }
    
    template<typename T, typename U>
    inline Handle<T> constCast( const Handle<U>& handle  ) noexcept {
        return detail::cast<T,U,Handle>::constCast(handle);
    }
    
    template<typename T, typename U>
    inline Handle<T> reinterpretCast( const Handle<U>& handle ) noexcept {
        return detail::cast<T,U,Handle>::reinterpretCast(handle);
    }
    
    /*
     //would be coupled to the allocation model
     template<typename Alloc>
     struct alloc_deleter
     {
     alloc_deleter(const Alloc& a) : a(a) { }
     
     typedef typename std::allocator_traits<Alloc>::pointer pointer;
     
     void operator()(pointer p) const
     {
     Alloc aa(a);
     std::allocator_traits<Alloc>::destroy(aa, std::addressof(*p));
     std::allocator_traits<Alloc>::deallocate(aa, p, 1);
     }
     
     private:
     Alloc a;
     };
     
     template<typename T, typename Alloc, typename... Args>
     auto
     allocate_unique(const Alloc& alloc, Args&&... args)
     {
     using AT = std::allocator_traits<Alloc>;
     static_assert(std::is_same<typename AT::value_type, std::remove_cv_t<T>>{}(),
     "Allocator has the wrong value_type");
     
     Alloc a(alloc);
     auto p = AT::allocate(a, 1);
     try {
     AT::construct(a, std::addressof(*p), std::forward<Args>(args)...);
     using D = alloc_deleter<Alloc>;
     return std::unique_ptr<T, D>(p, D(a));
     }
     catch (...)
     {
     AT::deallocate(a, p, 1);
     throw;
     }
     }
     
     int main()
     {
     std::allocator<int> a;
     auto p = allocate_unique<int>(a, 0);
     return *p;
     }
     */
    
}//end namespace mediasystem
