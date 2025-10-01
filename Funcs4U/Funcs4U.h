// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _Functions4U_Functions4U_h
#define _Functions4U_Functions4U_h

#include <float.h>
#include <drx/Draw/Draw.h>
#ifdef flagGUI
#include <drx/Funcs4U/GatherTpp.h>
#endif

#include "SvgColors.h"
#include "StaticPlugin.h"
#include "LocalProcess2.h"
#include <random>

namespace drx {

/*
enum EXT_FILE_FLAGS {
    NO_FLAG = 0,
    USE_TRASH_BIN = 1,
    BROWSE_LINKS = 2,
    DELETE_READ_ONLY = 4
};
*/

Txt GetDesktopManagerNew();

bool LaunchFile(tukk file, tukk params = nullptr, tukk directory = ".");

bool FileCat(tukk file, tukk appendFile);

i32 FileCompare(tukk path1, tukk path2);

z64 FindTxtInFile(tukk file, const Txt text, z64 pos0 = 0);

bool FileStrAppend(tukk file, tukk str);
bool AppendFile(tukk filename, tukk str);

template<typename T>
Txt AppendFNameX(T t) {
    return t;
}

template<typename T, typename... Args>
Txt AppendFNameX(T t, Args... args) {
    return AppendFName(t, AppendFNameX(args...));
}

inline Txt Trim(const Txt& s)   {return TrimBoth(s);}

Txt FitFName(Txt fileName, i32 len);

Vec<Txt> Tokenize(const Txt &str, const Txt &token, i32 pos = 0);
void Tokenize(const Txt &str, const Txt &token, Vec<Txt> &ret, i32 pos = 0);
Txt Tokenize2(const Txt &str, const Txt &token, i32 &pos);
Txt Tokenize2(const Txt &str, const Txt &token);
Txt GetLine(const Txt &str, i32 &pos);

/////////
bool DirExistsX(tukk path, EXT_FILE_FLAGS flags = NO_FLAG);
void DirCopyX(tukk dir, tukk newPlace, bool replaceOnlyNew, Txt filesToExclude, Txt &errorList);
bool DirMove(tukk dir, tukk newPlace);
bool DeleteDeepWildcardsX(tukk path, bool filefolder, EXT_FILE_FLAGS flags = NO_FLAG, bool deep = true);
bool DeleteDeepWildcardsX(tukk pathwc, tukk namewc, bool filefolder, EXT_FILE_FLAGS flags = NO_FLAG, bool deep = true);
bool DeleteFolderDeepWildcardsX(tukk path, EXT_FILE_FLAGS flags = NO_FLAG);
bool DeleteFolderDeepWildcardsX(tukk path, tukk name, EXT_FILE_FLAGS flags = NO_FLAG);
bool DeleteFileDeepWildcardsX(tukk path, EXT_FILE_FLAGS flags = NO_FLAG);
bool DeleteFileWildcardsX(tukk path, EXT_FILE_FLAGS flags = NO_FLAG);
bool DeleteFolderDeepX(tukk path, EXT_FILE_FLAGS flags = NO_FLAG);
bool RenameDeepWildcardsX(tukk path, tukk namewc, tukk newname, bool forfile, bool forfolder, EXT_FILE_FLAGS flags = NO_FLAG);
bool FolderIsEmpty(tukk path);

bool DirCreateX(tukk path);

//bool FileMoveX(tukk oldpath, tukk newpath, EXT_FILE_FLAGS flags = NO_FLAG);
bool FileDeleteX(tukk path, EXT_FILE_FLAGS flags = NO_FLAG);

bool IsRootFolder(tukk folderName);
Txt GetUpperFolder(const Txt &folderName);
Txt GetNextFolder(const Txt &folder, const Txt &lastFolder);
Txt FileRealName(tukk fileName);
bool IsFile(tukk fileName);
bool IsFolder(tukk fileName);
Txt GetRelativePath(Txt from, Txt path, bool normalize = true);
Txt GetAbsolutePath(Txt from, Txt relative);

bool IsSymLink(tukk path);

//bool SetReadOnly(tukk path, bool readOnly);
//bool SetReadOnly(tukk path, bool usr, bool grp, bool oth);
//bool IsReadOnly(tukk path, bool &usr, bool &grp, bool &oth);

Txt LoadFile_Safe(const Txt fileName);
Txt LoadFile(tukk fileName, off_t from, size_t len = 0);

z64 GetLength(tukk fileDirName);
z64 GetDirLength(tukk directoryName);

///////////////////////////////
Vec<Txt> SearchFile(Txt dir, const Vec<Txt> &condFiles, const Vec<Txt> &condFolders,
                                 const Vec<Txt> &extFiles,  const Vec<Txt> &extFolders,
                                 const Txt text, Vec<Txt> &errorList);
Vec<Txt> SearchFile(Txt dir, Txt condFile, Txt text, Vec<Txt> &errorList);//, i32 flags = 0);
Vec<Txt> SearchFile(Txt dir, Txt condFile = "*.*", Txt text = "");//, i32 flags = 0);
///////////////////////////////

bool FileToTrashBin(tukk path);
z64 TrashBinGetCount();
bool TrashBinClear();

Txt GetPersonalFolder();
Txt GetRootFolder();
Txt GetTempFolder();
Txt GetOsFolder();
Txt GetSystemFolder();
#ifdef TARGET_WIN32
Txt GetCommonAppDataFolder();
#endif
//bool SetEnv(tukk id, tukk val);

struct FileData : Moveable<FileData> {
    bool isFolder;
    Txt fileName;
    Txt relFilename;
    z64 length;
    struct drx::Time t;
    z64 id;

    Txt ToTxt() const { return Format("%s %0n", fileName, length); }

