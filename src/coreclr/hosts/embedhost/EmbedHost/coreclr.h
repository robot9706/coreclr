#ifndef __CORECLR_EMBED_H__
#define __CORECLR_EMBED_H__

#define CLR_CALLCONV __stdcall

typedef int (CLR_CALLCONV *coreclr_initialize_ptr)(
    const char* exePath,
    const char* appDomainFriendlyName,
    int propertyCount,
    const char** propertyKeys,
    const char** propertyValues,
    void** hostHandle,
    unsigned int* domainId);

typedef int (CLR_CALLCONV *coreclr_shutdown_ptr)(
    void* hostHandle,
    unsigned int domainId);

typedef int (CLR_CALLCONV *coreclr_execute_assembly_ptr)(
    void* hostHandle,
    unsigned int domainId,
    int argc,
    const char** argv,
    const char* managedAssemblyPath,
    unsigned int* exitCode);

typedef void* (CLR_CALLCONV *coreclr_assembly_load_memory_ptr)(
    void* hostHandle,
    const char* dataPtr,
    int dataLength);

typedef void* (CLR_CALLCONV *coreclr_assembly_load_file_ptr)(
    void* hostHandle,
    void* pathW);

typedef void* (CLR_CALLCONV *coreclr_assembly_get_name_ptr)(
    void* hostHandle,
    void* assembly);

typedef unsigned int (CLR_CALLCONV *coreclr_assembly_exec_main_ptr)(
    void* hostHandle,
    void* assembly);

typedef void* (CLR_CALLCONV *coreclr_assembly_find_method_ptr)(
    void* hostHandle, 
    unsigned int appDomainID,
    void* assembly,
    const char* className,
    const char* methodName);

typedef void (CLR_CALLCONV *coreclr_icall_ptr)(
    void* hostHandle,
    void* iCallTablePtr);

typedef void* (CLR_CALLCONV *coreclr_string_getpointer_ptr)(
    void* hostHandle,
    void* stringPointer);

typedef void (CLR_CALLCONV *coreclr_array_getpointer_ptr)(
    void* hostHandle,
    void* arrayPointer,
    void** targetPointer, 
    unsigned long *elementCount);

typedef void* (CLR_CALLCONV *coreclr_string_new_ptr)(
    void* hostHandle,
    void* dataPointer);

typedef void* (CLR_CALLCONV *coreclr_init_ptr)(
	const char* exePath);

typedef int (CLR_CALLCONV *coreclr_start_ptr)(
	void* hostHandle,
	const char* appDomainFriendlyName,
	int propertyCount, 
	const char** propertyKeys,
	const char** propertyValues,
	unsigned int* domainId);

#endif
