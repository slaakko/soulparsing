namespace soul.code
{
    grammar DeclarationGrammar
    {
        BlockDeclaration: soul::codedom::CppObject*;
        SimpleDeclaration(var std::unique_ptr<SimpleDeclaration> sd): soul::codedom::SimpleDeclaration*;
        DeclSpecifierSeq(soul::codedom::SimpleDeclaration* declaration);
        DeclSpecifier: soul::codedom::DeclSpecifier*;
        StorageClassSpecifier: soul::codedom::StorageClassSpecifier*;
        TypeSpecifier: soul::codedom::TypeSpecifier*;
        SimpleTypeSpecifier: soul::codedom::TypeSpecifier*;
        TypeName: soul::codedom::TypeName*;
        TemplateArgumentList(soul::codedom::TypeName* typeName);
        TemplateArgument: soul::codedom::CppObject*;
        Typedef: soul::codedom::DeclSpecifier*;
        CVQualifier: soul::codedom::TypeSpecifier*;
        NamespaceAliasDefinition: soul::codedom::UsingObject*;
        UsingDeclaration: soul::codedom::UsingObject*;
        UsingDirective: soul::codedom::UsingObject*;
    }
    grammar StatementGrammar
    {
        Statement: soul::codedom::Statement*;
        LabeledStatement: soul::codedom::Statement*;
        Label: std::u32string;
        EmptyStatement: soul::codedom::Statement*;
        ExpressionStatement: soul::codedom::Statement*;
        CompoundStatement(var std::unique_ptr<CompoundStatement> cs): soul::codedom::CompoundStatement*;
        SelectionStatement: soul::codedom::Statement*;
        IfStatement: soul::codedom::Statement*;
        SwitchStatement: soul::codedom::Statement*;
        IterationStatement: soul::codedom::Statement*;
        WhileStatement: soul::codedom::Statement*;
        DoStatement: soul::codedom::Statement*;
        ForStatement: soul::codedom::Statement*;
        ForInitStatement: soul::codedom::CppObject*;
        JumpStatement: soul::codedom::Statement*;
        BreakStatement: soul::codedom::Statement*;
        ContinueStatement: soul::codedom::Statement*;
        ReturnStatement: soul::codedom::Statement*;
        GotoStatement: soul::codedom::Statement*;
        GotoTarget: std::u32string;
        DeclarationStatement: soul::codedom::Statement*;
        Condition(var std::unique_ptr<TypeId> ti): soul::codedom::CppObject*;
        TryStatement: soul::codedom::TryStatement*;
        HandlerSeq(TryStatement* st);
        Handler: soul::codedom::Handler*;
        ExceptionDeclaration(var std::unique_ptr<ExceptionDeclaration> ed): soul::codedom::ExceptionDeclaration*;
    }
    grammar DeclaratorGrammar
    {
        InitDeclaratorList(var std::unique_ptr<InitDeclaratorList> idl): soul::codedom::InitDeclaratorList*;
        InitDeclarator: soul::codedom::InitDeclarator*;
        Declarator: std::u32string;
        DirectDeclarator(var std::unique_ptr<CppObject> o);
        DeclaratorId(var std::unique_ptr<CppObject> o);
        TypeId(var std::unique_ptr<TypeId> ti): soul::codedom::TypeId*;
        Type(var std::unique_ptr<Type> t): soul::codedom::Type*;
        TypeSpecifierSeq(soul::codedom::TypeId* typeId);
        AbstractDeclarator: std::u32string;
        DirectAbstractDeclarator(var std::unique_ptr<CppObject> o);
        PtrOperator;
        CVQualifierSeq(var std::unique_ptr<CppObject> o);
        Initializer: soul::codedom::Initializer*;
        InitializerClause: soul::codedom::AssignInit*;
        InitializerList(soul::codedom::AssignInit* init);
    }
    grammar ExpressionGrammar
    {
        Expression: soul::codedom::CppObject*;
        ConstantExpression: soul::codedom::CppObject*;
        AssignmentExpression(var std::unique_ptr<CppObject> lor): soul::codedom::CppObject*;
        AssingmentOp: Operator;
        ThrowExpression: soul::codedom::CppObject*;
        ConditionalExpression: soul::codedom::CppObject*;
        LogicalOrExpression: soul::codedom::CppObject*;
        LogicalAndExpression: soul::codedom::CppObject*;
        InclusiveOrExpression: soul::codedom::CppObject*;
        ExclusiveOrExpression: soul::codedom::CppObject*;
        AndExpression: soul::codedom::CppObject*;
        EqualityExpression: soul::codedom::CppObject*;
        EqOp: Operator;
        RelationalExpression: soul::codedom::CppObject*;
        RelOp: Operator;
        ShiftExpression: soul::codedom::CppObject*;
        ShiftOp: Operator;
        AdditiveExpression: soul::codedom::CppObject*;
        AddOp: Operator;
        MultiplicativeExpression: soul::codedom::CppObject*;
        MulOp: Operator;
        PmExpression: soul::codedom::CppObject*;
        PmOp: Operator;
        CastExpression(var std::unique_ptr<CppObject> ce, var std::unique_ptr<CppObject> ti): soul::codedom::CppObject*;
        UnaryExpression(var std::unique_ptr<CppObject> ue): soul::codedom::CppObject*;
        UnaryOperator: Operator;
        NewExpression(var bool global, var TypeId* typeId, var bool parens): soul::codedom::CppObject*;
        NewTypeId(var std::unique_ptr<TypeId> ti): soul::codedom::TypeId*;
        NewDeclarator: std::u32string;
        DirectNewDeclarator(var std::unique_ptr<CppObject> e);
        NewPlacement: std::vector<soul::codedom::CppObject*>;
        NewInitializer: std::vector<soul::codedom::CppObject*>;
        DeleteExpression(var bool global, var bool array): soul::codedom::CppObject*;
        PostfixExpression(var std::unique_ptr<CppObject> pe): soul::codedom::CppObject*;
        TypeSpecifierOrTypeName: soul::codedom::CppObject*;
        PostCastExpression: soul::codedom::CppObject*;
        ExpressionList: std::vector<soul::codedom::CppObject*>;
        PrimaryExpression(var std::unique_ptr<CppObject> pe): soul::codedom::CppObject*;
        IdExpression: soul::codedom::IdExpr*;
        OperatorFunctionId;
        Operator;
    }
    grammar LiteralGrammar
    {
        Literal: soul::codedom::Literal*;
        IntegerLiteral: soul::codedom::Literal*;
        DecimalLiteral;
        OctalLiteral;
        OctalDigit;
        HexadecimalLiteral;
        IntegerSuffix;
        UnsignedSuffix;
        LongLongSuffix;
        LongSuffix;
        CharacterLiteral: soul::codedom::Literal*;
        NarrowCharacterLiteral;
        UniversalCharacterLiteral;
        WideCharacterLiteral;
        CCharSequence: std::u32string;
        CChar;
        EscapeSequence;
        SimpleEscapeSequence;
        OctalEscapeSequence;
        HexadecimalEscapeSequence;
        FloatingLiteral: soul::codedom::Literal*;
        FractionalConstant;
        DigitSequence;
        ExponentPart;
        Sign;
        FloatingSuffix;
        StringLiteral: soul::codedom::Literal*;
        EncodingPrefix;
        SCharSequence;
        SChar;
        BooleanLiteral: soul::codedom::Literal*;
        PointerLiteral: soul::codedom::Literal*;
    }
    grammar IdentifierGrammar
    {
        Identifier: std::u32string;
        QualifiedId: std::u32string;
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
}
