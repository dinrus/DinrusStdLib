// can trace only if NDEBUG is not defined
#ifndef NDEBUG
// #define TRACEswitch
// #define TRACEcase
// #define TRACEmisc
#endif

#include "astyle.h"

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef TRACEswitch
#define TRswitch(a,b)   *traceOut << lineNumber << a << b << endl;
#else
#define TRswitch(a,b)   ((void)0)
#endif // TRACEswitch
#ifdef TRACEcase
#define TRcase(a,b)     *traceOut << lineNumber << a << b << endl;
#else
#define TRcase(a,b)     ((void)0)
#endif // TRACEcase
#ifdef TRACEmisc
#define TRmisc(a)       *traceOut << lineNumber << a << endl;
#else
#define TRmisc(a)       ((void)0)
#endif // TRACEmisc


namespace astyle
{

// ---------------------------- functions for ASEnhancer Class -------------------------------------

/**
 * ASEnhancer constructor
 */
ASEnhancer::ASEnhancer()
{
    // variables are initialized by init()
    traceOut = new StringStream;
}

/**
 * Destructor of ASEnhancer
 * Display the TRACE entries.
 */
ASEnhancer::~ASEnhancer()
{
#if defined(TRACEswitch) || defined(TRACEcase) || defined(TRACEmisc)
    WString line;
    WString msg = "TRACE Entries\n\n";
    char countLine[50];
    int count = 0;

    while (getline(*traceOut, line))
    {
        msg += line + '\n';
        count++;
    }
    sprintf(countLine, "\n%d Entries", count);
    msg += countLine;
    // write a text file to "My Documents" (Windows)
    char filename [_MAX_PATH + _MAX_FNAME + _MAX_EXT + 1];   // full path and filename
    strcpy(filename, getenv("USERPROFILE"));
    strcat(filename, "\\My Documents\\tracee.txt");
    ofstream outfile(filename);
    outfile << msg;
    outfile.close();
#endif
    delete traceOut;
}

/**
 * initialize the ASEnhancer.
 *
 * init() is called each time an ASFormatter object is initialized.
 */
void ASEnhancer::init(int _indentLength,
                      WString _indentString,
                      bool _isCStyle,
                      bool _isJavaStyle,
                      bool _isSharpStyle,
                      bool _caseIndent,
                      bool _emptyLineFill)
{
    // formatting variables from ASFormatter and ASBeautifier
    indentLength = _indentLength;
    if (_indentString[0] == '\t')
        useTabs = true;
    else
        useTabs = false;
    isCStyle      = _isCStyle;
    isJavaStyle   = _isJavaStyle;
    isSharpStyle  = _isSharpStyle;
    caseIndent    = _caseIndent;
    emptyLineFill = _emptyLineFill;

    // unindent variables
    lineNumber = 0;
    bracketCount = 0;
    isInComment = false;
    isInQuote = false;
    switchDepth = 0;
    lookingForCaseBracket = false;
    unindentNextLine = false;

#if defined(TRACEswitch) || defined(TRACEcase) || defined(TRACEmisc)
    *traceOut << "New file -------------" << endl;
#endif
}

/**
 * additional formatting for line of source code.
 * every line of source code in a source code file should be sent
 *     one after the other to this function.
 * indents event tables
 * unindents the case blocks
 *
 * @param line       the original formatted line will be updated if necessary.
 */
void ASEnhancer::enhance(WString &line)
{
    static Array<switchVariables>  swVector;       // stack Vector of switch variables
    static switchVariables sw;                      // switch variables struct

    static bool nextLineIsEventTable;               // begin event table is reached
    static bool isInEventTable;                     // need to indent an event table

    bool   isSpecialChar = false;
    int    lineLength;                             // length of the line being parsed

    lineNumber++;
    lineLength = line.GetCount();

    // check for beginning of event table
    if (nextLineIsEventTable)
    {
        isInEventTable = true;
        nextLineIsEventTable = false;
    }

    if (lineLength == 0
            && ! isInEventTable
            && ! emptyLineFill)
        return;

    // test for unindent on attached brackets
    if (unindentNextLine)
    {
        sw.unindentDepth++;
        sw.unindentCase = true;
        unindentNextLine = false;
        TRcase(" unindent case ", sw.unindentDepth);
    }

    // parse characters in the current line.

    for (int i = 0; i < lineLength; i++)
    {
        wchar ch = line[i];

        // bypass whitespace
        if (isWhiteSpaceX(ch))
            continue;

        // handle special characters (i.e. backslash+character such as \n, \t, ...)
        if (isSpecialChar)
        {
            isSpecialChar = false;
            continue;
        }
        if (!(isInComment) && line.Mid(i, 2) ==  WString("\\\\"))
        {
            i++;
            continue;
        }
        if (!(isInComment) && ch == '\\')
        {
            isSpecialChar = true;
            continue;
        }

        // handle quotes (such as 'x' and "Hello Dolly")
        if (!(isInComment) && (ch == '"' || ch == '\'')) {
            if (!isInQuote)
            {
                quoteChar = ch;
                isInQuote = true;
            }
            else if (quoteChar == ch)
            {
                isInQuote = false;
                continue;
            }
        }

        if (isInQuote)
            continue;

        // handle comments

        if (!(isInComment) && line.Mid(i, 2) ==  WString("//"))
        {
            // check for windows line markers
//@@ CHECK !!!!         if (line.Mid(i + 2, 1) > WString("\xf0"))
            if (line.Mid(i + 2, 1) > WString("\xf0") && line.Mid(i + 2, 1) > WString("\xff"))
                lineNumber--;
            break;                 // finished with the line
        }
        else if (!(isInComment) && line.Mid(i, 2) ==  WString("/*"))
        {
            isInComment = true;
            i++;
            continue;
        }
        else if ((isInComment) && line.Mid(i, 2) == WString("*/"))
        {
            isInComment = false;
            i++;
            continue;
        }

        if (isInComment)
            continue;

        // if we have reached this far then we are NOT in a comment or String of special characters

        if (line[i] == '{')                                 // if open bracket
            bracketCount++;

        if (line[i] == '}')                     // if close bracket
            bracketCount--;

        // ----------------  process event tables  --------------------------------------

        // check for event table begin
        if (findKeyword(line, i, "BEGIN_EVENT_TABLE")
                || findKeyword(line, i, "BEGIN_MESSAGE_MAP"))
            nextLineIsEventTable = true;

        // check for event table end
        if (findKeyword(line, i, "END_EVENT_TABLE")
                || findKeyword(line, i, "END_MESSAGE_MAP"))
            isInEventTable = false;

        // ----------------  process switch statements  ---------------------------------

        if (findKeyword(line, i, "switch"))                 // if switch statement
        {
            switchDepth++;                                  // bump switch depth
            TRswitch(" switch ", switchDepth);
            swVector.push_back(sw);                         // save current variables
            sw.switchBracketCount = 0;
            sw.unindentCase = false;                        // don't clear case until end of switch
            i += 5;                                         // bypass switch statement
            continue;
        }

        // just want switch statements from this point

        if (caseIndent || switchDepth == 0)                 // from here just want switch statements
            continue;                                      // get next char

        if (line[i] == '{')                                 // if open bracket
        {
            sw.switchBracketCount++;
            if (lookingForCaseBracket)                      // if 1st after case statement
            {
                sw.unindentCase = true;                     // unindenting this case
                sw.unindentDepth++;                         // bump depth
                lookingForCaseBracket = false;              // not looking now
                TRcase(" unindent case ", sw.unindentDepth);
            }
            continue;
        }

        lookingForCaseBracket = false;                      // no opening bracket, don't indent

        if (line[i] == '}')                                 // if close bracket
        {
            sw.switchBracketCount--;
            if (sw.switchBracketCount == 0)                 // if end of switch statement
            {
                TRswitch("  endsw ", switchDepth);
                switchDepth--;                              // one less switch
                sw = swVector.back();                       // restore sw struct
                swVector.pop_back();                        // remove last entry from stack
            }
            continue;
        }

        // look for case or default header

        if (findKeyword(line, i, "case") || findKeyword(line, i, "default"))
        {
            if (sw.unindentCase)                            // if unindented last case
            {
                sw.unindentCase = false;                    // stop unindenting previous case
                sw.unindentDepth--;                         // reduce depth
            }
            for (; i < lineLength; i++)                     // bypass colon
            {
                if (line[i] == ':') {
                    if ((i + 1 < lineLength) && (line[i + 1] == ':'))
                        i++;                                // bypass scope resolution operator
                    else
                        break;
                }
            }
            i++;
            for (; i < lineLength; i++)                     // bypass whitespace
            {
                if (!(isWhiteSpaceX(line[i])))
                    break;
            }
            if (i < lineLength)                             // check for bracket
            {
                if (line[i] == '{')                         // if bracket found
                {
                    sw.switchBracketCount++;
                    unindentNextLine = true;                // start unindenting on next line
                    continue;
                }
            }
            lookingForCaseBracket = true;                   // bracket must be on next line
            i--;                                            // need to check for comments
            continue;
        }
    }   // end of for loop

    if (isInEventTable)                                     // if need to indent
        indentLine(line, 1);                                //    do it

    if (sw.unindentDepth > 0)                               // if need to unindent
        unindentLine(line, sw.unindentDepth);               //    do it
}

/**
 * indent a line by a given number of tabsets
 *    by inserting leading whitespace to the line argument.
 *
 * @param line          a pointer to the line to indent.
 * @param unindent      the number of tabsets to insert.
 * @return              the number of characters inserted.
 */
int ASEnhancer::indentLine(WString  &line, const int indent) const
{
    if (line.GetCount() == 0
            && ! emptyLineFill)
        return 0;

    int charsToInsert;                      // number of chars to insert

    if (useTabs)                                // if formatted with tabs
    {
        charsToInsert = indent;                 // tabs to insert
        line.Insert(0, WString('\t', charsToInsert));    // insert the tabs
    }
    else
    {
        charsToInsert = indent * indentLength;  // compute chars to insert
        line.Insert(0, WString('\t', charsToInsert));    // insert the tabs
    }

    return charsToInsert;
}

/**
 * unindent a line by a given number of tabsets
 *    by erasing the leading whitespace from the line argument.
 *
 * @param line          a pointer to the line to unindent.
 * @param unindent      the number of tabsets to erase.
 * @return              the number of characters erased.
 */
int ASEnhancer::unindentLine(WString  &line, const int unindent) const
{
    int whitespace = ASString_Find_First_Not_Of(line, " \t");
    if (whitespace == -1)         // if line is blank
        whitespace = line.GetCount();         // must remove padding, if any

    if (whitespace == 0)
        return 0;

    int charsToErase;                    // number of chars to erase

    if (useTabs)                            // if formatted with tabs
    {
        charsToErase = unindent;            // tabs to erase
        if (charsToErase <= whitespace)     // if there is enough whitespace
            line.Remove(0, charsToErase);    // erase the tabs
        else
            charsToErase = 0;
    }
    else
    {
        charsToErase = unindent * indentLength; // compute chars to erase
        if (charsToErase <= whitespace)         // if there is enough whitespace
            line.Remove(0, charsToErase);        // erase the spaces
        else
            charsToErase = 0;
    }

    return charsToErase;
}

/**
 * check if a specific line position contains a keyword.
 *
 * @return    true if the word was found. false if the word was not found.
 */
bool ASEnhancer::findKeyword(const WString &line, int i, const char *keyword) const
{
    if (line.Mid(i, strlen(keyword)) ==  WString(keyword))
    {
        // check that this is a header and not a part of a longer word
        // (e.g. not at its begining, not at its middle...)

        int lineLength = line.GetCount();
        int wordEnd = i + strlen(keyword);
        char startCh = keyword[0];      // first char of header
        char endCh = 0;                // char just after header
        char prevCh = 0;               // char just before header

        if (wordEnd < lineLength)
        {
            endCh = line[wordEnd];
        }
        if (i > 0)
        {
            prevCh = line[i-1];
        }

        if (prevCh != 0
                && isLegalNameCharX(startCh)
                && isLegalNameCharX(prevCh))
        {
            return false;
        }
        else if (wordEnd >= lineLength
                 || !isLegalNameCharX(startCh)
                 || !isLegalNameCharX(endCh))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

}   // end namespace astyle
