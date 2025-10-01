// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include "SysInfo_in.h"


namespace drx {

#ifdef TARGET_POSIX
bool GetBatteryStatus(bool &discharging, i32 &percentage, i32 &remainingMin)
{
/* This is only if acpi package is present
    TxtParse data = Sys("acpi -V");

    data.GoAfter("AC Adapter", ":");
    Txt sacStatus = data.GetText();
    discharging = sacStatus != "on-line";
    data.GoInit();
    data.GoAfter("Battery", ":");
    data.GoAfter(",");
    percentage = data.GetInt("%");
    data.GoAfter(",");
    Txt remaining;
    if (discharging) {
        remaining = data.GetText(" ");
        i32 hour, min;
        double secs;
        TxtToHMS(remaining, hour, min, secs);    // It is really days:hour:min in this case
        remainingMin = i32(secs) + min*60 + hour*24*60;
    } else
        remainingMin = Null;
*/
    percentage = 100;
    Vec<Txt> files = SearchFile("/proc/acpi/battery", "state");
    if (files.GetCount() == 0)
        return false;
    TxtParse state = LoadFile_Safe(files[0]);
    if (state == "")
        return false;
    bool present;
    if(!state.GoAfter_Init("present", ":"))
        return false;
    present = state.GetText() == "yes";
    if (!present)
        return false;   // No battery inserted
    state.GoAfter_Init("charging state", ":");      discharging = state.GetText() == "discharging";
    i32 presentRate, remainingCapacity;
    state.GoAfter_Init("present rate", ":");        presentRate = state.GetInt();
    state.GoAfter_Init("remaining capacity", ":");  remainingCapacity = state.GetInt();
    if (presentRate == 0 || !discharging)
        remainingMin = Null;
    else
        remainingMin = (i32)((60.*remainingCapacity)/presentRate);

    //i32 designCapacity,lastFullCapacity;
    //Txt vendor, type, model, serial;
    //if (!GetBatteryInfo(present/*, designCapacity, lastFullCapacity, vendor, type, model, serial*/))
        //percentage = (i32)((100.*remainingCapacity)/lastFullCapacity);

    return true;
}

bool GetBatteryInfo(bool &present/*, i32 &designCapacity, i32 &lastFullCapacity, Txt &vendor, Txt &type, Txt &model, Txt &serial*/)
{
    Vec<Txt> files = SearchFile("/proc/acpi/battery", "info");
    if (files.GetCount() == 0)
        return false;
    TxtParse info = LoadFile_Safe(files[0]);
    if (info == "")
        return false;
    info.GoAfter_Init("present", ":");          present = info.GetText() == "yes";
    /*
    info.GoAfter_Init("design capacity", ":");  designCapacity = info.GetInt();
    info.GoAfter_Init("last full capacity", ":");lastFullCapacity = info.GetInt();
    info.GoAfter_Init("OEM info", ":");         vendor = info.GetText();
    info.GoAfter_Init("battery type", ":");     type = info.GetText();
    info.GoAfter_Init("model number", ":");     model = info.GetText();
    info.GoAfter_Init("serial number", ":");    serial = info.GetText();
    */
    return true;
}

bool OpenCDTray(Txt drive)
{
    Txt dummy;
    return Sys("eject", dummy) > 0;
}
bool CloseCDTray(Txt drive)
{
    Txt dummy;
    return Sys("eject -t", dummy) > 0;
}

#endif

#if defined(TARGET_WIN32) || defined (TARGET_WIN64)

bool GetBatteryStatus(bool &discharging, i32 &percentage, i32 &remainingMin)
{
    SYSTEM_POWER_STATUS power;

    if(::GetSystemPowerStatus(&power) != 1)
        return false;

    if (power.ACLineStatus == 1)
        discharging = false;
    else
        discharging = true;
    if (power.BatteryLifePercent <= 100)
        percentage = power.BatteryLifePercent;
    else
        percentage = Null;
    if (discharging && (i32(power.BatteryLifeTime) != -1))
        remainingMin = i32(power.BatteryLifeTime/60);
    else
        remainingMin = Null;
    return true;
}

bool GetBatteryInfo(bool &present/*, i32 &designCapacity, i32 &lastFullCapacity, Txt &vendor, Txt &type, Txt &model, Txt &serial*/)
{
    SYSTEM_POWER_STATUS power;

    if(::GetSystemPowerStatus(&power) == 0)
        return false;
    if (power.BatteryFlag == 128)
        return false;           // No battery

    //designCapacity = (i32)(power.BatteryFullLifeTime/60.);

    present = true;
    //power.ACLineStatus == 0;
    //lastFullCapacity = 0;
    //vendor = type = model = serial = "UNKNOWN";

    return true;
}

bool DriveOpenClose(Txt drive, bool open)
{
    i32 operation;
    if (open)
        operation = IOCTL_STORAGE_EJECT_MEDIA;
    else
        operation = IOCTL_STORAGE_LOAD_MEDIA;
    if (drive.IsEmpty())
        return false;
    else if (drive.GetCount() == 1)
        drive += ":";
    else {
        drive = drive.Left(2);
        if (drive[1] != ':')
            return false;
    }
    HANDLE hDrive;
    hDrive = CreateFile("\\\\.\\" + drive, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hDrive == INVALID_HANDLE_VALUE)
        return false;
    bool ret = false;
    DWORD dummyBytesReturned;
    if (DeviceIoControl(hDrive, operation, 0, 0, 0, 0, &dummyBytesReturned, 0))
        ret = true;
    CloseHandle(hDrive);
    return ret;
}

bool OpenCDTray(Txt drive)
{
    return DriveOpenClose(drive, true);
}

bool CloseCDTray(Txt drive)
{
    return DriveOpenClose(drive, false);
}

#endif

}