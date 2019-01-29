#ifndef __FCALL_CONV_H__
#define __FCALL_CONV_H__

// Calling conventions for an FCall

#pragma region FCALL HEADER
// Calling convention for varargs
#define F_CALL_VA_CONV __cdecl


#ifdef _TARGET_X86_

// Choose the appropriate calling convention for FCALL helpers on the basis of the JIT calling convention
#ifdef __GNUC__
#define F_CALL_CONV __attribute__((cdecl, regparm(3)))

// GCC fastcall convention (simulated via stdcall) is different from MSVC fastcall convention. GCC can use up
// to 3 registers to store parameters. The registers used are EAX, EDX, ECX. Dummy parameters and reordering
// of the actual parameters in the FCALL signature is used to make the calling convention to look like in MSVC.
#define SWIZZLE_REGARG_ORDER
#else // __GNUC__
#define F_CALL_CONV __fastcall
#endif // !__GNUC__

#define SWIZZLE_STKARG_ORDER
#else // _TARGET_X86_

//
// non-x86 platforms don't have messed-up calling convention swizzling
//
#define F_CALL_CONV
#endif // !_TARGET_X86_

struct FCallData //Structure (a linked list entry) that holds an entry of an InternalCall
{
    const char* methodNamespce;
    const char* methodClassname;
    const char* methodName;

    void* impl;
    void* internalReserved; //NULL

    FCallData* next;
};

#endif //__FCALL_CONV_H__
