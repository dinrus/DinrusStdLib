#include "Sql.h"

#define LTIMING(x)

namespace drx {

template <class T>
Txt MakeSqlVal(i32 code, T& value)
{
    TxtBuf b(sizeof(T) + 1);
    b[0] = code;
    memcpy(~b + 1, &value, sizeof(T));
    return Txt(b);
}

template <class T>
T ReadSqlVal(T& x, tukk &s) {
    memcpy(&x, s, sizeof(T));
    s += sizeof(T);
    return x;
}

static bool sSqlIdQuoted;
static bool sToUpperCase;
static bool sToLowerCase;

bool SqlId::IsUseQuotes()
{
    return sSqlIdQuoted;
}

void SqlId::UseQuotes(bool b)
{
    sSqlIdQuoted = b;
}

void SqlId::ToLowerCase(bool b)
{
    sToUpperCase = sToUpperCase && !b;
    sToLowerCase = b;
}

void SqlId::ToUpperCase(bool b)
{
    sToLowerCase = sToLowerCase && !b;
    sToUpperCase = b;
}

Txt SqlId::Quoted() const
{
    if(!id.IsNull())
        return Txt().Cat() << '\t' << id << '\t';
    return id.ToTxt();
}

void SqlCompile(tukk &s, TxtBuf *r, u8 dialect, Vec<SqlVal> *split)
{
    char quote = dialect == MY_SQL ? '`' : '\"';
    tukk b = s;
    i32 lvl = 0;
    for(;;) {
        i32 c = *s++;
        switch(c) {
        case SQLC_OF:
            if(r)
                *r << '.';
            break;
        case SQLC_AS:
            if(r) {
                if(dialect & (MSSQL | PGSQL))
                    *r << " as ";
                else
                    *r << ' ';
            }
            break;
        case SQLC_COMMA:
            if(r)
                *r << ", ";
            break;
        case SQLC_ID: {
                for(;;) {
                    tukk b = s;
                    bool do_quote = sSqlIdQuoted && *s != '*';
                    while((u8)*s >= 32)
                        s++;
                    i32 c = *s;
                    if(r) {
                        if(do_quote)
                            *r << quote;
                        if(sToUpperCase)
                            r->Cat(ToUpper(Txt(b, s)));
                        else
                        if(sToLowerCase)
                            r->Cat(ToLower(Txt(b, s)));
                        else
                            r->Cat(b, s);
                        if(do_quote)
                            *r << quote;
                        if(c == SQLC_AS) {
                            if(dialect & (MSSQL | PGSQL))
                                *r << " as ";
                            else
                                *r << ' ';
                        }
                        else
                        if(c == SQLC_OF)
                            *r << '.';
                        else
                        if(c == SQLC_COMMA)
                            *r << ", ";
                    }
                    s++;
                    if(c == SQLC_ID)
                        break;
                    if(c == '\0')
                        return;
                }
            }
            break;
        case SQLC_IF: {
            LTIMING("SqlCompile IF");
            TxtBuf *er = r;
            for(;;) {
                c = *s++;
                if(c & dialect) {
                    SqlCompile(s, er, dialect, NULL);
                    er = NULL;
                }
                else
                    SqlCompile(s, NULL, dialect, NULL);
                if(*s == '\0')
                    return;
                c = *s++;
                if(c == SQLC_ELSE) {
                    SqlCompile(s, er, dialect, NULL);
                    ASSERT(*s == SQLC_ENDIF);
                    s++;
                    break;
                }
                if(c == SQLC_ENDIF)
                    break;
                ASSERT(c == SQLC_ELSEIF);
            }
            break;
        }
        case SQLC_DATE: {
            LTIMING("SqlCompile DATE");
            Date x;
            ReadSqlVal(x, s);
            if(!r) break;
            if(IsNull(x)) {
                *r << "NULL";
                break;
            }
            switch(dialect) {
            case MSSQL:
                if(x.year < 1753) x.year = 1753; // Date::Low()
                *r << Format("convert(datetime, '%d/%d/%d', 120)", x.year, x.month, x.day);
                break;
            case ORACLE:
                *r << Format("to_date('%d/%d/%d', 'SYYYY/MM/DD')", x.year, x.month, x.day);
                break;
            case PGSQL:
                if(x.year < 1) x.year = 1; // Date::Low()
                *r << "date ";
            default:
                *r << Format("\'%04d-%02d-%02d\'", x.year, x.month, x.day);
            }
            break;
        }
        case SQLC_TIME: {
            LTIMING("SqlCompile TIME");
            Time x;
            ReadSqlVal(x, s);
            if(!r) break;
            if(IsNull(x)) {
                *r << "NULL";
                break;
            }
            switch(dialect) {
            case MSSQL:
                if(x.year < 1753) x.year = 1753; // Date::Low()
                *r << Format(x.hour || x.minute || x.second
                             ? "convert(datetime, '%d/%d/%d %d:%d:%d', 120)"
                             : "convert(datetime, '%d/%d/%d', 120)",
                             x.year, x.month, x.day, x.hour, x.minute, x.second);
                break;
            case ORACLE:
                *r << Format("to_date('%d/%d/%d/%d', 'SYYYY/MM/DD/SSSSS')",
                             x.year, x.month, x.day, x.second + 60 * (x.minute + 60 * x.hour));
                break;
            case PGSQL:
                if(x.year < 1) x.year = 1; // Date::Low()
                *r << "timestamp ";
            default:
                *r << Format("\'%04d-%02d-%02d %02d:%02d:%02d\'",
                             x.year, x.month, x.day, x.hour, x.minute, x.second);
            }
            break;
        }
        case SQLC_BINARY: {
            i32 l;
            ReadSqlVal(l, s);
            if(r) {
                if(l == 0)
                    *r << "NULL";
                else
                    switch(dialect) {
                    case PGSQL: {
                        *r << "E\'";
                        tukk e = s + l;
                        while(s < e) {
                            u8 c = *s++;
                            if(c < 32 || c > 126 || c == 39 || c == 92) {
                                *r << '\\' << '\\';
                                r->Cat(((c >> 6) & 3) + '0');
                                r->Cat(((c >> 3) & 7) + '0');
                                r->Cat((c & 7) + '0');
                            }
                            else
                                r->Cat(c);
                        }
                        *r << "\'::bytea";
                        break;
                    }
                    case MSSQL:
                        *r << "0x" << HexTxt(s, l);
                        s += l;
                        break;
                    case SQLITE3:
                    case MY_SQL:
                        *r << "X";
                    default:
                        *r << "\'" << HexTxt(s, l) << "\'";
                        s += l;
                        break;
                    }
            }
            else
                s += l;
            break;
        }
        case SQLC_TXT: {
            LTIMING("SqlCompile TXT");
            i32 l;
            ReadSqlVal(l, s);
            Txt x = Txt(s, l);
            s += l;
            if(!r) break;
            if(IsNull(x)) {
                *r << "NULL";
                break;
            }
            if(dialect == PGSQL)
                r->Cat('E');
            r->Cat('\'');
            for(tukk q = x; *q; q++) {
                i32 c = (u8)*q;
                if(c == '\'') {
                    if(dialect == MY_SQL)
                        r->Cat("\\\'");
                    else if(dialect == PGSQL)
                        r->Cat("\\'");
                    else
                        r->Cat("\'\'");
                }
                else {
                    if((c == '\"' || c == '\\') && (dialect == MY_SQL || dialect == PGSQL))
                        r->Cat('\\');
                    if(dialect == PGSQL && c < 32) {
                        if(c == '\n')
                            r->Cat("\\n");
                        else
                        if(c == '\r')
                            r->Cat("\\r");
                        else
                        if(c == '\t')
                            r->Cat("\\t");
                        else {
                            char h[4];
                            h[0] = '\\';
                            h[1] = (3 & (c >> 6)) + '0';
                            h[2] = (7 & (c >> 3)) + '0';
                            h[3] = (7 & c) + '0';
                            r->Cat(h, 4);
                        }
                    }
                    else
                        r->Cat(c);
                }
            }
            r->Cat('\'');
            break;
        }
        default:
            bool end = c >= 0 && c < 32;
            if(split) {
                if(c == '(')
                    lvl++;
                if(c == ')')
                    lvl--;
                if((c == ',' && lvl == 0 || end) && s - 1 > b) {
                    while(*b == ' ')
                        b++;
                    split->Add(SqlVal(Txt(b, s - 1), SqlS::HIGH));
                    b = s;
                }
            }
            if(end) {
                s--;
                return;
            }
            else
                if(r) {
                    tukk p = s - 1;
                    while((u8)*s >= 32)
                        s++;
                    r->Cat(p, s);
                }
        }
    }
}

Txt SqlCompile(u8 dialect, const Txt& s)
{
    TxtBuf b;
    b.Reserve(s.GetLength() + 100);
    tukk q = s;
    SqlCompile(q, &b, dialect, NULL);
    return Txt(b);
}

#ifndef NOAPPSQL
Txt SqlCompile(const Txt& s)
{
    return SqlCompile(SQL.GetDialect(), s);
}
#endif

Vec<SqlVal> SplitSqlSet(const SqlSet& set)
{
    Txt h = ~set;
    tukk q = h;
    Vec<SqlVal> r;
    SqlCompile(q, NULL, ORACLE, &r);
    return r;
}

Txt SqlFormat(i32 x)
{
    if(IsNull(x)) return "NULL";
    return Format("%d", x);
}

Txt SqlFormat(double x)
{
    if(IsNull(x)) return "NULL";
    return FormatDouble(x, 20);
}

Txt SqlFormat(z64 x)
{
    if(IsNull(x)) return "NULL";
    return FormatInt64(x);
}

Txt SqlFormat0(tukk s, i32 l, i32 code)
{
    TxtBuf b(1 + sizeof(i32) + l);
    b[0] = code;
    memcpy(~b + 1, &l, sizeof(i32));
    memcpy(~b + 1 + sizeof(i32), s, l);
    return Txt(b);
}

Txt SqlFormat(tukk s, i32 l)
{
    return SqlFormat0(s, l, SQLC_TXT);
}

Txt SqlFormat(tukk s)
{
    return SqlFormat(s, (i32)strlen(s));
}

Txt SqlFormat(const Txt& x)
{
    return SqlFormat(x, x.GetLength());
}

Txt SqlFormatBinary(tukk s, i32 l)
{
    return SqlFormat0(s, l, SQLC_BINARY);
}

Txt SqlFormatBinary(const Txt& x)
{
    return SqlFormatBinary(x, x.GetLength());
}

Txt SqlFormat(Date x)
{
    return MakeSqlVal(SQLC_DATE, x);
}

Txt SqlFormat(Time x)
{
    return MakeSqlVal(SQLC_TIME, x);
}

Txt SqlFormat(const Val& x)
{
    if(x.IsNull()) return "NULL";
    switch(x.GetType()) {
    case BOOL_V:
    case INT_V:
        return SqlFormat((i32) x);
    case INT64_V:
        return SqlFormat((z64) x);
    case DOUBLE_V:
        return SqlFormat((double) x);
    case TXT_V:
    case WTXT_V:
        return SqlFormat(Txt(x));
    case DATE_V:
        return SqlFormat(Date(x));
    case TIME_V:
        return SqlFormat(Time(x));
    case SQLRAW_V:
        return SqlFormatBinary(SqlRaw(x));
    }
    NEVER();
    return "NULL";
}

Txt SqlCode::operator()(const Txt& text) {
    return s << (char)SQLC_ELSE << text << (char)SQLC_ENDIF;
}

Txt SqlCode::operator()() {
    return s << (char)SQLC_ENDIF;
}

SqlCode SqlCode::operator()(u8 cond, const Txt& text) {
    s << (char)SQLC_ELSEIF << (char)cond << text;
    return *this;
}

SqlCode::SqlCode(u8 cond, const Txt& text) {
    s << (char)SQLC_IF << (char)cond << text;
}

}
