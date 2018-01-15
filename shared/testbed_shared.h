#ifndef __TESTBED_SHARED_H__
#define __TESTBED_SHARED_H__

#define		__TESTBED_MMRWX__			L"MemoryMon"
#define		TESTBED_MMRWX_SYS_FILE		__TESTBED_MMRWX__ \
											L".sys"
// There are symbols for driver
#define		TESTBED_MMRWX_DEVICENAME_DRV	L"\\Device\\dev" __TESTBED_MMRWX__
#define		TESTBED_MMRWX_LINKNAME_DRV 	L"\\DosDevices\\" __TESTBED_MMRWX__

// There are symbols for command line app
#define		TESTBED_MMRWX_LINKNAME_APP 	L"\\\\.\\" __TESTBED_MMRWX__
#define		TESTBED_MMRWX_SERVNAME_APP	__TESTBED_MMRWX__


// Device type in user defined range
#define TESTBED_MMRWX_DEVICE_IOCTL  0x8301

#define TESTBED_MMRWX_VIRT_CPU_ON		(unsigned) CTL_CODE(TESTBED_MMRWX_DEVICE_IOCTL, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)
#define TESTBED_MMRWX_VIRT_CPU_OFF		(unsigned) CTL_CODE(TESTBED_MMRWX_DEVICE_IOCTL, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)


#define TESTBED_MMRWX_SET_A_POLICY		(unsigned) CTL_CODE(TESTBED_MMRWX_DEVICE_IOCTL, 0x820, METHOD_NEITHER, FILE_ANY_ACCESS)

typedef struct _MEMORY_POLICY {
	void*				drvStartAddr;
	unsigned __int64	drvSize;
	void*				allocStartAddr;
	unsigned __int64	allocSize;
	int					is_readable;
	int					is_overwritable;
}MEMORY_POLICY, *PMEMORY_POLICY;

// To provide CONFIDENTIALITY	 set 'is_readable=0'
// To provide	INTEGRITY		 set 'is_overwritable=0'
// 
// Comments:
// 1. The allocator from 'drvStartAddr-drvSize' have all access to 'allocStartAddr-allocSize'
// 2. if 'is_readable = 1' ==>> All other drivers can read this memory 
// 3. if 'is_readable = 0' ==>> All other drivers cannot read this memory 
//    To allow another driver from 'drvStartAddr2-drvSize2' read this memory
//    you have to add similar MEMORY_POLICY
// 4. if 'is_overwritable = 1' ==>> All other drivers can write to this memory 
// 5. if 'is_overwritable = 0' ==>> All other drivers cannot write to this memory 
//    To allow another driver from 'drvStartAddr2-drvSize2' write to this memory
//    you have to add similar MEMORY_POLICY

#define TESTBED_MMRWX_PRINT_POLICIES		(unsigned) CTL_CODE(TESTBED_MMRWX_DEVICE_IOCTL, 0x820+1, METHOD_NEITHER, FILE_ANY_ACCESS)


#endif // __TESTBED_SHARED_H__