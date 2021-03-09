#pragma once
#include <Windows.h>

#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004
#define STATUS_SUCCESS 0

#define OBJ_NAME_MAX 0x1000

typedef enum _OBJECT_INFORMATION_CLASS {
	ObjectNameInformation = 0x1,
	ObjectTypeInformation = 0x2
}ObjectInformationClass;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemHandleInformation = 0x10
}SystemInformationClass;


typedef NTSTATUS(NTAPI* fpNtQuerySystemInformation)(SystemInformationClass SystemInformationClass,
	OUT PVOID                   SystemInformation,
	IN ULONG                    SystemInformationLength,
	OUT PULONG                  ReturnLength);

typedef NTSTATUS(NTAPI* fpNtDuplicateObject)(
	HANDLE SourceProcessHandle,
	HANDLE SourceHandle,
	HANDLE TargetProcessHandle,
	PHANDLE TargetHandle,
	ACCESS_MASK DesiredAccess,
	ULONG Attributes,
	ULONG Options
	);
typedef NTSTATUS(NTAPI* fpNtQueryObject)(
	HANDLE ObjectHandle,
	ObjectInformationClass ObjectInformationClass,
	PVOID ObjectInformation,
	ULONG ObjectInformationLength,
	PULONG ReturnLength
	);

typedef struct UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
}UNICODE_STRING, *PUNICODE_STRING;

typedef enum _POOL_TYPE {
	NonPagedPool,
	PagedPool,
	NonPagedPoolMustSucceed,
	DontUseThisType,
	NonPagedPoolCacheAligned,
	PagedPoolCacheAligned,
	NonPagedPoolCacheAlignedMustS
} POOL_TYPE, * PPOOL_TYPE;

struct OBJECT_TYPE_INFORMATION {
	UNICODE_STRING Name;
	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
	ULONG TotalPagedPoolUsage;
	ULONG TotalNonPagedPoolUsage;
	ULONG TotalNamePoolUsage;
	ULONG TotalHandleTableUsage;
	ULONG HighWaterNumberOfObjects;
	ULONG HighWaterNumberOfHandles;
	ULONG HighWaterPagedPoolUsage;
	ULONG HighWaterNonPagedPoolUsage;
	ULONG HighWaterNamePoolUsage;
	ULONG HighWaterHandleTableUsage;
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccess;
	BOOLEAN SecurityRequired;
	BOOLEAN MaintainHandleCount;
	USHORT MaintainTypeList;
	POOL_TYPE PoolType;
	ULONG PagedPoolUsage;
	ULONG NonPagedPoolUsage;
};

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
	USHORT 	UniqueProcessId;
	USHORT 	CreatorBackTraceIndex;
	UCHAR 	ObjectTypeIndex;
	UCHAR 	HandleAttributes;
	USHORT 	HandleValue;
	PVOID 	Object;
	ULONG 	GrantedAccess;
}SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

struct SYSTEM_HANDLE_INFORMATION
{
	ULONG 	NumberOfHandles;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO 	Handles[1];
};
