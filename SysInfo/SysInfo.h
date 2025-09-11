// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _SysInfo_SysInfo_h
#define _SysInfo_SysInfo_h

#include <drx/Funcs4U/Funcs4U.h>

namespace drx {

/////////////////////////////////////////////////////////////////////
// Processor Info
void GetSystemInfo(Txt &manufacturer, Txt &productName, Txt &version, i32 &numberOfProcessors, Txt &mbSerial);
void GetBiosInfo(Txt &biosVersion, Date &biosReleaseDate, Txt &biosSerial);
bool GetProcessorInfo(i32 number, Txt &vendor, Txt &identifier, Txt &architecture, i32 &speed);
i32 GetCpuSpeed();
double GetCpuTemperature();

struct NetAdapter : DeepCopyOption<NetAdapter> {
    Txt description;
    Txt fullname;
    Txt dnsSuffix;
    Txt mac;
    Txt type;
    Txt ip4, ip6;
    Txt gatewayip4, gatewayip6;
    bool connected;

    void Xmlize(XmlIO &xml);
    void Jsonize(JsonIO &json);
    void Serialize(Stream &stream);
    void Copy(const NetAdapter& src);
    NetAdapter(const NetAdapter& src, i32)  {Copy(src);}
    //NetAdapter(const NetAdapter& src)         {Copy(src);}
    NetAdapter() {}
};

drx::Array <NetAdapter> GetAdapterInfo();

bool GetNetworkInfo(Txt &name, Txt &domain, Txt &ip4, Txt &ip6);

i32 GetAvailableSocketPort(i32 from = 49152);
bool IsPortFree(i32 port);

#if defined(TARGET_WIN32)
bool GetVideoInfo(drx::Array <Val> &name, drx::Array <Val> &description, drx::Array <Val> &videoProcessor,
                  drx::Array <Val> &ram, drx::Array <Val> &videoMode);
bool GetPackagesInfo(drx::Array <Val> &name, drx::Array <Val> &version, drx::Array <Val> &vendor,
            drx::Array <Val> &installDate, drx::Array <Val> &caption, drx::Array <Val> &description, drx::Array <Val> &state);
#endif

Txt GetHDSerial();

/////////////////////////////////////////////////////////////////////
// Memory Info
bool GetMemoryInfo(i32 &memoryLoad, zu64 &totalPhys, zu64 &freePhys, zu64 &totalPageFile, zu64 &freePageFile, zu64 &totalVirtual, zu64 &freeVirtual);

/////////////////////////////////////////////////////////////////////
// Windows list
// They get arrays with handles to all the opened windows with additional info as
// pid: Handle to the process that manages the window
// name: Window name
// fileName: Window process program file name
// title: Window title (caption)
void GetWindowsList(Array<z64> &wid, Array<z64> &pid, Array<Txt> &name,
                    Array<Txt> &fileName, Array<Txt> &title, bool getAll = true);
Array<z64> GetWindowsList();

Rect GetDesktopRect();

bool Window_GetRect(z64 windowId, i32 &left, i32 &top, i32 &right, i32 &bottom);
bool Window_SetRect(z64 windowId, i32 left, i32 top, i32 right, i32 bottom);
#if defined(TARGET_WIN32)
void Window_Bottom(z64 windowId);
void Window_Top(z64 windowId);
void Window_TopMost(z64 windowId);
#endif

enum WINDOW_STATUS {
    WINDOW_MAXIMIZED, WINDOW_MINIMIZED, WINDOW_NORMAL};
i32 Window_GetStatus(z64 windowId);

/////////////////////////////////////////////////////////////////////
// Process list
bool GetProcessList(Array<z64> &pid, Array<Txt> &pNames);
Array<z64> GetProcessList();
Txt GetProcessName(z64 pid);
Txt GetProcessFName(z64 processID);

#if defined(TARGET_WIN32)
i32 GetProcessCPUUsage(z64 pid);
#endif

z64 GetProcessIdFromWindowCaption(Txt windowCaption, bool exactMatch = false);
z64 GetProcessIdFromName(Txt name);

z64 GetWindowIdFromCaption(Txt windowCaption, bool exactMatch = false);

z64 GetProcessIdFz64romWindowId(z64 wid);
z64 GetWindowIdFromProcessId(z64 pid);

bool ProcessTerminate(z64 pid, i32 timeout = 500);

i32 GetProcessPriority(z64 pid);
bool SetProcessPriority(z64 pid, i32 priority);

bool ProcessExists(z64 pid);

/////////////////////////////////////////////////////////////////////
// Os Info
bool GetOsInfo(Txt &kernel, Txt &kerVersion, Txt &kerArchitecture, Txt &distro, Txt &distVersion, Txt &desktop, Txt &deskVersion);
/////////////////////////////////////////////////////////////////////
// Get drives info
bool GetDriveSpace(Txt drive, zu64 &freeBytesUser, zu64 &totalBytesUser, zu64 &totalFreeBytes);
bool GetDriveInformation(Txt drive, Txt &type, Txt &volume, /*zu64 &serial, */i32 &maxName, Txt &fileSystem);

/////////////////////////////////////////////////////////////////////
// Others
z64 GetProcessId();

bool Shutdown(Txt action);

void GetCompilerInfoAux(Txt &name, i32 &version, drx::Time &time, Txt &mode, i32 &bits, tukk sdate, tukk stime);
#define GetCompilerInfo(name, version, time, mode, bits)    GetCompilerInfoAux(name, version, time, mode, bits,"","")// __DATE__, __TIME__)

bool GetBatteryStatus(bool &discharging, i32 &percentage, i32 &remainingMin);
bool GetBatteryInfo(bool &present/*, i32 &designCapacity, i32 &lastFullCapacity, Txt &vendor, Txt &type, Txt &model, Txt &serial*/);

bool OpenCDTray(Txt drive);
bool CloseCDTray(Txt drive);

/////////////////////////////////////////////////////////////////////
// Key and mouse keys
bool Mouse_GetPos(i32 &x, i32 &y);
bool Mouse_SetPos(i32 x, i32 y, z64 windowId = 0);

#if defined(TARGET_WIN32) || !defined(flagNO_XTEST)
void Mouse_LeftClick();
void Mouse_LeftDown();
void Mouse_LeftUp();
void Mouse_MiddleClick();
void Mouse_MiddleDown();
void Mouse_MiddleUp();
void Mouse_RightClick();
void Mouse_RightDown();
void Mouse_RightUp();
void Mouse_LeftDblClick();
void Mouse_MiddleDblClick();
void Mouse_RightDblClick();

void Keyb_SendKeys(Txt text, long finalDelay = 100, long delayBetweenKeys = 50);
#endif

bool Window_SaveCapture(z64 windowId, Txt fileName, i32 left = -1, i32 top = -1, i32 width = -1, i32 height = -1);
Image Window_SaveCapture(z64 windowId, i32 left = -1, i32 top = -1, i32 width = -1, i32 height = -1);

bool Snap_Desktop(Txt fileName);
bool Snap_DesktopRectangle(Txt fileName, i32 left, i32 top, i32 width, i32 height);
bool Snap_Window(Txt fileName, z64 handle);
Image Snap_Desktop();
Image Snap_DesktopRectangle(i32 left, i32 top, i32 width, i32 height);
Image Snap_Window(z64 handle);

#if !defined(flagNO_XTEST)
bool GetKeyLockStatus(bool &caps, bool &num, bool &scroll);
bool SetKeyLockStatus(bool caps, bool num, bool scroll);
#endif

#if defined(TARGET_WIN32)

bool Record_Desktop(Txt fileName, i32 duration, double secsFrame = 1, bool viewMouse = true);
bool Record_DesktopRectangle(Txt fileName, i32 duration, i32 left, i32 top, i32 width, i32 height, double secsFrame = 1, bool viewMouse = true);
bool Record_Window(Txt fileName, i32 duration, z64 handle, double secsFrame = 1, bool viewMouse = true);

#endif

void SetDesktopWallPaper(tuk path);

struct SystemSignature : DeepCopyOption<SystemSignature> {
    Txt manufacturer, productName, version, mbSerial;
    i32 numberOfProcessors;
    Txt hdSerial;
    Txt userName;
    drx::Array <NetAdapter> netAdapters;

