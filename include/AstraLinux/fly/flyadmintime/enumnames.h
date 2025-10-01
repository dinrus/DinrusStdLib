#ifndef ENUMNAMES_H
#define ENUMNAMES_H

enum class ServiceState
{
    NotIstalled,
    Installed,
    Active,
    Synced
};

enum class ServiceEnable
{
    Disabled,
    Enabled
};


enum class PackageIndex
{
    ntp,
    linuxptp,
    openntpd,
    chrony,
    systemd,
    END_OF_LIST
};
PackageIndex& operator++( PackageIndex &c );
PackageIndex operator++( PackageIndex &c, int );

enum class ServerType
{
    Server,
    Pool,
    Fallback
};

enum class VersionOfSystemd
{
    v232,
    v241plus
};

#endif // ENUMNAMES_H
