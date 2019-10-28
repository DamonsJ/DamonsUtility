#ifndef _SINGLETON_HEADER_
#define _SINGLETON_HEADER_

#include <assert.h>
#include <mutex>
// Description: Implementation of generic base class of thread-safe singleton (anti)pattern.
// Usage: Inherit from it using CRTP and then on the first invocation of ::get_instance singleton 
//        an object'll be instantiated. You can also pass subclass ctor arguments at that moment.

template <typename T>
class OnceSingleton
{
public:
	OnceSingleton(){}
	virtual ~OnceSingleton(){}
private:
	static T * instance_;
	static std::once_flag instance_alloc_;
	static std::once_flag instance_free_;
	static bool initialized_;

	inline static T & get_instance_dispatch(std::true_type::type)
	{
		//static T new_instance;
		std::call_once(instance_alloc_, [&]() {
			//instance_ = &new_instance;

			// 2次初始化
			assert(instance_ == nullptr);
			instance_ = new T;
			initialized_ = true;
		});
		return *instance_;
	}

	inline static T & get_instance_dispatch(std::false_type::type)
	{
		// prevents uninitialized access
		assert(initialized_ == true);
		return *instance_;
	}

public:
	template <typename... Args>
	static T & get_instance(Args... args)
	{
		//static T new_instance(args...);
		std::call_once(instance_alloc_, [&]()
		{
			//instance_ = &new_instance;
			instance_ = new T(args...);
			initialized_ = true;
		});
		return *instance_;
	}

	static T & get_instance()
	{
		// ERROR: VS2013 - private构造被表示为非默认
		/*return get_instance_dispatch(
			typename std::is_same<
			std::true_type::type,
			typename std::is_default_constructible<T>::type>
			::type());*/
		std::true_type::type res = typename std::is_same <std::false_type::type,typename std::is_copy_constructible<T>::type >::type();
		return get_instance_dispatch(res);
	}

	static void destroy_instance()
	{
		std::call_once(instance_free_, [&]()
		{
			if (nullptr != instance_)
			{
				delete instance_;
			}
			instance_ = nullptr;
		});
	}
};
template <typename T> T * OnceSingleton<T>::instance_ = nullptr;
template <class T> std::once_flag OnceSingleton<T>::instance_alloc_;
template <class T> std::once_flag OnceSingleton<T>::instance_free_;
template <class T> bool OnceSingleton<T>::initialized_ = false;


#define MAKE_ONCESINGLETON(Class) \
    friend class OnceSingleton<Class>; \
    private: \
        Class() {\
		} \
        Class(const Class&) = delete; \
        Class& operator=(const Class&) = delete; \
        ~Class() {\
		} 

#define MAKE_ONCESINGLETON_DECLARE(Class) \
    friend class OnceSingleton<Class>; \
    private: \
        Class(); \
        Class(const Class&) = delete; \
        Class& operator=(const Class&) = delete; \
        ~Class(); 

#endif// 2018/10/25
