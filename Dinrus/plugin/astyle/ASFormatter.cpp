#include "astyle.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#ifdef __VMS
#include <assert>
#else
#include <cassert>
#endif

// can trace only if NDEBUG is not defined
#ifndef NDEBUG
// #define TRACEunpad
// #define TRACEcomment
// #define TRACEheader
// #define TRACEbracket
// #define TRACEarray
#if defined(TRACEunpad) || defined(TRACEcomment) || defined(TRACEheader) \
|| defined(TRACEbracket) || defined(TRACEarray)
ofstream *traceOutF;
#define TRACEF
#endif
#endif

#ifdef TRACEunpad
#define TRunpad(a,b,c)  if(b > 0 || c > 0) *traceOutF << outLineNumber << " " << b << a << c << endl
#else
#define TRunpad(a,b,c)  ((void)0)
#endif

#ifdef TRACEcomment
#define TRcomment(a)    *traceOutF << outLineNumber << " " << a << endl
#else
#define TRcomment(a)    ((void)0)
#endif

#ifdef TRACEheader
#define TRxtra(a)       *traceOutF << outLineNumber << " " << a << endl
#else
#define TRxtra(a)    ((void)0)
#endif

#ifdef TRACEbracket
#define TRbracket(a)       *traceOutF << outLineNumber << " " << a << endl
#else
#define TRbracket(a)    ((void)0)
#endif

#ifdef TRACEarray
#define TRarray(a)      *traceOutF << outLineNumber << " " << a << endl
#else
#define TRarray(a)      ((void)0)
#endif

#define INIT_CONTAINER(container, value)     {if ( (container) != NULL ) delete (container); (container) = (value); }
#define DELETE_CONTAINER(container)          {if ( (container) != NULL ) delete (container); }
#define IS_A(a,b)                            ( ((a) & (b)) == (b))

using namespace std;

