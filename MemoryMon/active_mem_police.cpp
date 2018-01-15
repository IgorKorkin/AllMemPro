
#include "active_mem_police.h"

void ActiveMemoryPolice::add(const MEMORY_POLICY & mem_police) {
	memory_policies.push_back(mem_police);
}

void ActiveMemoryPolice::print_policies() {
	if (memory_policies.size()){
		DbgPrint("[POLICE] Active Memory Protection [BEGIN] \r\n");
		for (std::vector<MEMORY_POLICY>::iterator it = memory_policies.begin();
			it != memory_policies.end(); it++) {
			DbgPrint(" Driver %I64X-%I64X -->> allocated %I64X-%I64X \r\n",
				it->drvStartAddr,
				it->drvSize,
				it->allocStartAddr,
				it->allocSize);
		}
		DbgPrint("[POLICE] Active Memory Protection [END] \r\n");
	}
	else {
		DbgPrint("[POLICE] Active Memory Protection is empty \r\n");
	}
	
}

bool ActiveMemoryPolice::is_in_range(const _In_ void* src_address, const _In_ void* dst_address) {
	bool b_res = false;
	KeWaitForMutexObject(&MemPolicyMutex, Executive, KernelMode, FALSE, NULL);
	if (memory_policies.size()) {
		// 1. Should we hide this dst_address ? 
		for (std::vector<MEMORY_POLICY>::iterator it = memory_policies.begin(); it != memory_policies.end(); it++) {
			if ((it->allocStartAddr <= dst_address) && (((char*)dst_address <= ((char*)it->allocStartAddr + it->allocSize)))) {
				// 2. Yes. now we check who is trying to access to this memory
				//    Something wrong might happen if this access is unauthorized
				b_res = true;
				if ((it->drvStartAddr <= src_address) && (((char*)src_address <= ((char*)it->drvStartAddr + it->drvSize)))) {
					// 3. Everything is OK. The driver who has allocated this memory tries to access it
					b_res = false;
					break;
				}
			}
		}// for ()

		if (b_res == true) {
			// It means that unauthorized code tries to access the protected data
			DbgPrint(" illegal access %I64X =>> %I64X \r\n", src_address, dst_address);
			static int i = 1;
			if (i) {
				__debugbreak();
			}
		}
	}
	KeReleaseMutex(&MemPolicyMutex, FALSE);
	return b_res;

}