    FileData(bool _isFolder, Txt _fileName, Txt _relFilename, z64 _length,
        struct drx::Time _t, z64 _id) : isFolder(_isFolder), fileName(_fileName),
        relFilename(_relFilename), length(_length), t(_t), id(_id) {}
    FileData() {}
};

struct FileDiffData {
    char action;    // 'n': New, 'u': Update, 'd': Delete, 'p': Problem
    bool isFolder;
    Txt relPath;
    Txt fileName;
    zu64 idMaster, idSecondary;
    struct drx::Time tMaster, tSecondary;
    zu64 lengthMaster, lengthSecondary;
};

class ErrorHandling {
public:
    void SetLastError(Txt _lastError)    {lastError = _lastError;};
    Txt GetLastError()                   {return lastError;};

private:
    Txt lastError;
};

class FileDiffArray;

class FileDataArray : public ErrorHandling {
public:
    FileDataArray(bool use = false, i32 fileFlags = 0);
    bool Init(Txt folder, FileDataArray &orig, FileDiffArray &diff);
    void Clear();
    bool Search(Txt dir, Txt condFile, bool recurse = false, Txt text = "");
    FileData& operator[](long i)    {return fileList[i];}
    long GetFileCount()             {return fileCount;};
    long GetFolderCount()           {return folderCount;};
    long GetCount()                 {return fileCount + folderCount;};
    z64 GetSize()                 {return fileSize;};
    inline bool UseId()             {return useId;};
    void SortByName(bool ascending = true);
    void SortByDate(bool ascending = true);
    void SortBySize(bool ascending = true);
    Vec<Txt> &GetLastError()  {return errorList;};
    i32 Find(Txt &relFName, Txt &fileName, bool isFolder);
    i32 Find(FileDataArray &data, i32 id);
    Txt FullFName(i32 i)      {return AppendFNameX(basePath, fileList[i].fileName);};
    bool SaveFile(tukk fileName);
    bool AppendFile(tukk fileName);
    bool LoadFile(tukk fileName);

private:
    void Search_Each(Txt dir, Txt condFile, bool recurse, Txt text);
    z64 GetFileId(Txt fileName);
    Txt GetRelativePath(const Txt &fullPath);
    Txt GetFileText();

