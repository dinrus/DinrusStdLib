
#include "SysInfo_in.h"

#if defined(TARGET_WIN32)
#include <iphlpapi.h>
#endif

#define IN6ADDRSZ   16
#define INT16SZ      2

#include <stdint.h>


#if defined (TARGET_POSIX)
#ifndef flagGUI
    i32 SysInfoX11ErrorHandler(_XDisplay *, XErrorEvent *)  {return 0;}
    void SetSysInfoX11ErrorHandler()                        {XSetErrorHandler(SysInfoX11ErrorHandler);}
#endif
#endif

namespace drx {

#define TFILE <drx/SysInfo/SysInfo.t>
#include <drx/Core/t.h>

#ifdef COMPILER_MSC
#pragma comment(lib, "ws2_32.lib")
#endif

/////////////////////////////////////////////////////////////////////
// Hardware Info
#if defined(TARGET_WIN32)

#ifdef COMPILER_MINGW
REFCLSID ___CLSID_WbemAdministrativeLocator =
   { 0xcb8555cc, 0x9128, 0x11d1, {0xad, 0x9b, 0x00, 0xc0, 0x4f, 0xd8, 0xfd, 0xff}};
#else
#define ___CLSID_WbemAdministrativeLocator CLSID_WbemAdministrativeLocator
#endif

bool GetWMIInfo(Txt system, Vec <Txt> &data, Array <Val> *ret[], Txt nameSpace = "root\\cimv2") {
    HRESULT hRes;

    hRes = CoInitialize(NULL);
    if (hRes != S_OK && hRes != S_FALSE)
        return false;

    hRes = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT,
            RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0);
    if (hRes != S_OK && hRes != RPC_E_TOO_LATE) {
        CoUninitialize();
        return false;
    }
    IWbemLocator* pIWbemLocator = NULL;
    if (CoCreateInstance(___CLSID_WbemAdministrativeLocator, NULL,
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown, reinterpret_cast<uk *>(&pIWbemLocator)) != S_OK) {
        CoUninitialize();
        return false;
    }

    BSTR bstrNamespace;
    if (!BSTRSet(nameSpace, bstrNamespace)) {
        CoUninitialize();
        return false;
    }
    IWbemServices* pWbemServices = NULL;
    if (pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, NULL, 0, NULL, NULL,
        &pWbemServices) != S_OK) {
        CoUninitialize();
        return false;
    }
    SysFreeTxt(bstrNamespace);

    hRes = CoSetProxyBlanket(pWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
            RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (FAILED(hRes)) {
        pWbemServices->Release();
        pIWbemLocator->Release();
        CoUninitialize();
        return false;
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    Txt query;
    query << "Select * from " << system;

    BSTR bstrQuery, bstrQL;
    if (!BSTRSet(query, bstrQuery) || !BSTRSet("WQL", bstrQL)) {
        pWbemServices->Release();
        pIWbemLocator->Release();
        CoUninitialize();
        return false;
    }
    hRes = pWbemServices->ExecQuery(bstrQL, bstrQuery, WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
    if (hRes != S_OK) {
        pWbemServices->Release();
        pIWbemLocator->Release();
        CoUninitialize();
        return false;
    }
    SysFreeTxt(bstrQuery);
    SysFreeTxt(bstrQL);

    IWbemClassObject *pClassObject;
    ULONG uReturn = 0;
    i32 row = 0;
    bool rt = false;
    while (pEnumerator) {
        hRes = pEnumerator->Next(LONG(WBEM_INFINITE), 1, &pClassObject, &uReturn);
        if(0 == uReturn) {
            if (rt)
                break;
            else {
                pIWbemLocator->Release();
                pWbemServices->Release();
                pEnumerator->Release();
                CoUninitialize();
                return false;
            }
        }
        if(hRes != S_OK) {
            pWbemServices->Release();
            pIWbemLocator->Release();
            pEnumerator->Release();
            pClassObject->Release();
            CoUninitialize();
            return false;
        }
        for (i32 col = 0; col < data.GetCount(); ++col) {
            VARIANT vProp;
            VariantInit(&vProp);
            BSTR bstrClassProp;
            if (!BSTRSet(data[col], bstrClassProp)) {
                pWbemServices->Release();
                pIWbemLocator->Release();
                pEnumerator->Release();
                pClassObject->Release();
                CoUninitialize();
                return false;
            }
            hRes = pClassObject->Get(bstrClassProp, 0, &vProp, 0, 0);
            if(hRes != S_OK){
                pWbemServices->Release();
                pIWbemLocator->Release();
                pEnumerator->Release();
                pClassObject->Release();
                CoUninitialize();
                return false;
            }
            SysFreeTxt(bstrClassProp);
            ret[col]->Add(GetVARIANT(vProp));
            VariantClear(&vProp);
            rt = true;
        }
        row++;
    }
    pIWbemLocator->Release();
    pWbemServices->Release();
    pEnumerator->Release();
    pClassObject->Release();
    CoUninitialize();

    return true;
}

bool GetWMIInfo(Txt system, Txt data, Val &res, Txt nameSpace = "root\\cimv2") {
    Array <Val> arrRes;
    Array <Val> *arrResP[1];
    arrResP[0] = &arrRes;
    Vec <Txt> arrData;
    arrData.Add(data);
    bool ret = GetWMIInfo(system, arrData, arrResP, nameSpace);
    if (ret)
        res = arrRes[0];
    return ret;
}

void GetSystemInfo(Txt &manufacturer, Txt &productName, Txt &version, i32 &numberOfProcessors, Txt &mbSerial) {
    manufacturer = "";
    Val vmanufacturer;
    if (GetWMIInfo("Win32_ComputerSystem", "manufacturer", vmanufacturer))
        manufacturer = Trim(vmanufacturer.ToTxt());
    if (manufacturer.IsEmpty())
        manufacturer = FromSystemCharset(GetWinRegTxt("SystemManufacturer", "HARDWARE\\DESCRIPTION\\System\\BIOS", HKEY_LOCAL_MACHINE));
    if (manufacturer.IsEmpty()) {
        TxtParse fileData = LoadFile(AppendFNameX(GetSystemFolder(), "oeminfo.ini"));
        fileData.GoAfter("Manufacturer=");
        manufacturer = fileData.GetText("\r\n");
    }
    productName = "";
    Val vproductName;
    if (GetWMIInfo("Win32_ComputerSystem", "model", vproductName))
        productName = Trim(vproductName.ToTxt());
    if (productName.IsEmpty())
        productName = FromSystemCharset(GetWinRegTxt("SystemProductName", "HARDWARE\\DESCRIPTION\\System\\BIOS", HKEY_LOCAL_MACHINE));
    if (productName.IsEmpty())
        productName = FromSystemCharset(GetWinRegTxt("Model", "SOFTWARE\\Microsoft\\PCHealth\\HelpSvc\\OEMInfo", HKEY_LOCAL_MACHINE));

    version = FromSystemCharset(GetWinRegTxt("SystemVersion", "HARDWARE\\DESCRIPTION\\System\\BIOS", HKEY_LOCAL_MACHINE));
    numberOfProcessors = atoi(GetEnv("NUMBER_OF_PROCESSORS"));
    Val vmbSerial;
    if (GetWMIInfo("Win32_BaseBoard", "SerialNumber", vmbSerial))
        mbSerial = Trim(vmbSerial.ToTxt());
}

void GetBiosInfo(Txt &biosVersion, Date &biosReleaseDate, Txt &biosSerial) {
    // Alternative is "wmic bios get name" and "wmic bios get releasedate"
    Txt strBios = FromSystemCharset(GetWinRegTxt("BIOSVersion", "HARDWARE\\DESCRIPTION\\System\\BIOS", HKEY_LOCAL_MACHINE));
    if (strBios.IsEmpty())
        strBios = FromSystemCharset(GetWinRegTxt("SystemBiosVersion", "HARDWARE\\DESCRIPTION\\System", HKEY_LOCAL_MACHINE));
    for (i32 i = 0; i < strBios.GetLength(); ++i) {
        if (strBios[i] == '\0')
            biosVersion.Cat(". ");
        else
            biosVersion.Cat(strBios[i]);
    }
    Txt strDate = FromSystemCharset(GetWinRegTxt("BIOSReleaseDate", "HARDWARE\\DESCRIPTION\\System\\BIOS", HKEY_LOCAL_MACHINE));
    if (strDate.IsEmpty())
        strDate = FromSystemCharset(GetWinRegTxt("SystemBiosDate", "HARDWARE\\DESCRIPTION\\System", HKEY_LOCAL_MACHINE));
    i32 lang = GetCurrentLanguage();
    SetLanguage(LNG_ENGLISH);
    if (!StrToDate(biosReleaseDate, strDate))
        biosReleaseDate = Null;
    SetLanguage(lang);
    Val vmbSerial;
    if (GetWMIInfo("Win32_BIOS", "SerialNumber", vmbSerial))
        biosSerial = Trim(vmbSerial.ToTxt());
}

bool GetProcessorInfo(i32 number, Txt &vendor, Txt &identifier, Txt &architecture, i32 &speed) {
    Txt strReg = Format("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d", number);
    vendor = FromSystemCharset(GetWinRegTxt("VendorIdentifier", strReg, HKEY_LOCAL_MACHINE));
    identifier = FromSystemCharset(GetWinRegTxt("ProcessorNameTxt", strReg, HKEY_LOCAL_MACHINE));
    architecture = FromSystemCharset(GetWinRegTxt("Identifier", strReg, HKEY_LOCAL_MACHINE));
    speed = GetWinRegInt("~MHz", strReg, HKEY_LOCAL_MACHINE);

    return true;
}
/*
Txt GetMacAddressWMI() {
    Val vmac;
    if (!GetWMIInfo("Win32_NetworkAdapterConfiguration", "MacAddress", vmac))
        return Null;
    Txt mac = Trim(vmac);
    if (mac.GetCount() > 0)
        return mac;
    return Null;
}
*/

Txt inet_ntop4(u8k* src) {
    return Format("%d.%d.%d.%d", src[0], src[1], src[2], src[3]);
}

Txt inet_ntop6(u8k* src) {
    struct {i32 base, len;} best, cur;
    uint16_t words[IN6ADDRSZ / INT16SZ];

    memset(words, 0, sizeof words);
    for (i32 i = 0; i < IN6ADDRSZ; i++)
        words[i32(i / INT16SZ)] |= (static_cast<uint8_t>(src[i]) << ((1 - (i % INT16SZ)) << 3));

    best.base = -1;
    cur.base = -1;
    for (i32 i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
        if (words[i] == 0) {
            if (cur.base == -1)
                cur.base = i, cur.len = 1;
            else
                cur.len++;
        } else {
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len)
                    best = cur;
                cur.base = -1;
            }
        }
    }
    if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
            best = cur;
    }
    if (best.base != -1 && best.len < 2)
        best.base = -1;

    Txt ret;
    for (i32 i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
        if (best.base != -1 && i >= best.base && i < (best.base + best.len)) {
            if (i == best.base)
                ret.Cat(':');
            continue;
        }
        if (i != 0)
            ret.Cat(':');
        if (i == 6 && best.base == 0 && (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
            ret.Cat(inet_ntop4(src + 12));
            break;
        }
        ret.Cat(Format("%x", words[i]));
    }
    if (best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
        ret.Cat(':');

    return ret;
}

Array <NetAdapter> GetAdapterInfo() {
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG family = AF_UNSPEC;
    DWORD flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;
    ULONG outBufLen = 0;
    Buffer<BYTE> pBuffer;
    Array <NetAdapter> ret;

    switch (GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen)) {
    case ERROR_BUFFER_OVERFLOW:
        pBuffer.Alloc(outBufLen);
        pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(~pBuffer);
        break;
    case ERROR_NO_DATA:
    case ERROR_INVALID_FUNCTION:
    default:
        return ret;
    }
    if (NO_ERROR != GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen))
        return ret;

    for (PIP_ADAPTER_ADDRESSES pAdd = pAddresses; pAdd; pAdd = pAdd->Next) {
        NetAdapter &adapter = ret.Add();
        i32 len = min(static_cast<DWORD>(6), pAdd->PhysicalAddressLength);
        if (len > 0)
            adapter.mac = ToUpper(HexTxt(pAdd->PhysicalAddress, len, 1, ':'));
        adapter.description = Trim(WideToTxt(pAdd->Description));
        adapter.fullname = Trim(WideToTxt(pAdd->FriendlyName));
        adapter.dnsSuffix = Trim(WideToTxt(pAdd->DnsSuffix));
        adapter.connected = pAdd->OperStatus == 1;
#ifdef COMPILER_MSC
        PIP_ADAPTER_GATEWAY_ADDRESS_LH pGateway = pAdd->FirstGatewayAddress;
#else
        PIP_ADAPTER_UNICAST_ADDRESS_XP pGateway = pAdd->FirstUnicastAddress;
#endif
        if (pGateway != NULL) {
            for (i32 i = 0; pGateway != NULL; i++) {
                if (pGateway->Address.lpSockaddr->sa_family == AF_INET) {
                    sockaddr_in *sa_in = reinterpret_cast<sockaddr_in *>(pGateway->Address.lpSockaddr);
                    adapter.gatewayip4 = inet_ntop4(reinterpret_cast<u8k* >(&(sa_in->sin_addr)));
                } else if (pGateway->Address.lpSockaddr->sa_family == AF_INET6) {
                    sockaddr_in6 *sa_in6 = reinterpret_cast<sockaddr_in6 *>(pGateway->Address.lpSockaddr);
                    adapter.gatewayip6 = inet_ntop6(reinterpret_cast<u8k* >(&(sa_in6->sin6_addr)));
                }
                pGateway = pGateway->Next;
            }
        }
        PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pAdd->FirstUnicastAddress;
        if (pUnicast != NULL) {
            for (i32 i = 0; pUnicast != NULL; i++) {
                if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                    sockaddr_in *sa_in = reinterpret_cast<sockaddr_in *>(pUnicast->Address.lpSockaddr);
                    adapter.ip4 = inet_ntop4(reinterpret_cast<u8k* >(&(sa_in->sin_addr)));
                } else if (pUnicast->Address.lpSockaddr->sa_family == AF_INET6) {
                    sockaddr_in6 *sa_in6 = reinterpret_cast<sockaddr_in6 *>(pUnicast->Address.lpSockaddr);
                    adapter.ip6 = inet_ntop6(reinterpret_cast<u8k* >(&(sa_in6->sin6_addr)));
                }
                pUnicast = pUnicast->Next;
            }
        }
        switch (pAdd->IfType) {
        case IF_TYPE_ETHERNET_CSMACD:       adapter.type = "ETHERNET";  break;
        case IF_TYPE_ISO88025_TOKENRING:    adapter.type = "TOKENRING"; break;
        case IF_TYPE_PPP:                   adapter.type = "MODEM";     break;
        case IF_TYPE_SOFTWARE_LOOPBACK:     adapter.type = "SOFTWARE_LOOPBACK";     break;
        case IF_TYPE_ATM:                   adapter.type = "ATM";       break;
        case IF_TYPE_IEEE80211:             adapter.type = "IEEE80211"; break;
        case IF_TYPE_TUNNEL:                adapter.type = "TUNNEL";    break;
        case IF_TYPE_IEEE1394:              adapter.type = "IEEE1394";  break;
        default:                            adapter.type = "OTHER";
        }
        if (adapter.type == "ETHERNET" && ToLower(adapter.description).Find("wireless") >= 0)
            adapter.type = "IEEE80211";
    }
    return ret;
}

