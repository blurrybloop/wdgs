#include "memmng.h"

template <class T>
bool ptr_equals<T>::operator()(const T& p1, const T& p2) const
{
	return *p2 = *p1;
}