    Array<FileData> fileList;
    Vec<Txt> errorList;
    Txt basePath;
    long fileCount, folderCount;
    z64 fileSize;
    bool useId;
    i32 fileFlags;
};

class FileDiffArray : public ErrorHandling {
public:
    FileDiffArray();
    void Clear();
    FileDiffData& operator[](long i)    {return diffList[i];}
    bool Compare(FileDataArray &master, FileDataArray &secondary, const Txt folderFrom,
        Vec<Txt> &excepFolders, Vec<Txt> &excepFiles, i32 sensSecs = 0);
    bool Apply(Txt toFolder, Txt fromFolder, EXT_FILE_FLAGS flags = NO_FLAG);
    long GetCount()             {return diffList.size();};
    bool SaveFile(tukk fileName);
    bool LoadFile(tukk fileName);
    Txt ToTxt();

private:
    Array<FileDiffData> diffList;
};

Txt Replace(Txt str, Txt find, Txt replace);
Txt Replace(Txt str, char find, char replace);

i32 ReverseFind(const Txt& s, const Txt& toFind, i32 from = 0);

Txt FormatLong(long a);

drx::Time StrToTime(tukk s);
Date StrToDate(tukk s);

Txt BytesToTxt(zu64 bytes, bool units = true);

Txt SecondsToTxt(double seconds, i32 dec = 2, bool units = false, bool space = false,
                        bool tp = false, bool longUnits = false, bool forceSec = false);
Txt HMSToTxt(i32 hour, i32 min, double seconds, i32 dec = 2, bool units = false, bool space = false,
                        bool tp = false, bool longUnits = false, bool forceSec = false);
double TxtToSeconds(Txt str);
void TxtToHMS(Txt durat, i32 &hour, i32 &min, double &seconds);

Txt SeasonName(i32 iseason);
i32 GetSeason(Date &date);

i32 NumAdvicedDigits(double d, double range);
Txt FormatDoubleAutosize(double d);
Txt FormatDoubleAutosize(double d, double range);
Txt FormatDoubleSize(double d, i32 fieldWidth, bool fillSpaces = false);
#define FDAS    FormatDoubleAutosize
#define FDS     FormatDoubleSize

Txt RemoveAccents(Txt str);
Txt RemoveAccent(wchar c);
bool IsPunctuation(wchar c);
Txt RemovePunctuation(Txt str);

template<typename T>
inline T ToRad(T angle) {
    static_assert(std::is_floating_point<T>::value, "Тип должен быть с плавающей запятой");
    return angle*M_PI/180.;     // If not, this division will be zero
}
template <class Range>
Range ToRadArray(const Range& r) {
    Range ret(r.size());
    for (i32 i = 0; i < r.size(); i++)
        ret[i] = ToRad(r[i]);
    return ret;
}

template<typename T>
inline T ToDeg(T angle) {
    static_assert(std::is_floating_point<T>::value, "Тип должен быть с плавающей запятой");
    return angle*180./M_PI;     // If not, this division will be zero
}
template <class Range>
Range ToDegArray(const Range& r) {
    Range ret(r.size());
    for (i32 i = 0; i < r.size(); i++)
        ret[i] = ToDeg(r[i]);
    return ret;
}

template<typename T>
inline T atan2_360(T y, T x) {
    static_assert(std::is_floating_point<T>::value, "Тип должен быть с плавающей запятой");
    T ret = ToDeg(atan2(y, x));
    return ret > 90 ? 450 - ret : 90 - ret;
}

inline bool Odd(i32 val)            {return val%2;}
inline bool Even(i32 val)           {return !Odd(val);}
inline i32 RoundEven(i32 val)       {return Even(val) ? val : val+1;}
template<class T>
inline i32 Sign(T a)                {return (a > 0) - (a < 0);}
template<class T>
inline T Neg(T a)                   {return a > 0 ? -a : a;}
template<class T>
inline T Average(T a, T b)          {return T(a+b)/T(2);}
template<class T>
inline T Avg(T a, T b)              {return Average(a, b);}
template<class T>
inline T Average(T a, T b, T c)     {return T(a+b+c)/T(3);}
template<class T>
inline T Avg(T a, T b, T c)         {return Average(a, b, c);}
template<class T>
inline T Average(T a, T b, T c, T d){return T(a+b+c+d)/T(4);}
template<class T>
inline T Avg(T a, T b, T c, T d)    {return Average(a, b, c, d);}
template<class T>
inline T pow2(T a) {return a*a;}
template<class T>
inline T pow3(T a) {return a*a*a;}
template<class T>
inline T pow4(T a) {return pow2(pow2(a));}
template<class T>
inline T fround(T x, i32 numdec) {
    T pow10 = pow(10, numdec);
    return round(x*pow10)/pow10;
}

template <typename T>
inline bool IsNum(const std::complex<T> &n) {return IsFin(n.real()) && IsFin(n.imag());}
inline bool IsNum(double n) {return IsFin(n) && !IsNull(n);}
inline bool IsNum(float n)  {return IsFin(n);}
inline bool IsNum(i32 n)    {return !IsNull(n);}

template <typename T>
bool IsNull(const std::complex<T> &d)   {return !IsNum(d);};

#define NaNComplex      std::numeric_limits<std::complex<double>>::quiet_NaN()
#define NaNDouble       std::numeric_limits<double>::quiet_NaN()

template<class T>
T AvgSafe(const T &a, const T &b) {
    if (IsNum(a)) {
        if (IsNum(b))
            return Avg(a, b);
        else
            return a;
    } else
        return b;
}

template<class T>
inline T Nvl2(T a, T b) {return IsFin(a) && !IsNull(a) ? a : b;}

template<class T>
inline T Nvl2(T cond, T a, T b) {return IsFin(cond) && !IsNull(cond) ? a : b;}

template<typename T>
T fact(T val) {
    if (val <= 0)
        throw std::invalid_argument("fact() принимает только положительные числа ненулевого значения");
    T ret = 1;
    while (val > 1)
        ret *= val--;
    return ret;
}

template <class T>
inline bool Between(const T& val, const T& min, const T& max) {
    ASSERT(max > min);
    return val >= min && val <= max;
}
template <class T>
inline bool Between(const T& val, const T& range) {
    ASSERT(range > 0);
    return val >= -range && val <= range;
}
template <class T>
inline T BetweenVal(const T& val, const T& _min, const T& _max) {
    return max(_min, min(_max, val));
}
template <class T>
inline T FixFloat(T val) {
    if(std::isnan(val) || std::isinf(val) || val == HUGE_VAL || val == -HUGE_VAL)
        return Null;
    return val;
}

template <class T>
T AngleAdd360(T ang, T val) {
    ang += val;
    while (ang >= 360)
        ang -= 360;
    while (ang < 0)
        ang += 360;
    return ang;
}

template <class T>
inline const T Norm(const T& dx, const T& dy)  {
    return static_cast<T>(sqrt(dx*dx + dy*dy)); }

template <class T>
inline const T Distance(const T& x1, const T& y1, const T& x2, const T& y2)  {
    return Norm(x1-x2, y1-y2); }

template <class T>
inline const T Distance(const Point_<T>& p1, const Point_<T>& p2)  {
    return Distance<T>(p1.x, p1.y, p2.x, p2.y); }

template <class T>
inline const T Distance(const T& x1, const T& y1, const T& z1, const T& x2, const T& y2, const T& z2)  {
    return static_cast<T>(sqrt(pow2(x1-x2) + pow2(y1-y2) + pow2(z1-z2))); }

template <class T>
inline const T Angle(const T& x1, const T& y1, const T& x2, const T& y2)  {
    return static_cast<T>(atan2(y2-y1, x2-x1));
}

template <class T>
inline const T Angle(const Point_<T>& p1, const Point_<T>& p2)  {
    return Angle<T>(p1.x, p1.y, p2.x, p2.y);
}

template <class T>
inline const Point_<T> Middle(const Point_<T>& p1, const Point_<T>& p2)  {
    return Point_<T>(Avg(p1.x, p2.x), Avg(p1.y, p2.y));
}

Vec<Vec <Val> > ReadCSV(const Txt &strFile, char separator = ',', bool bycols = true, bool removeRepeated = true, char decimalSign = '.', bool onlyTxts = false, i32 fromRow = 0);
Vec<Vec <Val> > ReadCSVFile(const Txt &fileName, char separator = ',', bool bycols = true, bool removeRepeated = true, char decimalSign = '.', bool onlyTxts = false, i32 fromRow = 0);
bool ReadCSVFileByLine(const Txt &fileName, Gate<i32, Vec<Val>&, Txt &> WhenRow, char separator = ',', char decimalSign = '.', bool onlyTxts = false, i32 fromRow = 0);
Txt WriteCSV(Vec<Vec <Val> > &data, char separator = ',', bool bycols = true, char decimalSign = '.');
bool WriteCSVFile(const Txt &fileName, Vec<Vec <Val> > &data, char separator = ',', bool bycols = true, char decimalSign = '.');
bool GuessCSV(const Txt &fileName, bool onlyNumbers, Txt &header, Vec<Txt> &parameters, char &separator, bool &repetition, char &decimalSign, z64 &beginData, i32 &beginDataRow);

// A Txt based class to parse into
class TxtParse : public Txt {
public:
    explicit TxtParse() : Txt("") {GoInit();};
    TxtParse(Txt _s): Txt(_s)  {GoInit();};

    void GoInit()   {pos = 0; lastSeparator='\0';};
    bool GoBefore(const Txt text) {
        if (pos >= GetLength()) {
            pos = GetLength()-1;
            return false;
        }
        i32 newpos = Txt::Find(text, pos);
        if (newpos < 0)
            return false;   // If it does not find it, it does not move
        pos = newpos;
        return true;
    };
    bool GoAfter(const Txt text) {
        if(!GoBefore(text))
            return false;
        pos += i32(strlen(text));
        return true;
    };
    bool GoAfter(const Txt text, const Txt text2) {
        if(!GoAfter(text))
            return false;
        if(!GoAfter(text2))
            return false;
        return true;
    };
    bool GoAfter(const Txt text, const Txt text2, const Txt text3) {
        if(!GoAfter(text))
            return false;
        if(!GoAfter(text2))
            return false;
        if(!GoAfter(text3))
            return false;
        return true;
    };
    bool GoAfter_Init(const Txt text) {GoInit(); return GoAfter(text);};
    bool GoAfter_Init(const Txt text, const Txt text2) {GoInit(); return GoAfter(text, text2);};
    bool GoAfter_Init(const Txt text, const Txt text2, const Txt text3) {GoInit(); return GoAfter(text, text2, text3);};

