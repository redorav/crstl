#pragma once

template<typename To, typename From>
inline To union_cast(const From& from)
{
	static_assert(sizeof(To) == sizeof(From), "union_cast size mismatch");

	union cast_union
	{
		const From* from;
		const To* to;
	} cast;

	cast.from = &from;

	return *cast.to;
}