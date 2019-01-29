#include "coreclr.h"
#include "fcall.h"

#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

//Path stuff
static wchar_t *CLRDirectory = [Path to the output folder ex: L"C:\\...\\coreclr\\bin\\Product\\Windows_NT.x86.Debug"];
static wchar_t *ManagedDLLPath = [Path to the managed dll L"C:\\...\\coreclr\\src\\coreclr\\hosts\\embedhost\\ManagedLibrary\\bin\\Debug\\netcoreapp2.2\\ManagedLibrary.dll";

//CoreCLR functions
static coreclr_initialize_ptr _clrInit;
static coreclr_shutdown_ptr _clrShutdown;
static coreclr_assembly_load_file_ptr _clrAssemblyLoad;
static coreclr_assembly_find_method_ptr _clrAssemblyFindMethod;
static coreclr_icall_ptr _clrSetICallTable;
static coreclr_string_new_ptr _clrNewString;

//C/C++ to Managed call
typedef void(_stdcall *TestMethod_ptr)();

//CLR tools
string getTPAList()
{
    string tpaList;

    wchar_t *tpaExtensions[] = {
        L"*.dll",
        L"*.exe",
        L"*.winmd"
    };

    for (int i = 0; i < _countof(tpaExtensions); i++)
    {
        wchar_t searchPath[MAX_PATH];
        wcscpy_s(searchPath, MAX_PATH, CLRDirectory);
        wcscat_s(searchPath, MAX_PATH, L"\\");
        wcscat_s(searchPath, MAX_PATH, tpaExtensions[i]);

        WIN32_FIND_DATAW findData;
        HANDLE fileHandle = FindFirstFileW(searchPath, &findData);

        if (fileHandle != INVALID_HANDLE_VALUE)
        {
            do
            {
                wchar_t pathToAdd[MAX_PATH];
                wcscpy_s(pathToAdd, MAX_PATH, CLRDirectory);
                wcscat_s(pathToAdd, MAX_PATH, L"\\");
                wcscat_s(pathToAdd, MAX_PATH, findData.cFileName);

                wstring pathWide(pathToAdd);
                string pathShort(pathWide.begin(), pathWide.end());

                tpaList.append(pathShort);
                tpaList.append(";");

            } while (FindNextFileW(fileHandle, &findData));
            FindClose(fileHandle);
        }
    }

    return tpaList;
}

//CLR Init
static void* _clrHost = nullptr;
static unsigned int _clrDomainID;

#define CHECK_PTR(name) if((name) == nullptr) return false;

static bool initCLR()
{
    wchar_t clrDll[MAX_PATH];
    wcscpy_s(clrDll, MAX_PATH, CLRDirectory);
    wcscat_s(clrDll, MAX_PATH, L"\\coreclr.dll");

    //Load the CLR module
    HMODULE coreclr = LoadLibraryExW(clrDll, NULL, 0);

    //Find functions
    CHECK_PTR(_clrInit = (coreclr_initialize_ptr)GetProcAddress(coreclr, "coreclr_initialize"));
    CHECK_PTR(_clrShutdown = (coreclr_shutdown_ptr)GetProcAddress(coreclr, "coreclr_shutdown"));
    CHECK_PTR(_clrAssemblyLoad = (coreclr_assembly_load_file_ptr)GetProcAddress(coreclr, "coreclr_assembly_load_file"));
    CHECK_PTR(_clrAssemblyFindMethod = (coreclr_assembly_find_method_ptr)GetProcAddress(coreclr, "coreclr_assembly_find_method"));
    CHECK_PTR(_clrSetICallTable = (coreclr_icall_ptr)GetProcAddress(coreclr, "coreclr_icall_ptr"));
    CHECK_PTR(_clrNewString = (coreclr_string_new_ptr)GetProcAddress(coreclr, "coreclr_string_new"));

    //Create a CLR host
    string tpaList = getTPAList();

    const char *propertyKeys[] = {
        "TRUSTED_PLATFORM_ASSEMBLIES"
    };
    const char *propertyValues[] = {
        tpaList.c_str()
    };

    if (_clrInit(nullptr, "AppDomain", sizeof(propertyKeys) / sizeof(propertyKeys[0]), propertyKeys, propertyValues, &_clrHost, &_clrDomainID) != 0)
    {
        return false;
    }

    return true;
}

static void shutdownCLR()
{
    if (_clrHost == nullptr)
        return;

    _clrShutdown(_clrHost, _clrDomainID);
}

//ICalls
static DWORD F_CALL_CONV ICall_GetExternalSum(DWORD a, DWORD b) //The calling convention is a MUST, don't forget to setup the compiler defines too!
{
    return (a + b);
}

FCallData _fcallTableSecond = {
    "ManagedLibrary", //Namespace
    "TestClass", //Class
    "GetExternalSum", //Method
    (void*)&ICall_GetExternalSum, //Implementation
    nullptr, //Reserved = nullptr
    nullptr //End of linked list
};

static wchar_t *_testString = L"Hello from native!";

static void* ICall_GetExternalString()
{
    return _clrNewString(_clrHost, _testString); //Strings can only be made in CoreCLR threads (ICalls are running in CoreCLR threads)
}

FCallData _fcallTableFirst = {
    "ManagedLibrary", //Namespace
    "TestClass", //Class
    "GetExternalString", //Method
    (void*)&ICall_GetExternalString, //Implementation
    nullptr, //Reserved = nullptr
    &_fcallTableSecond //Next in the linked list
};

static void registerICallTable()
{
    _clrSetICallTable(_clrHost, (void*)&_fcallTableFirst);
}

//Test
static void doTest()
{
    void* testAssembly = _clrAssemblyLoad(_clrHost, (void*)ManagedDLLPath);

    TestMethod_ptr methodPtr = (TestMethod_ptr)_clrAssemblyFindMethod(_clrHost, _clrDomainID, testAssembly, "ManagedLibrary.TestClass", "TestMethod");

    methodPtr(); //Call the test method
}

//Main
int main()
{
    if (!initCLR())
    {
        cout << "Failed to init CoreCLR." << endl;
        cin.get();

        return -1;
    }

    registerICallTable();

    doTest();

    shutdownCLR();

    cout << "Press enter to exit." << endl;
    cin.get();

    return 0;
}
