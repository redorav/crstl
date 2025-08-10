#pragma once

template<typename To, typename From>
To union_cast(const From& from)
{
	union
	{
		From from;
		To to;
	} cast;

	return cast.to;
}