/*bool GetNetworkInfo(Txt &name, Txt &domain)
{
    LPWKSTA_INFO_100 pBuf = NULL;

    NET_API_STATUS nStatus = NetWkstaGetInfo(NULL, 100, (LPBYTE *)&pBuf);

    bool ret = true;
    if (nStatus == NERR_Success) {
        name = FromSystemCharsetW(pBuf->wki100_computername);
        domain = FromSystemCharsetW(pBuf->wki100_langroup);
    }  else
        ret = false;

    if (pBuf != NULL)
        NetApiBufferFree(pBuf);

    if (name.IsEmpty() || !ret) {   // Second try
        char path[256] = "";
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        gethostname(path, sizeof(path));
        name = path;
        domain = "";
        WSACleanup();
        ret = true;
    }
    return ret;
}*/

Txt GetHDSerial() {
    Val vmbSerial;
    if (!GetWMIInfo("Win32_PhysicalMedia", "SerialNumber", vmbSerial))
        return Null;
    Txt serial = Trim(vmbSerial.ToTxt());
    if (serial.GetCount() > 0)
        return serial;
    return Null;
}

bool GetVideoInfo(Array <Val> &name, Array <Val> &description, Array <Val> &videoProcessor, Array <Val> &ram, Array <Val> &videoMode) {
    Array <Val> *res[5];
    res[0] = &name;
    res[1] = &description;
    res[2] = &videoProcessor;
    res[3] = &ram;
    res[4] = &videoMode;
    Vec <Txt> data;
    data.Add("Name");
    data.Add("Description");
    data.Add("VideoProcessor");
    data.Add("AdapterRAM");
    data.Add("VideoModeDescription");
    if (!GetWMIInfo("Win32_VideoController", data, res))
        return false;

    for (i32 row = 0; row < ram.GetCount(); ++row)
        ram[row] = (atoi(ram[row].ToTxt()) + 524288)/1048576;
    return true;
}

bool GetPackagesInfo(Array <Val> &name, Array <Val> &version, Array <Val> &vendor,
        Array <Val> &installDate, Array <Val> &caption, Array <Val> &description, Array <Val> &state)
{
    Array <Val> *res[7];
    res[0] = &name;
    res[1] = &version;
    res[2] = &vendor;
    res[3] = &installDate;
    res[4] = &caption;
    res[5] = &description;
    res[6] = &state;
    Vec <Txt> data;
    data.Add("Name");
    data.Add("Version");
    data.Add("Vendor");
    data.Add("InstallDate2");
    data.Add("Caption");
    data.Add("Description");
    data.Add("InstallState");
    if (!GetWMIInfo("Win32_Product", data, res))
        return false;

    for (i32 i = 0; i < installDate.GetCount(); ++i) {
        Txt sdate = installDate[i];
        Time t(atoi(sdate.Left(4)), atoi(sdate.Mid(4, 2)), atoi(sdate.Mid(6, 2)),
               atoi(sdate.Mid(8, 2)), atoi(sdate.Mid(10, 2)), atoi(sdate.Mid(12, 2)));
        installDate[i] = t;
        i32 istate = state[i];
        switch (istate) {
        case -6:    state[i] = "Неправильная Конфигурация"; break;
        case -2:    state[i] = "Неполноценный Аргумент";  break;
        case -1:    state[i] = "Неизвестный Пакет";   break;
        case 1:     state[i] = "Advertised";        break;
        case 2:     state[i] = "Отсутствует";            break;
        case 5:     state[i] = "Ok";                break;
        default:    return false;
        }
    }
    return true;
}

double GetCpuTemperature() {
    Val data;
    if (GetWMIInfo("MSAcpi_ThermalZoneTemperature", "CurrentTemperature", data, "root\\wmi"))
        return (double(data) - 2732.) / 10.;
    if (GetWMIInfo("Win32_TemperatureProbe", "CurrentReading", data))
        return data;
    return Null;
}
#endif


