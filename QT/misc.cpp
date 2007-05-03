#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#ifdef WIN32

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

#else // WIN32

#include <stdlib.h>
#include <wchar.h>

#endif
#include <qstring.h>
#include <misc.h>

void wcstoqstring(QString &dst, const wchar_t *src, int length /*= -1*/)
{
	int i;
	QChar *temp;

	dst.truncate(0);
	if (!src)
	{
		return;
	}
	if (length == -1)
	{
		length = wcslen(src);
	}
	temp = new QChar[length];
	for (i = 0; i < length; i++)
	{
		temp[i] = (QChar)src[i];
	}
	dst.insert(0, temp, length);
	delete temp;
}

void wstringtoqstring(QString &dst, const std::wstring &src)
{
	wcstoqstring(dst, src.c_str(), src.length());
}

void ucstringtoqstring(QString &dst, const ucstring &src)
{
#ifdef TC_NO_UNICODE
	std::wstring wsrc;

	stringtowstring(wsrc, src);
	wstringtoqstring(dst, wsrc);
#else // TC_NO_UNICODE
	wstringtoqstring(dst, src);
#endif // TC_NO_UNICODE
}
