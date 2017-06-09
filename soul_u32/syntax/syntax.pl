namespace soul.syntax
{
    grammar CompositeGrammar
    {
        Alternative(soul::parsing::Scope* enclosingScope): soul::parsing::Parser*;
        Sequence(soul::parsing::Scope* enclosingScope): soul::parsing::Parser*;
        Difference(soul::parsing::Scope* enclosingScope): soul::parsing::Parser*;
        ExclusiveOr(soul::parsing::Scope* enclosingScope): soul::parsing::Parser*;
        Intersection(soul::parsing::Scope* enclosingScope): soul::parsing::Parser*;
        List(soul::parsing::Scope* enclosingScope): soul::parsing::Parser*;
        Postfix(soul::parsing::Scope* enclosingScope): soul::parsing::Parser*;
    }
    grammar RuleGrammar
    {
        Rule(soul::parsing::Scope* enclosingScope): soul::parsing::Rule*;
        RuleHeader(soul::parsing::Scope* enclosingScope): soul::parsing::Rule*;
        RuleBody(soul::parsing::Rule* rule);
    }
    grammar ElementGrammar
    {
        RuleLink(soul::parsing::Grammar* grammar);
        Signature(soul::parsing::Rule* rule);
        ParameterList(soul::parsing::Rule* rule);
        Variable(soul::parsing::Rule* rule);
        Parameter(soul::parsing::Rule* rule);
        ReturnType(soul::parsing::Rule* rule);
        Keyword;
        Identifier: std::u32string;
        QualifiedId: std::u32string;
        StringArray(std::vector<std::u32string>* array);
    }
    grammar GrammarGrammar
    {
        Grammar(soul::parsing::Scope* enclosingScope): soul::parsing::Grammar*;
        GrammarContent(soul::parsing::Grammar* grammar);
        StartClause(soul::parsing::Grammar* grammar);
        SkipClause(soul::parsing::Grammar* grammar);
    }
    grammar ParserFileGrammar
    {
        ParserFile(int id_, soul::parsing::ParsingDomain* parsingDomain_): ParserFileContent*;
        IncludeDirectives(ParserFileContent* parserFileContent);
        IncludeDirective(ParserFileContent* parserFileContent);
        FileAttribute: std::u32string;
        IncludeFileName: std::u32string;
        NamespaceContent(ParserFileContent* parserFileContent);
        Namespace(ParserFileContent* parserFileContent);
    }
    grammar PrimitiveGrammar
    {
        Primitive: soul::parsing::Parser*;
        Char: soul::parsing::Parser*;
        String: soul::parsing::Parser*;
        CharSet(var bool inverse, var std::u32string s): soul::parsing::Parser*;
        CharSetRange: std::u32string;
        CharSetChar: char32_t;
        Keyword: soul::parsing::Parser*;
        KeywordBody: soul::parsing::Parser*;
        KeywordList: soul::parsing::Parser*;
        KeywordListBody(var std::vector<std::u32string> keywords): soul::parsing::Parser*;
        Range: soul::parsing::Parser*;
        CodePoint: uint32_t;
        Empty: soul::parsing::Parser*;
        Space: soul::parsing::Parser*;
        AnyChar: soul::parsing::Parser*;
        Letter: soul::parsing::Parser*;
        Digit: soul::parsing::Parser*;
        HexDigit: soul::parsing::Parser*;
        UpperLetter: soul::parsing::Parser*;
        LowerLetter: soul::parsing::Parser*;
        TitleLetter: soul::parsing::Parser*;
        ModifierLetter: soul::parsing::Parser*;
        OtherLetter: soul::parsing::Parser*;
        CasedLetter: soul::parsing::Parser*;
        Mark: soul::parsing::Parser*;
        NonspacingMark: soul::parsing::Parser*;
        SpacingMark: soul::parsing::Parser*;
        EnclosingMark: soul::parsing::Parser*;
        Number: soul::parsing::Parser*;
        DecimalNumber: soul::parsing::Parser*;
        LetterNumber: soul::parsing::Parser*;
        OtherNumber: soul::parsing::Parser*;
        Punctuation: soul::parsing::Parser*;
        ConnectorPunctuation: soul::parsing::Parser*;
        DashPunctuation: soul::parsing::Parser*;
        OpenPunctuation: soul::parsing::Parser*;
        ClosePunctuation: soul::parsing::Parser*;
        InitialPunctuation: soul::parsing::Parser*;
        FinalPunctuation: soul::parsing::Parser*;
        OtherPunctuation: soul::parsing::Parser*;
        Symbol: soul::parsing::Parser*;
        MathSymbol: soul::parsing::Parser*;
        CurrencySymbol: soul::parsing::Parser*;
        ModifierSymbol: soul::parsing::Parser*;
        OtherSymbol: soul::parsing::Parser*;
        Separator: soul::parsing::Parser*;
        SpaceSeparator: soul::parsing::Parser*;
        LineSeparator: soul::parsing::Parser*;
        ParagraphSeparator: soul::parsing::Parser*;
        Other: soul::parsing::Parser*;
        Control: soul::parsing::Parser*;
        Format: soul::parsing::Parser*;
        Surrogate: soul::parsing::Parser*;
        PrivateUse: soul::parsing::Parser*;
        Unassigned: soul::parsing::Parser*;
        Graphic: soul::parsing::Parser*;
        BaseChar: soul::parsing::Parser*;
        Alphabetic: soul::parsing::Parser*;
        IdStart: soul::parsing::Parser*;
        IdCont: soul::parsing::Parser*;
    }
    grammar LibraryFileGrammar
    {
        LibraryFile(soul::parsing::ParsingDomain* parsingDomain);
        NamespaceContent(soul::parsing::ParsingDomain* parsingDomain);
        Namespace(soul::parsing::ParsingDomain* parsingDomain);
        Grammar(soul::parsing::Scope* enclosingScope): soul::parsing::Grammar*;
        GrammarContent(soul::parsing::Grammar* grammar);
        Rule(soul::parsing::Scope* enclosingScope): soul::parsing::Rule*;
    }
    grammar PrimaryGrammar
    {
        Primary(soul::parsing::Scope* enclosingScope): soul::parsing::Parser*;
        RuleCall: soul::parsing::Parser*;
        Nonterminal: std::u32string;
        Alias: std::u32string;
        Grouping(soul::parsing::Scope* enclosingScope): soul::parsing::Parser*;
        Token(soul::parsing::Scope* enclosingScope): soul::parsing::Parser*;
        Expectation(soul::parsing::Parser* child): soul::parsing::Parser*;
        Action(soul::parsing::Parser* child): soul::parsing::Parser*;
    }
    grammar ProjectFileGrammar
    {
        ProjectFile: Project*;
        ProjectFileContent(Project* project);
        Source: std::u32string;
        Reference: std::u32string;
        FilePath: std::u32string;
    }
}