namespace astyle
{
Vector<const WString*> ASFormatter::headers;
Vector<const WString*> ASFormatter::nonParenHeaders;
Vector<const WString*> ASFormatter::preDefinitionHeaders;
Vector<const WString*> ASFormatter::preCommandHeaders;
Vector<const WString*> ASFormatter::operators;
Vector<const WString*> ASFormatter::assignmentOperators;
Vector<const WString*> ASFormatter::castOperators;

/**
 * Constructor of ASFormatter
 */
ASFormatter::ASFormatter()
{
    preBracketHeaderStack = NULL;
    bracketTypeStack = NULL;
    parenStack = NULL;
    lineCommentNoIndent = false;
    sourceIterator = NULL;
    bracketFormatMode = NONE_MODE;
    shouldPadOperators = false;
    shouldPadParensOutside = false;
    shouldPadParensInside = false;
    shouldUnPadParens = false;
    shouldBreakOneLineBlocks = true;
    shouldBreakOneLineStatements = true;
    shouldConvertTabs = false;
    shouldBreakBlocks = false;
    shouldBreakClosingHeaderBlocks = false;
    shouldBreakClosingHeaderBrackets = false;
    shouldBreakElseIfs = false;
#ifdef TRACEF
    // create a trace text file
    WString filename = "tracef.txt";
    char* env = getenv("HOME");
    if (env != NULL)
        filename = WString(env) + WString("/tracef.txt");
    else
    {
        env = getenv("USERPROFILE");
        if (env != NULL)
            filename = WString(env) + WString("\\My Documents\\tracef.txt");
        else
        {
            cout << "\Не удалось открыть tracef.txt\n" << endl;
            exit(1);
        }
    }
    traceOutF = new ofstream(filename.c_str());
#endif
}

/**
 * Destructor of ASFormatter
 */
ASFormatter::~ASFormatter()
{
    DELETE_CONTAINER(preBracketHeaderStack);
    DELETE_CONTAINER(bracketTypeStack);
    DELETE_CONTAINER(parenStack);
#ifdef TRACEF
    delete traceOutF;
#endif
}

/**
 * initialization of static data of ASFormatter.
 */
void ASFormatter::staticInit()
{
    static int formatterFileType = 9;      // initialized with an invalid type

    if (fileType == formatterFileType)     // don't build unless necessary
        return;

    formatterFileType = fileType;

    headers.clear();
    nonParenHeaders.clear();
    assignmentOperators.clear();
    operators.clear();
    preDefinitionHeaders.clear();
    preCommandHeaders.clear();
    castOperators.clear();

    ASResource::buildHeaders(headers, fileType);
    ASResource::buildNonParenHeaders(nonParenHeaders, fileType);
    ASResource::buildAssignmentOperators(assignmentOperators);
    ASResource::buildOperators(operators);
    ASResource::buildPreDefinitionHeaders(preDefinitionHeaders);
    ASResource::buildPreCommandHeaders(preCommandHeaders);
    ASResource::buildCastOperators(castOperators);
}

/**
 * initialize the ASFormatter.
 *
 * init() should be called every time a ASFormatter object is to start
 * formatting a NEW source file.
 * init() recieves a pointer to a DYNAMICALLY CREATED ASSourceIterator object
 * that will be used to iterate through the source code. This object will be
 * deleted during the ASFormatter's destruction, and thus should not be
 * deleted elsewhere.
 *
 * @param iter     a pointer to the DYNAMICALLY CREATED ASSourceIterator object.
 */
void ASFormatter::init(ASSourceIterator *si)
{
    staticInit();

    ASBeautifier::init(si);
    ASEnhancer::init(ASBeautifier::getIndentLength(),
                     ASBeautifier::getIndentString(),
                     ASBeautifier::getCStyle(),
                     ASBeautifier::getJavaStyle(),
                     ASBeautifier::getSharpStyle(),
                     ASBeautifier::getCaseIndent(),
                     ASBeautifier::getEmptyLineFill());
    sourceIterator = si;

    INIT_CONTAINER(preBracketHeaderStack, new Vector<const WString*>);
    INIT_CONTAINER(bracketTypeStack, new Vector<BracketType>);
    bracketTypeStack->push_back(NULL_TYPE);
    INIT_CONTAINER(parenStack, new Vector<int>);
    parenStack->push_back(0);

    currentHeader = NULL;
    currentLine = WString("");
    readyFormattedLine = WString("");
    formattedLine = "";
    currentChar = ' ';
    previousChar = ' ';
    previousCommandChar = ' ';
    previousNonWSChar = ' ';
    quoteChar = '"';
    charNum = 0;
    spacePadNum = 0;
    previousReadyFormattedLineLength = -1;
    templateDepth = 0;
    previousBracketType = NULL_TYPE;
    previousOperator = NULL;

    isVirgin = true;
    isInLineComment = false;
    isInComment = false;
    isInPreprocessor = false;
    doesLineStartComment = false;
    isInQuote = false;
    isSpecialChar = false;
    isNonParenHeader = true;
    foundNamespaceHeader = false;
    foundClassHeader = false;
    foundPreDefinitionHeader = false;
    foundPreCommandHeader = false;
    foundCastOperator = false;
    foundQuestionMark = false;
    isInLineBreak = false;
    endOfCodeReached = false;
    isLineReady = false;
    isPreviousBracketBlockRelated = true;
    isInPotentialCalculation = false;
    shouldReparseCurrentChar = false;
    passedSemicolon = false;
    passedColon = false;
    isInTemplate = false;
    isInBlParen = false;
    shouldBreakLineAfterComments = false;
    isImmediatelyPostComment = false;
    isImmediatelyPostLineComment = false;
    isImmediatelyPostEmptyBlock = false;
    isImmediatelyPostPreprocessor = false;

    isPrependPostBlockEmptyLineRequested = false;
    isAppendPostBlockEmptyLineRequested = false;
    prependEmptyLine = false;
    appendOpeningBracket = false;

    foundClosingHeader = false;
    previousReadyFormattedLineLength = 0;

    isImmediatelyPostHeader = false;
    isInHeader = false;
#ifdef TRACEF
    // fileName will be empty if ASTYLE_LIB is defined
    if (fileName.empty())
        *traceOutF << "new file" << endl;
    else
        *traceOutF << fileName << endl;
#endif
}

/**
 * get the next formatted line.
 *
 * @return    formatted line.
 */

WString ASFormatter::nextLine()
{
    // these are reset with each new line
    const WString *newHeader;
    bool isInVirginLine = isVirgin;
    isCharImmediatelyPostComment = false;
    isPreviousCharPostComment = false;
    isCharImmediatelyPostLineComment = false;
    isCharImmediatelyPostOpenBlock = false;
    isCharImmediatelyPostCloseBlock = false;
    isCharImmediatelyPostTemplate = false;

    while (!isLineReady)
    {
        if (shouldReparseCurrentChar)
            shouldReparseCurrentChar = false;
        else if (!getNextChar())
        {
            breakLine();
            return beautify(readyFormattedLine);
        }
        else // stuff to do when reading a new character...
        {
            // make sure that a virgin '{' at the begining ofthe file will be treated as a block...
            if (isInVirginLine && currentChar == '{')
                previousCommandChar = '{';
            isPreviousCharPostComment = isCharImmediatelyPostComment;
            isCharImmediatelyPostComment = false;
            isCharImmediatelyPostTemplate = false;
        }

        //if (inLineNumber >= 185)
        //  int x = 1;

        if (isInLineComment)
        {
            appendCurrentChar();

            // explicitely break a line when a line comment's end is found.
            if (charNum + 1 == (int) currentLine.GetCount())
            {
                isInLineBreak = true;
                isInLineComment = false;
                isImmediatelyPostLineComment = true;
                currentChar = 0;  //make sure it is a neutral char.
            }
            continue;
        }
        else if (isInComment)
        {
            if (isSequenceReached("*/"))
            {
                isInComment = false;
                isImmediatelyPostComment = true;
                appendSequence(AS_CLOSE_COMMENT);
                goForward(1);
            }
            else
                appendCurrentChar();

            continue;
        }

        // not in line comment or comment

        else if (isInQuote)
        {
            if (isSpecialChar)
            {
                isSpecialChar = false;
                appendCurrentChar();
            }
            else if (currentChar == '\\')
            {
                isSpecialChar = true;
                appendCurrentChar();
            }
            else if (quoteChar == currentChar)
            {
                isInQuote = false;
                appendCurrentChar();
            }
            else
            {
                appendCurrentChar();
            }

            continue;
        }

        // handle white space - needed to simplify the rest.
        if (isWhiteSpace(currentChar) || isInPreprocessor)
        {
            appendCurrentChar();
            continue;
        }

        /* not in MIDDLE of quote or comment or white-space of any type ... */

        if (isSequenceReached("//"))
        {
            if (currentLine[charNum+2] == '\xf2')       // check for windows line marker
                isAppendPostBlockEmptyLineRequested = false;
            isInLineComment = true;
            // do not indent if in column 1 or 2
            if (lineCommentNoIndent == false)
            {
                if (charNum == 0)
                    lineCommentNoIndent = true;
                else if (charNum == 1 && currentLine[0] == ' ')
                    lineCommentNoIndent = true;
            }
            // move comment if spaces were added or deleted
            if (lineCommentNoIndent == false && spacePadNum != 0)
                adjustComments();
            formattedLineCommentNum = formattedLine.GetCount();
            appendSequence(AS_OPEN_LINE_COMMENT);
            goForward(1);
            // explicitely break a line when a line comment's end is found.
            if (charNum + 1 == (int) currentLine.GetCount())
            {
                isInLineBreak = true;
                isInLineComment = false;
                isImmediatelyPostLineComment = true;
                currentChar = 0;  //make sure it is a neutral char.
            }
            continue;
        }
        else if (isSequenceReached("/*"))
        {
            isInComment = true;
            if (spacePadNum != 0)
                adjustComments();
            formattedLineCommentNum = formattedLine.GetCount();
            appendSequence(AS_OPEN_COMMENT);
            goForward(1);
            continue;
        }
        else if (currentChar == '"' || currentChar == '\'')
        {
            isInQuote = true;
            quoteChar = currentChar;
            appendCurrentChar();
            continue;
        }

        /* not in quote or comment or white-space of any type ... */

        // check if in preprocessor
        // ** isInPreprocessor will be automatically reset at the begining
        //    of a new line in getnextChar()
        if (currentChar == '#')
        {
            isInPreprocessor = true;
            appendCurrentChar();
            continue;
        }

        /* not in preprocessor ... */

        if (isImmediatelyPostComment)
        {
            isImmediatelyPostComment = false;
            isCharImmediatelyPostComment = true;
        }

        if (isImmediatelyPostLineComment)
        {
            isImmediatelyPostLineComment = false;
            isCharImmediatelyPostLineComment = true;
        }

        if (shouldBreakLineAfterComments)
        {
            shouldBreakLineAfterComments = false;
            shouldReparseCurrentChar = true;
            breakLine();
            continue;
        }

        // reset isImmediatelyPostHeader information
        if (isImmediatelyPostHeader)
        {
            isImmediatelyPostHeader = false;

            // Make sure headers are broken from their succeeding blocks
            // (e.g.
            //     if (isFoo) DoBar();
            //  should become
            //     if (isFoo)
            //         DoBar;
            // )
            // But treat else if() as a special case which should not be broken!
            if (shouldBreakOneLineStatements)
            {
                // if may break 'else if()'s, then simply break the line

                if (shouldBreakElseIfs)
                    isInLineBreak = true;
            }
        }

        if (passedSemicolon)    // need to break the formattedLine
        {
            passedSemicolon = false;
            if (parenStack->back() == 0 && currentChar != ';') // allow ;;
            {
                // does a one-line statement have ending comments?
                if (IS_A(bracketTypeStack->back(), SINGLE_LINE_TYPE))
                {
                    int blockEnd = ASString_ReverseFind(currentLine, AS_CLOSE_BRACKET);
                    assert(blockEnd != -1);

                    // move ending comments to this formattedLine
                    if (isBeforeLineEndComment(blockEnd))
                    {
                        int commentStart = ASString_Find_First_Not_Of(currentLine, " \t", blockEnd + 1);
                        assert(commentStart != -1);
                        assert((currentLine.Mid(commentStart, 2) == WString("//"))
                               || (currentLine.Mid(commentStart, 2) == WString("/*")));
                        int commentLength = currentLine.GetCount() - commentStart;
                        int tabCount = getIndentLength();
                        appendSpacePad();
                        for (int i=1; i<tabCount; i++)
                            formattedLine.Cat(' ');
                        formattedLine.Cat(currentLine.Mid(commentStart, commentLength));
                        currentLine.Remove(commentStart, commentLength);
                    }
                }
                shouldReparseCurrentChar = true;
                isInLineBreak = true;
                continue;
            }
        }

        if (passedColon)
        {
            passedColon = false;
            if (parenStack->back() == 0 && !isBeforeComment())
            {
                shouldReparseCurrentChar = true;
                isInLineBreak = true;
                continue;
            }
        }

        // Check if in template declaration, e.g. foo<bar> or foo<bar,fig>
        // If so, set isInTemplate to true
        if (!isInTemplate && currentChar == '<')
        {
            int maxTemplateDepth = 0;
            templateDepth = 0;
            const WString *oper;
            for (int i = charNum;
                    i < currentLine.GetCount();
                    i += (oper ? oper->GetCount() : 1))
            {
                oper = ASBeautifier::findHeader(currentLine, i, operators);

                if (oper == &AS_LS)
                {
                    templateDepth++;
                    maxTemplateDepth++;
                }
                else if (oper == &AS_GR)
                {
                    templateDepth--;
                    if (templateDepth == 0)
                    {
                        // this is a template!
                        isInTemplate = true;
                        templateDepth = maxTemplateDepth;
                        break;
                    }
                }
                else if (oper == &AS_COMMA            // comma,     e.g. A<int, char>
                         || oper == &AS_BIT_AND       // reference, e.g. A<int&>
                         || oper == &AS_MULT          // pointer,   e.g. A<int*>
                         || oper == &AS_COLON_COLON)  // ::,        e.g. std::string
                {
                    continue;
                }
                else if (!isLegalNameChar(currentLine[i]) && !isWhiteSpace(currentLine[i]))
                {
                    // this is not a template -> leave...
                    isInTemplate = false;
                    break;
                }
            }
        }

        // handle parenthesies
        if (currentChar == '(' || currentChar == '[' || (isInTemplate && currentChar == '<'))
        {
            parenStack->back()++;
            if (currentChar == '[')
                isInBlParen = true;
        }
        else if (currentChar == ')' || currentChar == ']' || (isInTemplate && currentChar == '>'))
        {
            parenStack->back()--;
            if (isInTemplate && currentChar == '>')
            {
                templateDepth--;
                if (templateDepth == 0)
                {
                    isInTemplate = false;
                    isCharImmediatelyPostTemplate = true;
                }
            }

            // check if this parenthesis closes a header, e.g. if (...), while (...)
            if (isInHeader && parenStack->back() == 0)
            {
                isInHeader = false;
                isImmediatelyPostHeader = true;
            }
            if (currentChar == ']')
                isInBlParen = false;
            if (currentChar == ')')
                foundCastOperator = false;
        }

        // handle brackets
        if (currentChar == '{' || currentChar == '}')
        {
            if (currentChar == '{')
            {
                BracketType newBracketType = getBracketType();
                foundNamespaceHeader = false;
                foundClassHeader = false;
                foundPreDefinitionHeader = false;
                foundPreCommandHeader = false;
                isInPotentialCalculation = false;

                bracketTypeStack->push_back(newBracketType);
                preBracketHeaderStack->push_back(currentHeader);
                currentHeader = NULL;

                isPreviousBracketBlockRelated = !IS_A(newBracketType, ARRAY_TYPE);
            }

            // this must be done before the bracketTypeStack is popped
            BracketType bracketType = bracketTypeStack->back();
            bool isOpeningArrayBracket = (IS_A(bracketType, ARRAY_TYPE)
                                          && bracketTypeStack->size() >= 2
                                          && !IS_A((*bracketTypeStack)[bracketTypeStack->size()-2], ARRAY_TYPE)
                                         );

            if (currentChar == '}')
            {
                // if a request has been made to append a post block empty line,
                // but the block exists immediately before a closing bracket,
                // then there is not need for the post block empty line.
                //
                isAppendPostBlockEmptyLineRequested = false;

                if (!bracketTypeStack->IsEmpty())
                {
                    previousBracketType = bracketTypeStack->back();
                    bracketTypeStack->pop_back();
                    isPreviousBracketBlockRelated = !IS_A(bracketType, ARRAY_TYPE);
                }

                if (!preBracketHeaderStack->IsEmpty())
                {
                    currentHeader = preBracketHeaderStack->back();
                    preBracketHeaderStack->pop_back();
                }
                else
                    currentHeader = NULL;
            }

            // format brackets
            if (IS_A(bracketType, ARRAY_TYPE))
                formatArrayBrackets(bracketType, isOpeningArrayBracket);
            else
                formatBrackets(bracketType);
            continue;
        }

        if (((previousCommandChar == '{' && isPreviousBracketBlockRelated)
                || (previousCommandChar == '}'
                    && bracketFormatMode != NONE_MODE
                    && !isImmediatelyPostEmptyBlock
                    && isPreviousBracketBlockRelated
                    && !isPreviousCharPostComment       // Fixes wrongly appended newlines after '}' immediately after comments
                    && peekNextChar() != ' '
                    && !IS_A(previousBracketType,  DEFINITION_TYPE)
                    && !(ASBeautifier::isJavaStyle && currentChar == ')'))
                && !IS_A(bracketTypeStack->back(),  DEFINITION_TYPE))
                && (shouldBreakOneLineBlocks
                    || !IS_A(bracketTypeStack->back(),  SINGLE_LINE_TYPE)))
        {
            isCharImmediatelyPostOpenBlock = (previousCommandChar == '{');
            isCharImmediatelyPostCloseBlock = (previousCommandChar == '}');

            //if (bracketFormatMode != NONE_MODE)
            //{
            previousCommandChar = ' ';
            isInLineBreak = true;
            //}
        }

        // reset block handling flags
        isImmediatelyPostEmptyBlock = false;

        // look for headers
        if (!isInTemplate)
        {
            if ((newHeader = findHeader(headers)) != NULL)
            {
                foundClosingHeader = false;
                const WString *previousHeader;

                // recognize closing headers of do..while, if..else, try..catch..finally
                if ((newHeader == &AS_ELSE && currentHeader == &AS_IF)
                        || (newHeader == &AS_WHILE && currentHeader == &AS_DO)
                        || (newHeader == &AS_CATCH && currentHeader == &AS_TRY)
                        || (newHeader == &AS_CATCH && currentHeader == &AS_CATCH)
                        || (newHeader == &AS_FINALLY && currentHeader == &AS_TRY)
                        || (newHeader == &AS_FINALLY && currentHeader == &AS_CATCH))
                    foundClosingHeader = true;

                previousHeader = currentHeader;
                currentHeader = newHeader;

                // If in ATTACH or LINUX bracket modes, attach closing headers (e.g. 'else', 'catch')
                // to their preceding bracket,
                // But do not perform the attachment if the shouldBreakClosingHeaderBrackets is set!
                if (!shouldBreakClosingHeaderBrackets
                        && foundClosingHeader
                        && (bracketFormatMode == ATTACH_MODE || bracketFormatMode == BDAC_MODE)
                        && (shouldBreakOneLineBlocks || !IS_A(previousBracketType,  SINGLE_LINE_TYPE))
                        && previousNonWSChar == '}')
                {
                    spacePadNum = 0;                // don't count as padding

                    int firstChar = ASString_Find_First_Not_Of(formattedLine, " \t");

                    if (firstChar >= 0)     // if a blank line does not preceed this
                    {
                        isInLineBreak = false;
                        appendSpacePad();
                    }

                    if (shouldBreakBlocks)
                        isAppendPostBlockEmptyLineRequested = false;
                }

                // If NONE bracket mode, leave closing headers as they are (e.g. 'else', 'catch')
                if (foundClosingHeader && bracketFormatMode == NONE_MODE && previousCommandChar == '}')
                {
                    if (lineBeginsWith('}'))                    // is closing bracket broken?
                    {
                        isInLineBreak = false;
                        appendSpacePad();
                    }

                    if (shouldBreakBlocks)
                        isAppendPostBlockEmptyLineRequested = false;
                }

                if (foundClosingHeader && bracketFormatMode == BREAK_MODE && previousCommandChar == '}')
                    breakLine();

                //Check if a template definition as been reached, e.g. template<class A>
                //if (newHeader == &AS_TEMPLATE)
                //{
                //  isInTemplate = true;
                //}

                // check if the found header is non-paren header
                isNonParenHeader = (find(nonParenHeaders.begin(), nonParenHeaders.end(),
                                         newHeader) != nonParenHeaders.end());

                appendSequence(*currentHeader);
                goForward(currentHeader->GetCount() - 1);
                // if a paren-header is found add a space after it, if needed
                // this checks currentLine, appendSpacePad() checks formattedLine
                if (!isNonParenHeader && charNum < (int) currentLine.GetCount() && !isWhiteSpace(currentLine[charNum+1]))
                    appendSpacePad();

                // Signal that a header has been reached
                // *** But treat a closing while() (as in do...while)
                //     as if it where NOT a header since a closing while()
                //     should never have a block after it!
                if (!(foundClosingHeader && currentHeader == &AS_WHILE))
                {
                    isInHeader = true;
                    if (isNonParenHeader)
                    {
                        isImmediatelyPostHeader = true;
                        isInHeader = false;
                    }
                }

                if (currentHeader == &AS_IF && previousHeader == &AS_ELSE)
                    isInLineBreak = false;

                if (shouldBreakBlocks)
                {
                    if (previousHeader == NULL
                            && !foundClosingHeader
                            && !isCharImmediatelyPostOpenBlock)
                    {
                        isPrependPostBlockEmptyLineRequested = true;
                    }

                    if (currentHeader == &AS_ELSE
                            || currentHeader == &AS_CATCH
                            || currentHeader == &AS_FINALLY
                            || foundClosingHeader)
                    {
                        isPrependPostBlockEmptyLineRequested = false;
                    }

                    if (shouldBreakClosingHeaderBlocks
                            &&  isCharImmediatelyPostCloseBlock)
                    {
                        isPrependPostBlockEmptyLineRequested = true;
                    }

                }

                continue;
            }
            else if ((newHeader = findHeader(preDefinitionHeaders)) != NULL
                     && parenStack->back() == 0)
            {
                if (newHeader == &AS_NAMESPACE)
                    foundNamespaceHeader = true;
                if (newHeader == &AS_CLASS)
                    foundClassHeader = true;
                foundPreDefinitionHeader = true;
                appendSequence(*newHeader);
                goForward(newHeader->GetCount() - 1);

                if (shouldBreakBlocks)
                    isPrependPostBlockEmptyLineRequested = true;

                continue;
            }
            else if ((newHeader = findHeader(preCommandHeaders)) != NULL)
            {
                if (ASBeautifier::isJavaStyle
                        || (*newHeader == AS_CONST && previousCommandChar == ')') // 'const' member functions is a command bracket
                        || *newHeader == AS_EXTERN)
                    foundPreCommandHeader = true;
                appendSequence(*newHeader);
                goForward(newHeader->GetCount() - 1);

                continue;
            }
            else if ((newHeader = findHeader(castOperators)) != NULL)
            {
                foundCastOperator = true;
                appendSequence(*newHeader);
                goForward(newHeader->GetCount() - 1);

                continue;
            }

        }

        if (isInLineBreak)          // OK to break line here
            breakLine();

        if (previousNonWSChar == '}' || currentChar == ';')
        {
            if (shouldBreakOneLineStatements && currentChar == ';'
                    && (shouldBreakOneLineBlocks || !IS_A(bracketTypeStack->back(),  SINGLE_LINE_TYPE))
                    //&& (! bracketFormatMode == NONE_MODE)
               )
            {
                passedSemicolon = true;
            }

            if (shouldBreakBlocks && currentHeader != NULL && parenStack->back() == 0)
            {
                isAppendPostBlockEmptyLineRequested = true;
            }

            if (currentChar != ';')
                currentHeader = NULL;

            foundQuestionMark = false;
            foundNamespaceHeader = false;
            foundClassHeader = false;
            foundPreDefinitionHeader = false;
            foundPreCommandHeader = false;
            foundCastOperator = false;
            isInPotentialCalculation = false;
            isNonInStatementArray = false;
        }

        if (currentChar == ':'
                && shouldBreakOneLineStatements
                && !foundQuestionMark           // not in a ... ? ... : ... sequence
                && !foundPreDefinitionHeader    // not in a definition block (e.g. class foo : public bar
                && previousCommandChar != ')'   // not immediately after closing paren of a method header, e.g. ASFormatter::ASFormatter(...) : ASBeautifier(...)
                && previousChar != ':'          // not part of '::'
                && peekNextChar() != ':')       // not part of '::'
        {
            passedColon = true;
            if (shouldBreakBlocks)
                isPrependPostBlockEmptyLineRequested = true;
        }

        if (currentChar == '?')
            foundQuestionMark = true;

        // determine if this is a potential calculation
        newHeader = findHeader(operators);

        if (newHeader != NULL)
        {
            if (!isInPotentialCalculation)
            {
                if (find(assignmentOperators.begin(), assignmentOperators.end(), newHeader)
                        != assignmentOperators.end())
                {
                    wchar peekedChar = peekNextChar();
                    isInPotentialCalculation = (newHeader != &AS_RETURN
                                                && !(newHeader == &AS_EQUAL && peekedChar == '*')
                                                && !(newHeader == &AS_EQUAL && peekedChar == '&'));
                }
            }
        }
        else
        {
            // the following are not calculations
            if (currentLine.Mid(charNum, 3) ==  WString("new") && !isLegalNameChar(currentLine[charNum+3]))
                isInPotentialCalculation = false;
        }

        if (shouldPadOperators && newHeader != NULL)
        {
            padOperators(newHeader);
            continue;
        }

        if ((shouldPadParensOutside || shouldPadParensInside || shouldUnPadParens)
                && (currentChar == '(' || currentChar == ')'))
        {
            padParens();
            continue;
        }

        appendCurrentChar();
    }   // end of while loop  *  end of while loop  *  end of while loop  *  end of while loop

    // return a beautified (i.e. correctly indented) line.

    WString beautifiedLine;
    int readyFormattedLineLength = trim(readyFormattedLine).GetCount();

    if (prependEmptyLine                // prepend a blank line before this formatted line
            && readyFormattedLineLength > 0
            && previousReadyFormattedLineLength > 0)
    {
        isLineReady = true;             // signal that a readyFormattedLine is still waiting
        beautifiedLine = beautify("");
        previousReadyFormattedLineLength = 0;
    }
    else                                // format the current formatted line
    {
        isLineReady = false;
        beautifiedLine = beautify(readyFormattedLine);
        previousReadyFormattedLineLength = readyFormattedLineLength;
        lineCommentNoBeautify = lineCommentNoIndent;
        lineCommentNoIndent = false;
        if (appendOpeningBracket)       // insert bracket after this formatted line
        {
            appendOpeningBracket = false;
            isLineReady = true;         // signal that a readyFormattedLine is still waiting
            readyFormattedLine = "{";
            isPrependPostBlockEmptyLineRequested = false;   // next line should not be empty
        }
    }

    prependEmptyLine = false;
    enhance(beautifiedLine);                // call the enhancer function
    return beautifiedLine;
}


/**
* check if there are any indented lines ready to be read by nextLine()
*
* @return    are there any indented lines ready?
*/
bool ASFormatter::hasMoreLines() const
{
    return !endOfCodeReached;
}

/**
 * set the bracket formatting mode.
 * options:
 *    astyle::NONE_MODE     no formatting of brackets.
 *    astyle::ATTACH_MODE   Java, K&R style bracket placement.
 *    astyle::BREAK_MODE    ANSI C/C++ style bracket placement.
 *
 * @param mode         the bracket formatting mode.
 */
void ASFormatter::setBracketFormatMode(BracketMode mode)
{
    bracketFormatMode = mode;
}

/**
 * set closing header bracket breaking mode
 * options:
 *    true     brackets just before closing headers (e.g. 'else', 'catch')
 *             will be broken, even if standard brackets are attached.
 *    false    closing header brackets will be treated as standard brackets.
 *
 * @param state         the closing header bracket breaking mode.
 */
void ASFormatter::setBreakClosingHeaderBracketsMode(bool state)
{
    shouldBreakClosingHeaderBrackets = state;
}

/**
 * set 'else if()' breaking mode
 * options:
 *    true     'else' headers will be broken from their succeeding 'if' headers.
 *    false    'else' headers will be attached to their succeeding 'if' headers.
 *
 * @param state         the 'else if()' breaking mode.
 */
void ASFormatter::setBreakElseIfsMode(bool state)
{
    shouldBreakElseIfs = state;
}

/**
 * set operator padding mode.
 * options:
 *    true     statement operators will be padded with spaces around them.
 *    false    statement operators will not be padded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setOperatorPaddingMode(bool state)
{
    shouldPadOperators = state;
}

/**
* set parenthesis outside padding mode.
* options:
*    true     statement parenthesiss will be padded with spaces around them.
*    false    statement parenthesiss will not be padded.
*
* @param state         the padding mode.
*/
void ASFormatter::setParensOutsidePaddingMode(bool state)
{
    shouldPadParensOutside = state;
}

/**
* set parenthesis inside padding mode.
* options:
*    true     statement parenthesis will be padded with spaces around them.
*    false    statement parenthesis will not be padded.
*
* @param state         the padding mode.
*/
void ASFormatter::setParensInsidePaddingMode(bool state)
{
    shouldPadParensInside = state;
}

/**
* set parenthesis unpadding mode.
* options:
*    true     statement parenthesis will be unpadded with spaces removed around them.
*    false    statement parenthesis will not be unpadded.
*
* @param state         the padding mode.
*/
void ASFormatter::setParensUnPaddingMode(bool state)
{
    shouldUnPadParens = state;
}

/**
 * set option to break/not break one-line blocks
 *
 * @param state        true = break, false = don't break.
 */
void ASFormatter::setBreakOneLineBlocksMode(bool state)
{
    shouldBreakOneLineBlocks = state;
}

/**
 * set option to break/not break lines consisting of multiple statements.
 *
 * @param state        true = break, false = don't break.
 */
void ASFormatter::setSingleStatementsMode(bool state)
{
    shouldBreakOneLineStatements = state;
}

/**
 * set option to convert tabs to spaces.
 *
 * @param state        true = convert, false = don't convert.
 */
void ASFormatter::setTabSpaceConversionMode(bool state)
{
    shouldConvertTabs = state;
}


/**
 * set option to break unrelated blocks of code with empty lines.
 *
 * @param state        true = convert, false = don't convert.
 */
void ASFormatter::setBreakBlocksMode(bool state)
{
    shouldBreakBlocks = state;
}

/**
 * set option to break closing header blocks of code (such as 'else', 'catch', ...) with empty lines.
 *
 * @param state        true = convert, false = don't convert.
 */
void ASFormatter::setBreakClosingHeaderBlocksMode(bool state)
{
    shouldBreakClosingHeaderBlocks = state;
}

/**
 * jump over several characters.
 *
 * @param i       the number of characters to jump over.
 */
void ASFormatter::goForward(int i)
{
    while (--i >= 0)
        getNextChar();
}

/**
* peek at the next unread character.
*
* @return     the next unread character.
*/
wchar ASFormatter::peekNextChar() const
{
    char ch = ' ';
    int peekNum = ASString_Find_First_Not_Of(currentLine, " \t", charNum + 1);
    if (peekNum < 0)
        return ch;

    ch = currentLine[peekNum];

//  if (shouldConvertTabs && ch == '\t')
//      ch = ' ';

    return ch;
}

/**
* check if current placement is before a comment or line-comment
*
* @return     is before a comment or line-comment.
*/
bool ASFormatter::isBeforeComment() const
{
    bool foundComment = false;
    int peekNum = ASString_Find_First_Not_Of(currentLine, " \t", charNum + 1);
    if (peekNum < 0)
        return foundComment;

    foundComment = (currentLine.Mid(peekNum, 2) == WString("/*")
                    || currentLine.Mid(peekNum, 2) == WString("//"));

    return foundComment;
}

/**
* check if current placement is before a comment or line-comment
* if a block comment it must be at the end of the line
*
* @return     is before a comment or line-comment.
*/
bool ASFormatter::isBeforeLineEndComment(int startPos) const
{
    bool foundLineEndComment = false;
    int peekNum = ASString_Find_First_Not_Of(currentLine, " \t", startPos + 1);
    if (peekNum >= 0)
    {
        if (currentLine.Mid(peekNum, 2) == WString("//"))
            foundLineEndComment = true;
        else if (currentLine.Mid(peekNum, 2) == WString("/*"))
        {
            // comment must be closed on this line with nothing after it
            int endNum = currentLine.Find("*/", peekNum + 2);
            if (endNum >= 0)
                if(ASString_Find_First_Not_Of(currentLine, " \t", endNum + 2) < 0)
                    foundLineEndComment = true;

        }
    }
    return foundLineEndComment;
}


/**
* get the next character, increasing the current placement in the process.
* the new character is inserted into the variable currentChar.
*
* @return   whether succeded to recieve the new character.
*/
bool ASFormatter::getNextChar()
{
    isInLineBreak = false;
    previousChar = currentChar;

    if (!isWhiteSpace(currentChar))
    {
        previousNonWSChar = currentChar;
        if (!isInComment && !isInLineComment && !isInQuote
                && !isImmediatelyPostComment
                && !isImmediatelyPostLineComment
                && !isSequenceReached("/*")
                && !isSequenceReached("//"))
            previousCommandChar = previousNonWSChar;
    }

    int currentLineLength = currentLine.GetCount();

    if (charNum + 1 < currentLineLength
            && (!isWhiteSpace(peekNextChar()) || isInComment || isInLineComment))
    {
        currentChar = currentLine[++charNum];

        if (shouldConvertTabs && currentChar == '\t')
            currentChar = ' ';

        return true;
    }
    else        // end of line has been reached
    {
        if (sourceIterator->hasMoreLines())
        {
            currentLine = sourceIterator->nextLine();
            spacePadNum = 0;
            inLineNumber++;

            if (currentLine.GetCount() == 0)
            {
                currentLine = WString(" ");        // a null is inserted if this is not done
            }

            // unless reading in the first line of the file,
            // break a new line.
            if (!isVirgin)
                isInLineBreak = true;
            else
                isVirgin = false;

            if (isInLineComment)
                isImmediatelyPostLineComment = true;
            isInLineComment = false;

            // check if is in preprocessor before line trimming
            isImmediatelyPostPreprocessor = isInPreprocessor;
            if (previousNonWSChar != '\\')
                isInPreprocessor = false;

            trimNewLine();
            currentChar = currentLine[charNum];

            if (shouldConvertTabs && currentChar == '\t')
                currentChar = ' ';

            return true;
        }
        else
        {
            endOfCodeReached = true;
            return false;
        }
    }
}

/**
* jump over the leading white space in the current line,
* IF the line does not begin a comment or is in a preprocessor definition.
*/
void ASFormatter::trimNewLine()
{
    int len = currentLine.GetCount();
    charNum = 0;

    if (isInComment || isInPreprocessor)
        return;

    while (isWhiteSpace(currentLine[charNum]) && charNum + 1 < len)
        ++charNum;

    doesLineStartComment = false;
    if (isSequenceReached("/*"))
    {
        charNum = 0;
        doesLineStartComment = true;
    }
}

/**
 * append a character to the current formatted line.
 * Unless disabled (via canBreakLine == false), first check if a
 * line-break has been registered, and if so break the
 * formatted line, and only then append the character into
 * the next formatted line.
 *
 * @param ch               the character to append.
 * @param canBreakLine     if true, a registered line-break
 */
void ASFormatter::appendChar(wchar ch, bool canBreakLine)
{
    if (canBreakLine && isInLineBreak)
        breakLine();
    formattedLine.Cat(ch);
}

/**
 * append a string sequence to the current formatted line.
 * Unless disabled (via canBreakLine == false), first check if a
 * line-break has been registered, and if so break the
 * formatted line, and only then append the sequence into
 * the next formatted line.
 *
 * @param sequence         the sequence to append.
 * @param canBreakLine     if true, a registered line-break
 */
void ASFormatter::appendSequence(const WString &sequence, bool canBreakLine)
{
    if (canBreakLine && isInLineBreak)
        breakLine();
    formattedLine.Cat(sequence);
}

/**
 * append a space to the current formattedline, UNLESS the
 * last character is already a white-space character.
 */
void ASFormatter::appendSpacePad()
{
    int len = formattedLine.GetCount();
    if (len > 0 && !isWhiteSpace(formattedLine[len-1]))
    {
        formattedLine.Cat(' ');
        spacePadNum++;
    }
}

/**
 * append a space to the current formattedline, UNLESS the
 * next character is already a white-space character.
 */
void ASFormatter::appendSpaceAfter()
{
    int len = currentLine.GetCount();
    if (charNum + 1 < len && !isWhiteSpace(currentLine[charNum+1]))
    {
        formattedLine.Cat(' ');
        spacePadNum++;
    }
}

/**
 * register a line break for the formatted line.
 */
void ASFormatter::breakLine()
{
    isLineReady = true;
    isInLineBreak = false;
    spacePadNum = 0;
    formattedLineCommentNum = -1;

    // queue an empty line prepend request if one exists
    prependEmptyLine = isPrependPostBlockEmptyLineRequested;
    readyFormattedLine =  formattedLine;
    if (isAppendPostBlockEmptyLineRequested)
    {
        isAppendPostBlockEmptyLineRequested = false;
        isPrependPostBlockEmptyLineRequested = true;
    }
    else
    {
        isPrependPostBlockEmptyLineRequested = false;
    }

    formattedLine = "";
}

/**
 * check if the currently reached open-bracket (i.e. '{')
 * opens a:
 * - a definition type block (such as a class or namespace),
 * - a command block (such as a method block)
 * - a static array
 * this method takes for granted that the current character
 * is an opening bracket.
 *
 * @return    the type of the opened block.
 */
BracketType ASFormatter::getBracketType() const
{
    BracketType returnVal;

    if (foundPreDefinitionHeader)
    {
        returnVal = DEFINITION_TYPE;
        if (foundNamespaceHeader)
            returnVal = (BracketType)(returnVal | NAMESPACE_TYPE);
        else if (foundClassHeader)
            returnVal = (BracketType)(returnVal | CLASS_TYPE);
    }
    else
    {
        bool isCommandType = false;

        if (previousNonWSChar != '=')
            isCommandType = (foundPreCommandHeader
                             || (currentHeader != NULL && isNonParenHeader)
                             || (previousCommandChar == ')')
                             || (previousCommandChar == ':' && !foundQuestionMark)
                             || (previousCommandChar == ';')
                             || ((previousCommandChar == '{' ||  previousCommandChar == '}')
                                 && isPreviousBracketBlockRelated));

        returnVal = (isCommandType ? COMMAND_TYPE : ARRAY_TYPE);
    }

    if (isOneLineBlockReached())
        returnVal = (BracketType)(returnVal | SINGLE_LINE_TYPE);

    TRbracket(returnVal);
    return returnVal;
}

/**
 * check if the currently reached  '*' or '&' character is
 * a pointer-or-reference symbol, or another operator.
 * this method takes for granted that the current character
 * is either a '*' or '&'.
 *
 * @return        whether current character is a reference-or-pointer
 */
bool ASFormatter::isPointerOrReference() const
{
    bool isPR;
    isPR = (!isInPotentialCalculation
            || IS_A(bracketTypeStack->back(), DEFINITION_TYPE)
            || (!isLegalNameChar(previousNonWSChar)
                && previousNonWSChar != ')'
                && previousNonWSChar != ']')
           );

    if (!isPR)
    {
        wchar nextChar = peekNextChar();
        isPR |= (!isWhiteSpace(nextChar)
                 && nextChar != '-'
                 && nextChar != '('
                 && nextChar != '['
                 && !isLegalNameChar(nextChar));
    }

    return isPR;
}


/**
 * check if the currently reached '-' character is
 * a unary minus
 * this method takes for granted that the current character
 * is a '-'.
 *
 * @return        whether the current '-' is a unary minus.
 */
bool ASFormatter::isUnaryMinus() const
{
    return ((previousOperator == &AS_RETURN || !isalnum(previousCommandChar))
            && previousCommandChar != '.'
            && previousCommandChar != ')'
            && previousCommandChar != ']');
}


/**
 * check if the currently reached '-' or '+' character is
 * part of an exponent, i.e. 0.2E-5.
 * this method takes for granted that the current character
 * is a '-' or '+'.
 *
 * @return        whether the current '-' is in an exponent.
 */
bool ASFormatter::isInExponent() const
{
    int formattedLineLength = formattedLine.GetCount();
    if (formattedLineLength >= 2)
    {
        char prevPrevFormattedChar = formattedLine[formattedLineLength - 2];
        char prevFormattedChar = formattedLine[formattedLineLength - 1];

        return ((prevFormattedChar == 'e' || prevFormattedChar == 'E')
                && (prevPrevFormattedChar == '.' || isdigit(prevPrevFormattedChar)));
    }
    else
        return false;
}

/**
 * check if a one-line bracket has been reached,
 * i.e. if the currently reached '{' character is closed
 * with a complimentry '}' elsewhere on the current line,
 *.
 * @return        has a one-line bracket been reached?
 */
bool ASFormatter::isOneLineBlockReached() const
{
    bool isInComment = false;
    bool isInQuote = false;
    int bracketCount = 1;
    int currentLineLength = currentLine.GetCount();
    char quoteChar = ' ';

    for (int i = charNum + 1; i < currentLineLength; ++i)
    {
        char ch = currentLine[i];

        if (isInComment)
        {
            if (currentLine.Mid(i, 2) == WString("*/"))
            {
                isInComment = false;
                ++i;
            }
            continue;
        }

        if (ch == '\\')
        {
            ++i;
            continue;
        }

        if (isInQuote)
        {
            if (ch == quoteChar)
                isInQuote = false;
            continue;
        }

        if (ch == '"' || ch == '\'')
        {
            isInQuote = true;
            quoteChar = ch;
            continue;
        }

        if (currentLine.Mid(i, 2) == WString("//"))
            break;

        if (currentLine.Mid(i, 2) == WString("/*"))
        {
            isInComment = true;
            ++i;
            continue;
        }

        if (ch == '{')
            ++bracketCount;
        else if (ch == '}')
            --bracketCount;

        if (bracketCount == 0)
            return true;
    }

    return false;
}

/**
 * check if one of a set of headers has been reached in the
 * current position of the current line.
 *
 * @return             a pointer to the found header. Or a NULL if no header has been reached.
 * @param headers      a vector of headers.
 * @param checkBoundry
 */
const WString *ASFormatter::findHeader(const Vector<const WString*> &headers, bool checkBoundry)
{
    return ASBeautifier::findHeader(currentLine, charNum, headers, checkBoundry);
}

/**
 * check if a line begins with the specified character
 * i.e. if the current line begins with a open bracket.
 *
 * @return        true or false
 */
bool ASFormatter::lineBeginsWith(wchar charToCheck) const
{
    bool beginsWith = false;
    int i = ASString_Find_First_Not_Of(currentLine, " \t");
    if (i >= 0)
        if (currentLine[i] == charToCheck && (int) i == charNum)
            beginsWith = true;

    return beginsWith;
}

/**
 * adjust comment position because of adding or deleting spaces
 * the spaces are added or deleted to formattedLine
 * spacePadNum contains the adjustment
 */
void ASFormatter::adjustComments(void)
{
    assert(spacePadNum != 0);
    assert(currentLine.Mid(charNum, 2) ==  WString("//")
           || currentLine.Mid(charNum, 2) == WString("/*"));


    // block comment must be closed on this line with nothing after it
    if (currentLine.Mid(charNum, 2) == WString("/*"))
    {
        int endNum = currentLine.Find("*/", charNum + 2);
        if (endNum < 0)
            return;
        if(ASString_Find_First_Not_Of(currentLine, " \t", endNum + 2) >= 0)
            return;
    }

    int len = formattedLine.GetCount();
    // if spaces were removed, need to add spaces before the comment
    if (spacePadNum < 0)
    {
        int adjust = -spacePadNum;          // make the number positive
        if (formattedLine[len-1] != '\t')   // don't adjust if a tab
            formattedLine.Cat(WString(' ', adjust));
//      else                                // comment out to avoid compiler warning
//          adjust = 0;
//      TRcomment(adjust);                  // trace macro
    }
    // if spaces were added, need to delete spaces before the comment, if possible
    else if (spacePadNum > 0)
    {
        int adjust = spacePadNum;
        if (ASString_Find_Last_Not_Of(formattedLine, " ") < len - adjust - 1
                && formattedLine[len-1] != '\t')    // don't adjust a tab
            formattedLine.Trim(len - adjust);
        // the following are commented out to avoid a Borland compiler warning
        //else
        //    adjust = 0;
        TRcomment(-adjust);                 // trace macro
    }
}

/**
 * append the current bracket inside the end of line comments
 * currentChar contains the bracket, it will be appended to formattedLine
 * formattedLineCommentNum is the comment location on formattedLine
 */
void ASFormatter::appendCharInsideComments(void)
{
    if (formattedLineCommentNum == -1               // does the comment start on the previous line?
            || isBeforeComment())                   // does a comment follow on this line?
    {
        appendCurrentChar(true);            // don't attach
        return;
    }
    assert(formattedLine.Mid(formattedLineCommentNum, 2) == WString("//")
           || formattedLine.Mid(formattedLineCommentNum, 2) == WString("/*"));

    // find the previous non space char
    int end = formattedLineCommentNum;
    int beg = ASString_Find_Last_Not_Of(formattedLine, " \t", end-1);
    if (beg < 0)                            // is the previous line comment only?
    {
        appendCurrentChar(true);            // don't attach
        return;
    }
    beg++;

    // insert the bracket
    if (end - beg < 3)                      // is there room to insert?
        formattedLine.Insert(beg, WString(' ', 3-end+beg));
    if (formattedLine[beg] == '\t')         // don't pad with a tab
        formattedLine.Insert(beg, ' ');
    formattedLine.Set(beg+1, currentChar);
}

/**
 * add or remove space padding to operators
 * currentChar contains the paren
 * the operators and necessary padding will be appended to formattedLine
 * the calling function should have a continue statement after calling this method
 *
 * @param *newOperator     the operator to be padded
 */
void ASFormatter::padOperators(const WString *newOperator)
{
    assert (shouldPadOperators);
    assert(newOperator != NULL);

    bool shouldPad = (newOperator != &AS_COLON_COLON
                      && newOperator != &AS_PAREN_PAREN
                      && newOperator != &AS_BLPAREN_BLPAREN
                      && newOperator != &AS_PLUS_PLUS
                      && newOperator != &AS_MINUS_MINUS
                      && newOperator != &AS_NOT
                      && newOperator != &AS_BIT_NOT
                      && newOperator != &AS_ARROW
                      && newOperator != &AS_OPERATOR
                      && newOperator != &AS_RETURN
                      && !(newOperator == &AS_MINUS && isInExponent())
                      && !(newOperator == &AS_MINUS             // check for negative number
                           && (previousNonWSChar == '('
                               || previousNonWSChar == '='
                               || previousNonWSChar == ','))
                      && !(newOperator == &AS_PLUS && isInExponent())
                      && previousOperator != &AS_OPERATOR
                      && !((newOperator == &AS_MULT || newOperator == &AS_BIT_AND)
                           && isPointerOrReference())
                      && !(newOperator == &AS_MULT
                           && (previousNonWSChar == '.'
                               || previousNonWSChar == '>'))    // check for ->
                      && !((isInTemplate || isCharImmediatelyPostTemplate)
                           && (newOperator == &AS_LS || newOperator == &AS_GR))
                     );
    // pad before operator
    if (shouldPad
            && !isInBlParen
            && !(newOperator == &AS_COLON && !foundQuestionMark)
            && newOperator != &AS_SEMICOLON
            && newOperator != &AS_COMMA)
        appendSpacePad();
    appendSequence(*newOperator);
    goForward(newOperator->GetCount() - 1);

    // since this block handles '()' and '[]',
    // the parenStack must be updated here accordingly!
    if (newOperator == &AS_PAREN_PAREN
            || newOperator == &AS_BLPAREN_BLPAREN)
        parenStack->back()--;

    currentChar = (*newOperator)[newOperator->GetCount() - 1];
    // pad after operator
    // but do not pad after a '-' that is a unary-minus.
    if (shouldPad
            && !isInBlParen
            && !isBeforeComment()
            && !(newOperator == &AS_MINUS && isUnaryMinus())
            && (currentLine[charNum + 1] != ';')
            && (currentLine.Mid(charNum + 1, 2) != WString("::")))
        appendSpaceAfter();

    previousOperator = newOperator;
    return;
}

/**
 * add or remove space padding to parens
 * currentChar contains the paren
 * the parens and necessary padding will be appended to formattedLine
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::padParens(void)
{
    assert(shouldPadParensOutside || shouldPadParensInside || shouldUnPadParens);
    assert (currentChar == '(' || currentChar == ')');

    if (currentChar == '(')
    {
        int spacesOutsideToDelete = formattedLine.GetCount() - 1;
        int spacesInsideToDelete = 0;

        // compute spaces outside the opening paren to delete
        if (shouldUnPadParens)
        {
            char lastChar = ' ';
            bool prevIsParenHeader = false;
            int i = ASString_Find_Last_Not_Of(formattedLine, " \t");
            if (i >= 0)
            {
                int end = i;
                spacesOutsideToDelete -= i;
                lastChar = formattedLine[i];
                // was last word a paren header?
                int start;          // start of the previous word
                for (start = i; start > 0; start--)
                {
                    if (isLegalNameChar(formattedLine[start]) || formattedLine[start] == '*')
                        continue;
                    start++;
                    break;
                }
                WString prevWord = formattedLine.Mid(start, end-start+1);
                // if previous word is a header, it will be a paren header
                const WString *prevWordH = ASBeautifier::findHeader(formattedLine, start, headers);
                if (prevWordH != NULL)
                {
                    prevIsParenHeader = true;
                    TRxtra(*prevWordH);         // trace macro
                }
                else if (prevWord == WString("return")  // don't unpad return statements
                         || prevWord == WString("*"))     // don't unpad multiply or pointer
                {
                    prevIsParenHeader = true;
                    TRxtra(prevWord);           // trace macro
                }
                // don't unpad variables
                else if (prevWord == WString("bool")
                         || prevWord ==  WString("int")
                         || prevWord ==  WString("void")
                         || prevWord ==  WString("void*")
                         || (prevWord.GetCount() >= 6     // check end of word for _t
                             && prevWord.Mid(prevWord.GetCount()-2, 2) == WString("_t"))
                         || prevWord ==  WString("BOOL")
                         || prevWord ==  WString("DWORD")
                         || prevWord ==  WString("HWND")
                         || prevWord ==  WString("INT")
                         || prevWord ==  WString("LPSTR")
                         || prevWord ==  WString("VOID")
                         || prevWord ==  WString("LPVOID")
                        )
                {
                    prevIsParenHeader = true;
                    TRxtra(prevWord);           // trace macro
                }
            }
            // do not unpad operators, but leave them if already padded
            if (shouldPadParensOutside || prevIsParenHeader)
                spacesOutsideToDelete--;
            else if (lastChar == '|'          // check for ||
                     || lastChar == '&'      // check for &&
                     || lastChar == ','
                     || (lastChar == '>' && !foundCastOperator)
                     || lastChar == '<'
                     || lastChar == '?'
                     || lastChar == ':'
                     || lastChar == ';'
                     || lastChar == '='
                     || lastChar == '+'
                     || lastChar == '-'
                     || (lastChar == '*' && isInPotentialCalculation)
                     || lastChar == '/'
                     || lastChar == '%')
                spacesOutsideToDelete--;

            if (spacesOutsideToDelete > 0)
            {
                formattedLine.Remove(i + 1, spacesOutsideToDelete);
                spacePadNum -= spacesOutsideToDelete;
            }
        }

        // pad open paren outside
        wchar peekedCharOutside = peekNextChar();
        if (shouldPadParensOutside)
            if (!(currentChar == '(' && peekedCharOutside == ')'))
                appendSpacePad();

        appendCurrentChar();

        // unpad open paren inside
        if (shouldUnPadParens)
        {
            int j = ASString_Find_First_Not_Of(currentLine, " \t", charNum + 1);
            if (j >= 0)
                spacesInsideToDelete = j - charNum - 1;
            if (shouldPadParensInside)
                spacesInsideToDelete--;
            if (spacesInsideToDelete > 0)
            {
                currentLine.Remove(charNum + 1, spacesInsideToDelete);
                spacePadNum -= spacesInsideToDelete;
            }
        }

        // pad open paren inside
        wchar peekedCharInside = peekNextChar();
        if (shouldPadParensInside)
            if (!(currentChar == '(' && peekedCharInside == ')'))
                appendSpaceAfter();

        TRunpad('(', spacesOutsideToDelete, spacesInsideToDelete);       // trace macro
    }
    else if (currentChar == ')' /*|| currentChar == ']'*/)
    {
        int spacesOutsideToDelete = 0;
        int spacesInsideToDelete = formattedLine.GetCount();

        // unpad close paren inside
        if (shouldUnPadParens)
        {
            int i = ASString_Find_Last_Not_Of(formattedLine, " \t");
            if (i >= 0)
                spacesInsideToDelete = formattedLine.GetCount() - 1 - i;
            if (shouldPadParensInside)
                spacesInsideToDelete--;
            if (spacesInsideToDelete > 0)
            {
                formattedLine.Remove(i + 1, spacesInsideToDelete);
                spacePadNum -= spacesInsideToDelete;
            }
        }

        // pad close paren inside
        if (shouldPadParensInside)
            if (!(previousChar == '(' && currentChar == ')'))
                appendSpacePad();

        appendCurrentChar();

        // unpad close paren outside
        if (shouldUnPadParens)
        {
            // may have end of line comments
            int j = ASString_Find_First_Not_Of(currentLine, " \t", charNum + 1);
            if (j >= 0)
                if (currentLine[j] == '[' || currentLine[j] == ']')
                    spacesOutsideToDelete = j - charNum - 1;
            if (shouldPadParensOutside)
                spacesOutsideToDelete--;
//                    spacesOutsideToDelete--;            // always leave 1 space

            if (spacesOutsideToDelete > 0)
            {
                currentLine.Remove(charNum + 1, spacesOutsideToDelete);
                spacePadNum -= spacesOutsideToDelete;
            }
        }

        // pad close paren outside
        wchar peekedCharOutside = peekNextChar();
        if (shouldPadParensOutside)
            if (peekedCharOutside != ';'
                    && peekedCharOutside != ','
                    && peekedCharOutside != '.'
                    && peekedCharOutside != '-')    // check for ->
//                            && !(currentChar == ']' && peekedCharOutside == '['))
                appendSpaceAfter();

        TRunpad(')', spacesInsideToDelete, 0 /*spacesOutsideToDelete*/);       // trace macro
    }
    return;
}