    void GoBeginLine() {
        for (; pos >= 0; --pos) {
            if ((ToTxt()[pos-1] == '\r') || (ToTxt()[pos-1] == '\n'))
                return;
        }
    }
    bool IsBeginLine() {
        if (pos == 0)
            return true;
        if ((ToTxt()[pos-1] == '\r') || (ToTxt()[pos-1] == '\n'))
            return true;
        return false;
    }
    bool IsSpaceRN(i32 c) {
        if (IsSpace(c))
            return true;
        if ((c == '\r') || (c == '\n'))
             return true;
        return false;
    }
    // Gets text between "" or just a word until an space
    // It considers special characters with \ if between ""
    // If not between "" it gets the word when it finds one of the separator characters
    Txt GetText(Txt separators = "") {
        Txt ret = "";
        if (pos > GetCount() || pos == -1)
            return ret;
        i32 newpos = pos;

        while ((IsSpaceRN(ToTxt()[newpos]) && (ToTxt()[newpos] != '\"') &&
               (ToTxt()[newpos] != '\0')))
            newpos++;
        if (ToTxt()[newpos] == '\0') {
            pos = newpos;
            return "";
        }

        if (ToTxt()[newpos] == '\"') {   // Between ""
            newpos++;
            while (ToTxt()[newpos] != '\"' && ToTxt()[newpos] != '\0') {
                if (ToTxt()[newpos] == '\\') {
                    newpos++;
                    if (ToTxt()[newpos] == '\0')
                        return "";
                }
                ret.Cat(ToTxt()[newpos]);
                newpos++;
            }
            lastSeparator = '"';
        } else if (separators == "") {      // Simple word
            while (!IsSpaceRN(ToTxt()[newpos]) && ToTxt()[newpos] != '\0') {
                if (ToTxt()[newpos] == '\"') {
                    newpos--;   // This " belongs to the next
                    break;
                }
                ret.Cat(ToTxt()[newpos]);
                newpos++;
            }
            lastSeparator = ToTxt()[newpos];
        } else {                            // Simple word, special separator
            while (ToTxt()[newpos] != '\0') {// Only consider included spaces (!IsSpaceRN(ToTxt()[newpos]) && ToTxt()[newpos] != '\0') {
                if (ToTxt()[newpos] == '\"') {
                    newpos--;   // This " belongs to the next
                    break;
                }
                if (separators.Find(ToTxt()[newpos]) >= 0) {
                    lastSeparator = ToTxt()[newpos];
                    break;
                }
                ret.Cat(ToTxt()[newpos]);
                newpos++;
            }
            lastSeparator = ToTxt()[newpos];
        }
        pos = ++newpos;     // After the separator: ", space or separator
        return ret;
    }
    Txt GetLine() {
        Txt ret;
        if (pos > GetCount() || pos == -1)
            return Txt();
        while (ToTxt()[pos] != '\0') {
            if (ToTxt()[pos] == '\n') {
                pos++;
                return ret;
            }
            if (ToTxt()[pos] == '\r' && ToTxt()[pos+1] == '\n') {
                pos += 2;
                return ret;
            }
            ret.Cat(ToTxt()[pos]);
            pos++;
        }
        return ret;
    }
    double GetDouble(Txt separators = "")    {return FixFloat(atof(GetText(separators)));};
    i32 GetInt(Txt separators = "")          {return static_cast<i32>(FixFloat(atof(GetText(separators))));};
    long GetLong(Txt separators = "")        {return static_cast<long>(FixFloat(atof(GetText(separators))));};
    zu64 GetUInt64(Txt separators = "")    {return static_cast<zu64>(FixFloat(atof(GetText(separators))));};

    Txt Right()          {return Txt::Mid(pos+1);}
    i32 GetLastSeparator()  {return lastSeparator;}
    void MoveRel(i32 val) {
        pos += val;
        if (pos < 0)
            pos = 0;
        else if (pos >= GetCount())
            pos = GetCount() - 1;
    }
    i32 GetPos() {return pos;};
    bool SetPos(i32 i)
    {
        if (i < 0 || i >= GetCount())
            return false;
        else {
            pos = i;
            return true;
        }
    }
    bool Eof()
    {
        return pos >= GetCount();
    }
    unsigned Count(Txt _s)
    {
        i32 from = 0;
        unsigned count = 0;

        while ((from = ToTxt().Find(_s, from)) >= 0) {
            count++;
            from++;
        }
        return count;
    }
private:
    i32 pos;
    i32 lastSeparator;
};

#if defined(TARGET_WIN32)
Val GetVARIANT(VARIANT &result);
Txt WideToTxt(LPCWSTR wcs, i32 len = -1);
bool TxtToWide(Txt str, LPCWSTR &wcs);
bool BSTRSet(const Txt str, BSTR &bstr);
Txt BSTRGet(BSTR &bstr);
#endif


Txt GetExtExecutable(const Txt ext);

Vec<Txt> GetDriveList();

#define DLLFunction(dll, ret, function, args) auto function = (ret(*)args)dll.GetFunction_throw(#function)
#define DLLGetFunction(dll, ret, function, args)              (ret(*)args)dll.GetFunction_throw(#function)

class Dl {
public:
    virtual ~Dl();
    bool Load(const Txt &fileDll);
    uk GetFunction(const Txt &functionName) const;
    uk GetFunction_throw(const Txt &functionName) const;
#if defined(TARGET_WIN32)
    HINSTANCE GetHandle() {return hinstLib;}
#else
    uk GetHandle()     {return hinstLib;}
#endif
    operator bool() const {return hinstLib;}

private:
#if defined(TARGET_WIN32)
    HINSTANCE hinstLib = 0;
#else
    uk hinstLib = 0;
#endif
};

typedef Dl Dll;



Txt BsGetLastError();
bool BSPatch(Txt oldfile, Txt newfile, Txt patchfile);
bool BSDiff(Txt oldfile, Txt newfile, Txt patchfile);


template <class T>
Rect_<T> FitInFrame(const Size_<T> &frame, const Size_<T> &object)
{
    double frameAspect  = frame.cx/static_cast<double>(frame.cy);
    double objectAspect = object.cx/static_cast<double>(object.cy);

    if (frameAspect > objectAspect) {
        double x = (frame.cx - objectAspect*frame.cy)/2.;
        return Rect_<T>(static_cast<T>(x), 0, static_cast<T>(x + objectAspect*frame.cy), frame.cy);
    } else {
        double y = (frame.cy - frame.cx/objectAspect)/2.;
        return Rect_<T>(0, static_cast<T>(y), frame.cx, static_cast<T>(y + frame.cx/objectAspect));
    }
}

Color RandomColor();

Image GetRect(const Image& orig, const Rect &r);

double tmGetTimeX();

i32 SysX(tukk cmd, Txt& out, Txt& err, double timeOut = Null,
            Gate3<double, Txt&, Txt&> progress = false, bool convertcharset = true);


class _NRFuse {
public:
    explicit _NRFuse(bool *_inside) {inside = _inside; failed = true;}
    virtual ~_NRFuse()              {if (!failed) *inside = false;}
    bool failed;
private:
    bool *inside;
};

#define NON_REENTRANT_V     static bool _insideNR; _NRFuse _fuseNR(&_insideNR); \
                            if(!_insideNR) {                                    \
                                _insideNR = true;                               \
                                _fuseNR.failed = false;                         \
                            } else                                              \
                                return
#define NON_REENTRANT(v)    static bool _insideNR; _NRFuse _fuseNR(&_insideNR); \
                            if(!_insideNR) {                                    \
                                _insideNR = true;                               \
                                _fuseNR.failed = false;                         \
                            } else                                              \
                                return v

template <class T>
struct TempAssign {
    TempAssign(T &_variable, T newvalue) : oldvalue(_variable), variable(&_variable) {
        *variable = newvalue;
    }
    virtual ~TempAssign() {
        *variable = oldvalue;
    }

