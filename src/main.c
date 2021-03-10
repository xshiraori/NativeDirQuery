#include <stdio.h>
#include <stdlib.h>
#include "ntdefs.h"


LPWSTR wndNameList[256];
ULONG wndNameCount;

BOOL EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	LPWSTR wndText;
	SIZE_T wndTextLength;
	ULONG pid;

	if (GetWindowThreadProcessId(hwnd, &pid))
	{
		if (pid == *(ULONG*)lParam)
		{
			wndTextLength = GetWindowTextLengthW(hwnd);
			if (wndTextLength > 0 && wndTextLength < 512)
			{
				wndText = (LPWSTR)malloc(wndTextLength * 2 + 1);
				GetWindowTextW(hwnd, wndText, wndTextLength * 2 + 1);
				wndNameList[wndNameCount++] = wndText;
			}
		}
	}
	return TRUE;
}

int main()
{
	ULONG pidExplorer, outSize, inSize = 0;
	HANDLE hExplorer, hDuplicated = NULL;
	HMODULE hNtdll;
	HWND wndShell;

	fpNtQuerySystemInformation NtQuerySystemInformation;
	fpNtQueryObject NtQueryObject;
	fpNtDuplicateObject NtDuplicateObject;

	struct OBJECT_TYPE_INFORMATION* lpObjTypeInfo;
	struct SYSTEM_HANDLE_INFORMATION* lpHandleInfo = NULL;

	wndShell = GetShellWindow();
	GetWindowThreadProcessId(wndShell, &pidExplorer);

	hExplorer = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pidExplorer);

	if (hExplorer == INVALID_HANDLE_VALUE || hExplorer == 0)
	{
		wprintf(L"Error %#x\n", GetLastError());
		return -1;
	}
	
	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&pidExplorer);

	hNtdll = GetModuleHandle(L"ntdll.dll");

	if (hNtdll == NULL)
	{
		wprintf(L"Error %#x\n", GetLastError());
		return -1;
	}

	NtQuerySystemInformation = (fpNtQuerySystemInformation)GetProcAddress(hNtdll, "NtQuerySystemInformation");
	NtQueryObject = (fpNtQueryObject)GetProcAddress(hNtdll, "NtQueryObject");
	NtDuplicateObject = (fpNtDuplicateObject)GetProcAddress(hNtdll, "NtDuplicateObject");

	while (NtQuerySystemInformation(SystemHandleInformation, lpHandleInfo, inSize, &outSize) == STATUS_INFO_LENGTH_MISMATCH)
		lpHandleInfo = (struct SYSTEM_HANDLE_INFORMATION*)malloc(inSize = outSize * 2);

	if (lpHandleInfo == NULL)
	{
		wprintf(L"Error %#x\n", GetLastError());
		return -1;
	}
	
	for (ULONG i = 0; i < lpHandleInfo->NumberOfHandles; i++)
	{
		struct UNICODE_STRING* objName;
		ULONG objNameLen = 0;

		if (lpHandleInfo->Handles[i].UniqueProcessId == pidExplorer)
		{
			HANDLE hOriginal = (HANDLE)lpHandleInfo->Handles[i].HandleValue;

			if (NtDuplicateObject(hExplorer, hOriginal, GetCurrentProcess(), &hDuplicated, 0, 0, 0) == 0)
			{
				lpObjTypeInfo = (struct OBJECT_TYPE_INFORMATION*)malloc(0x1000);

				if (NtQueryObject(hDuplicated, ObjectTypeInformation, lpObjTypeInfo, 0x1000, NULL) == STATUS_SUCCESS)
				{	
					if(lstrcmpiW(lpObjTypeInfo->Name.Buffer, L"File") == 0)
					{
						objName = (struct UNICODE_STRING*)malloc(OBJ_NAME_MAX);

							if (NtQueryObject(hDuplicated, ObjectNameInformation, objName, OBJ_NAME_MAX, &objNameLen) == STATUS_SUCCESS)
							{
								if(objName->Length)
								{
									for (ULONG i = 0; i != wndNameCount; i++)
									{
										if (lstrcmpW(wndNameList[i], L"Window") && wcsstr(objName->Buffer, wndNameList[i])) 
											wprintf(L"%ls\n", objName->Buffer);

									}
								}
							}
							else
							{
								objName = (struct UNICODE_STRING*)malloc(objNameLen);

								if (NtQueryObject(hDuplicated, ObjectNameInformation, objName, objNameLen, NULL) == STATUS_SUCCESS)
								{
									for (ULONG i = 0; i != wndNameCount; i++)
									{
										if (objName->Length && wcsstr(objName->Buffer, wndNameList[i]))
											wprintf(L"%ls\n", objName->Buffer);
									}
								}
							}

						free(objName);
						free(lpObjTypeInfo);

						CloseHandle(hOriginal);
						CloseHandle(hDuplicated);
					}
				}
			}
		}
	}

	return 0;
}