void NetAdapter::Copy(const NetAdapter& src) {
    description = src.description;
    fullname = src.fullname;
    mac = src.mac;
    type = src.type;
}

void NetAdapter::Xmlize(XmlIO &xml) {
    xml ("description", description)("fullname", fullname)("mac", mac)("type", type);
}

void NetAdapter::Jsonize(JsonIO &json) {
    json ("description", description)("fullname", fullname)("mac", mac)("type", type);
}

void NetAdapter::Serialize(Stream &stream) {
    stream % description % fullname % mac % type;
}

bool GetNetworkInfo(Txt &name, Txt &domain, Txt &ip4, Txt &ip6) {
    Buffer<char> sname(255);

#ifdef _WIN32
    bool close = false;
#endif

    if (0 != gethostname(sname, 255)) {
#ifdef _WIN32
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
            return false;
        if (0 != gethostname(sname, 255))
            return false;
        close = true;
#else
        return false;
#endif
    }
    name = sname;

    struct hostent *host = gethostbyname(sname);
    domain = host->h_name;

#ifdef _WIN32
    u8* h_addr_list = reinterpret_cast<u8* >(*host->h_addr_list);
    ip4 = inet_ntop4(h_addr_list);
    ip6 = inet_ntop6(h_addr_list);
#else
    Buffer<char> str(max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN) + 1);
    inet_ntop(AF_INET, (uk )(struct in_addr *)*host->h_addr_list, ~str, INET_ADDRSTRLEN);
    ip4 = str;
    inet_ntop(AF_INET6, (uk )(struct in_addr *)*host->h_addr_list, ~str, INET6_ADDRSTRLEN);
    ip6 = str;
#endif

#ifdef _WIN32
    if (close)
        WSACleanup();
#endif
    return true;
}


#if defined (TARGET_POSIX)
void GetSystemInfo(Txt &manufacturer, Txt &productName, Txt &version, i32 &numberOfProcessors, Txt &mbSerial)
{
    manufacturer = LoadFile_Safe("/sys/devices/virtual/dmi/id/board_vendor");
    productName = LoadFile_Safe("/sys/devices/virtual/dmi/id/board_name");
    version = LoadFile_Safe("/sys/devices/virtual/dmi/id/product_version");
    mbSerial = LoadFile_Safe("/sys/devices/virtual/dmi/id/board_serial");
    if (mbSerial.IsEmpty())
        mbSerial = FormatInt(gethostid());

    TxtParse cpu(LoadFile_Safe("/proc/cpuinfo"));
    numberOfProcessors = 1;
    while (cpu.GoAfter("processor")) {
        cpu.GoAfter(":");
        numberOfProcessors = atoi(cpu.GetText()) + 1;
    }
}

void GetBiosInfo(Txt &biosVersion, Date &biosReleaseDate, Txt &biosSerial)
{
    Txt biosVendor = LoadFile_Safe("/sys/devices/virtual/dmi/id/bios_vendor");
    biosVersion = LoadFile_Safe("/sys/devices/virtual/dmi/id/bios_version");
    StrToDate(biosReleaseDate, LoadFile_Safe("/sys/devices/virtual/dmi/id/bios_date"));
    biosSerial = LoadFile_Safe("/sys/devices/virtual/dmi/id/chassis_serial");
}

bool GetProcessorInfo(i32 number, Txt &vendor, Txt &identifier, Txt &architecture, i32 &speed)
{
    TxtParse cpu(LoadFile_Safe("/proc/cpuinfo"));

    i32 cpuNumber;
    do {
        if (!cpu.GoAfter("processor", ":"))
            return false;
        Txt sCpu = cpu.GetText();
        if (sCpu == "")
            return false;
        cpuNumber = atoi(sCpu);
    } while (cpuNumber != number);

    cpu.GoAfter("vendor_id", ":");
    vendor = cpu.GetText();
    cpu.GoAfter("cpu family", ":");
    Txt family = cpu.GetText();      // 6 means 686
    cpu.GoAfter("model", ":");
    Txt model = cpu.GetText();
    cpu.GoAfter("model name", ":");
    identifier = cpu.GetText("\n");
    cpu.GoAfter("stepping", ":");
    Txt stepping = cpu.GetText();
    architecture = Sys("uname -m");     // CPU type
    architecture << " Family " << family << " Model " << model << " Stepping " << stepping;     // And 64 bits ?? uname -m
    cpu.GoAfter_Init("cpu MHz", ":");
    speed = cpu.GetInt();
    return true;
}

static double GetCpuTemperatureACPI() {
    TxtParse info = Sys("acpi -t");
    if (info.IsEmpty())
        return Null;
    TxtParse line;
    double tempAcc = 0;
    i32 count = 0;
    while (true) {
        line = info.GetLine();
        if (line.IsEmpty()) {
            if (info.Eof())
                break;
            else
                continue;
        }
        if (!line.GoAfter("Thermal", "ok,"))
            continue;
        double temp = line.GetDouble();
        if (IsNull(temp))
            continue;
        tempAcc += temp;
        count++;
    }
    if (count == 0)
        return Null;
    return tempAcc/count;
}

static double GetCpuTemperatureSensors() {
    TxtParse info = Sys("sensors");
    if (info.IsEmpty())
        return Null;
    TxtParse line;
    double tempAcc = 0;
    i32 count = 0;
    while (true) {
        if (!info.GoAfter("coretemp"))
            break;
        while(true) {
            line = info.GetLine();
            if (line.IsEmpty())
                break;
            if (!line.GoAfter("Core", ":"))
                continue;
            double temp = line.GetDouble();
            if (IsNull(temp))
                continue;
            tempAcc += temp;
            count++;
        }
        if (info.Eof())
            break;
    }
    if (count == 0)
        return Null;
    return tempAcc/count;
}

double GetCpuTemperature() {
    double temp = GetCpuTemperatureACPI();
    if (IsNull(temp))
        temp = GetCpuTemperatureSensors();
    return temp;
}


#ifndef TARGET_OSX

Array<NetAdapter> GetAdapterInfo() {
    Array<NetAdapter> res;

    i32 sck = socket(PF_INET, SOCK_DGRAM, 0);
    if(sck < 0)
        return res;

    char buf[8192] = {0};
    struct ifconf ifc = {0};
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if(ioctl(sck, SIOCGIFCONF, &ifc) < 0)
        return res;

    struct ifreq *ifr = ifc.ifc_req;
    i32 nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
    for(i32 i = 0; i < nInterfaces; i++) {
        Txt MAC;

        char hostname[NI_MAXHOST];
        bzero(hostname, NI_MAXHOST);
        struct ifreq *item = &ifr[i];

        struct sockaddr *addr = &(item->ifr_addr);
        socklen_t salen;
        switch(addr->sa_family) {
        case AF_INET:   salen = sizeof(struct sockaddr_in);     break;
        case AF_INET6:  salen = sizeof(struct sockaddr_in6);    break;
        default:        salen = 0;
        }

        getnameinfo(addr, salen, hostname, sizeof(hostname), NULL, 0, NI_NAMEREQD);

        if(ioctl(sck, SIOCGIFADDR, item) < 0)
            continue;

        NetAdapter &adapter = res.Add();
        adapter.description = hostname;

        char ip[INET6_ADDRSTRLEN] = {0};
        switch(addr->sa_family) {
        case AF_INET:   inet_ntop(AF_INET, &(((struct sockaddr_in *)addr)->sin_addr), ip, INET6_ADDRSTRLEN);
                        adapter.ip4 = ip;
                        break;
        case AF_INET6:  inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)addr)->sin6_addr), ip, INET6_ADDRSTRLEN);
                        adapter.ip6 = ip;
                        break;
        }
        adapter.fullname = item->ifr_name;

        if(adapter.fullname.StartsWith("eth"))
            adapter.type = "ETHERNET";
        else if(adapter.fullname.StartsWith("lo"))
            adapter.type = "SOFTWARE_LOOPBACK";
        else if(adapter.fullname.StartsWith("ppp"))
            adapter.type = "MODEM";
        else if(adapter.fullname.StartsWith("hci"))
            adapter.type = "BLUETOOTH";
        else if(adapter.fullname.StartsWith("tr"))
            adapter.type = "TOKENRING";
        else if(adapter.fullname.StartsWith("vbox") || adapter.fullname.StartsWith("wifi") ||
                adapter.fullname.StartsWith("ath"))
            adapter.type = "VIRTUALBOX";
        else if(adapter.fullname.StartsWith("wlan"))
            adapter.type = "IEEE80211";
        else if(adapter.fullname.StartsWith("vmnet"))
            adapter.type = "VMWARE";
        else
            adapter.type = "OTHER";

#ifdef SIOCGIFHWADDR
        if(ioctl(sck, SIOCGIFHWADDR, item) < 0)
            continue;

        MAC = Format("%02x:%02x:%02x:%02x:%02x:%02x",
                    (u8)item->ifr_hwaddr.sa_data[0],
                    (u8)item->ifr_hwaddr.sa_data[1],
                    (u8)item->ifr_hwaddr.sa_data[2],
                    (u8)item->ifr_hwaddr.sa_data[3],
                    (u8)item->ifr_hwaddr.sa_data[4],
                    (u8)item->ifr_hwaddr.sa_data[5]);
#elif SIOCGENADDR
        if(ioctl(sck, SIOCGENADDR, item) < 0)
            continue;

        MAC = Format("%02x:%02x:%02x:%02x:%02x:%02x",
                    (u8)item->ifr_enaddr[0],
                    (u8)item->ifr_enaddr[1],
                    (u8)item->ifr_enaddr[2],
                    (u8)item->ifr_enaddr[3],
                    (u8)item->ifr_enaddr[4],
                    (u8)item->ifr_enaddr[5]);
