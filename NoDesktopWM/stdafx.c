#include "stdafx.h"

void XTraceW(INT flags, LPCSTR file, LPCSTR function, INT line, LPCWSTR message, ...) {
	WCHAR buffer[2048] = { 0, };
	WCHAR msg[1500] = { 0, };
	WCHAR msg2[1400] = { 0, };
	DWORD messageLen = 0;

	va_list args;
	va_start(args, message);

	if (!message) {
		return;
	}

	messageLen = (DWORD)wcslen(message);
	if (messageLen > 1398) {
		memcpy_s(msg2, 1400 * sizeof(WCHAR), message, 1398 * sizeof(WCHAR));
	}
	else {
		memcpy_s(msg2, 1400 * sizeof(WCHAR), message, messageLen * sizeof(WCHAR));
		if (flags & EDM_LOG_USEDEBUGVIEW && msg2[messageLen - 1] == L'\n') {
			msg2[messageLen - 1] = L'\0';
		}
	}

	vswprintf_s(msg, 1500, msg2, args);
	if (flags & EDM_LOG_INCLUDEFILEPATH) {
		swprintf_s(buffer, 2048, L"[NoDesktopWM] %ls [%S : %d]\n", msg, function, line);
	}
	else {
		swprintf_s(buffer, 2048, L"[NoDesktopWM] %ls [%S : %d]\n", msg, function, line);
	}

	if (flags & EDM_LOG_USEDEBUGVIEW)
		OutputDebugStringW(buffer);
	else {
		wprintf(buffer);
	}
	va_end(args);
}