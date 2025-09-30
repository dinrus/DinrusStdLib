#ifndef _drx_Funcs4U_ru_h_
#define _drx_Funcs4U_ru_h_
#include "Funcs4U.h"

namespace drx {

enum РАСШ_ФАЙЛ_ФЛАГИ {
    БЕЗ_ФЛАГА = 0,
    ИСПОЛЬЗУЙ_КОРЗИНУ = 1,
   // BROWSE_LINKS = 2,
    УДАЛИ_ТОЛЬКО_ЧТЕН = 4
};

#define  дайМенеджерРабСтолаНов GetDesktopManagerNew // -> Ткст;
#define  запустиФайл LaunchFile //(тукк файл, тукк парамы = нук, тукк дир = ".") -> бул;
#define  флКат FileCat //(тукк file, тукк appendFile) -> бул;
#define  флСравни FileCompare //(тукк path1, тукк path2) -> цел;
#define  найдиТкстВФл FindTxtInFile //(тукк file, const Ткст text, z64 pos0 = 0) -> z64;
#define  ПриставьТкстКФл//(тукк file, тукк str) -> бул;
#define  ПриставьФл//(тукк filename, тукк str) -> бул;
#define  приставьФимяХ AppendFNameX //1:(T t) -> Ткст;
                 //2:(T t, Args... args)-> Ткст;
#define  трим Trim//(const Ткст& s)-> Ткст;
#define  вместиФимя FitFName//(Ткст fileName, i32 len)-> Ткст;
#define  токенируй Tokenize//1:(const Ткст &str, const Ткст &token, i32 pos = 0) -> Век<Ткст>;
                //2:(const Ткст &str, const Ткст &token, Век<Ткст> &ret, i32 pos = 0); (проц)
#define токенируй2 Tokenize2//1:(const Ткст &str, const Ткст &token, i32 &pos) -> Ткст;
                //2:(const Ткст &str, const Ткст &token) -> Ткст;
#define дайСтроку GetLine//(const Ткст &str, i32 &pos) -> Ткст;

/////////
#define естьДирХ DirExistsX//(тукк path, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА) -> бул;
#define копируйДирХ DirCopyX//(тукк dir, тукк newPlace, бул replaceOnlyNew, Ткст filesToExclude,
                                          // Ткст &errorList); (проц)
#define переместиДирХ DirMove//(тукк dir, тукк newPlace) -> бул;
#define удалиГлубУайлдХ DeleteDeepWildcardsX//1:(тукк path, бул filefolder, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА,
                                         // бул deep = true) -> бул;
                //2:(тукк pathwc, тукк namewc, бул filefolder, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА,
                                        // бул deep = true) -> бул;
#define удалиПапГлубУайлдХ DeleteFolderDeepWildcardsX//1:(тукк path, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА) -> бул;
               //2:(тукк path, тукк name, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА) -> бул;
#define удалиФлГлубУайлдХ DeleteFileDeepWildcardsX//(тукк path, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА) -> бул;
#define удалиФлУайлдХ DeleteFileWildcardsX//(тукк path, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА) -> бул;
#define удалиПапГлубХ DeleteFolderDeepX//(тукк path, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА) -> бул;
#define перейменуйГлубУайлдХ RenameDeepWildcardsX//(тукк path, тукк namewc, тукк newname, бул forfile, бул forfolder,
                                         // РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА) -> бул;
#define пустаПап_ли FolderIsEmpty//(тукк path) -> бул;
#define создайДирХ DirCreateX//(тукк path) -> бул;
#define переместиФлХ FileMoveX//(тукк oldpath, тукк newpath, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА) -> бул;
#define удалиФлХ FileDeleteX//(тукк path, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА) -> бул;
#define кореньПап_ли IsRootFolder//(тукк folderName) -> бул;

#define дайПапВыше GetUpperFolder//(const Ткст &folderName) -> Ткст;
#define дайСледщПап GetNextFolder//(const Ткст &folder, const Ткст &lastFolder) -> Ткст;
#define реальнИмяф FileRealName//(тукк fileName) -> Ткст;
#define файл_ли IsFile//(тукк fileName) -> бул;
#define папка_ли IsFolder//(тукк fileName) -> бул;
#define дайОтнПуть GetRelativePath//(Ткст from, Ткст path, бул normalize = true) -> Ткст;
#define ДайАбсПуть GetAbsolutePath//(Ткст from, Ткст relative) -> Ткст;

#define симссыл_ли IsSymLink//(тукк path);

#define устТолькоЧтен SetReadOnly//1:(тукк path, бул readOnly);
                 //2:(тукк path, бул usr, бул grp, бул oth);
#define толькоЧтен_ли IsReadOnly//(тукк path, бул &usr, бул &grp, бул &oth) -> бул;

#define грузиФл_безоп LoadFile_Safe//(const Ткст fileName) -> Ткст;
#define грузиФл LoadFile//(тукк fileName, off_t from, size_t len = 0) -> Ткст;

#define дайДлину GetLength//(тукк fileDirName) -> z64;
#define дайДлинуДир GetDirLength//(тукк directoryName) -> z64;

///////////////////////////////
#define найдиВФл SearchFile//1:(Ткст dir, const Век<Ткст> &condFiles, const Век<Ткст> &condFolders,
                                // const Век<Ткст> &extFiles,  const Век<Ткст> &extFolders,
                                // const Ткст text, Век<Ткст> &errorList) -> Век<Ткст> ;
          //2:(Ткст dir, Ткст condFile, Ткст text, Век<Ткст> &errorList) -> Век<Ткст> ;
          //3:(Ткст dir, Ткст condFile = "*.*", Ткст text = "") -> Век<Ткст> ;
///////////////////////////////

#define флВКорзину FileToTrashBin//(тукк path) -> бул;
#define дайСчётФлВКорзине TrashBinGetCount//() -> z64;
#define очистьКорзину TrashBinClear//() -> бул;

#define дайЛичнПап GetPersonalFolder//() -> Ткст;
#define дайКореньПап GetRootFolder//() -> Ткст;
#define дайВремПап GetTempFolder//() -> Ткст;
#define дайОсПап GetOsFolder//() -> Ткст;
#define дайСисПап GetSystemFolder//() -> Ткст;
#ifdef TARGET_WIN32
#define ДайОбщДанПрилПап GetCommonAppDataFolder//() -> Ткст;
#endif
/*
struct FileData : Moveable<FileData> {
    бул isFolder;
    Ткст fileName;
    Ткст relFilename;
    z64 length;
    struct drx::Time t;
    z64 id;

    Ткст ToTxt() const { return Format("%s %0n", fileName, length); }

    FileData(бул _isFolder, Ткст _fileName, Ткст _relFilename, z64 _length,
        struct drx::Time _t, z64 _id) : isFolder(_isFolder), fileName(_fileName),
        relFilename(_relFilename), length(_length), t(_t), id(_id) {}
    FileData() {}
};

struct FileDiffData {
    char action;    // 'n': New, 'u': Update, 'd': Delete, 'p': Problem
    бул isFolder;
    Ткст relPath;
    Ткст fileName;
    zu64 idMaster, idSecondary;
    struct drx::Time tMaster, tSecondary;
    zu64 lengthMaster, lengthSecondary;
};

class ErrorHandling {
public:
    void SetLastError(Ткст _lastError)    {lastError = _lastError;};
    Ткст GetLastError()                   {return lastError;};

private:
    Ткст lastError;
};

class FileDiffArray;

class FileDataArray : public ErrorHandling {
public:
    FileDataArray(бул use = false, i32 fileFlags = 0);
    бул Init(Ткст folder, FileDataArray &orig, FileDiffArray &diff);
    void Clear();
    бул Search(Ткст dir, Ткст condFile, бул recurse = false, Ткст text = "");
    FileData& operator[](long i)    {return fileList[i];}
    long GetFileCount()             {return fileCount;};
    long GetFolderCount()           {return folderCount;};
    long GetCount()                 {return fileCount + folderCount;};
    z64 GetSize()                 {return fileSize;};
    inline бул UseId()             {return useId;};
    void SortByName(бул ascending = true);
    void SortByDate(бул ascending = true);
    void SortBySize(бул ascending = true);
    Век<Ткст> &GetLastError()  {return errorList;};
    i32 Find(Ткст &relFName, Ткст &fileName, бул isFolder);
    i32 Find(FileDataArray &data, i32 id);
    Ткст FullFName(i32 i)      {return AppendFNameX(basePath, fileList[i].fileName);};
    бул SaveFile(тукк fileName);
    бул AppendFile(тукк fileName);
    бул LoadFile(тукк fileName);

private:
    void Search_Each(Ткст dir, Ткст condFile, бул recurse, Ткст text);
    z64 GetFileId(Ткст fileName);
    Ткст GetRelativePath(const Ткст &fullPath);
    Ткст GetFileText();

    Array<FileData> fileList;
    Век<Ткст> errorList;
    Ткст basePath;
    long fileCount, folderCount;
    z64 fileSize;
    бул useId;
    i32 fileFlags;
};

class FileDiffArray : public ErrorHandling {
public:
    FileDiffArray();
    void Clear();
    FileDiffData& operator[](long i)    {return diffList[i];}
    бул Compare(FileDataArray &master, FileDataArray &secondary, const Ткст folderFrom,
        Век<Ткст> &excepFolders, Век<Ткст> &excepFiles, i32 sensSecs = 0);
    бул Apply(Ткст toFolder, Ткст fromFolder, РАСШ_ФАЙЛ_ФЛАГИ flags = БЕЗ_ФЛАГА);
    long GetCount()             {return diffList.size();};
    бул SaveFile(тукк fileName);
    бул LoadFile(тукк fileName);
    Ткст ToTxt();

private:
    Array<FileDiffData> diffList;
};
*/

Ткст Replace(Ткст str, Ткст find, Ткст replace);
Ткст Replace(Ткст str, char find, char replace);

i32 ReverseFind(const Ткст& s, const Ткст& toFind, i32 from = 0);

Ткст FormatLong(long a);

drx::Time StrToTime(тукк s);
Date StrToDate(тукк s);

Ткст BytesToTxt(zu64 bytes, бул units = true);

Ткст SecondsToTxt(double seconds, i32 dec = 2, бул units = false, бул space = false,
                        бул tp = false, бул longUnits = false, бул forceSec = false);
Ткст HMSToTxt(i32 hour, i32 min, double seconds, i32 dec = 2, бул units = false, бул space = false,
                        бул tp = false, бул longUnits = false, бул forceSec = false);
double TxtToSeconds(Ткст str);
void TxtToHMS(Ткст durat, i32 &hour, i32 &min, double &seconds);

Ткст SeasonName(i32 iseason);
i32 GetSeason(Date &date);

i32 NumAdvicedDigits(double d, double range);
Ткст FormatDoubleAutosize(double d);
Ткст FormatDoubleAutosize(double d, double range);
Ткст FormatDoubleSize(double d, i32 fieldWidth, бул fillSpaces = false);
#define FDAS    FormatDoubleAutosize
#define FDS     FormatDoubleSize

Ткст RemoveAccents(Ткст str);
Ткст RemoveAccent(wchar c);
бул IsPunctuation(wchar c);
Ткст RemovePunctuation(Ткст str);

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

inline бул Odd(i32 val)            {return val%2;}
inline бул Even(i32 val)           {return !Odd(val);}
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
inline бул IsNum(const std::complex<T> &n) {return IsFin(n.real()) && IsFin(n.imag());}
inline бул IsNum(double n) {return IsFin(n) && !IsNull(n);}
inline бул IsNum(float n)  {return IsFin(n);}
inline бул IsNum(i32 n)    {return !IsNull(n);}

template <typename T>
бул IsNull(const std::complex<T> &d)   {return !IsNum(d);};

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
inline бул Between(const T& val, const T& min, const T& max) {
    ASSERT(max > min);
    return val >= min && val <= max;
}
template <class T>
inline бул Between(const T& val, const T& range) {
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

Век<Век <Val> > ReadCSV(const Ткст &strFile, char separator = ',', бул bycols = true, бул removeRepeated = true, char decimalSign = '.', бул onlyTxts = false, i32 fromRow = 0);
Век<Век <Val> > ReadCSVFile(const Ткст &fileName, char separator = ',', бул bycols = true, бул removeRepeated = true, char decimalSign = '.', бул onlyTxts = false, i32 fromRow = 0);
бул ReadCSVFileByLine(const Ткст &fileName, Gate<i32, Век<Val>&, Ткст &> WhenRow, char separator = ',', char decimalSign = '.', бул onlyTxts = false, i32 fromRow = 0);
Ткст WriteCSV(Век<Век <Val> > &data, char separator = ',', бул bycols = true, char decimalSign = '.');
бул WriteCSVFile(const Ткст &fileName, Век<Век <Val> > &data, char separator = ',', бул bycols = true, char decimalSign = '.');
бул GuessCSV(const Ткст &fileName, бул onlyNumbers, Ткст &header, Век<Ткст> &parameters, char &separator, бул &repetition, char &decimalSign, z64 &beginData, i32 &beginDataRow);

// A Ткст based class to parse into
class TxtParse : public Ткст {
public:
    explicit TxtParse() : Ткст("") {GoInit();};
    TxtParse(Ткст _s): Ткст(_s)  {GoInit();};

    void GoInit()   {pos = 0; lastSeparator='\0';};
    бул GoBefore(const Ткст text) {
        if (pos >= GetLength()) {
            pos = GetLength()-1;
            return false;
        }
        i32 newpos = Ткст::Find(text, pos);
        if (newpos < 0)
            return false;   // If it does not find it, it does not move
        pos = newpos;
        return true;
    };
    бул GoAfter(const Ткст text) {
        if(!GoBefore(text))
            return false;
        pos += i32(strlen(text));
        return true;
    };
    бул GoAfter(const Ткст text, const Ткст text2) {
        if(!GoAfter(text))
            return false;
        if(!GoAfter(text2))
            return false;
        return true;
    };
    бул GoAfter(const Ткст text, const Ткст text2, const Ткст text3) {
        if(!GoAfter(text))
            return false;
        if(!GoAfter(text2))
            return false;
        if(!GoAfter(text3))
            return false;
        return true;
    };
    бул GoAfter_Init(const Ткст text) {GoInit(); return GoAfter(text);};
    бул GoAfter_Init(const Ткст text, const Ткст text2) {GoInit(); return GoAfter(text, text2);};
    бул GoAfter_Init(const Ткст text, const Ткст text2, const Ткст text3) {GoInit(); return GoAfter(text, text2, text3);};

    void GoBeginLine() {
        for (; pos >= 0; --pos) {
            if ((ToTxt()[pos-1] == '\r') || (ToTxt()[pos-1] == '\n'))
                return;
        }
    }
    бул IsBeginLine() {
        if (pos == 0)
            return true;
        if ((ToTxt()[pos-1] == '\r') || (ToTxt()[pos-1] == '\n'))
            return true;
        return false;
    }
    бул IsSpaceRN(i32 c) {
        if (IsSpace(c))
            return true;
        if ((c == '\r') || (c == '\n'))
             return true;
        return false;
    }
    // Gets text between "" or just a word until an space
    // It considers special characters with \ if between ""
    // If not between "" it gets the word when it finds one of the separator characters
    Ткст GetText(Ткст separators = "") {
        Ткст ret = "";
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
    Ткст GetLine() {
        Ткст ret;
        if (pos > GetCount() || pos == -1)
            return Ткст();
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
    double GetDouble(Ткст separators = "")    {return FixFloat(atof(GetText(separators)));};
    i32 GetInt(Ткст separators = "")          {return static_cast<i32>(FixFloat(atof(GetText(separators))));};
    long GetLong(Ткст separators = "")        {return static_cast<long>(FixFloat(atof(GetText(separators))));};
    zu64 GetUInt64(Ткст separators = "")    {return static_cast<zu64>(FixFloat(atof(GetText(separators))));};

    Ткст Right()          {return Ткст::Mid(pos+1);}
    i32 GetLastSeparator()  {return lastSeparator;}
    void MoveRel(i32 val) {
        pos += val;
        if (pos < 0)
            pos = 0;
        else if (pos >= GetCount())
            pos = GetCount() - 1;
    }
    i32 GetPos() {return pos;};
    бул SetPos(i32 i)
    {
        if (i < 0 || i >= GetCount())
            return false;
        else {
            pos = i;
            return true;
        }
    }
    бул Eof()
    {
        return pos >= GetCount();
    }
    unsigned Count(Ткст _s)
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
Ткст WideToTxt(LPCWSTR wcs, i32 len = -1);
бул TxtToWide(Ткст str, LPCWSTR &wcs);
бул BSTRSet(const Ткст str, BSTR &bstr);
Ткст BSTRGet(BSTR &bstr);
#endif


Ткст GetExtExecutable(const Ткст ext);

Век<Ткст> GetDriveList();

#define DLLFunction(dll, ret, function, args) auto function = (ret(*)args)dll.GetFunction_throw(#function)
#define DLLGetFunction(dll, ret, function, args)              (ret(*)args)dll.GetFunction_throw(#function)

class Dl {
public:
    virtual ~Dl();
    бул Load(const Ткст &fileDll);
    uk GetFunction(const Ткст &functionName) const;
    uk GetFunction_throw(const Ткст &functionName) const;
#if defined(TARGET_WIN32)
    HINSTANCE GetHandle() {return hinstLib;}
#else
    uk GetHandle()     {return hinstLib;}
#endif
    operator бул() const {return hinstLib;}

private:
#if defined(TARGET_WIN32)
    HINSTANCE hinstLib = 0;
#else
    uk hinstLib = 0;
#endif
};

typedef Dl Dll;



Ткст BsGetLastError();
бул BSPatch(Ткст oldfile, Ткст newfile, Ткст patchfile);
бул BSDiff(Ткст oldfile, Ткст newfile, Ткст patchfile);


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

i32 SysX(тукк cmd, Ткст& out, Ткст& err, double timeOut = Null,
            Gate3<double, Ткст&, Ткст&> progress = false, бул convertcharset = true);


class _NRFuse {
public:
    explicit _NRFuse(бул *_inside) {inside = _inside; failed = true;}
    virtual ~_NRFuse()              {if (!failed) *inside = false;}
    бул failed;
private:
    бул *inside;
};

#define NON_REENTRANT_V     static бул _insideNR; _NRFuse _fuseNR(&_insideNR); \
                            if(!_insideNR) {                                    \
                                _insideNR = true;                               \
                                _fuseNR.failed = false;                         \
                            } else                                              \
                                return
#define NON_REENTRANT(v)    static бул _insideNR; _NRFuse _fuseNR(&_insideNR); \
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
бул EqualRatio(const std::complex<T>& a, const std::complex<T>& b, const T& ratio, const T& zero = 0) {
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
бул EqualRatio(const T& a, const T& b, const T& ratio, const T& zero = 0) {
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
бул EqualDecimals(const T1& a, const T2& b, i32 numdecimals) {
    Ткст sa = FormatF(a, numdecimals);
    Ткст sb = FormatF(b, numdecimals);
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
i32 FindFunction(const Range& r, Function<бул(const typename Range::value_type &)> IsEqual, i32 from = 0) {
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
        Ткст svalue = FormatF(value, numDecimals);
        if (FormatF(r[id], numDecimals) == svalue)
            return id;
    }
    return -1;
}

template <class Range>
бул Compare(const Range& a, const Range& b) {
    if (a.size() != b.size())
        return false;
    for (i32 i = 0; i < a.size(); i++) {
        if(a[i] != b[i])
            return false;
    }
    return true;
}

template <class Range1, class Range2>
бул CompareRatio(const Range1& a, const Range2& b, const typename Range1::value_type& ratio) {
    if (a.size() != b.size())
        return false;
    for(i32 i = 0; i < a.size(); i++)
        if (!EqualRatio(a[i], b[i], ratio))
            return false;
    return true;
}

template <class Range1, class Range2>
бул CompareDecimals(const Range1& a, const Range2& b, i32 numDecimals) {
    if (a.size() != b.size())
        return false;
    for (i32 i = 0; i < a.size(); i++)
        if (!EqualDecimals(a[i], b[i], numDecimals))
            return false;
    return true;
}

template <class Range>
Ткст ToTxt(const Range& a) {
    Ткст ret;
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
    void Pause(бул pause) {
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
    бул IsPaused()     {return isPaused;}

private:
    double timeElapsed;             // Time elapsed
    double time0;                   // Time of last Continue()
    double lastTimeElapsed;
    бул isPaused;
#ifdef CTRLLIB_H
    бул callbackOn;
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
    бул Start(тукк cmd, тукк envptr = nullptr, тукк dir = nullptr, double _refreshTime = -1,
        double _maxTimeWithoutOutput = -1, double _maxRunTime = -1, бул convertcharset = true) {
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
        Ткст out;
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

        бул resetTimeout = false;
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
    бул IsPaused()         {return p.IsPaused();}
    double Seconds()        {return timeElapsed.Seconds();}
#endif
    void Write(Ткст str)  {p.Write(str);}
    i32 GetStatus()         {return status;}
    бул IsRunning()        {return status > 0;}
    Function<бул(double, Ткст&, бул, бул&)> WhenTimer;
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
    бул callbackOn = false;
    TimeCallback timeCallback;
#endif
};

i32 LevenshteinDistance(тукк s, тукк t);
i32 DamerauLevenshteinDistance(тукк s, тукк t, i32 alphabetLength = 256);
i32 SentenceSimilitude(тукк s, тукк t);

//#define S(y)  drx::Ткст(y)
Ткст S(тукк s);
Ткст S(const Val &v);

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
    explicit FileInLine(Ткст _fileName) : FileIn(_fileName), line(0), fileName(_fileName) {};

    бул Open(тукк fn) {
        line = 0;
        return FileIn::Open(fn);
    }
    Ткст GetLine() {
        line++;
        return FileIn::GetLine();
    }
    Ткст GetLine(i32 num) {
        if (num == 0)
            return Ткст();
        for (i32 i = 0; i < num-1; ++i)
            GetLine();
        return GetLine();
    }
    i32 GetLineNumber() const   {return line;}
    Ткст Str() const          {return Format(t_("[Файл: '%s', строка: %d]: "), fileName, line);}

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
    Ткст fileName;
};

class FileInBinary : public FileIn {
public:
    FileInBinary()                                      {}
    explicit FileInBinary(тукк fn) : FileIn(fn)  {}

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
    explicit FileOutBinary(тукк fn) : FileOut(fn)    {}
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

    FieldSplit& Load(Ткст _line) {
        line = _line;
        fields = Split(line, IsSeparator, true);
        return *this;
    }
    FieldSplit& Load(Ткст _line, const Век<i32> &separators) {
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
    FieldSplit& LoadFields(Ткст _line, const Век<i32> &pos) {
        line = _line;
        fields.Clear();
        for (i32 i = 0; i < pos.size()-1 && pos[i] < line.GetCount(); ++i)
            fields << Trim(line.Mid(pos[i], pos[i+1]-pos[i]));
        return *this;
    }
    Ткст& GetLine(i32 num = 1) {
        ASSERT(in);
        Load(in->GetLine(num));
        return line;
    }
    Ткст& GetLine_discard_empty() {
        ASSERT(in);
        while (!in->IsEof()) {
            Load(in->GetLine());
            if (size() > 0)
                return line;
        }
        return line;
    }
    Ткст& GetLineFields(const Век<i32> &pos) {
        ASSERT(in);
        LoadFields(in->GetLine(), pos);
        return line;
    }
    бул IsEof() {
        ASSERT(in);
        return in->IsEof();
    }
    Ткст GetText() const {
        return line;
    }
    Ткст GetText(i32 i) const {
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
    бул IsInt(i32 i) const {
        return !IsNull(GetInt_nothrow(i));
    }
    double GetDouble_nothrow(i32 i) const {
        if (fields.IsEmpty())
            throw Exc(in->Str() + t_("Нет доступных данных"));
        if (IsNull(i))
            i = fields.GetCount()-1;
        if (!CheckId_nothrow(i))
            return Null;
        Ткст data = fields[i];
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
    бул IsDouble(i32 i) const {
        return !IsNull(GetDouble_nothrow(i));
    }
    бул IsInLine(Ткст str) {
        return line.Find(str) >= 0;
    }

    i32   size() const        {return fields.GetCount();}
    i32   GetCount() const    {return size();}
    бул  IsEmpty() const    {return size() == 0;}

    wchar (*IsSeparator)(wchar) = defaultIsSeparator;

protected:
    Ткст line;
    Век<Ткст> fields;
    FileInLine *in = nullptr;

    бул CheckId_nothrow(i32 i) const {
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


бул SetConsoleColor(CONSOLE_COLOR color);
void ConsoleOutputDisable(бул disable);

Ткст GetPythonDeclaration(const Ткст &name, const Ткст &include);
Ткст CleanCFromDeclaration(const Ткст &include, бул removeSemicolon = true);

class CoutStreamX : public Stream {
public:
    static void NoPrint(бул set = true) {noprint = set;}

private:
    Ткст buffer;
    static бул noprint;

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
    virtual   бул  IsOpen() const { return true; }
};

Stream& CoutX();

}

#endif
