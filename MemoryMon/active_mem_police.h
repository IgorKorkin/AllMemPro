/************************************************************************/
/*                                                                      */
/************************************************************************/


#ifndef ACTIVE_MEM_POLICE_H_
#define ACTIVE_MEM_POLICE_H_

#include <fltKernel.h>
#undef _HAS_EXCEPTIONS
#define _HAS_EXCEPTIONS 0
#include <vector>
#include "..\shared\testbed_shared.h"

class ActiveMemoryPolice {

public:
	/*  */
	ActiveMemoryPolice::ActiveMemoryPolice() {
		KeInitializeMutex(&MemPolicyMutex, NULL);
	}

	/*  */
	void add(const MEMORY_POLICY & mem_police);

	/*  */
	void print_policies();
	
	/*  */
	bool is_in_range(const _In_ void* src_address, const _In_ void* dst_address);

private:
	KMUTEX MemPolicyMutex = { 0 };
	std::vector<MEMORY_POLICY> memory_policies;
};




#endif // ACTIVE_MEM_POLICE_H_