/**
 * format brackets as attached or broken
 * currentChar contains the bracket
 * the brackets will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 *
 * @param bracketType    the type of bracket to be formatted.
 */
void ASFormatter::formatBrackets(BracketType bracketType)
{
    assert(!IS_A(bracketType, ARRAY_TYPE));
    assert (currentChar == '{' || currentChar == '}');

    if (currentChar == '{')
    {
        parenStack->push_back(0);
    }
    else if (currentChar == '}')
    {
        if (!parenStack->IsEmpty())
        {
            parenStack->pop_back();
        }
    }

    if (currentChar == '{')
    {
        bool bdacBreak = false;
        // should a Linux bracket be broken?
        if (bracketFormatMode == BDAC_MODE)
        {
            // always break a class
            if (IS_A((*bracketTypeStack)[bracketTypeStack->size()-1], CLASS_TYPE))
                bdacBreak = true;
            // break a namespace and the first bracket if a function
            else if (bracketTypeStack->size() <= 2)
            {
                if (IS_A((*bracketTypeStack)[bracketTypeStack->size()-1], NAMESPACE_TYPE)
                        || IS_A((*bracketTypeStack)[bracketTypeStack->size()-1], COMMAND_TYPE))
                    bdacBreak = true;
            }
            // break the first bracket after a namespace if a function
            else if (IS_A((*bracketTypeStack)[bracketTypeStack->size()-2], NAMESPACE_TYPE))
            {
                if (IS_A((*bracketTypeStack)[bracketTypeStack->size()-1], COMMAND_TYPE))
                    bdacBreak = true;
            }
            // if not C style then break the first bracket after a class if a function
            else if (!ASBeautifier::isCStyle)
            {
                if (IS_A((*bracketTypeStack)[bracketTypeStack->size()-2], CLASS_TYPE)
                        && IS_A((*bracketTypeStack)[bracketTypeStack->size()-1], COMMAND_TYPE))
                    bdacBreak = true;
            }
        }
        if (bracketFormatMode == ATTACH_MODE
                || (bracketFormatMode == BDAC_MODE && !bdacBreak))
        {
            // are there comments before the bracket?
            if (isCharImmediatelyPostComment || isCharImmediatelyPostLineComment)
            {
                if ((shouldBreakOneLineBlocks || !IS_A(bracketType,  SINGLE_LINE_TYPE))
                        && peekNextChar() != '}')
                    appendCharInsideComments();
                else
                    appendCurrentChar(true);        // don't attach
            }
            else if (previousCommandChar == '{'
                     || previousCommandChar == '}'
                     || previousCommandChar == ';')  // '}' , ';' chars added for proper handling of '{' immediately after a '}' or ';'
            {
                appendCurrentChar(true);            // don't attach
            }
            else
            {
                int firstChar = ASString_Find_First_Not_Of(formattedLine, " \t");
                if (firstChar < 0)                  // if a blank line preceeds this
                    appendCurrentChar(true);        // don't attach
                else if (shouldBreakOneLineBlocks
                         || !IS_A(bracketType,  SINGLE_LINE_TYPE)
                         || peekNextChar() == '}')
                {
                    appendSpacePad();
                    appendCurrentChar(false);       // OK to attach
                }
                else
                    appendCurrentChar(true);        // don't attach
            }
        }
        else if (bracketFormatMode == BREAK_MODE
                 || (bracketFormatMode == BDAC_MODE && bdacBreak))
        {
            if (isBeforeComment())
            {
                // do not break unless comment is at line end
                if (isBeforeLineEndComment(charNum))
                {
                    currentChar = ' ';              // remove bracket from current line
                    appendOpeningBracket = true;    // append bracket to following line
                }
            }
            else if (!IS_A(bracketType,  SINGLE_LINE_TYPE))
                breakLine();
            else if (shouldBreakOneLineBlocks && peekNextChar() != '}')
                breakLine();

            appendCurrentChar();
        }
        else if (bracketFormatMode == NONE_MODE)
        {
            if (lineBeginsWith('{'))                // is opening bracket broken?
                appendCurrentChar(true);
            else
                appendCurrentChar(false);
        }
    }
    else if (currentChar == '}')
    {
        // mark state of immediately after empty block
        // this state will be used for locating brackets that appear immedately AFTER an empty block (e.g. '{} \n}').
        if (previousCommandChar == '{')
            isImmediatelyPostEmptyBlock = true;

        if ((!(previousCommandChar == '{' && isPreviousBracketBlockRelated))            // this '{' does not close an empty block
                && (shouldBreakOneLineBlocks || !IS_A(bracketType,  SINGLE_LINE_TYPE))  // astyle is allowed to break on line blocks
                && (!(bracketFormatMode == NONE_MODE && IS_A(bracketType,  SINGLE_LINE_TYPE)))
                && !isImmediatelyPostEmptyBlock)                                        // this '}' does not immediately follow an empty block
        {
            breakLine();
            appendCurrentChar();
        }
        else
        {
            if (!isCharImmediatelyPostComment
                    && !(bracketFormatMode == NONE_MODE)
                    && !isImmediatelyPostEmptyBlock)
                isInLineBreak = false;

            appendCurrentChar();

            //if (!bracketFormatMode == NONE_MODE)
            //  if ((shouldBreakOneLineBlocks || !IS_A(bracketType,  SINGLE_LINE_TYPE))
            //          && !(currentChar == '}' && peekNextChar() == ';'))      // fixes }; placed on separate lines
            //      shouldBreakLineAfterComments = true;
        }

        if (shouldBreakBlocks)
        {
            isAppendPostBlockEmptyLineRequested = true;
        }
    }
    return;
}

