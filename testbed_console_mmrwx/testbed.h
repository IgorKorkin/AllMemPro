#ifndef __TESTBED_CONSOLE_H__
#define __TESTBED_CONSOLE_H__

#include "windows.h"
#include "windef.h"
#include "stdlib.h"
#include "stdio.h" // sscanf_s, _snscanf_s
#include "Ntsecapi.h" // UNICODE_STRING
#include "resource.h"
#include "Shlwapi.h" // PathFileExists
#pragma comment(lib,"shlwapi.lib") // PathFileExists

#include "print_messages.h"
#include "service_functions.h" // ServiceManager
#include "..\shared\testbed_shared.h" // strings defines

#include "disable_compatibility_window.h" // Disable Program Compatibility Assistant 

#include <iostream>
using namespace std;

namespace check_windows_support {

	bool is_ok();

	void set_minimal_os_info(OSVERSIONINFOEX & os_info);

	void print_windows_info(const char* title, const OSVERSIONINFOEX & os_version);
}

namespace resource_functions {

	typedef struct _RESOURCE
	{
		DWORD   data_sz;
		LPVOID  data;
	}RESOURCE, *PRESOURCE;

	bool extract(RESOURCE & resourse, LPCTSTR lpName, LPCTSTR lpType);

	bool set_tmp_file(LPTSTR lpTempFileName);

	bool set_tmp_file_path(LPCTSTR lpPrefixString, LPTSTR lpTempFileName);
}

namespace testbed_for_exploitation{

	class TestBed{

	public:
		bool is_ok() {
			return activate_testbed();
		}

		/*  */
		bool set_rule(MEMORY_POLICY & memPolicy);

		/*  */
		bool print_policies();

	private:
		service_functions::ServiceManager service_manager;
		
		/* extract and load driver, deactivate PCA*/
		bool activate_testbed();

		/* extract driver via resource_functions::RESOURCE */
		bool extract_driver_file(TCHAR * binFile);

		/* load driver via service_manager */
		bool load_driver_from_file(TCHAR * binFile);
	};
}

extern "C"
{
	NTSYSAPI BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(
		__in PCWSTR DosFileName,
		__out UNICODE_STRING *NtFileName,
		__out_opt PWSTR *FilePart,
		__out_opt PVOID RelativeName
		);

#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_AMBIGUOUS   (0x00000001)
#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_UNC         (0x00000002)
#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_DRIVE       (0x00000003)
#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_ALREADY_DOS (0x00000004)

	typedef struct _RTL_BUFFER {
		PWCHAR        Buffer;
		PWCHAR        StaticBuffer;
		SIZE_T    Size;
		SIZE_T    StaticSize;
		SIZE_T    ReservedForAllocatedSize; // for future doubling
		PVOID     ReservedForIMalloc; // for future pluggable growth
	} RTL_BUFFER, *PRTL_BUFFER;

	typedef struct _RTL_UNICODE_STRING_BUFFER {
		UNICODE_STRING String;
		RTL_BUFFER     ByteBuffer;
		UCHAR          MinimumStaticBufferForTerminalNul[sizeof(WCHAR)];
	} RTL_UNICODE_STRING_BUFFER, *PRTL_UNICODE_STRING_BUFFER;

	NTSYSAPI NTSTATUS NTAPI RtlNtPathNameToDosPathName(
			__in ULONG Flags,
			__inout PRTL_UNICODE_STRING_BUFFER Path,
			__out_opt PULONG Disposition,
			__inout_opt PWSTR* FilePart
			);

};
#endif // __TESTBED_CONSOLE_H__