    T oldvalue, *variable;
};

/*                      Replaced with std::atomic
template <class T>
class ThreadSafe {
...
};*/


template <class C>
static void ShuffleAscending(C &data, std::default_random_engine &generator) {
    for (i32 i = 0; i < data.size() - 2; i++) {
        std::uniform_int_distribution<i32> distribution(i, data.size() - 1);
        Swap(data[i], data[distribution(generator)]);
    }
}

template <class C>
static void ShuffleDescending(C &data, std::default_random_engine &generator) {
    for (i32 i = data.size() - 1; i > 0; i--) {
        std::uniform_int_distribution<i32> distribution(0, i);
        Swap(data[i], data[distribution(generator)]);
    }
}

template <class C>
void Shuffle(C &data, i32 randomSeed = Null) {
    if (IsNull(randomSeed)) {
        std::random_device rd;
        randomSeed = rd();
    }
    std::default_random_engine re(randomSeed);

    std::mt19937 generator(randomSeed);

    ShuffleAscending(data, re);
    ShuffleDescending(data, re);
}

template <class T>
bool EqualRatio(const std::complex<T>& a, const std::complex<T>& b, const T& ratio, const T& zero = 0) {
    T absa = abs(a);
    T absb = abs(b);
    if (absa <= zero) {
        if (absb <= zero)
            return true;
        else {
            if(abs((zero - absb)/absb) <= ratio)
                return true;
            else
                return false;
        }
    } else if (absb <= zero) {
        if(abs((absa - zero)/absa) <= ratio)
            return true;
        else
            return false;
    }
    if(abs((a - b)/b) <= ratio)
        return true;
    return false;
}

template <class T>
bool EqualRatio(const T& a, const T& b, const T& ratio, const T& zero = 0) {
    if (abs(a) <= zero) {
        if (abs(b) <= zero)
            return true;
        else {
            if(abs((zero - b)/b) <= ratio)
                return true;
            else
                return false;
        }
    } else if (abs(b) <= zero) {
        if(abs((a - zero)/a) <= ratio)
            return true;
        else
            return false;
    }
    if(abs((a - b)/b) <= ratio)
        return true;
    return false;
}

template <class T1, class T2>
bool EqualDecimals(const T1& a, const T2& b, i32 numdecimals) {
    Txt sa = FormatF(a, numdecimals);
    Txt sb = FormatF(b, numdecimals);
    return sa == sb;
}

template <class Range>
i32 Find(const Range& r, const typename Range::value_type& value, i32 from = 0) {
    for (i32 i = from; i < r.size(); i++)
        if(r[i] == value)
            return i;
    return -1;
}

template <class Range>
i32 FindFunction(const Range& r, Function<bool(const typename Range::value_type &)> IsEqual, i32 from = 0) {
    for (i32 i = from; i < r.size(); i++)
        if(IsEqual(r[i]))
            return i;
    return -1;
}

template <class Range>
i32 FindAdd(Range& r, const typename Range::value_type& value, i32 from = 0) {
    i32 id = Find(r, value, from);
    if (id >= 0)
        return id;
    r.Add(value);
    return r.size()-1;
}

template<class Range, typename T>
i32 FindClosest(const Range& r, const std::complex<T>& value, i32 from = 0) {
    i32 minId = -1;
    T minDiff = std::numeric_limits<T>::max();
    for (i32 i = from; i < r.size(); i++) {
        T diff = abs(value - r[i]);
        if (diff < minDiff) {
            minDiff = diff;
            minId = i;
        }
    }
    return minId;
}

template<class Range>
i32 FindClosest(const Range& r, const typename Range::value_type& value, i32 from = 0) {
    i32 minId = -1;
    typename Range::value_type minDiff = std::numeric_limits<typename Range::value_type>::max();
    for (i32 i = from; i < r.size(); i++) {
        typename Range::value_type diff = abs(value - r[i]);
        if (diff < minDiff) {
            minDiff = diff;
            minId = i;
        }
    }
    return minId;
}

template <class Range>
i32 FindRatio(const Range& r, const typename Range::value_type& value, const typename Range::value_type& ratio, i32 from = 0) {
    i32 id = FindClosest(r, value, from);
    if (id >= 0) {
        if (EqualRatio(r[id], value, ratio))
            return id;
    }
    return -1;
}

template <class Range>
i32 FindAddRatio(Range& r, const typename Range::value_type& value, const typename Range::value_type& ratio, i32 from = 0) {
    i32 id = FindRatio(r, value, ratio, from);
    if (id >= 0)
        return id;
    r.Add(value);
    return r.size()-1;
}

template<class Range, typename T>
i32 FindDelta(const Range& r, const std::complex<T>& value, const T& delta, i32 from = 0) {
    i32 id = FindClosest(r, value, from);
    if (id >= 0) {
        if (abs(r[id] - value) <= delta)
            return id;
    }
    return -1;
}

template <class Range>
i32 FindDelta(const Range& r, const typename Range::value_type& value, const typename Range::value_type& delta, i32 from = 0) {
    i32 id = FindClosest(r, value, from);
    if (id >= 0) {
        if (abs(r[id] - value) <= delta)
            return id;
    }
    return -1;
}

template <class Range, typename T>
i32 FindAddDelta(Range& r, const std::complex<T>& value, const T& delta, i32 from = 0) {
    i32 id = FindDelta(r, value, delta, from);
    if (id >= 0)
        return id;
    r.Add(value);
    return r.size()-1;
}

template <class Range>
i32 FindAddDelta(Range& r, const typename Range::value_type& value, const typename Range::value_type& delta, i32 from = 0) {
    i32 id = FindDelta(r, value, delta, from);
    if (id >= 0)
        return id;
    r.Add(value);
    return r.size()-1;
}

template <class Range>
i32 FindRoundDecimals(const Range& r, const typename Range::value_type& value, i32 numDecimals, i32 from = 0) {
    i32 id = FindClosest(r, value, from);
    if (id >= 0) {
        Txt svalue = FormatF(value, numDecimals);
        if (FormatF(r[id], numDecimals) == svalue)
            return id;
    }
    return -1;
}

template <class Range>
bool Compare(const Range& a, const Range& b) {
    if (a.size() != b.size())
        return false;
    for (i32 i = 0; i < a.size(); i++) {
        if(a[i] != b[i])
            return false;
    }
    return true;
}

template <class Range1, class Range2>
bool CompareRatio(const Range1& a, const Range2& b, const typename Range1::value_type& ratio) {
    if (a.size() != b.size())
        return false;
    for(i32 i = 0; i < a.size(); i++)
        if (!EqualRatio(a[i], b[i], ratio))
            return false;
    return true;
}

template <class Range1, class Range2>
bool CompareDecimals(const Range1& a, const Range2& b, i32 numDecimals) {
    if (a.size() != b.size())
        return false;
    for (i32 i = 0; i < a.size(); i++)
        if (!EqualDecimals(a[i], b[i], numDecimals))
            return false;
    return true;
}

template <class Range>
Txt ToTxt(const Range& a) {
    Txt ret;
    for (i32 i = 0; i < a.size(); i++) {
        if (i > 0)
            ret << ";";
        ret << a[i];
    }
    return ret;
}

template <class Range1, class Range2>
void SetSortOrder(Range1& a, const Range2& order) {
    ASSERT(a.size() == order.size());
    Range1 temp = clone(a);
    for (i32 i = 0; i < order.size(); ++i)
        a[i] = pick(temp[order[i]]);
}


class RealTimeStop {
typedef RealTimeStop CLASSNAME;
public:
    RealTimeStop() {
#ifdef CTRLLIB_H
        callbackOn = false;
        lastTick = -1;
#endif
        Start();
    }
    void Reset() {
        timeElapsed = lastTimeElapsed = 0;
#ifdef CTRLLIB_H
        if (!callbackOn) {
            timeCallback.Set(-5*1000, OTVET(Tick));
            callbackOn = true;
        }
#endif
        isPaused = true;
        Continue();
    }
    void Start() {Reset();}
    void Pause(bool pause) {
        if (pause)
            Pause();
        else
            Continue();
    }
    void Pause() {
        if (!isPaused) {
            timeElapsed += (tmGetTimeX() - time0);
            isPaused = true;
        }
    }
    void Continue() {
        if (isPaused) {
            time0 = tmGetTimeX();
            isPaused = false;
        }
    }
    double Seconds() {
        if (isPaused)
            return timeElapsed;
        else
            return timeElapsed + (tmGetTimeX() - time0);
    }
    double Elapsed() {
        double t = Seconds();
        double elapsed = t - lastTimeElapsed;
        lastTimeElapsed = t;
        return elapsed;
    }
    void SetBack(double secs) {
        timeElapsed -= secs;
    }
    bool IsPaused()     {return isPaused;}

private:
    double timeElapsed;             // Time elapsed
    double time0;                   // Time of last Continue()
    double lastTimeElapsed;
    bool isPaused;
#ifdef CTRLLIB_H
    bool callbackOn;
    double lastTick;
    TimeCallback timeCallback;

