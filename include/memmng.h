#ifndef _WDGS_MEMMNG_H
#define _WDGS_MEMMNG_H

#include <memory>
#include <functional>
#include <algorithm>

#define DECLARE_MEMMNG(className)			\
public:										\
	typedef std::shared_ptr<className> Ptr;	\
	static Ptr Create()						\
	{										\
		return Ptr(new className);			\
	}										\



template <class T>
class ptr_equals : public std::binary_function<T, T, bool>
{
public:
	bool operator()(const T& p1, const T& p2) const;
};

#endif
