#include "astyle.h"


#define INIT_CONTAINER(container, value)     {if ( (container) != NULL ) delete (container); (container) = (value); }
#define DELETE_CONTAINER(container)          {if ( (container) != NULL ) delete (container); }


namespace astyle
{
Vector<const WString*> ASBeautifier::headers;
Vector<const WString*> ASBeautifier::nonParenHeaders;
Vector<const WString*> ASBeautifier::preBlockStatements;
Vector<const WString*> ASBeautifier::assignmentOperators;
Vector<const WString*> ASBeautifier::nonAssignmentOperators;


/*
 * initialize the static vars
 */
void ASBeautifier::initStatic()
{
    static int beautifierFileType = 9;     // initialized with an invalid type

    if (fileType == beautifierFileType)    // don't build unless necessary
        return;

    beautifierFileType = fileType;

    headers.clear();
    nonParenHeaders.clear();
    assignmentOperators.clear();
    nonAssignmentOperators.clear();
    preBlockStatements.clear();

    ASResource::buildHeaders(headers, fileType, true);
    ASResource::buildNonParenHeaders(nonParenHeaders, fileType, true);
    ASResource::buildAssignmentOperators(assignmentOperators);
    ASResource::buildNonAssignmentOperators(nonAssignmentOperators);
    ASResource::buildPreBlockStatements(preBlockStatements);

//  cout << "beaut" << endl;
}

/**
 * ASBeautifier's constructor
 */
ASBeautifier::ASBeautifier()
{
    waitingBeautifierStack = NULL;
    activeBeautifierStack = NULL;
    waitingBeautifierStackLengthStack = NULL;
    activeBeautifierStackLengthStack = NULL;

    headerStack  = NULL;
    tempStacks = NULL;
    blockParenDepthStack = NULL;
    blockStatementStack = NULL;
    parenStatementStack = NULL;
    bracketBlockStateStack = NULL;
    inStatementIndentStack = NULL;
    inStatementIndentStackSizeStack = NULL;
    parenIndentStack = NULL;
    sourceIterator = NULL;

    isMinimalConditinalIndentSet = false;
    shouldForceTabIndentation = false;

    setSpaceIndentation(4);
    setMaxInStatementIndentLength(40);
    setClassIndent(false);
    setSwitchIndent(false);
    setCaseIndent(false);
    setBlockIndent(false);
    setBracketIndent(false);
    setNamespaceIndent(false);
    setLabelIndent(false);
    setEmptyLineFill(false);
    fileType = C_TYPE;
    setCStyle();
    setPreprocessorIndent(false);
}

/**
 * ASBeautifier's copy constructor
 */
ASBeautifier::ASBeautifier(const ASBeautifier &other)
{
    waitingBeautifierStack = NULL;
    activeBeautifierStack = NULL;
    waitingBeautifierStackLengthStack = NULL;
    activeBeautifierStackLengthStack = NULL;

    headerStack  = new WithDeepCopy<Vector<const WString*> >;
    *headerStack <<= *other.headerStack;

    tempStacks = new WithDeepCopy<Vector<Vector<const WString*>*> >;
    WithDeepCopy<Vector<Vector<const WString*>*> >::iterator iter;
    for (iter = other.tempStacks->begin();
            iter != other.tempStacks->end();
            ++iter)
    {
        WithDeepCopy<Vector<const WString*> > *newVec = new WithDeepCopy<Vector<const WString*> >;
        *newVec = **iter;
        tempStacks->push_back(newVec);
    }
    blockParenDepthStack = new WithDeepCopy<Vector<int> >;
    *blockParenDepthStack = *other.blockParenDepthStack;

    blockStatementStack = new WithDeepCopy<Vector<bool> >;
    *blockStatementStack = *other.blockStatementStack;

    parenStatementStack =  new WithDeepCopy<Vector<bool> >;
    *parenStatementStack = *other.parenStatementStack;

    bracketBlockStateStack = new WithDeepCopy<Vector<bool> >;
    *bracketBlockStateStack = *other.bracketBlockStateStack;

    inStatementIndentStack = new WithDeepCopy<Vector<int> >;
    *inStatementIndentStack = *other.inStatementIndentStack;

    inStatementIndentStackSizeStack = new WithDeepCopy<Vector<int> >;
    *inStatementIndentStackSizeStack = *other.inStatementIndentStackSizeStack;

    parenIndentStack = new WithDeepCopy<Vector<int> >;
    *parenIndentStack = *other.parenIndentStack;

    sourceIterator = other.sourceIterator;

    // protected variables
    fileType = other.fileType;
    isCStyle = other.isCStyle;
    isJavaStyle = other.isJavaStyle;
    isSharpStyle = other.isSharpStyle;

    // variables set by ASFormatter
    // must also be updated in preprocessor
    inLineNumber = other.inLineNumber;
    outLineNumber = other.outLineNumber;
    lineCommentNoBeautify = other.lineCommentNoBeautify;
    isNonInStatementArray = other.isNonInStatementArray;

    // private variables
    indentString = other.indentString;
    currentHeader = other.currentHeader;
    previousLastLineHeader = other.previousLastLineHeader;
    immediatelyPreviousAssignmentOp = other.immediatelyPreviousAssignmentOp;
    probationHeader = other.probationHeader;
    isInQuote = other.isInQuote;
    isInComment = other.isInComment;
    isInCase = other.isInCase;
    isInQuestion = other.isInQuestion;
    isInStatement = other.isInStatement;
    isInHeader = other.isInHeader;
    isInOperator = other.isInOperator;
    isInTemplate = other.isInTemplate;
    isInDefine = other.isInDefine;
    isInDefineDefinition = other.isInDefineDefinition;
    classIndent = other.classIndent;
    isInClassHeader = other.isInClassHeader;
    isInClassHeaderTab = other.isInClassHeaderTab;
    switchIndent = other.switchIndent;
    caseIndent = other.caseIndent;
    namespaceIndent = other.namespaceIndent;
    bracketIndent = other.bracketIndent;
    blockIndent = other.blockIndent;
    labelIndent = other.labelIndent;
    preprocessorIndent = other.preprocessorIndent;
    isInConditional = other.isInConditional;
    isMinimalConditinalIndentSet = other.isMinimalConditinalIndentSet;
    shouldForceTabIndentation = other.shouldForceTabIndentation;
    emptyLineFill = other.emptyLineFill;
    backslashEndsPrevLine = other.backslashEndsPrevLine;
    blockCommentNoIndent = other.blockCommentNoIndent;
    blockCommentNoBeautify = other.blockCommentNoBeautify;
    previousLineProbationTab = other.previousLineProbationTab;
    minConditionalIndent = other.minConditionalIndent;
    parenDepth = other.parenDepth;
    indentLength = other.indentLength;
    blockTabCount = other.blockTabCount;
    leadingWhiteSpaces = other.leadingWhiteSpaces;
    maxInStatementIndent = other.maxInStatementIndent;
    templateDepth = other.templateDepth;
    prevFinalLineSpaceTabCount = other.prevFinalLineSpaceTabCount;
    prevFinalLineTabCount = other.prevFinalLineTabCount;
    defineTabCount = other.defineTabCount;
    quoteChar = other.quoteChar;
    prevNonSpaceCh = other.prevNonSpaceCh;
    currentNonSpaceCh = other.currentNonSpaceCh;
    currentNonLegalCh = other.currentNonLegalCh;
    prevNonLegalCh = other.prevNonLegalCh;
}

/**
 * ASBeautifier's destructor
 */
ASBeautifier::~ASBeautifier()
{
    DELETE_CONTAINER(headerStack);
    DELETE_CONTAINER(blockParenDepthStack);
    DELETE_CONTAINER(blockStatementStack);
    DELETE_CONTAINER(parenStatementStack);
    DELETE_CONTAINER(bracketBlockStateStack);
    DELETE_CONTAINER(inStatementIndentStack);
    DELETE_CONTAINER(inStatementIndentStackSizeStack);
    DELETE_CONTAINER(parenIndentStack);

    //this two were missing...
    DELETE_CONTAINER(waitingBeautifierStackLengthStack);
    DELETE_CONTAINER(activeBeautifierStackLengthStack);

    //the correct way to release tempStacks, waitingBeautifierStack and activeBeautifierStack
    if(tempStacks)
    {
        Vector< Vector<const WString*>* >::iterator iter = tempStacks->begin();
        for(; iter != tempStacks->end(); iter++)
            delete *iter;
        tempStacks->clear();
        delete tempStacks;
        tempStacks = NULL;
    }

    if(waitingBeautifierStack)
    {
        Vector<ASBeautifier*>::iterator iter = waitingBeautifierStack->begin();
        for(; iter != waitingBeautifierStack->end(); iter++)
            delete *iter;
        waitingBeautifierStack->clear();
        delete waitingBeautifierStack;
        waitingBeautifierStack = NULL;
    }

    if(activeBeautifierStack)
    {
        Vector<ASBeautifier*>::iterator iter = activeBeautifierStack->begin();
        for(; iter != activeBeautifierStack->end(); iter++)
            delete *iter;
        activeBeautifierStack->clear();
        delete activeBeautifierStack;
        activeBeautifierStack = NULL;
    }
}

/**
 * initialize the ASBeautifier.
 *
 * init() should be called every time a ABeautifier object is to start
 * beautifying a NEW source file.
 * init() recieves a pointer to a DYNAMICALLY CREATED ASSourceIterator object
 * that will be used to iterate through the source code. This object will be
 * deleted during the ASBeautifier's destruction, and thus should not be
 * deleted elsewhere.
 *
 * @param iter     a pointer to the DYNAMICALLY CREATED ASSourceIterator object.
 */
void ASBeautifier::init(ASSourceIterator *iter)
{
    sourceIterator = iter;
    init();
}

/**
 * initialize the ASBeautifier.
 */
void ASBeautifier::init()
{
    initStatic();

    INIT_CONTAINER(waitingBeautifierStack,  new Vector<ASBeautifier*>);
    INIT_CONTAINER(activeBeautifierStack,  new Vector<ASBeautifier*>);

    INIT_CONTAINER(waitingBeautifierStackLengthStack, new Vector<int>);
    INIT_CONTAINER(activeBeautifierStackLengthStack, new Vector<int>);

    INIT_CONTAINER(headerStack,  new WithDeepCopy<Vector<const WString*> >);
    INIT_CONTAINER(tempStacks, new WithDeepCopy<Vector<Vector<const WString*>*> >);
    tempStacks->push_back(new Vector<const WString*>);

    INIT_CONTAINER(blockParenDepthStack, new WithDeepCopy<Vector<int> >);
    INIT_CONTAINER(blockStatementStack, new WithDeepCopy<Vector<bool> >);
    INIT_CONTAINER(parenStatementStack, new WithDeepCopy<Vector<bool> >);

    INIT_CONTAINER(bracketBlockStateStack, new WithDeepCopy<Vector<bool> >);
    bracketBlockStateStack->push_back(true);

    INIT_CONTAINER(inStatementIndentStack, new WithDeepCopy<Vector<int> >);
    INIT_CONTAINER(inStatementIndentStackSizeStack, new WithDeepCopy<Vector<int> >);
    inStatementIndentStackSizeStack->push_back(0);
    INIT_CONTAINER(parenIndentStack, new WithDeepCopy<Vector<int> >);

    immediatelyPreviousAssignmentOp = NULL;
    previousLastLineHeader = NULL;
    currentHeader = NULL;

    isInQuote = false;
    isInComment = false;
    isInStatement = false;
    isInCase = false;
    isInQuestion = false;
    isInClassHeader = false;
    isInClassHeaderTab = false;
    isInHeader = false;
    isInOperator = false;
    isInTemplate = false;
    isInConditional = false;
    templateDepth = 0;
    parenDepth = 0;
    blockTabCount = 0;
    leadingWhiteSpaces = 0;
    prevNonSpaceCh = '{';
    currentNonSpaceCh = '{';
    prevNonLegalCh = '{';
    currentNonLegalCh = '{';
    quoteChar = ' ';
    prevFinalLineSpaceTabCount = 0;
    prevFinalLineTabCount = 0;
    probationHeader = NULL;
    backslashEndsPrevLine = false;
    isInDefine = false;
    isInDefineDefinition = false;
    defineTabCount = 0;
    lineCommentNoBeautify = false;
    blockCommentNoIndent = false;
    blockCommentNoBeautify = false;
    previousLineProbationTab = false;
    isNonInStatementArray = false;
    inLineNumber = -1;           // for debugging
    outLineNumber = 0;           // for debugging
}

/**
 * set indentation style to C/C++.
 */
void ASBeautifier::setCStyle()
{
    fileType = C_TYPE;
    isCStyle     = true;
    isJavaStyle  = false;
    isSharpStyle = false;
}

/**
 * set indentation style to Java.
 */
void ASBeautifier::setJavaStyle()
{
    fileType = JAVA_TYPE;
    isJavaStyle  = true;
    isCStyle     = false;
    isSharpStyle = false;
}

/**
 * set indentation style to C#.
 */
void ASBeautifier::setSharpStyle()
{
    fileType = SHARP_TYPE;
    isSharpStyle = true;
    isCStyle     = false;
    isJavaStyle  = false;
}

/**
 * indent using one tab per indentation
 */
void ASBeautifier::setTabIndentation(int length, bool forceTabs)
{
    indentString = "\t";
    indentLength = length;
    shouldForceTabIndentation = forceTabs;

    if (!isMinimalConditinalIndentSet)
        minConditionalIndent = indentLength * 2;
}

/**
 * indent using a number of spaces per indentation.
 *
 * @param   length     number of spaces per indent.
 */
void ASBeautifier::setSpaceIndentation(int length)
{
    indentString = WString(' ', length);
    indentLength = length;

    if (!isMinimalConditinalIndentSet)
        minConditionalIndent = indentLength * 2;
}

/**
 * set the maximum indentation between two lines in a multi-line statement.
 *
 * @param   max     maximum indentation length.
 */
void ASBeautifier::setMaxInStatementIndentLength(int max)
{
    maxInStatementIndent = max;
}

/**
 * set the minimum indentation between two lines in a multi-line condition.
 *
 * @param   min     minimal indentation length.
 */
void ASBeautifier::setMinConditionalIndentLength(int min)
{
    minConditionalIndent = min;
    isMinimalConditinalIndentSet = true;
}

/**
 * set the state of the bracket indentation option. If true, brackets will
 * be indented one additional indent.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setBracketIndent(bool state)
{
    bracketIndent = state;
}

/**
 * set the state of the block indentation option. If true, entire blocks
 * will be indented one additional indent, similar to the GNU indent style.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setBlockIndent(bool state)
{
    if (state)
        setBracketIndent(false); // so that we don't have both bracket and block indent
    blockIndent = state;
}

/**
 * set the state of the class indentation option. If true, C++ class
 * definitions will be indented one additional indent.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setClassIndent(bool state)
{
    classIndent = state;
}

/**
 * set the state of the switch indentation option. If true, blocks of 'switch'
 * statements will be indented one additional indent.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setSwitchIndent(bool state)
{
    switchIndent = state;
}

/**
 * set the state of the case indentation option. If true, lines of 'case'
 * statements will be indented one additional indent.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setCaseIndent(bool state)
{
    caseIndent = state;
}

/**
 * set the state of the namespace indentation option.
 * If true, blocks of 'namespace' statements will be indented one
 * additional indent. Otherwise, NO indentation will be added.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setNamespaceIndent(bool state)
{
    namespaceIndent = state;
}

/**
 * set the state of the label indentation option.
 * If true, labels will be indented one indent LESS than the
 * current indentation level.
 * If false, labels will be flushed to the left with NO
 * indent at all.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setLabelIndent(bool state)
{
    labelIndent = state;
}

/**
 * set the state of the preprocessor indentation option.
 * If true, multiline #define statements will be indented.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setPreprocessorIndent(bool state)
{
    preprocessorIndent = state;
}

/**
 * set the state of the empty line fill option.
 * If true, empty lines will be filled with the whitespace.
 * of their previous lines.
 * If false, these lines will remain empty.
 *
 * @param   state             state of option.
 */
void ASBeautifier::setEmptyLineFill(bool state)
{
    emptyLineFill = state;
}

/**
 * get the number of spaces per indent
 *
 * @return   value of indentLength option.
*/
int ASBeautifier::getIndentLength(void)
{
    return indentLength;
}

/**
 * get the char used for indentation, space or tab
  *
 * @return   the char used for indentation.
 */
WString ASBeautifier::getIndentString(void)
{
    return indentString;
}

/**
 * get the state of the case indentation option. If true, lines of 'case'
 * statements will be indented one additional indent.
 *
 * @return   state of caseIndent option.
 */
bool ASBeautifier::getCaseIndent(void)
{
    return caseIndent;
}

/**
 * get C style identifier.
 * If true, a C source is being indented.
 *
 * @return   state of isCStyle option.
 */
bool ASBeautifier::getCStyle(void)
{
    return isCStyle;
}

/**
 * get Java style identifier.
 * If true, a Java source is being indented.
 *
 * @return   state of isJavaStyle option.
 */
bool ASBeautifier::getJavaStyle(void)
{
    return isJavaStyle;
}

/**
 * get C# style identifier.
 * If true, a C# source is being indented.
 *
 * @return   state of isSharpStyle option.
 */
bool ASBeautifier::getSharpStyle(void)
{
    return isSharpStyle;
}

/**
 * get the state of the empty line fill option.
 * If true, empty lines will be filled with the whitespace.
 * of their previous lines.
 * If false, these lines will remain empty.
 *
 * @return   state of emptyLineFill option.
 */
bool ASBeautifier::getEmptyLineFill(void)
{
    return emptyLineFill;
}

/**
 * check if there are any indented lines ready to be read by nextLine()
 *
 * @return    are there any indented lines ready?
 */
bool ASBeautifier::hasMoreLines() const
{
    return sourceIterator->hasMoreLines();
}

/**
 * get the next indented line.
 *
 * @return    indented line.
 */
WString ASBeautifier::nextLine()
{
    return beautify(sourceIterator->nextLine());
}

/**
 * beautify a line of source code.
 * every line of source code in a source code file should be sent
 * one after the other to the beautify method.
 *
 * @return      the indented line.
 * @param originalLine       the original unindented line.
 */
WString ASBeautifier::beautify(const WString &originalLine)
{
    WString line;
    bool isInLineComment = false;
    bool lineStartsInComment = false;
    bool isInClass = false;
    bool isInSwitch = false;
    bool isImmediatelyAfterConst = false;
    bool isSpecialChar = false;
    wchar ch = ' ';
    wchar prevCh;
    WString outBuffer; // the newly idented line is bufferd here
    int tabCount = 0;
    const WString *lastLineHeader = NULL;
    bool closingBracketReached = false;
    int spaceTabCount = 0;
    wchar tempCh;
    int headerStackSize = headerStack->GetCount();
    bool shouldIndentBrackettedLine = true;
    int lineOpeningBlocksNum = 0;
    int lineClosingBlocksNum = 0;
    bool previousLineProbation = (probationHeader != NULL);
    int i;

    currentHeader = NULL;
    lineStartsInComment = isInComment;
    blockCommentNoBeautify = blockCommentNoIndent;
    previousLineProbationTab = false;
    outLineNumber++;

    // handle and remove white spaces around the line:
    // If not in comment, first find out size of white space before line,
    // so that possible comments starting in the line continue in
    // relation to the preliminary white-space.
    if (!isInComment)
    {
        int strlen = originalLine.GetCount();
        leadingWhiteSpaces = 0;

        for (int j = 0; j < strlen && isWhiteSpace(originalLine[j]); j++)
        {
            if (originalLine[j] == '\t')
                leadingWhiteSpaces += indentLength;
            else
                leadingWhiteSpaces++;
        }
        line = trim(originalLine);
    }
    else
    {
        // convert leading tabs to spaces
        WString spaceTabs(' ', indentLength);
        WString newLine = originalLine;
        int strlen = newLine.GetCount();

        for (int j=0; j < leadingWhiteSpaces && j < strlen; j++)
        {
            if (newLine[j] == '\t')
            {
                ASString_Replace(newLine, j, 1, spaceTabs);
                strlen = newLine.GetCount();
            }
        }

        // trim the comment leaving the new leading whitespace
        int trimSize = 0;
        strlen = newLine.GetCount();

        while (trimSize < strlen
                && trimSize < leadingWhiteSpaces
                && isWhiteSpace(newLine[trimSize]))
            trimSize++;


        while (trimSize < strlen && isWhiteSpace(newLine[strlen-1]))
            strlen--;

        line = newLine.Mid(trimSize, strlen);
        int trimEnd = ASString_Find_Last_Not_Of(line, " \t");
        if (trimEnd >= 0)
        {
            int spacesToDelete = line.GetCount() - 1 - trimEnd;
            if (spacesToDelete > 0)
                line.Remove(trimEnd + 1, spacesToDelete);
        }
    }


    if (line.GetCount() == 0)
    {
        if (backslashEndsPrevLine)  // must continue to clear variables
            line = " ";
        else if (emptyLineFill)
            return preLineWS(prevFinalLineSpaceTabCount, prevFinalLineTabCount);
        else
            return line;
    }

    // handle preprocessor commands

    if (isCStyle && !isInComment && (line[0] == '#' || backslashEndsPrevLine))
    {
        if (line[0] == '#')
        {
            WString preproc = trim(line.Mid(1));

            // When finding a multi-lined #define statement, the original beautifier
            // 1. sets its isInDefineDefinition flag
            // 2. clones a new beautifier that will be used for the actual indentation
            //    of the #define. This clone is put into the activeBeautifierStack in order
            //    to be called for the actual indentation.
            // The original beautifier will have isInDefineDefinition = true, isInDefine = false
            // The cloned beautifier will have   isInDefineDefinition = true, isInDefine = true
            if (preprocessorIndent && preproc.Mid(0, 6) == WString("define") &&  *line.Last() == '\\')
            {
                if (!isInDefineDefinition)
                {
                    ASBeautifier *defineBeautifier;

                    // this is the original beautifier
                    isInDefineDefinition = true;

                    // push a new beautifier into the active stack
                    // this beautifier will be used for the indentation of this define
                    defineBeautifier = new ASBeautifier(*this);
                    activeBeautifierStack->push_back(defineBeautifier);
                }
                else
                {
                    // the is the cloned beautifier that is in charge of indenting the #define.
                    isInDefine = true;
                }
            }
            else if (preproc.Mid(0, 2) == WString("if"))
            {
                // push a new beautifier into the stack
                waitingBeautifierStackLengthStack->push_back(waitingBeautifierStack->GetCount());
                activeBeautifierStackLengthStack->push_back(activeBeautifierStack->GetCount());
                waitingBeautifierStack->push_back(new ASBeautifier(*this));
            }
            else if (preproc.Mid(0, 4) == WString("else"))
            {
                if (waitingBeautifierStack && !waitingBeautifierStack->IsEmpty())
                {
                    // MOVE current waiting beautifier to active stack.
                    activeBeautifierStack->push_back(waitingBeautifierStack->back());
                    waitingBeautifierStack->pop_back();
                }
            }
            else if (preproc.Mid(0, 4) == WString("elif"))
            {
                if (waitingBeautifierStack && !waitingBeautifierStack->IsEmpty())
                {
                    // append a COPY current waiting beautifier to active stack, WITHOUT deleting the original.
                    activeBeautifierStack->push_back(new ASBeautifier(*(waitingBeautifierStack->back())));
                }
            }
            else if (preproc.Mid(0, 5) == WString("endif"))
            {
                int stackLength;
                ASBeautifier *beautifier;

                if (waitingBeautifierStackLengthStack && !waitingBeautifierStackLengthStack->IsEmpty())
                {
                    stackLength = waitingBeautifierStackLengthStack->back();
                    waitingBeautifierStackLengthStack->pop_back();
                    while ((int) waitingBeautifierStack->GetCount() > stackLength)
                    {
                        beautifier = waitingBeautifierStack->back();
                        waitingBeautifierStack->pop_back();
                        delete beautifier;
                    }
                }

                if (!activeBeautifierStackLengthStack->IsEmpty())
                {
                    stackLength = activeBeautifierStackLengthStack->back();
                    activeBeautifierStackLengthStack->pop_back();
                    while ((int) activeBeautifierStack->GetCount() > stackLength)
                    {
                        beautifier = activeBeautifierStack->back();
                        activeBeautifierStack->pop_back();
                        delete beautifier;
                    }
                }
            }
        }

        // check if the last char is a backslash
        if (line.GetCount() > 0)
            backslashEndsPrevLine = (line[line.GetCount() - 1] == '\\');
        else
            backslashEndsPrevLine = false;

        // check if this line ends a multi-line #define
        // if so, use the #define's cloned beautifier for the line's indentation
        // and then remove it from the active beautifier stack and delete it.
        if (!backslashEndsPrevLine && isInDefineDefinition && !isInDefine)
        {
            WString beautifiedLine;
            ASBeautifier *defineBeautifier;

            isInDefineDefinition = false;
            defineBeautifier = activeBeautifierStack->back();
            activeBeautifierStack->pop_back();

            beautifiedLine = defineBeautifier->beautify(line);
            delete defineBeautifier;
            return beautifiedLine;
        }

        // unless this is a multi-line #define, return this precompiler line as is.
        if (!isInDefine && !isInDefineDefinition)
            return originalLine;
    }

    // if there exists any worker beautifier in the activeBeautifierStack,
    // then use it instead of me to indent the current line.
    // variables set by ASFormatter must be updated.
    if (!isInDefine && activeBeautifierStack != NULL && !activeBeautifierStack->IsEmpty())
    {
        activeBeautifierStack->back()->inLineNumber = inLineNumber;
        activeBeautifierStack->back()->outLineNumber = outLineNumber;
        activeBeautifierStack->back()->lineCommentNoBeautify = lineCommentNoBeautify;
        activeBeautifierStack->back()->isNonInStatementArray = isNonInStatementArray;
        return activeBeautifierStack->back()->beautify(line);
    }

    // calculate preliminary indentation based on data from past lines
    if (!inStatementIndentStack->IsEmpty())
        spaceTabCount = inStatementIndentStack->back();


    for (i = 0; i < (int) headerStackSize; i++)
    {
        isInClass = false;

        if (blockIndent || (!(i > 0 && (*headerStack)[i-1] != &AS_OPEN_BRACKET
                              && (*headerStack)[i] == &AS_OPEN_BRACKET)))
            ++tabCount;

        if (!isJavaStyle && !namespaceIndent && i >= 1
                && (*headerStack)[i-1] == &AS_NAMESPACE
                && (*headerStack)[i] == &AS_OPEN_BRACKET)
            --tabCount;

        if (isCStyle && i >= 1
                && (*headerStack)[i-1] == &AS_CLASS
                && (*headerStack)[i] == &AS_OPEN_BRACKET)
        {
            if (classIndent)
                ++tabCount;
            isInClass = true;
        }

        // is the switchIndent option is on, indent switch statements an additional indent.
        else if (switchIndent && i > 1 &&
                 (*headerStack)[i-1] == &AS_SWITCH &&
                 (*headerStack)[i] == &AS_OPEN_BRACKET
                )
        {
            ++tabCount;
            isInSwitch = true;
        }

    }

    if (!lineStartsInComment
            && isCStyle
            && isInClass
            && classIndent
            && headerStackSize >= 2
            && (*headerStack)[headerStackSize-2] == &AS_CLASS
            && (*headerStack)[headerStackSize-1] == &AS_OPEN_BRACKET
            && line[0] == '}')
        --tabCount;

    else if (!lineStartsInComment
             && isInSwitch
             && switchIndent
             && headerStackSize >= 2
             && (*headerStack)[headerStackSize-2] == &AS_SWITCH
             && (*headerStack)[headerStackSize-1] == &AS_OPEN_BRACKET
             && line[0] == '}')
        --tabCount;

    if (isInClassHeader)
    {
        isInClassHeaderTab = true;
        tabCount += 2;
    }

    if (isInConditional)
    {
        --tabCount;
    }


    // parse characters in the current line.

    for (i = 0; i < (int) line.GetCount(); i++)
    {
        tempCh = line[i];

        prevCh = ch;
        ch = tempCh;

//      outBuffer.append(1, ch);
        outBuffer.Cat(ch);

        if (isWhiteSpace(ch))
            continue;

        // check for utf8 characters
        // isalnum() will display an assert message in debug if not bypassed here
        if (ch < 0)
            continue;

        // handle special characters (i.e. backslash+character such as \n, \t, ...)
        if (isSpecialChar)
        {
            isSpecialChar = false;
            continue;
        }
        if (!(isInComment || isInLineComment) && line.Mid(i, 2) == WString("\\\\"))
        {
            outBuffer.Cat('\\');
            i++;
            continue;
        }
        if (!(isInComment || isInLineComment) && ch == '\\')
        {
            isSpecialChar = true;
            continue;
        }

        // handle quotes (such as 'x' and "Hello Dolly")
        if (!(isInComment || isInLineComment) && (ch == '"' || ch == '\'')) {
            if (!isInQuote)
            {
                quoteChar = ch;
                isInQuote = true;
            }
            else if (quoteChar == ch)
            {
                isInQuote = false;
                isInStatement = true;
                continue;
            }
        }
        if (isInQuote)
            continue;

        // handle comments

        if (!(isInComment || isInLineComment) && line.Mid(i, 2) == WString("//"))
        {
            isInLineComment = true;
            outBuffer.Cat('/');
            i++;
            continue;
        }
        else if (!(isInComment || isInLineComment) && line.Mid(i, 2) == WString("/*"))
        {
            isInComment = true;
            outBuffer.Cat('*');
            i++;
            int j = ASString_Find_First_Not_Of(line, " \t");
            if (line.Mid(j, 2) != WString("/*"))    // does line start with comment?
                blockCommentNoIndent = true;        // if no, cannot indent continuation lines
            continue;
        }
        else if ((isInComment || isInLineComment) && line.Mid(i, 2) ==  WString("*/"))
        {
            isInComment = false;
            outBuffer.Cat('/');
            i++;
            blockCommentNoIndent = false;       // ok to indent next comment
            continue;
        }

        if (isInComment || isInLineComment)
            continue;

        // if we have reached this far then we are NOT in a comment or String of special character...

        if (probationHeader != NULL)
        {
            if (((probationHeader == &AS_STATIC || probationHeader == &AS_CONST) && ch == '{')
                    || (probationHeader == &AS_SYNCHRONIZED && ch == '('))
            {
                // insert the probation header as a new header
                isInHeader = true;
                headerStack->push_back(probationHeader);

                // handle the specific probation header
                isInConditional = (probationHeader == &AS_SYNCHRONIZED);
                if (probationHeader == &AS_CONST)
                    isImmediatelyAfterConst = true;

                isInStatement = false;
                // if the probation comes from the previous line, then indent by 1 tab count.
                if (previousLineProbation && ch == '{')
                {
                    tabCount++;
                    previousLineProbationTab = true;
                }
                previousLineProbation = false;
            }

            // dismiss the probation header
            probationHeader = NULL;
        }

        prevNonSpaceCh = currentNonSpaceCh;
        currentNonSpaceCh = ch;
        if (!isLegalNameChar(ch) && ch != ',' && ch != ';')
        {
            prevNonLegalCh = currentNonLegalCh;
            currentNonLegalCh = ch;
        }

        if (isInHeader)
        {
            isInHeader = false;
            currentHeader = headerStack->back();
        }
        else
            currentHeader = NULL;

        if (isCStyle && isInTemplate
                && (ch == '<' || ch == '>')
                &&  findHeader(line, i, nonAssignmentOperators) == NULL)
        {
            if (ch == '<')
            {
                ++templateDepth;
            }
            else if (ch == '>')
            {
                if (--templateDepth <= 0)
                {
                    if (isInTemplate)
                        ch = ';';
                    else
                        ch = 't';
                    isInTemplate = false;
                    templateDepth = 0;
                }
            }
        }

        // handle parenthesies
        if (ch == '(' || ch == '[' || ch == ')' || ch == ']')
        {
            if (ch == '(' || ch == '[')
            {
                if (parenDepth == 0)
                {
                    parenStatementStack->push_back(isInStatement);
                    isInStatement = true;
                }
                parenDepth++;

                inStatementIndentStackSizeStack->push_back(inStatementIndentStack->GetCount());

                if (currentHeader != NULL)
                    registerInStatementIndent(line, i, spaceTabCount, minConditionalIndent/*indentLength*2*/, true);
                else
                    registerInStatementIndent(line, i, spaceTabCount, 0, true);
            }
            else if (ch == ')' || ch == ']')
            {
                parenDepth--;
                if (parenDepth == 0)
                {
                    isInStatement = parenStatementStack->back();
                    parenStatementStack->pop_back();
                    ch = ' ';

                    isInConditional = false;
                }

                if (!inStatementIndentStackSizeStack->IsEmpty())
                {
                    int previousIndentStackSize = inStatementIndentStackSizeStack->back();
                    inStatementIndentStackSizeStack->pop_back();
                    while (previousIndentStackSize < (int) inStatementIndentStack->GetCount())
                        inStatementIndentStack->pop_back();

                    if (!parenIndentStack->IsEmpty())
                    {
                        int poppedIndent = parenIndentStack->back();
                        parenIndentStack->pop_back();

                        if (i == 0)
                            spaceTabCount = poppedIndent;
                    }
                }
            }

            continue;
        }


        if (ch == '{')
        {
            bool isBlockOpener;
            // first, check if '{' is a block-opener or an static-array opener
            isBlockOpener = ((prevNonSpaceCh == '{' && bracketBlockStateStack->back())
                             || prevNonSpaceCh == '}'
                             || prevNonSpaceCh == ')'
                             || prevNonSpaceCh == ';'
                             || peekNextChar(line, i) == '{'
                             || isNonInStatementArray
                             || isInClassHeader
//                           || isBlockOpener
                             || isImmediatelyAfterConst
                             || (isInDefine &&
                                 (prevNonSpaceCh == '('
                                  || prevNonSpaceCh == '_'
                                  || isalnum(prevNonSpaceCh))));

            isInClassHeader = false;
            if (!isBlockOpener && currentHeader != NULL)
            {
                for (int n = 0; n < nonParenHeaders.GetCount(); n++)
                    if (currentHeader == nonParenHeaders[n])
                    {
                        isBlockOpener = true;
                        break;
                    }
            }
            bracketBlockStateStack->push_back(isBlockOpener);
            if (!isBlockOpener)
            {
                inStatementIndentStackSizeStack->push_back(inStatementIndentStack->GetCount());
                registerInStatementIndent(line, i, spaceTabCount, 0, true);
                parenDepth++;
                if (i == 0)
                    shouldIndentBrackettedLine = false;

                continue;
            }

            // this bracket is a block opener...

            ++lineOpeningBlocksNum;
//          if (isInClassHeader)
//          isInClassHeader = false;

            if (isInClassHeaderTab)
            {
                isInClassHeaderTab = false;
                // decrease tab count if bracket is broken
                int firstChar = ASString_Find_First_Not_Of(line, " \t");
                if(firstChar >= 0)
                    if (line[firstChar] == '{' && (int) firstChar == i)
                        tabCount -= 2;
            }

            // do not allow inStatementIndent - should occur for Java files only
            if (inStatementIndentStack->GetCount() > 0)
            {
                spaceTabCount = 0;
                inStatementIndentStack->back() = 0;
            }

            blockParenDepthStack->push_back(parenDepth);
            blockStatementStack->push_back(isInStatement);

            inStatementIndentStackSizeStack->push_back(inStatementIndentStack->GetCount());
            if (inStatementIndentStack->GetCount() > 0)
                inStatementIndentStack->back() = 0;

            blockTabCount += isInStatement ? 1 : 0;
            parenDepth = 0;
            isInStatement = false;

            tempStacks->push_back(new Vector<const WString*>);
            headerStack->push_back(&AS_OPEN_BRACKET);
            lastLineHeader = &AS_OPEN_BRACKET;

            continue;
        }

        //check if a header has been reached
        if (isWhiteSpace(prevCh))
        {
            bool isIndentableHeader = true;
            const WString *newHeader = findHeader(line, i, headers);
            if (newHeader != NULL)
            {
                // if we reached here, then this is a header...
                isInHeader = true;

                Vector<const WString*> *lastTempStack;
                if (tempStacks->IsEmpty())
                    lastTempStack = NULL;
                else
                    lastTempStack = tempStacks->back();

                // if a new block is opened, push a new stack into tempStacks to hold the
                // future list of headers in the new block.

                // take care of the special case: 'else if (...)'
                if (newHeader == &AS_IF && lastLineHeader == &AS_ELSE)
                {
                    headerStack->pop_back();
                }

                // take care of 'else'
                else if (newHeader == &AS_ELSE)
                {
                    if (lastTempStack != NULL)
                    {
                        int indexOfIf = indexOf(*lastTempStack, &AS_IF);
                        if (indexOfIf != -1)
                        {
                            // recreate the header list in headerStack up to the previous 'if'
                            // from the temporary snapshot stored in lastTempStack.
                            int restackSize = lastTempStack->GetCount() - indexOfIf - 1;
                            for (int r = 0; r < restackSize; r++)
                            {
                                headerStack->push_back(lastTempStack->back());
                                lastTempStack->pop_back();
                            }
                            if (!closingBracketReached)
                                tabCount += restackSize;
                        }
                        /*
                         * If the above if is not true, i.e. no 'if' before the 'else',
                         * then nothing beautiful will come out of this...
                         * I should think about inserting an Exception here to notify the caller of this...
                         */
                    }
                }

                // check if 'while' closes a previous 'do'
                else if (newHeader == &AS_WHILE)
                {
                    if (lastTempStack != NULL)
                    {
                        int indexOfDo = indexOf(*lastTempStack, &AS_DO);
                        if (indexOfDo != -1)
                        {
                            // recreate the header list in headerStack up to the previous 'do'
                            // from the temporary snapshot stored in lastTempStack.
                            int restackSize = lastTempStack->GetCount() - indexOfDo - 1;
                            for (int r = 0; r < restackSize; r++)
                            {
                                headerStack->push_back(lastTempStack->back());
                                lastTempStack->pop_back();
                            }
                            if (!closingBracketReached)
                                tabCount += restackSize;
                        }
                    }
                }
                // check if 'catch' closes a previous 'try' or 'catch'
                else if (newHeader == &AS_CATCH || newHeader == &AS_FINALLY)
                {
                    if (lastTempStack != NULL)
                    {
                        int indexOfTry = indexOf(*lastTempStack, &AS_TRY);
                        if (indexOfTry == -1)
                            indexOfTry = indexOf(*lastTempStack, &AS_CATCH);
                        if (indexOfTry != -1)
                        {
                            // recreate the header list in headerStack up to the previous 'try'
                            // from the temporary snapshot stored in lastTempStack.
                            int restackSize = lastTempStack->GetCount() - indexOfTry - 1;
                            for (int r = 0; r < restackSize; r++)
                            {
                                headerStack->push_back(lastTempStack->back());
                                lastTempStack->pop_back();
                            }

                            if (!closingBracketReached)
                                tabCount += restackSize;
                        }
                    }
                }
                else if (newHeader == &AS_CASE)
                {
                    isInCase = true;
                    --tabCount;
                }
                else if (newHeader == &AS_DEFAULT)
                {
                    isInCase = true;
                    --tabCount;
                }
                else if (newHeader == &AS_STATIC
                         || newHeader == &AS_SYNCHRONIZED
                         || (newHeader == &AS_CONST && isCStyle))
                {
                    if (!headerStack->IsEmpty() &&
                            (headerStack->back() == &AS_STATIC
                             || headerStack->back() == &AS_SYNCHRONIZED
                             || headerStack->back() == &AS_CONST))
                    {
                        isIndentableHeader = false;
                    }
                    else
                    {
                        isIndentableHeader = false;
                        probationHeader = newHeader;
                    }
                }
                else if (newHeader == &AS_CONST)
                {
                    isIndentableHeader = false;
                }
                else if (newHeader == &AS_TEMPLATE)
                {
                    if (isCStyle)
                        isInTemplate = true;
                    isIndentableHeader = false;
                }


                if (isIndentableHeader)
                {
                    headerStack->push_back(newHeader);
                    isInStatement = false;
                    if (indexOf(nonParenHeaders, newHeader) == -1)
                    {
                        isInConditional = true;
                    }
                    lastLineHeader = newHeader;
                }
                else
                    isInHeader = false;

                outBuffer.Cat(newHeader->Mid(1));
                i += newHeader->GetCount() - 1;

                continue;
            }
        }

        if (isCStyle && !isalpha(prevCh)
                && line.Mid(i, 8) == WString("operator") && !isalnum(line[i+8]))
        {
            isInOperator = true;
            outBuffer.Cat(AS_OPERATOR.Mid(1));
            i += 7;
            continue;
        }

        // "new" operator is a pointer, not a calculation
        if (!isalpha(prevCh)
                && line.Mid(i, 3) == WString("new") && !isalnum(line[i+3]))
        {
            if (prevNonSpaceCh == '=' && isInStatement && !inStatementIndentStack->IsEmpty())
                inStatementIndentStack->back() = 0;
        }

        if (ch == '?')
            isInQuestion = true;


        // special handling of 'case' statements
        if (ch == ':')
        {
            if ((int) line.GetCount() > i + 1 && line[i+1] == ':') // look for ::
            {
                ++i;
                outBuffer.Cat(':');
                ch = ' ';
                continue;
            }

            else if (isInQuestion)
            {
                isInQuestion = false;
            }

            else if (isCStyle && isInClass && prevNonSpaceCh != ')')
            {
                --tabCount;
                // found a 'private:' or 'public:' inside a class definition
                // so do nothing special
            }

            else if (!isJavaStyle && isInClassHeader)
            {
                // found a 'class A : public B' definition
                // so do nothing special
            }

            else if (isJavaStyle && lastLineHeader == &AS_FOR)
            {
                // found a java for-each statement
                // so do nothing special
            }

            else if (isCStyle && prevNonSpaceCh == ')')
            {
                isInClassHeader = true;
                if (i == 0)
                    tabCount += 2;
            }
            else
            {
                currentNonSpaceCh = ';'; // so that brackets after the ':' will appear as block-openers
                if (isInCase)
                {
                    isInCase = false;
                    ch = ';'; // from here on, treat char as ';'
                }


                else // is in a label (e.g. 'label1:')
                {
                    if (labelIndent)
                        --tabCount; // unindent label by one indent
                    else
                        tabCount = 0; // completely flush indent to left
                }



            }
        }

        if ((ch == ';'  || (parenDepth > 0 && ch == ','))  && !inStatementIndentStackSizeStack->IsEmpty())
            while ((int) inStatementIndentStackSizeStack->back() + (parenDepth > 0 ? 1 : 0)
                    < (int) inStatementIndentStack->GetCount())
                inStatementIndentStack->pop_back();


        // handle ends of statements
        if ((ch == ';' && parenDepth == 0) || ch == '}'/* || (ch == ',' && parenDepth == 0)*/)
        {
            if (ch == '}')
            {
                // first check if this '}' closes a previous block, or a static array...
                if (!bracketBlockStateStack->IsEmpty())
                {
                    bool bracketBlockState = bracketBlockStateStack->back();
                    bracketBlockStateStack->pop_back();
                    if (!bracketBlockState)
                    {
                        if (!inStatementIndentStackSizeStack->IsEmpty())
                        {
                            // this bracket is a static array

                            int previousIndentStackSize = inStatementIndentStackSizeStack->back();
                            inStatementIndentStackSizeStack->pop_back();
                            while (previousIndentStackSize < (int) inStatementIndentStack->GetCount())
                                inStatementIndentStack->pop_back();
                            parenDepth--;
                            if (i == 0)
                                shouldIndentBrackettedLine = false;

                            if (!parenIndentStack->IsEmpty())
                            {
                                int poppedIndent = parenIndentStack->back();
                                parenIndentStack->pop_back();
                                if (i == 0)
                                    spaceTabCount = poppedIndent;
                            }
                        }
                        continue;
                    }
                }

                // this bracket is block closer...

                ++lineClosingBlocksNum;

                if (!inStatementIndentStackSizeStack->IsEmpty())
                    inStatementIndentStackSizeStack->pop_back();

                if (!blockParenDepthStack->IsEmpty())
                {
                    parenDepth = blockParenDepthStack->back();
                    blockParenDepthStack->pop_back();
                    isInStatement = blockStatementStack->back();
                    blockStatementStack->pop_back();

                    if (isInStatement)
                        blockTabCount--;
                }

                closingBracketReached = true;
                int headerPlace = indexOf(*headerStack, &AS_OPEN_BRACKET);
                if (headerPlace != -1)
                {
                    const WString *popped = headerStack->back();
                    while (popped != &AS_OPEN_BRACKET)
                    {
                        headerStack->pop_back();
                        popped = headerStack->back();
                    }
                    headerStack->pop_back();

                    if (!tempStacks->IsEmpty())
                    {
                        Vector<const WString*> *temp =  tempStacks->back();
                        tempStacks->pop_back();
                        delete temp;
                    }
                }


                ch = ' '; // needed due to cases such as '}else{', so that headers ('else' tn tih case) will be identified...
            }

            /*
             * Create a temporary snapshot of the current block's header-list in the
             * uppermost inner stack in tempStacks, and clear the headerStack up to
             * the begining of the block.
             * Thus, the next future statement will think it comes one indent past
             * the block's '{' unless it specifically checks for a companion-header
             * (such as a previous 'if' for an 'else' header) within the tempStacks,
             * and recreates the temporary snapshot by manipulating the tempStacks.
             */
            if (!tempStacks->back()->IsEmpty())
                while (!tempStacks->back()->IsEmpty())
                    tempStacks->back()->pop_back();
            while (!headerStack->IsEmpty() && headerStack->back() != &AS_OPEN_BRACKET)
            {
                tempStacks->back()->push_back(headerStack->back());
                headerStack->pop_back();
            }

            if (parenDepth == 0 && ch == ';')
                isInStatement = false;

            previousLastLineHeader = NULL;
            isInClassHeader = false;
            isInQuestion = false;

            continue;
        }


        // check for preBlockStatements ONLY if not within parenthesies
        // (otherwise 'struct XXX' statements would be wrongly interpreted...)
        if (isWhiteSpace(prevCh) && !isInTemplate && parenDepth == 0)
        {
            const WString *newHeader = findHeader(line, i, preBlockStatements);
            if (newHeader != NULL)
            {
                isInClassHeader = true;
                outBuffer.Cat(newHeader->Mid(1));
                i += newHeader->GetCount() - 1;
                headerStack->push_back(newHeader);
            }
        }

        // Handle operators

        immediatelyPreviousAssignmentOp = NULL;

        // Check if an operator has been reached.
        const WString *foundAssignmentOp = findHeader(line, i, assignmentOperators, false);
        if (foundAssignmentOp == &AS_RETURN)
            foundAssignmentOp = findHeader(line, i, assignmentOperators, true);
        const WString *foundNonAssignmentOp = findHeader(line, i, nonAssignmentOperators, false);

        // Since findHeader's boundry checking was not used above, it is possible
        // that both an assignment op and a non-assignment op where found,
        // e.g. '>>' and '>>='. If this is the case, treat the LONGER one as the
        // found operator.
        if (foundAssignmentOp != NULL && foundNonAssignmentOp != NULL) {
            if (foundAssignmentOp->GetCount() < foundNonAssignmentOp->GetCount())
                foundAssignmentOp = NULL;
            else
                foundNonAssignmentOp = NULL;
        }

        if (foundNonAssignmentOp != NULL)
        {
            if (foundNonAssignmentOp->GetCount() > 1)
            {
                outBuffer.Cat(foundNonAssignmentOp->Mid(1));
                i += foundNonAssignmentOp->GetCount() - 1;
            }
        }

        else if (foundAssignmentOp != NULL)
        {
            if (foundAssignmentOp->GetCount() > 1)
            {
                outBuffer.Cat(foundAssignmentOp->Mid(1));
                i += foundAssignmentOp->GetCount() - 1;
            }

            if (!isInOperator && !isInTemplate && !isNonInStatementArray)
            {
                registerInStatementIndent(line, i, spaceTabCount, 0, false);
                immediatelyPreviousAssignmentOp = foundAssignmentOp;
                isInStatement = true;
            }
        }

        if (isInOperator)
            isInOperator = false;
    }

    // handle special cases of unindentation:

    /*
     * if '{' doesn't follow an immediately previous '{' in the headerStack
     * (but rather another header such as "for" or "if", then unindent it
     * by one indentation relative to its block.
     */

    if (!lineStartsInComment
            && !blockIndent
            && outBuffer.GetCount() > 0
            && outBuffer[0] == '{'
            && !(lineOpeningBlocksNum > 0 && lineOpeningBlocksNum == lineClosingBlocksNum)
            && !(headerStack->GetCount() > 1 && (*headerStack)[headerStack->GetCount()-2] == &AS_OPEN_BRACKET)
            && shouldIndentBrackettedLine)
        --tabCount;

    else if (!lineStartsInComment
             && outBuffer.GetCount() > 0
             && outBuffer[0] == '}'
             && shouldIndentBrackettedLine)
        --tabCount;

    // correctly indent one-line-blocks...
    else if (!lineStartsInComment
             && outBuffer.GetCount() > 0
             && lineOpeningBlocksNum > 0
             && lineOpeningBlocksNum == lineClosingBlocksNum
             && previousLineProbationTab)
        --tabCount; //lineOpeningBlocksNum - (blockIndent ? 1 : 0);

    if (tabCount < 0)
        tabCount = 0;

    // take care of extra bracket indentatation option...
    if (bracketIndent && outBuffer.GetCount() > 0 && shouldIndentBrackettedLine)
        if (outBuffer[0] == '{' || outBuffer[0] == '}')
            tabCount++;


    if (isInDefine)
    {
        if (outBuffer[0] == '#')
        {
            WString preproc = trim(outBuffer.Mid(1));

            if (preproc.Mid(0, 6) == WString("define"))
            {
                if (!inStatementIndentStack->IsEmpty()
                        && inStatementIndentStack->back() > 0)
                {
                    defineTabCount = tabCount;
                }
                else
                {
                    defineTabCount = tabCount - 1;
                    tabCount--;
                }
            }
        }

        tabCount -= defineTabCount;
    }

    if (tabCount < 0)
        tabCount = 0;
    if (lineCommentNoBeautify || blockCommentNoBeautify)
        tabCount = spaceTabCount = 0;

    // finally, insert indentations into begining of line

    prevFinalLineSpaceTabCount = spaceTabCount;
    prevFinalLineTabCount = tabCount;

    if (shouldForceTabIndentation)
    {
        tabCount += spaceTabCount / indentLength;
        spaceTabCount = spaceTabCount % indentLength;
    }

    outBuffer = preLineWS(spaceTabCount, tabCount) + outBuffer;

    if (lastLineHeader != NULL)
        previousLastLineHeader = lastLineHeader;

    return outBuffer;
}


WString ASBeautifier::preLineWS(int spaceTabCount, int tabCount)
{
    WString ws;

    for (int i = 0; i < tabCount; i++)
        ws += indentString;

    while ((spaceTabCount--) > 0)
        ws += WString(" ");

    return ws;

}

/**
 * register an in-statement indent.
 */
void ASBeautifier::registerInStatementIndent(const WString &line, int i, int spaceTabCount,
        int minIndent, bool updateParenStack)
{
    int inStatementIndent;
    int remainingCharNum = line.GetCount() - i;
    int nextNonWSChar = getNextProgramCharDistance(line, i);

    // if indent is around the last char in the line, indent instead 2 spaces from the previous indent
    if (nextNonWSChar == remainingCharNum)
    {
        int previousIndent = spaceTabCount;
        if (!inStatementIndentStack->IsEmpty())
            previousIndent = inStatementIndentStack->back();

        inStatementIndentStack->push_back(/*2*/ indentLength + previousIndent);
        if (updateParenStack)
            parenIndentStack->push_back(previousIndent);
        return;
    }

    if (updateParenStack)
        parenIndentStack->push_back(i + spaceTabCount);

    inStatementIndent = i + nextNonWSChar + spaceTabCount;

    if (i + nextNonWSChar < minIndent)
        inStatementIndent = minIndent + spaceTabCount;

    if (i + nextNonWSChar > maxInStatementIndent)
        inStatementIndent =  indentLength * 2 + spaceTabCount;

    if (!inStatementIndentStack->IsEmpty() &&
            inStatementIndent < inStatementIndentStack->back())
        inStatementIndent = inStatementIndentStack->back();

    if (isNonInStatementArray)
        inStatementIndent = 0;

    inStatementIndentStack->push_back(inStatementIndent);
}

/**
 * get distance to the next non-white sspace, non-comment character in the line.
 * if no such character exists, return the length remaining to the end of the line.
 */
int ASBeautifier::getNextProgramCharDistance(const WString &line, int i)
{
    bool inComment = false;
    int  remainingCharNum = line.GetCount() - i;
    int  charDistance;
    wchar ch;

    for (charDistance = 1; charDistance < remainingCharNum; charDistance++)
    {
        ch = line[i + charDistance];
        if (inComment)
        {
            if (line.Mid(i + charDistance, 2) == WString("*/"))
            {
                charDistance++;
                inComment = false;
            }
            continue;
        }
        else if (isWhiteSpace(ch))
            continue;
        else if (ch == '/')
        {
            if (line.Mid(i + charDistance, 2) == WString("//"))
                return remainingCharNum;
            else if (line.Mid(i + charDistance, 2) == WString("/*"))
            {
                charDistance++;
                inComment = true;
            }
        }
        else
            return charDistance;
    }

    return charDistance;
}


/**
 * check if a specific line position contains a header, out of several possible headers.
 *
 * @return    a pointer to the found header. if no header was found then return NULL.
 */
const WString *ASBeautifier::findHeader(const WString &line, int i, const Vector<const WString*> &possibleHeaders, bool checkBoundry)
{
    int maxHeaders = possibleHeaders.GetCount();
    // const WString *header = NULL;
    int p;

    for (p = 0; p < maxHeaders; p++)
    {
        const WString *header = possibleHeaders[p];

        if (line.Mid(i, header->GetCount()) == *header)
        {
            // check that this is a header and not a part of a longer word
            // (e.g. not at its begining, not at its middle...)

            int lineLength = line.GetCount();
            int headerEnd = i + header->GetCount();
            wchar startCh = (*header)[0];   // first char of header
            wchar endCh = 0;                // char just after header
            wchar prevCh = 0;               // char just before header

            if (headerEnd < lineLength)
            {
                endCh = line[headerEnd];
            }
            if (i > 0)
            {
                prevCh = line[i-1];
            }

            if (!checkBoundry)
            {
                return header;
            }
            else if (prevCh != 0
                     && isLegalNameChar(startCh)
                     && isLegalNameChar(prevCh))
            {
                return NULL;
            }
            else if (headerEnd >= lineLength
                     || !isLegalNameChar(startCh)
                     || !isLegalNameChar(endCh))
            {
                return header;
            }
            else
            {
                return NULL;
            }
        }
    }

    return NULL;
}

/**
 * find the index number of a string element in a container of strings
 *
 * @return              the index number of element in the ocntainer. -1 if element not found.
 * @param container     a vector of strings.
 * @param element       the element to find .
 */
int ASBeautifier::indexOf(Vector<const WString*> &container, const WString *element)
{
/*  Vector<const WString*>::const_iterator where;
    where = find(container.begin(), container.end(), element);

    if (where == container.end())
        return -1;
    else
        return (int) (where - container.begin());
*/

    return FindIndex(container, element);
}

/**
 * trim removes the white space surrounding a line.
 *
 * @return          the trimmed line.
 * @param str       the line to trim.
 */
WString ASBeautifier::trim(const WString &str)
{

    int start = 0;
    int end = str.GetCount() - 1;

    while (start < end && isWhiteSpace(str[start]))
        start++;

    while (start <= end && isWhiteSpace(str[end]))
        end--;

//  WString returnStr(str, start, end + 1 - start);
//  return returnStr;
    return str.Mid(start, end+1-start);
}

/**
* peek at the next unread character.
*
* @return       the next unread character.
* @param line   the line to check.
* @param i      the current char position on the line.
*/
wchar ASBeautifier::peekNextChar(WString &line, int i)
{
    wchar ch = ' ';
    int peekNum = ASString_Find_First_Not_Of(line, " \t", i + 1);

    if (peekNum < 0)
        return ch;

    ch = line[peekNum];

    return ch;
}


}   // end namespace astyle