    void Tick() {
        double tActual = tmGetTimeX();
        if (!isPaused && lastTick > -1) {
            double deltaLastTick = tActual - lastTick;
            if (deltaLastTick > 5*10)   // Some external issue has stopped normal running
                SetBack(deltaLastTick); // Timeout timer is fixed accordingly
        }
        lastTick = tActual;
    }
#endif
};

class LocalProcessX
#ifdef CTRLLIB_H
 : public Ctrl
 #endif
 {
typedef LocalProcessX CLASSNAME;
public:
    virtual ~LocalProcessX()        {Stop();}
    enum ProcessStatus {RUNNING = 1, STOP_OK = 0, STOP_TIMEOUT = -1, STOP_USER = -2, STOP_NORESPONSE = -3};
    bool Start(tukk cmd, tukk envptr = nullptr, tukk dir = nullptr, double _refreshTime = -1,
        double _maxTimeWithoutOutput = -1, double _maxRunTime = -1, bool convertcharset = true) {
        status = STOP_OK;
        p.ConvertCharset(convertcharset);
        timeElapsed.Start();
        timeWithoutOutput.Start();
        if(!p.Start(cmd, envptr, dir))
            return false;
        status = RUNNING;
        maxTimeWithoutOutput = _maxTimeWithoutOutput;
        maxRunTime = _maxRunTime;
        refreshTime = _refreshTime;

#ifdef CTRLLIB_H
        if (refreshTime > 0) {
            if (!callbackOn) {
                timeCallback.Set(-i32(refreshTime*1000), OTVET(Perform));
                callbackOn = true;
            }
        }
#endif
        return true;
    }
    void Perform() {
        if (status <= 0)
            return;
        Txt out;
        p.Read(out);
        if(p.IsRunning()) {
#ifdef TARGET_WIN32
            if (!p.IsPaused()) {
#endif
                if (maxTimeWithoutOutput > 0 && timeWithoutOutput.Seconds() > maxTimeWithoutOutput)
                    status = STOP_NORESPONSE;
                else if (maxRunTime > 0 && timeElapsed.Seconds() > maxRunTime)
                    status = STOP_TIMEOUT;
#ifdef TARGET_WIN32
            }
#endif
        } else
            status = STOP_OK;

        bool resetTimeout = false;
        if (!out.IsEmpty())
            resetTimeout = true;

        if (!WhenTimer(timeElapsed.Seconds(), out, status <= 0, resetTimeout))
            status = STOP_USER;

        if (resetTimeout)
            timeWithoutOutput.Reset();

        if (status < 0)
            p.Kill();

#ifdef CTRLLIB_H
        if (callbackOn) {
            timeCallback.Kill();
            callbackOn = false;
        }
#endif
    }
    void Stop(ProcessStatus _status = STOP_USER) {
        if (!IsRunning())
            return;
        status = _status;
        p.Kill();
#ifdef CTRLLIB_H
        if (callbackOn) {
            timeCallback.Kill();
            callbackOn = false;
        }
#endif
    }
#ifdef TARGET_WIN32
    void Pause() {
        p.Pause();
        if (p.IsRunning()) {
            timeElapsed.Pause(p.IsPaused());
            timeWithoutOutput.Pause(p.IsPaused());
        }
    }
    bool IsPaused()         {return p.IsPaused();}
    double Seconds()        {return timeElapsed.Seconds();}
#endif
    void Write(Txt str)  {p.Write(str);}
    i32 GetStatus()         {return status;}
    bool IsRunning()        {return status > 0;}
    Function<bool(double, Txt&, bool, bool&)> WhenTimer;
    #ifdef TARGET_WIN32
    DWORD GetPid()  {return p.GetPid();}
    #endif

    virtual void  SetData(const Val& v)   {value = v;}
    virtual Val GetData() const           {return value;}

private:
    Val value;
    LocalProcess2 p;
    RealTimeStop timeElapsed, timeWithoutOutput;
    ProcessStatus status = STOP_OK;
    double maxTimeWithoutOutput = 0, maxRunTime = 0;
    double refreshTime = 0;
#ifdef CTRLLIB_H
    bool callbackOn = false;
    TimeCallback timeCallback;
#endif
};

i32 LevenshteinDistance(tukk s, tukk t);
i32 DamerauLevenshteinDistance(tukk s, tukk t, i32 alphabetLength = 256);
i32 SentenceSimilitude(tukk s, tukk t);

//#define S(y)  drx::Txt(y)
Txt S(tukk s);
Txt S(const Val &v);

template<class T>
void Jsonize(JsonIO& io, std::complex<T>& var) {
    T re, im;
    if (io.IsStoring()) {
        re = var.real();
        im = var.imag();
    }
    io("re", re)("im", im);
    if (io.IsLoading()) {
        var.real(re);
        var.imag(im);
    }
}

size_t GetNumLines(Stream &stream);

class FileInLine : public FileIn {
public:
    explicit FileInLine(Txt _fileName) : FileIn(_fileName), line(0), fileName(_fileName) {};