#elif __MACH__ || __NetBSD__ || __OpenBSD__ || __FreeBSD__
        i32 mib[6] = {0};
        i32 len = 0;
        tuk macbuf;
        struct if_msghdr *ifm;
        struct sockaddr_dl *sdl;
        u8 ptr[];

        mib[0] = CTL_NET;
        mib[1] = AF_ROUTE;
        mib[2] = 0;
        mib[3] = AF_LINK;
        mib[4] = NET_RT_IFLIST;
        mib[5] = if_nametoindex(item->ifr_name);
        if(mib[5] == 0)
            continue;

        if(sysctl(mib, 6, NULL, (size_t*)&len, NULL, 0) != 0)
            continue;

        macbuf = (tuk ) malloc(len);
        if(macbuf == NULL)
            continue;

        if(sysctl(mib, 6, macbuf, (size_t*)&len, NULL, 0) != 0)
            continue;

        ifm = (struct if_msghdr *)macbuf;
        sdl = (struct sockaddr_dl *)(ifm + 1);
        ptr = (u8*)LLADDR(sdl);

        MAC = Format("%02x:%02x:%02x:%02x:%02x:%02x",
                        ptr[0], ptr[1], ptr[2],
                        ptr[3], ptr[4], ptr[5]);

        free(macbuf);
#else
#error OS Distribution Not Recognized
#endif
        adapter.mac = MAC;
    }
    return res;
}
#endif

// Not implemented yet in Linux
Txt GetHDSerial() {
    return Null;
}

#endif

/////////////////////////////////////////////////////////////////////
// Memory Info

#if defined(TARGET_WIN32)

bool GetMemoryInfo(
    i32 &memoryLoad,            // percent of memory in use
    zu64 &totalPhys,          // physical memory
    zu64 &freePhys,           // free physical memory
    zu64 &totalPageFile,      // total paging file
    zu64 &freePageFile,       // free paging file
    zu64 &totalVirtual,       // total virtual memory
    zu64 &freeVirtual)        // free virtual memory
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof (status);
    if (!GlobalMemoryStatusEx(&status))
        return false;
    memoryLoad          = status.dwMemoryLoad;
    totalPhys           = status.ullTotalPhys;
    freePhys            = status.ullAvailPhys;
    totalPageFile       = status.ullTotalPageFile;
    freePageFile        = status.ullAvailPageFile;
    totalVirtual        = status.ullTotalVirtual;
    freeVirtual         = status.ullAvailVirtual;

    return true;
}

#else

bool GetMemoryInfo(
            i32 &memoryLoad,            // percent of memory in use
            zu64 &totalPhys,          // physical memory
            zu64 &freePhys,           // free physical memory
            zu64 &totalPageFile,      // total paging file
            zu64 &freePageFile,       // free paging file
            zu64 &totalVirtual,       // total virtual memory
            zu64 &freeVirtual)
{
    TxtParse meminfo = LoadFile_Safe("/proc/meminfo");
    if (meminfo == "")
        return false;
    meminfo.GoAfter_Init("MemTotal", ":");  totalPhys = 1024*meminfo.GetUInt64();
    meminfo.GoAfter_Init("MemFree", ":");   freePhys  = 1024*meminfo.GetUInt64();
    memoryLoad = (i32)(100.*(totalPhys-freePhys)/totalPhys);
    meminfo.GoAfter_Init("SwapCached", ":");freePageFile = 1024*meminfo.GetUInt64();
    meminfo.GoAfter_Init("Cached", ":");    totalPageFile = 1024*meminfo.GetUInt64() + freePageFile;
    meminfo.GoAfter_Init("SwapTotal", ":"); totalVirtual = 1024*meminfo.GetUInt64();
    meminfo.GoAfter_Init("SwapFree", ":");  freeVirtual = 1024*meminfo.GetUInt64();

    return true;
}
#endif


/////////////////////////////////////////////////////////////////////
// Process list

#if defined(TARGET_WIN32)

// Get the list of process identifiers.
bool GetProcessList(Array<z64> &pid, Array<Txt> &pNames)
{
    PROCESSENTRY32 proc;
    HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
        return false;
    proc.dwSize = sizeof(proc);
    long f = Process32First(hSnap, &proc);
    while (f) {
        pid.Add(proc.th32ProcessID);
        pNames.Add(proc.szExeFile);
        f = Process32Next(hSnap, &proc);
    }
    CloseHandle(hSnap);
    return true;
}

Array<z64> GetProcessList()
{
    PROCESSENTRY32 proc;
    Array<z64> ret;
    HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
        return ret;
    proc.dwSize = sizeof(proc);
    long f = Process32First(hSnap, &proc);
    while (f) {
        ret.Add(proc.th32ProcessID);
        f = Process32Next(hSnap, &proc);
    }
    CloseHandle(hSnap);
    return ret;
}

Array<z64> GetChildProcessList(z64 processID)
{
    PROCESSENTRY32 proc;
    Array<z64> child, all, parents;
    HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
        return child;
    proc.dwSize = sizeof(proc);
    long f = Process32First(hSnap, &proc);
    while (f) {
        all << proc.th32ProcessID;
        parents << proc.th32ParentProcessID;
        f = Process32Next(hSnap, &proc);
    }
    CloseHandle(hSnap);
    child << processID;
    i32 init = 0;
    while (true) {
        i32 count = child.GetCount();
        if (init >= count)
            break;
        for (i32 cid = init; cid < count; ++cid) {
            for (i32 i = 0; i < all.GetCount(); ++i) {
                if (all[i] == child[cid])
                    continue;
                else if (parents[i] == child[cid])
                    child << parents[i];
            }
        }
        init = count;
    }
    child.Remove(0);
    return child;
}

Txt GetProcessName(z64 processID)
{
    WCHAR szProcessName[MAX_PATH];
    Txt ret;

    // Get a handle to the process.
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, DWORD(processID));

    // Get the process name.
    if (hProcess != NULL) {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleBaseNameW(hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(WCHAR));
            ret = FromSystemCharset(WTxt(szProcessName).ToTxt());
        }
    }
    CloseHandle(hProcess);

    return ret;
}

Txt GetProcessFName(z64 processID)
{
    WCHAR szProcessName[MAX_PATH];
    Txt ret;

    // Get a handle to the process.
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, DWORD(processID));

    // Get the process name.
    if (hProcess != NULL) {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleFNameExW(hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(WCHAR));
            ret = FromSystemCharset(WTxt(szProcessName).ToTxt());
        }
    }
    CloseHandle(hProcess);

    return ret;
}


ULONGLONG SubtractFILETIME(FILETIME &to, FILETIME &from) {
    __z64 timeFrom = (static_cast<__z64>(from.dwHighDateTime) << 32) + from.dwLowDateTime;
    __z64 timeTo   = (static_cast<__z64>(to.dwHighDateTime) << 32) + to.dwLowDateTime;
    __z64 delta = timeTo - timeFrom;
    return delta;
}

i32 GetProcessCPUUsage(z64 pid) {
    HANDLE hp = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, DWORD(pid));
    if (hp == NULL)
        return Null;

    FILETIME iddleTime, kernelTimeS, userTimeS, kernelTimeS_0, userTimeS_0;
    FILETIME creationTime, exitTime, kernelTimeP, userTimeP, kernelTimeP_0, userTimeP_0;
    GetSystemTimes(&iddleTime, &kernelTimeS_0, &userTimeS_0);
    GetProcessTimes(hp, &creationTime, &exitTime, &kernelTimeP_0, &userTimeP_0);
    Sleep(50);      // It spends some ms elapsing the process used time
    GetSystemTimes(&iddleTime, &kernelTimeS, &userTimeS);
    GetProcessTimes(hp, &creationTime, &exitTime, &kernelTimeP, &userTimeP);

    z64 kernelS = SubtractFILETIME(kernelTimeS, kernelTimeS_0);
    z64 userS = SubtractFILETIME(userTimeS, userTimeS_0);
    z64 totalS = kernelS + userS;
    z64 kernelP = SubtractFILETIME(kernelTimeP, kernelTimeP_0);
    z64 userP = SubtractFILETIME(userTimeP, userTimeP_0);
    z64 totalP = kernelP + userP;

    i32 cpu = i32((100*totalP)/totalS);
    CloseHandle(hp);
    return cpu;
}


BOOL CALLBACK EnumGetWindowsList(HWND hWnd, LPARAM lParam)
{
    if (!hWnd)
        return TRUE;        // Not a window
    if (GetParent(hWnd) != 0)
        return TRUE;        // Child window
    Array<z64> *ret = reinterpret_cast<Array<z64> *>(lParam);
    ret->Add(reinterpret_cast<z64>(hWnd));
    return TRUE;
}

void GetWindowsList(Array<z64> &hWnd, Array<z64> &processId, Array<Txt> &name, Array<Txt> &fileName, Array<Txt> &caption, bool getAll)
{
    HANDLE hProcess;
    DWORD dwProcessId;
    HINSTANCE hInstance;
    WCHAR str[MAX_PATH];
    i32 count;

    EnumWindows(EnumGetWindowsList, reinterpret_cast<LPARAM>(&hWnd));
    for (i32 i = 0; i < hWnd.GetCount(); ++i) {
        if (!getAll) {
            LONG_PTR style = GetWindowLongPtr(reinterpret_cast<HWND>(hWnd[i]), GWL_STYLE);
            if (style & WS_MINIMIZE)    // WS_ICONIC == WS_MINIMIZE
                continue;
        }
        Txt sstr;
        hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(reinterpret_cast<HWND>(hWnd[i]), GWLP_HINSTANCE));
        GetWindowThreadProcessId(reinterpret_cast<HWND>(hWnd[i]), &dwProcessId);
        processId.Add(dwProcessId);
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
        if ((count = GetModuleFNameExW(hProcess, hInstance, str, sizeof(str)/sizeof(WCHAR)))) {
            sstr = WTxt(str, count).ToTxt();
            fileName << sstr;
        } else
            fileName << t_("Неизвестный процесс");
        if ((count = GetModuleBaseNameW(hProcess, hInstance, str, sizeof(str)/sizeof(WCHAR)))) {
            sstr = WTxt(str, count).ToTxt();
            name << sstr;
         } else
            name << t_("Неизвестный процесс");
        CloseHandle(hProcess);

        if (sstr == "TPClnt.dll")       // VMWare Thinprint crashes SendMessageW()
            caption << "";
        else if (IsWindowVisible(reinterpret_cast<HWND>(hWnd[i]))) {
            count = i32(SendMessageW(reinterpret_cast<HWND>(hWnd[i]), WM_GETTEXT, sizeof(str)/sizeof(WCHAR), reinterpret_cast<LPARAM>(str)));
            caption << WTxt(str, count).ToTxt();
        } else
            caption << "";
    }
}

