#pragma once
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector>


namespace MyApp {

    void RenderUI();

    void Test();

    static char buffer[256] = "";
}


namespace PL0 {

    /*
    pascal语言是上下文无关文法
    语法分析采用自顶向下的递归子程序法
    当语法分析需要新单词时，调用词法分析，每次调用仅分析一个单词
    语义分析包含语法分析之中
    */

    const static int maxIdLength = 10;
    const static int numLinkData = 3;
    static int symTableIndex = 1;
    static char InputBuffer[100];
    static char OutputBuffer[100];
    static int codeTableIdx = 1;
    enum Token
    {
        TMP, IDENT, NUM, PERIOD, CONSTSYM, COMMA, LPAREN, RPAREN, EQ, SEMICOLON, COLON,
        ASSIGN, VARSYM, PROCSYM, BEGINSYM, ENDSYM, ODDSYM, IFSYM, THENSYM,
        CALLSYM, WHILESYM, DOSYM, WRITESYM, READSYM, PLUS, MINUS, TIMES, SPLASH,
        NEQ, LSS, LE, GT, GE, SPACE, ENTER, TAB
    };
    enum SymbolType
    {
        CONST, VARIABLE, PROCEDURE
    };
    struct Symbol
    {
        int type;
        char name[maxIdLength + 1];
        int value;
        int level;
        int address;
    };
    enum
    {
        LIT, LOD, STO, CAL, INT, JMP, JPC, OPR
    };
    //LIT 0 ，a 取常量a放入数据栈栈顶
    //LOD L ，a 取变量（相对地址为a，层差为L）放到数据栈的栈顶
    //STO L ，a 将数据栈栈顶的内容存入变量（相对地址为a，层次差为L）
    //CAL L ，a 调用过程（转子指令）（入口地址为a，层次差为L）
    //INT 0 ，a 数据栈栈顶指针增加a
    //JMP 0 ，a无条件转移到地址为a的指令
    //JPC 0 ，a 条件转移指令，转移到地址为a的指令
    //OPR 0 ，a 执行运算，a表示执行某种运算

    enum OPS
    {
        OP_RET = 0, OP_ADD = 2, OP_MINUS = 3, OP_TIMES = 4, OP_DIV = 5,
        OP_NEQ = 9, OP_EQ = 8, OP_LSS = 7, OP_LE = 6, OP_GT = 10, OP_GE = 11,
        OP_READ = 16, OP_WRITE = 15, OP_MINUS_HEAD = 17
    };
    struct PCode
    {
        int op;//操作码 
        int l;//调用层和说明层的层差值 
        int a;//相对地址，若指令为OPS，则相关操作的相对地址定义如上 
        PCode(int op = -1, int l = 0, int a = 0)
        {
            this->op = op;
            this->l = l;
            this->a = a;
        }
    };




    int findKeyword(const char* str);

    int getSym(FILE* in);								//词法分析，每次调用只分析一个单词

    extern FILE* f;
    inline int getSym()
    {
        return getSym(f);
    }
    int block(int level, int index);					//分程序分析过程
    int constDeclaration(int index);					//常量定义分析过程
    int varDeclaration(int level, int index);			//变量定义分析过程
    int procDeclaration(int level, int index);			//过程定义分析过程
    int statement(int level, int index);				//对语句的类型进行判断，对不同的语句类型进行分析和校验，生成类pcode代码
    int assignStatement(int level, int index);			//:=语句的分析和校验，生成类pcode代码
    int ifStatement(int level, int index);				//if语句的分析和校验，生成类pcode代码
    int whileStatement(int level, int index);			//while语句的分析和校验，生成类pcode代码
    int callStatement(int level, int index);			//call语句的分析和校验，生成类pcode代码
    int readStatement(int level, int index);			//read语句的分析和校验，生成类pcode代码
    int writeStatement(int level, int index);			//write语句的分析和校验，生成类pcode代码
    int compositeStatement(int level, int index);		//begin-end语句的分析和校验，生成类pcode代码
    int condition(int level, int index);				//条件语句的分析和校验，生成类pcode代码
    int expression(int level, int index);				//表达式语句的分析和校验，生成类pcode代码
    int term(int level, int index);						//因子的判断，生成类pcode代码
    int factor(int level, int index);					//项的判断，生成类pcode代码

    int _find(int from, int to, const char* name);      //查找匹配关键字
    void printErr(const char* err);                     //出错处理机制

    inline void genCode(int op, int l, int a);			//代码生成过程

    void interprete();									//解释运行生成的类pcode代码

    //GUI设计
    void showPL0();
    void RenderSymTable();
    void showWordAnalysis();
    void wordAnalysis();
    void dockSpace();
    void showPcode();
    void showSourceCode();
    void showSymTable();
}