    bool Open(tukk fn) {
        line = 0;
        return FileIn::Open(fn);
    }
    Txt GetLine() {
        line++;
        return FileIn::GetLine();
    }
    Txt GetLine(i32 num) {
        if (num == 0)
            return Txt();
        for (i32 i = 0; i < num-1; ++i)
            GetLine();
        return GetLine();
    }
    i32 GetLineNumber() const   {return line;}
    Txt Str() const          {return Format(t_("[Файл: '%s', строка: %d]: "), fileName, line);}

    struct Pos {
        Pos() : byt(0), line(0) {}
        z64 byt;
        i32 line;
    };

    Pos GetPos() {
        Pos ret;
        ret.byt = FileIn::GetPos();
        ret.line = line;
        return ret;
    }

    void SeekPos(Pos &ps) {
        FileIn::Seek(ps.byt);
        line = ps.line;
    }

private:
    i32 line;
    Txt fileName;
};

class FileInBinary : public FileIn {
public:
    FileInBinary()                                      {}
    explicit FileInBinary(tukk fn) : FileIn(fn)  {}

    void ReadB(uk data, size_t sz) {
        z64 len = Get64(data, sz);
        if (len != z64(sz))
            throw Exc(Format(t_("Не загружены данные в FileInBinary::Read(%ld)"), z64(sz)));
    }

    template <class T>
    T ReadB() {
        T data;
        ReadB(&data, sizeof(T));
        return data;
    }
    template <class T, size_t len>
    T ReadB() {
        T data;
        ReadB(&data, min(len, sizeof(T)));
        return data;
    }
};

class FileOutBinary : public FileOut {
public:
    explicit FileOutBinary(tukk fn) : FileOut(fn)    {}
    FileOutBinary()                                         {}

    template <class T>
    void Write(T data) {
        Put64(&data, sizeof(T));
    }
};

class FieldSplit {
public:
    i32k FIRST = 0;
    i32k LAST = Null;

    explicit FieldSplit(FileInLine &_in) {in = &_in;}