Array<z64> GetWindowsList()
{
    Array<z64> ret;
    EnumWindows(EnumGetWindowsList, reinterpret_cast<LPARAM>(&ret));
    return ret;
}

BOOL CALLBACK TerminateAppEnum(HWND hwnd, LPARAM lParam)
{
    DWORD dwID;
    GetWindowThreadProcessId(hwnd, &dwID);
    if(dwID == DWORD(lParam))
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    return TRUE;
}

bool ProcessTerminate(z64 pId, i32 timeout)
{
    HANDLE hProc = ::OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, DWORD(pId));
    if(hProc == NULL)
        return false;
    ::EnumWindows(reinterpret_cast<WNDENUMPROC>(TerminateAppEnum), static_cast<LPARAM>(pId));

    i32 ret;
    DWORD state = ::WaitForSingleObject(hProc, timeout);
    if ((state == WAIT_TIMEOUT) || (state == WAIT_FAILED))
        ret = ::TerminateProcess(hProc, 0);
    else
        ret = true;
    CloseHandle(hProc);
    return ret;
}

i32 GetProcessPriority(z64 pid)
{
    i32 priority;
    HANDLE hProc = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, DWORD(pid));
    if(hProc == NULL)
        return -1;
    priority = ::GetPriorityClass(hProc);
    CloseHandle(hProc);

    switch(priority) {
    case REALTIME_PRIORITY_CLASS:   priority = 10;  // Process that has the highest possible priority. The threads of a real-time priority class process preempt the threads of all other processes, including operating system processes performing important tasks. For example, a real-time process that executes for more than a very brief interval can cause disk caches not to flush or cause the mouse to be unresponsive.
                                    break;
    case HIGH_PRIORITY_CLASS:       priority = 8;   // Process that performs time-critical tasks that must be executed immediately for it to run correctly. The threads of a high-priority class process preempt the threads of normal or idle priority class processes. An example is the Task List, which must respond quickly when called by the user, regardless of the load on the operating system. Use extreme care when using the high-priority class, because a high-priority class CPU-bound application can use nearly all available cycles.
                                    break;
    case ABOVE_NORMAL_PRIORITY_CLASS: priority = 6; // Process that has priority above NORMAL_PRIORITY_CLASS but below HIGH_PRIORITY_CLASS.
                                    break;
    case NORMAL_PRIORITY_CLASS:     priority = 5;   // Process with no special scheduling needs.
                                    break;
    case BELOW_NORMAL_PRIORITY_CLASS: priority = 3; // Process that has priority above IDLE_PRIORITY_CLASS but below NORMAL_PRIORITY_CLASS.
                                    break;
    case IDLE_PRIORITY_CLASS:       priority = 0;   // Process whose threads run only when the system is idle and are preempted by the threads of any process running in a higher priority class. An example is a screen saver. The idle priority class is inherited by child processes.
                                    break;
    default:                        return -1;
    }
    return priority;
}

bool SetProcessPriority(z64 pid, i32 priority)
{
    HANDLE hProc = ::OpenProcess(PROCESS_SET_INFORMATION , FALSE, DWORD(pid));
    if(hProc == NULL)
        return false;
    if (priority == 10)
        priority = REALTIME_PRIORITY_CLASS;
    else if (priority >= 8)
        priority = HIGH_PRIORITY_CLASS;
    else if (priority >= 6)
        priority = ABOVE_NORMAL_PRIORITY_CLASS;
    else if (priority >= 5)
        priority = NORMAL_PRIORITY_CLASS;
    else if (priority >= 3)
        priority = BELOW_NORMAL_PRIORITY_CLASS;
    else
        priority = IDLE_PRIORITY_CLASS;
    i32 ret = ::SetPriorityClass(hProc, priority);      // SetProcessAffinityMask
    CloseHandle(hProc) ;
    return ret;
}

#endif
#ifdef TARGET_POSIX            //Check with ps

bool IsInteger(Txt s)
{
    for (i32 i = 0; i < s.GetCount(); ++i) {
        if (!isdigit(s[i]))
            return false;
    }
    return true;
}

bool GetProcessList(Array<z64> &pid, Array<Txt> &pNames)
{
    FindFile ff;
    if(ff.Search("/proc/*")) {
        do {
            if (IsInteger(ff.GetName())) {
                Txt exe = Format("/proc/%s/exe", ff.GetName());
                TxtBuf exeb;
                exeb = exe;
                char procName[2048];
                i32 procNameLen = readlink(exeb, procName, sizeof(procName)-1);
                if (procNameLen > 0) {
                    procName[procNameLen] = 0;
                    pNames.Add(procName);
                    pid.Add(atoi(ff.GetName()));
                }
            }
        } while(ff.Next());
    }
    return true;
}

Array<z64> GetProcessList()
{
    FindFile ff;
    Array<z64> pid;
    if(ff.Search("/proc/*")) {
        do {
            if (IsInteger(ff.GetName())) {
                Txt exe = Format("/proc/%s/exe", ff.GetName());
                TxtBuf exeb;
                exeb = exe;
                char procName[2048];
                i32 procNameLen = readlink(exeb, procName, sizeof(procName)-1);
                if (procNameLen > 0)
                    pid.Add(atoi(ff.GetName()));
            }
        } while(ff.Next());
    }
    return pid;
}

Txt GetProcessName(z64 pid)
{
    return GetFName(GetProcessFName(pid));
}

// ls -l /proc/%d/fd gets also the files opened by the process
Txt GetProcessFName(z64 pid)
{
    Txt ret = "";
    Txt exe = Format("/proc/%s/exe", FormatLong(pid));
    TxtBuf exeb;
    exeb = exe;
    char procName[2048];
    i32 procNameLen = readlink(exeb, procName, sizeof(procName)-1);
    if (procNameLen > 0) {
        procName[procNameLen] = 0;
        ret = procName;
    }
    return ret;
}

void GetWindowsList_Rec (_XDisplay *dpy, Window w, i32 depth, Array<z64> &wid)
{
    if (depth > 3) // 1 is enough for Gnome. 2 is necessary for Xfce and Kde
        return;

    wid.Add(w);

    Window root, parent;
    u32 nchildren;
    Window *children = NULL;
    if (XQueryTree (dpy, w, &root, &parent, &children, &nchildren))  {
        for (i32 i = 0; i < nchildren; i++) {
            XWindowAttributes windowattr;
            XGetWindowAttributes(dpy, children[i], &windowattr);
            if (windowattr.map_state == IsViewable)
                GetWindowsList_Rec (dpy, children[i], depth + 1, wid);
        }
    }
    if (children)
        XFree((tuk )children);
    return;
}

Array<z64> GetWindowsList()
{
    Array<z64> ret;
    SetSysInfoX11ErrorHandler();

    _XDisplay *dpy = XOpenDisplay (NULL);
    if (!dpy) {
        SetX11ErrorHandler();
        return ret;
    }
    GetWindowsList_Rec (dpy, RootWindow(dpy, DefaultScreen(dpy)), 0, ret);
    XCloseDisplay (dpy);
    SetX11ErrorHandler();
    return ret;
}

void GetWindowsList(Array<z64> &hWnd, Array<z64> &processId, Array<Txt> &nameL, Array<Txt> &fileName, Array<Txt> &caption, bool getAll)
{
    SetSysInfoX11ErrorHandler();
    _XDisplay *dpy = XOpenDisplay (NULL);
    if (!dpy) {
        SetX11ErrorHandler();
        return;
    }
    GetWindowsList_Rec(dpy, RootWindow (dpy, DefaultScreen (dpy)), 0, hWnd);
    for (i32 i = 0; i < hWnd.GetCount(); ++i) {
        // Get window name
        XTextProperty tp;
        if (XGetWMName(dpy, hWnd[i], &tp) == 0)
            caption.Add("");
        else {
            if (tp.nitems > 0) {
                i32 count = 0, i, ret;
                tuk *list = NULL;
                ret = XmbTextPropertyToTextList(dpy, &tp, &list, &count);
                if((ret == Success || ret > 0) && list != NULL) {
                    Txt sret;
                    for(i = 0; i < count; i++)
                        sret << list[i];
                    XFreeStringList(list);
                    caption.Add(FromSystemCharset(sret));
                } else
                    caption.Add(FromSystemCharset((tuk )tp.value));
            } else
                caption.Add("");
        }
        // Get pid
        Atom atomPID = XInternAtom(dpy, "_NET_WM_PID", true);
        u64 pid = 0;
        if (atomPID == None)
            processId.Add(0LL);
        else {
            Atom type;
            i32 format;
            u64 nItems;
            u64 bytesAfter;
            u8* propPID = 0;
            if (0 == XGetWindowProperty(dpy, hWnd[i], atomPID, 0, 1024, false, XA_CARDINAL, &type, &format, &nItems, &bytesAfter, &propPID)) {
                if(propPID != 0) {
                    pid = *((u64 *)propPID);
                    processId.Add(pid);
                    XFree(propPID);
                } else
                    processId.Add(0LL);
            } else
                processId.Add(0LL);
        }
        if (pid != 0L)
            fileName.Add(GetProcessFName(pid));
        else
            fileName.Add("");
        // Name and class
        XClassHint ch;
        ch.res_name = ch.res_class = NULL;
        Status status = XGetClassHint(dpy, hWnd[i], &ch);
        if (status != BadWindow) {
            if (ch.res_name)
                nameL.Add(ch.res_name);
            else
                nameL.Add("");
        } else
            nameL.Add("");
        if (ch.res_name)
            XFree (ch.res_name);
        if (ch.res_class)
            XFree (ch.res_class);
    }
    XCloseDisplay (dpy);
    SetX11ErrorHandler();
    return;
}

