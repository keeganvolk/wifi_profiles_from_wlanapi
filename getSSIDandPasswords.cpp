#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

/*typedef struct _GUID {
	DWORD Data1;
	WORD Data2;
	WORD Data3;
	BYTE Data4[8];
} GUID;*/

typedef struct _WLAN_INTERFACE_INFO {
	GUID InterfaceGuid;
	WCHAR strInterfaceDescription[256];
	DWORD isState;
	//DWORD isState;
} WLAN_INTERFACE_INFO, *PWLAN_INTERFACE_INFO;

typedef struct _WLAN_INTERFACE_INFO_LIST {
	DWORD dwNumberOfItems;
	DWORD dwIndex;
	WLAN_INTERFACE_INFO InterfaceInfo[256];
} WLAN_INTERFACE_INFO_LIST, *PWLAN_INTERFACE_INFO_LIST;

typedef struct _WLAN_PROFILE_INFO {
	WCHAR strProfileName[256];
	DWORD dwFlags;
} WLAN_PROFILE_INFO, *PWLAN_PROFILE_INFO;

typedef struct _WLAN_PROFILE_INFO_LIST {
	DWORD dwNumberOfItems;
	DWORD dwIndex;
	WLAN_PROFILE_INFO ProfileInfo[256];
} WLAN_PROFILE_INFO_LIST, *PWLAN_PROFILE_INFO_LIST;

typedef DWORD (WINAPI *f_WlanOpenHandle)(DWORD, LPVOID, LPDWORD, PHANDLE);
typedef DWORD (WINAPI *f_WlanEnumInterfaces)(HANDLE, LPVOID, LPVOID);
typedef DWORD (WINAPI *f_WlanGetProfile)(HANDLE, GUID*, LPCWSTR, PVOID, LPWSTR*, DWORD*, DWORD*);
typedef DWORD (WINAPI *f_WlanCloseHandle)(HANDLE, PVOID);
typedef DWORD (WINAPI *f_WlanGetProfileList)(HANDLE, GUID*, LPVOID, LPVOID);

int main(int argc, LPCSTR argv[]) {
    if (argc != 2) {std::cout << "Error: Incorrect number of arcuments; should be 1 (path to WlanAPI DLL)"; return -1;}

    std::cout << "[=] Loading WlanAPI DLL...";
	HMODULE hWlanApi = LoadLibraryA(argv[1]);
    if (!hWlanApi) {
        std::cout << "\r[-] Couldn't load/find WlanAPI DLL, exiting..." << std::endl;
        return -1;
    }
    std::cout << "\r[+] Successfully loaded WlanAPI DLL\n[=] Locating functions...";

    f_WlanOpenHandle WlanOpenHandle = (f_WlanOpenHandle)GetProcAddress(hWlanApi, "WlanOpenHandle");
    if (!WlanOpenHandle) {std::cout << "\r[-] Couldn't locate WlanOpenHandle, exiting..." << std::endl;return -1;}
    f_WlanEnumInterfaces WlanEnumInterfaces = (f_WlanEnumInterfaces)GetProcAddress(hWlanApi, "WlanEnumInterfaces");
    if (!WlanEnumInterfaces) {std::cout << "\r[-] Couldn't locate WlanEnumInterfaces, exiting..." << std::endl;return -1;}
    f_WlanGetProfile WlanGetProfile = (f_WlanGetProfile)GetProcAddress(hWlanApi, "WlanGetProfile");
    if (!WlanGetProfile) {std::cout << "\r[-] Couldn't locate WlanGetProfile, exiting..." << std::endl;return -1;}
    f_WlanCloseHandle WlanCloseHandle = (f_WlanCloseHandle)GetProcAddress(hWlanApi, "WlanCloseHandle");
    if (!WlanCloseHandle) {std::cout << "\r[-] Couldn't locate WlanCloseHandle, exiting..." << std::endl;return -1;}
    f_WlanGetProfileList WlanGetProfileList = (f_WlanGetProfileList)GetProcAddress(hWlanApi, "WlanGetProfileList");
    if (!WlanGetProfileList) {std::cout << "\r[-] Couldn't locate WlanGetProfileList, exiting..." << std::endl;return -1;}
    std::cout << "\r[+] Located WlanAPI functions" << std::endl;

    HANDLE hClientHandle = NULL;
    DWORD dwVersion = 0;
    DWORD dwResult = WlanOpenHandle(2, NULL, &dwVersion, &hClientHandle);

    WLAN_INTERFACE_INFO_LIST *pIfList = NULL;
    dwResult = WlanEnumInterfaces(hClientHandle, NULL, (LPVOID*)&pIfList);
    for (DWORD i=0; i<pIfList->dwNumberOfItems; i++) {
        GUID pIfGuid = pIfList->InterfaceInfo[i].InterfaceGuid;
        std::wcout << L"[=] Interface: " << pIfList->InterfaceInfo[i].strInterfaceDescription << std::endl;

        WLAN_PROFILE_INFO_LIST *pPfList = NULL;
        dwResult = WlanGetProfileList(hClientHandle, &pIfGuid, NULL, &pPfList);
        std::wcout << "[=] " << pPfList->dwNumberOfItems << " Profiles:" << std::endl;
        for (DWORD j=0; j<pPfList->dwNumberOfItems; j++) {
            LPWSTR pstrProfileXml = NULL;
            DWORD dwFlags = 0x0C;
            DWORD dwGrantedAccess = 0;

            std::wcout << L"Profile: " << pPfList->ProfileInfo[j].strProfileName << std::endl;
            dwResult = WlanGetProfile(hClientHandle, &pIfList->InterfaceInfo[i].InterfaceGuid, pPfList->ProfileInfo[j].strProfileName, NULL, &pstrProfileXml, &dwFlags, &dwGrantedAccess);
            if (dwResult == 0) {
                std::wcout << L"[=] Profile XML: " << pstrProfileXml << std::endl;
            }
        }
    }

    FreeLibrary(hWlanApi);
    WlanCloseHandle(hClientHandle, NULL);

	return 0;
}