    FieldSplit& Load(Txt _line) {
        line = _line;
        fields = Split(line, IsSeparator, true);
        return *this;
    }
    FieldSplit& Load(Txt _line, const Vec<i32> &separators) {
        line = _line;
        fields.Clear();
        i32 from = 0, to;
        for (i32 i = 0; i < separators.size(); ++i) {
            to = separators[i];
            if (to > line.GetCount()-1)
                break;
            fields << line.Mid(from, to-from);
            from = to;
        }
        fields << line.Mid(from);
        return *this;
    }
    FieldSplit& LoadFields(Txt _line, const Vec<i32> &pos) {
        line = _line;
        fields.Clear();
        for (i32 i = 0; i < pos.size()-1 && pos[i] < line.GetCount(); ++i)
            fields << Trim(line.Mid(pos[i], pos[i+1]-pos[i]));
        return *this;
    }
    Txt& GetLine(i32 num = 1) {
        ASSERT(in);
        Load(in->GetLine(num));
        return line;
    }
    Txt& GetLine_discard_empty() {
        ASSERT(in);
        while (!in->IsEof()) {
            Load(in->GetLine());
            if (size() > 0)
                return line;
        }
        return line;
    }
    Txt& GetLineFields(const Vec<i32> &pos) {
        ASSERT(in);
        LoadFields(in->GetLine(), pos);
        return line;
    }
    bool IsEof() {
        ASSERT(in);
        return in->IsEof();
    }
    Txt GetText() const {
        return line;
    }
    Txt GetText(i32 i) const {
        if (fields.IsEmpty())
            throw Exc(in->Str() + t_("Нет доступных данных"));
        if (IsNull(i))
            i = fields.GetCount()-1;
        CheckId(i);
        return fields[i];
    }
    i32 GetInt_nothrow(i32 i) const {
        if (fields.IsEmpty())
            throw Exc(in->Str() + t_("Нет доступных данных"));
        if (IsNull(i))
            i = fields.GetCount()-1;
        CheckId(i);
        return ScanInt(fields[i]);
    }
    i32 GetInt(i32 i) const {
        i32 res = GetInt_nothrow(i);
        if (IsNull(res)) {
            if (i < fields.size())
                throw Exc(in->Str() + Format(t_("Неверный %s '%s' в поле #%d, строка\n'%s'"), "integer", fields[i], i+1, line));
            else
                throw Exc(in->Str() + Format(t_("Поле #%d не найдено в строке\n'%s'"), i+1, line));
        }
        return res;
    }
    bool IsInt(i32 i) const {
        return !IsNull(GetInt_nothrow(i));
    }
    double GetDouble_nothrow(i32 i) const {
        if (fields.IsEmpty())
            throw Exc(in->Str() + t_("Нет доступных данных"));
        if (IsNull(i))
            i = fields.GetCount()-1;
        if (!CheckId_nothrow(i))
            return Null;
        Txt data = fields[i];
        data.Replace("D", "");
        return ScanDouble(data);
    }
    double GetDouble(i32 i) const {
        double res = GetDouble_nothrow(i);
        if (IsNull(res)) {
            if (i < fields.size())
                throw Exc(in->Str() + Format(t_("Неверный %s '%s' в поле #%d, строка\n'%s'"), "double", fields[i], i+1, line));
            else
                throw Exc(in->Str() + Format(t_("Поле #%d не найдено в строке\n'%s'"), i+1, line));
        }
        return res;
    }
    bool IsDouble(i32 i) const {
        return !IsNull(GetDouble_nothrow(i));
    }
    bool IsInLine(Txt str) {
        return line.Find(str) >= 0;
    }

    i32   size() const        {return fields.GetCount();}
    i32   GetCount() const    {return size();}
    bool  IsEmpty() const    {return size() == 0;}

    wchar (*IsSeparator)(wchar) = defaultIsSeparator;

protected:
    Txt line;
    Vec<Txt> fields;
    FileInLine *in = nullptr;

    bool CheckId_nothrow(i32 i) const {
        return i >= 0 && i < fields.GetCount();
    }
    void CheckId(i32 i) const {
        if (!CheckId_nothrow(i))
            throw Exc(in->Str() + Format(t_("Поле #%d не найдено в строке\n'%s'"), i+1, line));
    }
    static wchar defaultIsSeparator(wchar c) {
        if (c == '\t' || c == ' ' || c == ';' || c == ',')
            return true;
        return false;
    }
};

enum CONSOLE_COLOR {
#ifdef TARGET_WIN32
    BLACK       = 0,
    BLUE        = FOREGROUND_BLUE,
    GREEN       = FOREGROUND_GREEN,
    CYAN        = FOREGROUND_GREEN | FOREGROUND_BLUE,
    RED         = FOREGROUND_RED,
    MAGENTA     = FOREGROUND_RED | FOREGROUND_BLUE,
    YELLOW      = FOREGROUND_RED | FOREGROUND_GREEN,
    GRAY        = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    LTBLUE      = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
    LTGREEN     = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
    LTCYAN      = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
    LTRED       = FOREGROUND_INTENSITY | FOREGROUND_RED,
    LTMAGENTA   = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
    LTYELLOW    = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
    WHITE       = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    RESET,
    PREVIOUS
#else
    BLACK       = 30,
    RED         = 31,
    GREEN       = 32,
    YELLOW      = 33,
    BLUE        = 34,
    MAGENTA     = 35,
    CYAN        = 36,
    GRAY        = 37,
    LTRED       = 131,
    LTGREEN     = 132,
    LTYELLOW    = 133,
    LTBLUE      = 134,
    LTMAGENTA   = 135,
    LTCYAN      = 136,
    WHITE       = 137,
    RESET       = 0,
    PREVIOUS    = 1000
#endif
};


bool SetConsoleColor(CONSOLE_COLOR color);
void ConsoleOutputDisable(bool disable);

Txt GetPythonDeclaration(const Txt &name, const Txt &include);
Txt CleanCFromDeclaration(const Txt &include, bool removeSemicolon = true);

class CoutStreamX : public Stream {
public:
    static void NoPrint(bool set = true) {noprint = set;}

private:
    Txt buffer;
    static bool noprint;

    virtual void Flush() {
#ifdef TARGET_POSIX
        fflush(stdout);
#else
        ONCELOCK {
            SetConsoleOutputCP(65001); // set console to UTF8 mode
        }
        static HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        dword dummy;
        WriteFile(h, ~buffer, buffer.GetCount(), &dummy, NULL);
        buffer.Clear();
#endif
    }

    void Put0(i32 w) {
#ifdef TARGET_WIN32
        buffer.Cat(w);
        if(CheckUtf8(buffer) || buffer.GetCount() > 8)
            Flush();
#else
        putchar(w);
#endif
    }
    virtual void    _Put(i32 w) {
        if (noprint)
            return;
        if(w == '\n') {
#ifdef TARGET_WIN32
            Put0('\r');
#endif
            Put0('\n');
            Put0('>');
        }
        else
        if(w != '\r')
            Put0(w);
    }
    virtual   bool  IsOpen() const { return true; }
};

Stream& CoutX();

}

#endif