bool WindowKill(z64 wid)
{
    if (wid == 0)
        return false;

    _XDisplay *dpy = XOpenDisplay (NULL);
    if (!dpy)
        return false;

    XSync (dpy, 0);
    XKillClient (dpy, wid);
    XSync (dpy, 0);

    XCloseDisplay (dpy);
    return true;
}

// Also possible to stop or cont
bool ProcessTerminate(z64 pid, i32 timeout)
{
    if (!ProcessExists(pid))
        return false;
    long wid = GetWindowIdFromProcessId(pid);       // Just in case

    // First... SIGTERM
    kill(pid, SIGTERM);
    Sleep(timeout/3);
    if (!ProcessExists(pid))
        return true;
    // Second... SIGKILL
    kill(pid, SIGKILL);
    Sleep(timeout/3);
    if (!ProcessExists(pid))
        return true;
    // Third ... WindowKill
    Sleep((i32)(timeout/3));
    return WindowKill(wid);
}

i32 GetProcessPriority(z64 pid)
{
    i32 priority = getpriority(PRIO_PROCESS, pid);
    return 10 - (priority + 20)/4;      // Rescale -20/20 to 10/0
}

bool SetProcessPriority(z64 pid, i32 priority)
{
    priority = 20 - 4*priority;
    if (0 == setpriority(PRIO_PROCESS, pid, priority))
        return true;
    else
        return false;
}

bool ProcessExists(z64 pid)
{
    return DirExists(Format("/proc/%s", Sprintf("%ld", pid)));
}

#endif

z64 GetWindowIdFromCaption(Txt windowCaption, bool exactMatch)
{
    Array<z64> wid, pid;
    Array<Txt> name, fileName, caption;
    GetWindowsList(wid, pid, name, fileName, caption);
    for (i32 i = 0; i < wid.GetCount(); ++i) {
        if (exactMatch) {
            if (caption[i] == windowCaption)
                return wid[i];
        } else {
            if (caption[i].Find(windowCaption) >= 0)
                return wid[i];
        }
    }
    return -1;
}

z64 GetProcessIdFromWindowCaption(Txt windowCaption, bool exactMatch)
{
    Array<z64> wid, pid;
    Array<Txt> name, fileName, caption;
    GetWindowsList(wid, pid, name, fileName, caption);
    for (i32 i = 0; i < wid.GetCount(); ++i) {
        if (exactMatch) {
            if (caption[i] == windowCaption)
                return pid[i];
        } else {
            if (caption[i].Find(windowCaption) >= 0)
                return pid[i];
        }
    }
    return -1;
}

z64 GetProcessIdFromWindowId(z64 _wId)
{
    Array<z64> wId, pid;
    Array<Txt> name, fileName, caption;
    GetWindowsList(wId, pid, name, fileName, caption);
    for (i32 i = 0; i < pid.GetCount(); ++i) {
        if (wId[i] == _wId)
            return pid[i];
    }
    return -1;
}

z64 GetWindowIdFromProcessId(z64 _pid)
{
    Array<z64> wId, pid;
    Array<Txt> name, fileName, caption;
    GetWindowsList(wId, pid, name, fileName, caption);
    for (i32 i = 0; i < pid.GetCount(); ++i) {
        if (pid[i] == _pid)
            return wId[i];
    }
    return -1;
}

z64 GetProcessIdFromName(Txt name)
{
    Array<z64> pid;
    Array<Txt> pNames;
    if (!GetProcessList(pid, pNames))
        return Null;
    name = ToLower(name);
    for (i32 i = 0; i < pid.GetCount(); ++i) {
        if (ToLower(pNames[i]) == name)
            return pid[i];
    }
    return Null;
}


/////////////////////////////////////////////////////////////////////
// Others

z64    GetProcessId()         {return getpid();}

/////////////////////////////////////////////////////////////////////
// Drives list
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)

bool GetDriveSpace(Txt drive,
    zu64 &freeBytesUser,  // Total number of free bytes on a disk that are available to the user who is associated with the calling thread.
    zu64 &totalBytesUser, // Total number of bytes on a disk that are available to the user who is associated with the calling thread.
    zu64 &totalFreeBytes) // Total number of free bytes on a disk.
{
    TxtBuf sb(drive);
    ULARGE_INTEGER _freeBytesUser, _totalBytesUser, _totalFreeBytes;

    if(!GetDiskFreeSpaceExA(sb, &_freeBytesUser, &_totalBytesUser, &_totalFreeBytes))
        return false;
    freeBytesUser = zu64(_freeBytesUser.QuadPart);
    totalBytesUser = zu64(_totalBytesUser.QuadPart);
    totalFreeBytes = zu64(_totalFreeBytes.QuadPart);
    return true;
}

// return true if mounted
bool GetDriveInformation(Txt drive, Txt &type, Txt &volume, /*zu64 &serial, */i32 &maxName, Txt &fileSystem)
{
    TxtBuf sb(drive);

    switch (::GetDriveType(sb)) {
    case DRIVE_UNKNOWN:     type = "Диск неизвестен";     break;
    case DRIVE_NO_ROOT_DIR: type = "Корневой папки не существует"; break;
    case DRIVE_REMOVABLE:
          switch (drive[0]) {
          case 'A':
          case 'B':         type = "Дискета";
                            volume = fileSystem = "";
                            /*serial = */maxName = 0;
                            return false;
          default:          type = "Съёмый"; break;
          }
          break;
    case DRIVE_FIXED:       type = "Жёсткий";      break;
    case DRIVE_REMOTE:      type = "Сеть";   break;
    case DRIVE_CDROM:       type = "Оптический";   break;
    case DRIVE_RAMDISK:     type = "RAM";       break;
    }
    char vol[MAX_PATH], fs[MAX_PATH];
    DWORD flags;
    DWORD serial;
    DWORD _maxName;
    if(!::GetVolumeInformation(sb, vol, MAX_PATH, &serial, &_maxName, &flags, fs, MAX_PATH)) {
        if (type == "Оптический") {
            volume = "";
            fileSystem = "";
            maxName = 0;
            return true;
        } else
            return false;
    }
    volume = vol;
    fileSystem = fs;
    maxName = i32(_maxName);

    return true;
}

#elif defined(TARGET_POSIX)

bool GetDriveSpace(Txt drive,
    //zu64 &totalBytes,   // To determine the total number of bytes on a disk or volume, use IOCTL_DISK_GET_LENGTH_INFO.
    zu64 &freeBytesUser,  // Total number of free bytes on a disk that are available to the user who is associated with the calling thread.
    zu64 &totalBytesUser, // Total number of bytes on a disk that are available to the user who is associated with the calling thread.
    zu64 &totalFreeBytes) // Total number of free bytes on a disk.
{
    freeBytesUser = totalBytesUser = totalFreeBytes = 0;

    TxtParse space = Sys("df -T");
    if (space == "")
        return false;

    while (!space.Eof() && drive != space.GetText())
        ;
    space.MoveRel(-10); space.GoBeginLine();
    space.GetText();    space.GetText();    // Jumps over device path and filesystem
    totalBytesUser = 1024*space.GetUInt64();
    space.GetText();                        // Jumps over used space
    freeBytesUser  = totalFreeBytes = 1024*space.GetUInt64();
    return true;
}

#ifndef TARGET_OSX
// return true if mounted
bool GetDriveInformation(Txt drive, Txt &type, Txt &volume, /*zu64 &serial, */i32 &maxName, Txt &fileSystem)
{
    TxtParse info = Sys("mount -l");
    if (info  == "")
        return false;

    Txt straux;
    while (drive != (straux = info.GetText()))
        if (straux == "")
            return false;

    if("type" != info.GetText())        // Jumps over "type"
        return false;

    fileSystem = info.GetText();
    Txt details = info.GetText();
    info.GoAfter("[");
    volume = info.GetText("]");
    //serial = 0;           // Unknown
    if ((fileSystem == "udf" || fileSystem == "iso9660") && details.Find("ro") >=0)
        type = "Оптический";
    else if (details.Find("flush") >=0)
        type = "Съёмный";
    else
        type = "Жёсткий";

    struct statfs buf;
    if (0 == statfs(drive, &buf))
        //puts(Format("%x", buf.f_type)); // Filesystem type
        maxName = buf.f_namelen;
    else
        maxName = 0;

    return true;
}
#endif

#endif

#if defined(TARGET_WIN32) || defined (TARGET_WIN64)

#ifndef SHTDN_REASON_MINOR_OTHER
    #define SHTDN_REASON_MINOR_OTHER 0
#endif

