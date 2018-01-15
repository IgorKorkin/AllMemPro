#include "testbed.h"

namespace check_windows_support {

	bool is_ok() {
		OSVERSIONINFOEX os_info_needed = { 0 };
		set_minimal_os_info(os_info_needed);

		DWORDLONG dwlConditionMask = 0;
		VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, VER_EQUAL);

		auto b_res = (0 != VerifyVersionInfo(&os_info_needed,
			VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER |
			VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR, dwlConditionMask));

		if (b_res) {
			cout << "This OS is supported!" << endl;
		}
		else if (ERROR_OLD_WIN_VERSION == GetLastError()) {
			OSVERSIONINFOEX os_version = { 0 };
			RtlZeroMemory(&os_version, sizeof OSVERSIONINFOEX);
			os_version.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			if (GetVersionEx((LPOSVERSIONINFOW)&os_version)) {
				cout << "This OS is not supported" << endl;
				print_windows_info("Current OS details:", os_version);
				print_windows_info("The minimal OS requirements: Built by: 15063.0.amd64fre.rs2_release.170317-1834", os_info_needed);
			}
		}
		return b_res;
	}

	void set_minimal_os_info(OSVERSIONINFOEX & os_info) {
		/*
		Windows 7 Kernel Version 15063 MP (4 procs) Free x64
		Product: WinNt, suite: TerminalServer SingleUserTS
		Built by: 15063.0.amd64fre.rs2_release.170317-1834
		*/
		ZeroMemory(&os_info, sizeof(OSVERSIONINFOEX));
		os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		os_info.dwMajorVersion = 6;
		os_info.dwMinorVersion = 2;
		os_info.dwBuildNumber = 9200; // or 0x23f0
		os_info.wServicePackMajor = 0;
		os_info.wServicePackMinor = 0;
	}

	void print_windows_info(const char* title, const OSVERSIONINFOEX & os_version) {
		cout << title << endl;
		cout << " MajorVersion : " << std::dec << os_version.dwMajorVersion << std::hex << " (0x" << os_version.dwMajorVersion << ")" << endl;
		cout << " MinorVersion : " << std::dec << os_version.dwMinorVersion << std::hex << " (0x" << os_version.dwMinorVersion << ")" << endl;
		cout << " BuildNumber : " << std::dec << os_version.dwBuildNumber << std::hex << " (0x" << os_version.dwBuildNumber << ")" << endl;
		cout << " ServicePackMajor : " << std::dec << os_version.wServicePackMajor << std::hex << " (0x" << os_version.wServicePackMajor << ")" << endl;
		cout << " ServicePackMinor : " << std::dec << os_version.wServicePackMinor << std::hex << " (0x" << os_version.wServicePackMinor << ")" << endl;
	}
} // namespace check_windows_support


namespace resource_functions
{
	bool extract(RESOURCE & resourse, LPCTSTR lpName, LPCTSTR lpType) {
		HRSRC   res_handle = NULL;
		if ((NULL != (res_handle = FindResource(NULL, lpName, lpType))) &&
			(0 != (resourse.data_sz = SizeofResource(NULL, res_handle)))) {
			HGLOBAL data_handle = NULL;
			if (NULL != (data_handle = LoadResource(NULL, res_handle))) {
			resourse.data = LockResource(data_handle);
			}
		}
		return (NULL != resourse.data);
	}

	bool set_tmp_file_path(LPCTSTR lpPrefixString, LPTSTR lpTempFileName) {
		auto b_res = false;
		const DWORD uUnique = 0;
		wchar_t tmp_path[MAX_PATH] = { 0 };
		if (GetTempPath(MAX_PATH, tmp_path) &&
			GetTempFileName(tmp_path, lpPrefixString, uUnique, lpTempFileName)){
			DeleteFile(lpTempFileName); // If uUnique is zero, GetTempFileName creates an empty file and closes it.
			b_res = ( 0 == wcscat_s(lpTempFileName, MAX_PATH, TEXT(".sys"))); 
			// lpTempFileName = <path>\<pre><uuuu>.TMP.sys
		}
		return b_res;
	}