    void Load();
    Txt ToTxt() const         {return StoreAsJson(*this, true);}
    operator Txt() const         {return ToTxt();}
    operator tukk () const   {return ToTxt();}
    Txt operator~() const        {return ToTxt();}
    void Copy(const SystemSignature& src);
    SystemSignature(const SystemSignature& src, i32) {Copy(src);}
    SystemSignature()   {}
    void Xmlize(XmlIO &xml);
    void Jsonize(JsonIO &json);
    void Serialize(Stream &stream);
    bool operator==(const SystemSignature &other) const;
    bool operator!=(const SystemSignature &other) const {return !(*this == other);}
    Vec<Txt> GetDiff(const SystemSignature &other) const;
};

struct SystemOverview : DeepCopyOption<SystemOverview> {
    SystemSignature signature;
    Txt biosVersion, biosSerial;
    Date biosReleaseDate;
    Txt computerName;
    Txt kernel;
    Txt kerVersion;
    Txt kerArchitecture;
    Txt distro;
    Txt distVersion;
    Txt desktop;
    Txt deskVersion;
    Txt compilerName;
    i32 compilerVersion;
    drx::Time compilerTime;
    Txt compilerMode;

    void Load();
    Txt ToTxt() const         {return StoreAsJson(*this, true);}
    operator Txt() const         {return ToTxt();}
    operator tukk () const   {return ToTxt();}
    Txt operator~() const        {return ToTxt();}
    void Copy(const SystemOverview& src);
    SystemOverview(const SystemOverview& src, i32) { Copy(src); }
    SystemOverview() {}
    void Xmlize(XmlIO &xml);
    void Jsonize(JsonIO &json);
    void Serialize(Stream &stream);
};

#if defined(TARGET_WIN32) || !defined(flagNO_XTEST)

class SimulateActivity {
public:
    SimulateActivity(i32 _deltaTime,
        i32 _fromH = Null, i32 _fromM = Null, i32 _toH = Null, i32 _toM = Null) :
        deltaTime(_deltaTime), fromH(_fromH), fromM(_fromM), toH(_toH), toM(_toM), rng(123987) {
#if defined(TARGET_WIN32)
        if (::CreateMutex(0, true, "__msdxp__")) {
            if (GetLastError() == ERROR_ALREADY_EXISTS)
                active = false;
        }
#endif
        nextDeltaTime = deltaTime + std::uniform_int_distribution<i32>(53*1000, 67*1000)(rng);
    }
    void DoActivity() {
        if (!active)
            return;

        Time tm = GetSysTime();
        if (!IsNull(fromH) && (tm.hour > fromH && tm.minute > fromM) && (tm.hour < toH && tm.minute < toM))
            return;

        i32 x, y;
        Mouse_GetPos(x, y);
        if (x != x0 && y != y0) {
            timer.Reset();
            x0 = x;
            y0 = y;
            nextDeltaTime = deltaTimeFast + std::uniform_int_distribution<i32>(5*1000, 10*1000)(rng);
        } else {
            if (timer.Elapsed() > nextDeltaTime) {
                Keyb_SendKeys("{INSERT}{INSERT}", 10);
                timer.Reset();
                std::uniform_int_distribution<i32> rand1min(53*1000, 67*1000);
                nextDeltaTime = deltaTime + std::uniform_int_distribution<i32>(53*1000, 67*1000)(rng);
            }
        }
    }
    bool IsActive() {return active;}

private:
    TimeStop timer;
    i32 x0 = -1, y0 = -1;
    dword deltaTime, nextDeltaTime;
    dword deltaTimeFast = 20*1000;
    i32 fromH, fromM, toH, toM;
    std::mt19937 rng;
    bool active = true;
};
#endif

}

#endif

// Known bugs
// GetWindowsList does not get the window title in Kde
// Shutdown in Linux only works with option "logoff", probably because of user permissions