/**
 * format array brackets as attached or broken
 * determine if the brackets can have an inStatement indent
 * currentChar contains the bracket
 * the brackets will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 *
 * @param bracketType            the type of bracket to be formatted, must be an ARRAY_TYPE.
 * @param isOpeningArrayBracket  indicates if this is the opening bracket for the array block.
 */
void ASFormatter::formatArrayBrackets(BracketType bracketType, bool isOpeningArrayBracket)
{
    assert(IS_A(bracketType, ARRAY_TYPE));
    assert (currentChar == '{' || currentChar == '}');

    if (currentChar == '{')
    {
        // is this the first opening bracket in the array?
        if (isOpeningArrayBracket)
        {
            if (bracketFormatMode == ATTACH_MODE || bracketFormatMode == BDAC_MODE)
            {
                // don't attach to a preprocessor directive
                if (isImmediatelyPostPreprocessor)
                    appendCurrentChar(true);            // don't attach
                // are there comments before the bracket?
                else if (isCharImmediatelyPostComment || isCharImmediatelyPostLineComment)
                {
                    appendCharInsideComments();
                }
                else
                {
                    // if bracket is broken or not an assignment
                    if (lineBeginsWith('{') || previousNonWSChar != '=')
                        appendSpacePad();
                    appendCurrentChar(false);           // OK to attach
                }
            }
            else if (bracketFormatMode == BREAK_MODE)
            {
                if (isWhiteSpace(peekNextChar()))
                    breakLine();
                else if (isBeforeComment())
                {
                    // do not break unless comment is at line end
                    if (isBeforeLineEndComment(charNum))
                    {
                        currentChar = ' ';              // remove bracket from current line
                        appendOpeningBracket = true;    // append bracket to following line
                    }
                }
                appendCurrentChar();
            }
            else if (bracketFormatMode == NONE_MODE)
            {
                if (lineBeginsWith('{'))                // is opening bracket broken?
                    appendCurrentChar();
                else
                    appendCurrentChar(false);
            }
        }
        else
            appendCurrentChar();     // not the first opening bracket - don't change

        // if an opening bracket ends the line there will be no inStatement indent
        wchar nextChar = peekNextChar();
        if (isWhiteSpace(nextChar)
                || isBeforeLineEndComment(charNum)
                || nextChar == '{')
            isNonInStatementArray = true;
        if (isNonInStatementArray)
            TRarray('x');
        else
            TRarray(' ');

    }
    else if (currentChar == '}')
    {
        // does this close the first opening bracket in the array?
        if (isOpeningArrayBracket && !IS_A(bracketType, SINGLE_LINE_TYPE) )
        {
            breakLine();
            appendCurrentChar();
        }
        else
            appendCurrentChar();
    }
}


}   // end namespace astyle