bool Shutdown(Txt action) {
    action = ToLower(action);

    bool hibernate, suspend;
    hibernate = suspend = false;

    u32 flag;
    if (action == "logoff")
        flag = EWX_LOGOFF;
    else if (action == "poweroff")
        flag = EWX_POWEROFF;
    else if (action == "reboot")
        flag = EWX_REBOOT;
    else if (action == "shutdown")
        flag = EWX_SHUTDOWN;
    else if (action == "hibernate")
        hibernate = true;
    else if (action == "suspend")
        suspend = true;
    else
        return false;

    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;

    // Get the LUID for the shutdown privilege.
    LookupPrivilegeVal(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;  // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the shutdown privilege for this process.
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
        PTOKEN_PRIVILEGES(NULL), 0);

    if (GetLastError() != ERROR_SUCCESS)
        return false;

    if (hibernate || suspend) {
        if (!SetSuspendState(hibernate, TRUE, FALSE))
            return false;
    } else {
        // Shut down the system and force all applications to close.
        if (!ExitWindowsEx(flag | EWX_FORCE, SHTDN_REASON_MINOR_OTHER))
            return false;
    }
    return true;
}
#endif

#ifdef TARGET_LINUX
bool Shutdown(Txt action) {
    if (action == "logoff") {
        kill(1, SIGTSTP);
        sync();
        signal(SIGTERM, SIG_IGN);
        setpgrp();
        kill(-1, SIGTERM);
        sleep(1);
        kill(-1, SIGHUP); //* Force PPPD's down, too *
        sleep(1);
        kill(-1, SIGKILL);
        sync();
        sleep(1);
    } else if (action == "shutdown") {
        sync();
        sleep(1);
#if !defined(__GNU_LIBRARY__)
        reboot(0xCDEF0123);
#elif __GNU_LIBRARY__ > 5
        reboot(0xCDEF0123);
#else
        reboot(0xfee1dead, 672274793, 0xCDEF0123);
#endif
        sync();
        sleep(1);
    } else if (action == "reboot") {        // LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2
        sync();
        sleep(1);
#if !defined(__GNU_LIBRARY__)
        reboot(0x01234567);
#elif __GNU_LIBRARY__ > 5
        reboot(0x01234567);
#else
        reboot(0xfee1dead, 672274793, 0x01234567);
#endif
        sync();
        sleep(1);
    }
    Exit(0);
    return true;
}
#endif


void GetCompilerInfoAux(Txt &name, i32 &version, drx::Time &time, Txt &mode, i32 &bits, tukk , tukk ) {
    time = FileGetTime(GetExeFilePath());

    name = "";
    version = 0;
    #if defined(TARGET_WIN32) || defined (TARGET_WIN64)
        #if defined(__MINGW32__)
            name = "mingw";
            version = __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__;    // __VERSION__
        #elif defined(COMPILER_MSC)
            name = "msc";
            version = _MSC_FULL_VER;
        #elif defined (__BORLANDC__)
            name = "borlandc"
            version = __BORLANDC__;
        #elif defined (__WATCOMC__)
            name = "watcomc"
            version = __WATCOMC__;
        #endif
    #elif defined (TARGET_POSIX)
        #if defined(__GNUC__)
            name = "gnuc";
            version = __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__;
        #endif
    #elif defined (__APPLE__)
        // In a next future?
    #endif
    #ifdef _DEBUG
        mode = "debug";
    #else
        mode = "release";
    #endif
    #if defined(_MSC_VER) && defined(_WIN64)
        bits = 64;
    #elif defined(__MINGW64__)
        bits = 64;
    #elif defined(__LP64__)
        bits = 64;
    #else
        bits = 32;
    #endif
}


#if defined(TARGET_WIN32) || defined (TARGET_WIN64)

bool PutWindowPlacement(HWND hwnd, RECT rcNormalPosition, POINT ptMinPosition, POINT ptMaxPosition, long showcmd, long flags)
{
    WINDOWPLACEMENT place;

    place.ptMinPosition = ptMinPosition;
    place.ptMaxPosition = ptMaxPosition;
    place.rcNormalPosition = rcNormalPosition;
    place.showCmd = showcmd;
    place.flags = flags;
    place.length = sizeof(place);
    return ::SetWindowPlacement(hwnd, &place);
}

bool TakeWindowPlacement(HWND hwnd, RECT &rcNormalPosition, POINT &ptMinPosition, POINT &ptMaxPosition, long &showcmd)
{
    WINDOWPLACEMENT place;

    place.length = sizeof(place);
    bool ret = ::GetWindowPlacement(hwnd, &place);
    if (!ret)
        return false;
    ptMinPosition = place.ptMinPosition;

    ptMaxPosition = place.ptMaxPosition;
    rcNormalPosition = place.rcNormalPosition;
    showcmd = place.showCmd;     //SW_SHOWMAXIMIZED, SW_SHOWMINIMIZED, SW_SHOWNORMAL

    return ret;
}

i32 Window_GetStatus(z64 windowId)
{
    WINDOWPLACEMENT place;

    place.length = sizeof(place);
    bool ret = GetWindowPlacement(reinterpret_cast<HWND>(windowId), &place);
    if (!ret) {
        return Null;
    }
    switch(place.showCmd) {
    case SW_SHOWMAXIMIZED:  return WINDOW_MAXIMIZED;
    case SW_SHOWMINIMIZED:  return WINDOW_MINIMIZED;
    case SW_SHOWNORMAL:     return WINDOW_NORMAL;
    }
    return Null;
}

bool Window_GetRect(z64 windowId, i32 &left, i32 &top, i32 &right, i32 &bottom)
{
    RECT rcNormalPosition = {0, 0, 0, 0};
    POINT ptMinPosition, ptMaxPosition;
    long showcmd;

    TakeWindowPlacement(reinterpret_cast<HWND>(windowId), rcNormalPosition, ptMinPosition, ptMaxPosition, showcmd);

    left = rcNormalPosition.left;
    top = rcNormalPosition.top;
    right = rcNormalPosition.right;
    bottom = rcNormalPosition.bottom;

    return true;
}

bool Window_SetRect(z64 windowId, i32 left, i32 top, i32 right, i32 bottom)
{
    RECT rcNormalPosition;
    POINT ptMinPosition, ptMaxPosition;
    long showcmd;

    if (!TakeWindowPlacement(reinterpret_cast<HWND>(windowId), rcNormalPosition, ptMinPosition, ptMaxPosition, showcmd))
        return false;

    rcNormalPosition.left = left;
    rcNormalPosition.top = top;
    rcNormalPosition.right = right;
    rcNormalPosition.bottom = bottom;
    return PutWindowPlacement(reinterpret_cast<HWND>(windowId), rcNormalPosition, ptMinPosition, ptMaxPosition, showcmd, 0);
}

