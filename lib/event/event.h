#ifndef __EVENT_H__
#define __EVENT_H__

class EventArgs {};

typedef void(*EventFunc)(void*, EventArgs*);

template<typename F>
class Event;

template<class R>
class Event<R(*)(void*, EventArgs*)>
{
	typedef R(*FuncType)(void*, EventArgs*);
	FuncType ls = NULL;
public:
	Event<FuncType>& operator+=(FuncType t)
	{
		ls = t;
		return *this;
	}

	Event<FuncType>& operator-=(FuncType t)
	{
		ls = NULL;
		return *this;
	}

	void operator()(void* sender, EventArgs* b)
	{
		if (ls != NULL)
			(*ls)(sender, b);
	}
};


#endif // __EVENT_H__