	bool set_tmp_file(LPTSTR lpTempFileName) {
		auto b_res = false;
		wchar_t tmp_path[MAX_PATH] = { 0 };
		if (GetTempPath(MAX_PATH, tmp_path)) {
			lpTempFileName[0] = 0;
			b_res =
				((0 == wcscat_s(lpTempFileName, MAX_PATH, tmp_path)) &&
					(0 == wcscat_s(lpTempFileName, MAX_PATH, TESTBED_MMRWX_SYS_FILE)));
			// lpTempFileName = <path>\lpTempFileName.sys
		}
		return b_res;
	}

} // namespace resource_functions

namespace testbed_for_exploitation {

	bool TestBed::extract_driver_file(TCHAR * binFile) {
		auto b_res = false;
		resource_functions::RESOURCE my_res = { 0 };
		RtlSecureZeroMemory(&my_res, sizeof(resource_functions::RESOURCE));
		if (resource_functions::extract(my_res, MAKEINTRESOURCE(TESTBED_BIN_SYSFILE), TEXT("bin")) &&
			resource_functions::set_tmp_file(binFile)) {
			HANDLE h_file = NULL;
			if ((HANDLE)INVALID_HANDLE_VALUE != (h_file = CreateFile(
				binFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL))) {
				DWORD number_of_bytes = 0;
				if (WriteFile(h_file, my_res.data, my_res.data_sz, &number_of_bytes, NULL) &&
					(number_of_bytes == my_res.data_sz)) {
					b_res = true;
				}
				CloseHandle(h_file);
			}
			else {
				print::print_last_err(L"err file %s ", binFile);
			}
		}
		return b_res;
	}

	bool TestBed::load_driver_from_file(TCHAR * binFile) {
		auto b_res = false;
		service_manager.set_names(TESTBED_MMRWX_SERVNAME_APP, binFile);
		if (service_manager.add_driver()) {
			if (service_manager.start_driver()) {
				if (INVALID_HANDLE_VALUE != service_manager.open_device(TESTBED_MMRWX_LINKNAME_APP)) {
					b_res = true;
					print::print_mes(L"The [%s] driver is active!", TESTBED_MMRWX_SERVNAME_APP);
					//	*for debugging process:
					// 	service_manager.close_device(m_hNeither);
					// 	service_manager.stop_driver();
					// 	service_manager.remove_driver();
				}
				else {
					print::print_last_err(L"err open device %s ", TESTBED_MMRWX_LINKNAME_APP);
				}
			}
			else {
				print::print_last_err(L"err start driver %s %s", TESTBED_MMRWX_SERVNAME_APP, binFile);
				service_manager.stop_driver();
				service_manager.remove_driver();
			}
		}
		else {
			print::print_last_err(L"err add driver %s %s", TESTBED_MMRWX_SERVNAME_APP, binFile);
			service_manager.stop_driver();
			service_manager.remove_driver();
		}
		service_manager.delete_binfile();
		return b_res;
	}

	bool TestBed::activate_testbed() {
		TCHAR bin_file[MAX_PATH] = { 0 };
		disable_compatibility_window :: disable();
		auto const b_res = extract_driver_file(bin_file) && load_driver_from_file(bin_file);
		disable_compatibility_window :: restore();
		DeleteFile(bin_file);
		return b_res;
	}

	//////////////////////////////////////////////////////////////////////////

	/*  */
	bool TestBed::set_rule(MEMORY_POLICY & memPolicy) {

		auto b_res = 
			service_manager.send_ctrl_code(TESTBED_MMRWX_SET_A_POLICY, (LPVOID)&memPolicy, sizeof MEMORY_POLICY, NULL, 0, 0);
		
		if (b_res){
			cout << "the rule has been added" << endl;
		}
		return b_res;
	}

	/*  */
	bool TestBed::print_policies() {

		auto b_res =
			service_manager.send_ctrl_code(TESTBED_MMRWX_PRINT_POLICIES, NULL, 0, NULL, 0, 0);

		return b_res;
	}

} // namespace testbed 