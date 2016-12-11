#ifndef _WDGS_MEMMNG_H
#define _WDGS_MEMMNG_H

#include <memory>
#include <functional>
#include <algorithm>
#include <iostream>


#define DECLARE_MEMMNG_NOCREATE(className)  \
public:										\
	typedef std::shared_ptr<className> Ptr;	\


#define DECLARE_MEMMNG(className)			\
DECLARE_MEMMNG_NOCREATE(className)			\
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

class Saveable
{
public:
	virtual void Save(std::ostream& fs) = 0;
	virtual void Load(std::istream& fs) = 0;
};

#endif
