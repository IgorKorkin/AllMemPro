
#include "windows.h"
#include "stdio.h"
#include "wchar.h"
#include <tchar.h>
#include <locale.h> // LC_ALL
#include "testbed.h"
#include "stdio.h"

#include <iostream>
#include <string>

using namespace std;

const char name_set_rule[] = "rule";
const int command_set_rule = 200;

const char name_print_policies[] = "print";
const int command_print_policies = 300;

const char name_exit[] = "exit";
const int command_exit = 0xFFF1;

const char name_quit[] = "q";
const int command_quit = 0xFFF2;

void print_hello() {
	cout << endl;
	cout << "<<TestBed console for MemoryMonRWX >>" << endl;
	cout << " '" << name_set_rule << " <DrvStartAddr> <DrvSz> <AllocStartAddr> <AllocSz>' -- protect range for driver " << endl;
	cout << " '" << name_print_policies << "' -- print policies" << endl;
	
	cout << " '" << name_exit << "' -- exit this app " << endl;
}

int input_command() {
	string mystring = { 0 };
	mystring = { 0 };
	cin >> mystring; // std::getline(std::cin >> std::ws, mystring);

	int i_res = 0x12345678;

	if (std::string::npos != mystring.find(name_set_rule)){
		i_res = command_set_rule;
	}
	else if (std::string::npos != mystring.find(name_print_policies)) {
		i_res = command_print_policies;
	}

	else if (std::string::npos != mystring.find(name_quit)) {
		i_res = command_quit;
	}
	else if (std::string::npos != mystring.find(name_exit)) {
		i_res = command_exit;
	}
	return i_res;
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	argc; argv; envp; // to avoid warning C4100
	setlocale(LC_ALL, "");
	setvbuf(stdout, NULL, _IONBF, 0);

	//if (check_windows_support::is_ok()) 
	{
		testbed_for_exploitation :: TestBed my_testbed; // activate testbed
		if (my_testbed.is_ok())
		{
			int code_command = 0;
			MEMORY_POLICY mem_policy = { 0 };
			do {
				print_hello();
				code_command = input_command();
				switch (code_command)
				{
				case command_set_rule:
					RtlZeroMemory(&mem_policy, sizeof MEMORY_POLICY);
					cin >> std::hex >> mem_policy.drvStartAddr;
					cin >> std::hex >> mem_policy.drvSize;
					cin >> std::hex >> mem_policy.allocStartAddr;
					cin >> std::hex >> mem_policy.allocSize;
					my_testbed.set_rule(mem_policy);
					break;
				case command_print_policies:
					my_testbed.print_policies();
					break;
				default: { };
				}
			} while (!((code_command == command_quit) || (code_command == command_exit)) );
		}
	}
	cin.ignore();
	cout << "Press enter to exit." << endl;
	cin.ignore(); // std::system("PAUSE");
}