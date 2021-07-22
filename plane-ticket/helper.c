#include "helper.h"

uint64_t hashing(wchar_t* str)
{
	uint64_t ret = 0;
	for (int i = 0; i < lstrlen(str); i++)
	{
		if (str[i] != 0)
		{
			ret *= 37;
			ret += (int)(str[i] >= L'0' && str[i] <= '9' ? str[i] - L'0' : str[i] - 'A');
		}
	}
	return ret;
}