#ifndef _WDGS_MEMMNG_H
#define _WDGS_MEMMNG_H

#include <memory>

#define DECLARE_MEMMNG(className)			\
public:										\
	typedef std::shared_ptr<className> Ptr;	\
	static Ptr Create()						\
	{										\
		return Ptr(new className);			\
	}										\

#endif