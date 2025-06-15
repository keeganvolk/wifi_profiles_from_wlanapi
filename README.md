# wifi_profiles_from_wlanapi
This application allows you to retrieve saved Wifi profile information, including cleartext passkeys by loading the wlanapi.dll library.

Profile information is output in XML format.

To use: C:\path\to\wifi_profiles_from_wlanapi.exe C:\path\to\wlanapi.dll

wlanapi.dll is located in C:\Windows\System32 and C:\Windows\SysWOW64, or you can try to copy/upload wlanapi.dll to another location if endpoint security is extremely strict. This is a pretty niche case though.

To do: Add argument flag to output XML to a file