void Window_Bottom(z64 windowId)
{
    ::SetWindowPos(reinterpret_cast<HWND>(windowId), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void Window_Top(z64 windowId)
{
    ::SetWindowPos(reinterpret_cast<HWND>(windowId), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void Window_TopMost(z64 windowId)
{
    ::SetWindowPos(reinterpret_cast<HWND>(windowId), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

#endif

#ifdef TARGET_POSIX

bool Window_GetRect(z64 windowId, i32 &left, i32 &top, i32 &right, i32 &bottom)
{
    SetSysInfoX11ErrorHandler();
    _XDisplay *dpy = XOpenDisplay (NULL);
    if (!dpy) {
        SetX11ErrorHandler();
        return false;
    }
    bool ret = false;
    Window rt;
    i32 x, y;//, rx, ry;
    u32 width, height, bw, depth_;
    if (XGetGeometry(dpy, windowId, &rt, &x, &y, &width, &height, &bw, &depth_)) {
        left = x;
        top = y;
        right = x + width;
        bottom = y + height;
        ret = true;
    }
    XCloseDisplay (dpy);
    SetX11ErrorHandler();
    return ret;
}

bool Window_SetRect(z64 windowId, i32 left, i32 top, i32 right, i32 bottom)
{
    SetSysInfoX11ErrorHandler();
    _XDisplay *dpy = XOpenDisplay (NULL);
    if (!dpy) {
        SetX11ErrorHandler();
        return false;
    }
    bool ret = false;

    if (XMoveWindow(dpy, windowId, left, top)) {
        if (!XResizeWindow(dpy, windowId, right-left, bottom-top))
            ret = false;
        else
            ret = true;
    }
    XCloseDisplay (dpy);
    SetX11ErrorHandler();
    return ret;
}

#endif


#ifdef TARGET_POSIX

void SetDesktopWallPaper(tukk path)
{
    Txt desktopManager = GetDesktopManagerNew();

    if (desktopManager == "gnome") {
        Sys("gconftool-2 -t str -s /desktop/gnome/background/picture_filename \"" + Txt(path) + "\"");
        Txt mode;
        if (*path == '\0')
            mode = "none";      // Vals "none", "wallpaper", "centered", "scaled", "stretched"
        else
            mode = "stretched";
        Sys("gconftool-2 -t str -s /desktop/gnome/background/picture_options \"" + mode + "\"");
    } else if (desktopManager == "kde") {
            // mode values
            // 1: disabled, only background color
            // 2: tiled with first image in top left corner
            // 3: tiled with first image centered
            // 4: centered stretched with proportions kept until one side hits screen, space filled by background color
            // 5: same as 4, though wallpaper aligned to top left and space after stretching filled by tiling
            // 6: stretched to fit screen
        i32 mode;
        if (*path == '\0')
            mode = 1;
        else
            mode = 6;
        Sys("dcop kdesktop KBackgroundIface setWallpaper \"" + Txt(path) + "\" " + AsTxt(mode));
    } else
        throw Exc(t_("Невозможно изменить обои рабочего стола"));
}

#endif

#if defined(TARGET_WIN32) || defined (TARGET_WIN64)

void SetDesktopWallPaper(tuk path)
{
    if (0 == SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, static_cast<LPVOID>(path), SPIF_UPDATEINIFILE|SPIF_SENDWININICHANGE))
        throw Exc(Txt(t_("Невозможно изменить обои рабочего стола")) + ": " + AsTxt(GetLastError()));
}
#endif



void SystemSignature::Load() {
    GetSystemInfo(manufacturer, productName, version, numberOfProcessors, mbSerial);
    hdSerial = GetHDSerial();
    userName = GetUserName();
#ifndef TARGET_OSX
    netAdapters = GetAdapterInfo();
#endif
}


Txt Serial_ReorderInPairs(Txt input) {
    Txt ret;
    for (i32 i = 0; i < input.GetCount(); i += 2)
        ret << char(input[i + 1]) << char(input[i]);
    return ret;
}

Txt Serial_HexToStr(Txt input) {
    return TrimBoth(HexDecode(input));
}

bool Serial_Check(Txt hdSerial, Txt other_hdSerial) {
    Vec<Txt> list, otherList;
    Txt toStr = Serial_HexToStr(hdSerial);
    list << hdSerial << Serial_ReorderInPairs(hdSerial) << toStr << Serial_ReorderInPairs(toStr);
    Txt toStrOther = Serial_HexToStr(other_hdSerial);
    otherList << other_hdSerial << Serial_ReorderInPairs(other_hdSerial) << toStrOther << Serial_ReorderInPairs(toStrOther);

    // Check them all
    for (i32 i = 0; i < list.GetCount(); ++i) {
        for (i32 j = 0; j < otherList.GetCount(); ++j)
            if (list[i] == otherList[j])
                return true;
    }
    return false;
}


bool SystemSignature::operator==(const SystemSignature &other) const {
//  if (!(manufacturer == other.manufacturer && productName == other.productName))
//      return false;       // Manufacturers do change manufacturer name and product name...

    if (!(mbSerial == other.mbSerial &&
          numberOfProcessors == other.numberOfProcessors &&
          userName == other.userName))
        return false;

    // hdSerial check
    if (Odd(hdSerial.GetCount()))
        return false;

    if (!Serial_Check(hdSerial, other.hdSerial))
        return false;

    for (i32 i = 0; i < netAdapters.GetCount(); ++i) {
        if (TrimBoth(netAdapters[i].mac).IsEmpty())
            continue;
        for (i32 j = 0; j < other.netAdapters.GetCount(); ++j)
            if(netAdapters[i].mac == other.netAdapters[j].mac)
                return true;
    }
    return false;
}

Vec<Txt> SystemSignature::GetDiff(const SystemSignature &other) const {
    Vec<Txt> list;

    if (userName != other.userName)
        list << Format("User name: %s != %s", userName, other.userName);
    if (manufacturer != other.manufacturer)
        list << Format("Manufacturer: %s != %s", manufacturer, other.manufacturer);
    if (productName != other.productName)
        list << Format("ProductName: %s != %s", productName, other.productName);
    if (version != other.version)
        list << Format("Version: %s != %s", version, other.version);
    if (mbSerial != other.mbSerial)
        list << Format("MBSerial: %s != %s", mbSerial, other.mbSerial);
    if (numberOfProcessors != other.numberOfProcessors)
        list << Format("NumberOfProcessors: %d != %d", numberOfProcessors, other.numberOfProcessors);
    if (hdSerial != other.hdSerial)
        list << Format("HDSerial: %s != %s", hdSerial, other.hdSerial);
    bool sameNetworkAdapters = false;
    for (i32 i = 0; i < netAdapters.GetCount(); ++i) {
        if (TrimBoth(netAdapters[i].mac).IsEmpty())
            continue;
        for (i32 j = 0; j < other.netAdapters.GetCount(); ++j) {
            if(netAdapters[i].mac == other.netAdapters[j].mac) {
                sameNetworkAdapters = true;
                break;
            }
        }
    }
    if (!sameNetworkAdapters)
        list << "Сетевые адаптеры не совпадают";
    return list;
}

void SystemSignature::Copy(const SystemSignature& src) {
    manufacturer = src.manufacturer;
    productName = src.productName;
    version = src.version;
    mbSerial = src.mbSerial;
    numberOfProcessors = src.numberOfProcessors;
    hdSerial = src.hdSerial;
    userName = src.userName;
    netAdapters.SetCount(src.netAdapters.GetCount());
    for (i32 i = 0; i < src.netAdapters.GetCount(); ++i)
        netAdapters[i] = src.netAdapters[i];
}

void SystemSignature::Xmlize(XmlIO &xml) {
    xml
        ("manufacturer", manufacturer)("productName", productName)("version", version)
        ("numberOfProcessors", numberOfProcessors)("mbSerial", mbSerial)("hdserial", hdSerial)
        ("userName", userName)("netAdapters", netAdapters)
    ;
}

void SystemSignature::Jsonize(JsonIO& json) {
    json
        ("manufacturer", manufacturer)("productName", productName)("version", version)
        ("numberOfProcessors", numberOfProcessors)("mbSerial", mbSerial)("hdserial", hdSerial)
        ("userName", userName)("netAdapters", netAdapters)
    ;
}

void SystemSignature::Serialize(Stream& stream) {
    stream % manufacturer % productName % version % numberOfProcessors % mbSerial % hdSerial
           % userName % netAdapters;
}

void SystemOverview::Load() {
    signature.Load();
    GetBiosInfo(biosVersion, biosReleaseDate, biosSerial);
    computerName = GetComputerName();
    GetOsInfo(kernel, kerVersion, kerArchitecture, distro, distVersion, desktop, deskVersion);
    i32 compilerBits;
    GetCompilerInfo(compilerName, compilerVersion, compilerTime, compilerMode, compilerBits);
}

void SystemOverview::Copy(const SystemOverview& src) {
    signature.Copy(src.signature);
    biosVersion = src.biosVersion;
    biosSerial = src.biosSerial;
    biosReleaseDate = src.biosReleaseDate;
    computerName = src.computerName;
    kernel = src.kernel;
    kerVersion = src.kerVersion;
    kerArchitecture = src.kerArchitecture;
    distro = src.distro;
    distVersion = src.distVersion;
    desktop = src.desktop;
    deskVersion = src.deskVersion;
    compilerName = src.compilerName;
    compilerVersion = src.compilerVersion;
    compilerTime = src.compilerTime;
    compilerMode = src.compilerMode;
}

void SystemOverview::Xmlize(XmlIO &xml) {
    xml
        ("signature", signature)("biosVersion", biosVersion)("biosSerial", biosSerial)
        ("biosReleaseDate", biosReleaseDate)("computerName", computerName)("kernel", kernel)
        ("kerVersion", kerVersion)("kerArchitecture", kerArchitecture)("distro", distro)
        ("distVersion", distVersion)("desktop", desktop)("deskVersion", deskVersion)
        ("compilerName", compilerName)("compilerVersion", compilerVersion)
        ("compilerTime", compilerTime)("compilerMode", compilerMode)
    ;
}

void SystemOverview::Jsonize(JsonIO& json) {
    json
        ("signature", signature)("biosVersion", biosVersion)("biosSerial", biosSerial)
        ("biosReleaseDate", biosReleaseDate)("computerName", computerName)("kernel", kernel)
        ("kerVersion", kerVersion)("kerArchitecture", kerArchitecture)("distro", distro)
        ("distVersion", distVersion)("desktop", desktop)("deskVersion", deskVersion)
        ("compilerName", compilerName)("compilerVersion", compilerVersion)
        ("compilerTime", compilerTime)("compilerMode", compilerMode)
    ;
}

void SystemOverview::Serialize(Stream& stream) {
    stream % signature % biosVersion % biosSerial % biosReleaseDate % computerName % kernel
         % kerVersion % kerArchitecture % distro % distVersion % desktop % deskVersion
         % compilerName % compilerVersion % compilerTime % compilerMode;
}

#ifdef TARGET_POSIX
static void GetPorts(SortedIndex<i32> &ports, tukk file) {
    TxtParse text;
    text = LoadFile_Safe(Txt("/proc/net/") + file);
    text.GetLine();
    while (true) {
        TxtParse line = text.GetLine();
        if (line.IsEmpty())
            break;
        line.GoAfter(":");
        line.GoAfter(":");
        Txt sport = line.GetText();
        u32 port;
        sscanf(~sport, "%x", &port);
        ports.FindAdd(port);
    }
}
#endif

i32 GetAvailableSocketPort(i32 from) {
    SortedIndex<i32> ports;
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
    DWORD size = sizeof(MIB_TCPTABLE);
    Buffer<char> table(size);
    MIB_TCPTABLE *pTable = reinterpret_cast<MIB_TCPTABLE*>(~table);
    if (GetTcpTable(pTable, &size, TRUE) == ERROR_INSUFFICIENT_BUFFER)
        table.Alloc(size);
    pTable = reinterpret_cast<MIB_TCPTABLE*>(~table);
    if (GetTcpTable(pTable, &size, TRUE) != NO_ERROR) {
        return Null;
    }
    for (i32 i = 0; i < static_cast<i32>(pTable->dwNumEntries); i++)
        ports.FindAdd(ntohs(static_cast<u_short>(pTable->table[i].dwLocalPort)));
#else
    GetPorts(ports, "tcp");
    GetPorts(ports, "raw");
    GetPorts(ports, "udp");
#endif
    i32 i;
    for (i = 0; i < ports.GetCount() && ports[i] < from; ++i)
        ;
    if (i == ports.GetCount())
        return from;
    for (; ports[i] == from; ++i, ++from)
        ;
    return from;
}

bool IsPortFree(i32 port) {
#ifdef TARGET_WIN32
    WSADATA wsaData;

    i32 res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != NO_ERROR)
        return false;
#endif

    SOCKET socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == INVALID_SOCKET)
        return false;

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(port);

    bool ret = true;
    if(bind(socket_desc, reinterpret_cast<struct sockaddr *>(&service), sizeof(service)) < 0)
        ret = false;
#ifdef TARGET_WIN32
    closesocket(socket_desc);
#else
    close(socket_desc);
#endif

#ifdef TARGET_WIN32
    WSACleanup();
#endif

    return ret;
}

}