#include  "MyApplication.h"
#include "imgui.h"

namespace MyApp {

    void RenderUI() {

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
                if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
                if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::Begin("code");
        ImGui::Button("Hello");
        static float value = 0.0f;
        ImGui::DragFloat("Value", &value);
        ImGui::End();


        ImGui::Begin("测试");
        ImGui::End();

        ImGui::End();
    }


    void Test() {
        ImGui::Begin("文本输入框");

        ImGui::InputText("Enter Text", buffer, sizeof(buffer));

        if (ImGui::Button("Print Text"))
        {
            printf("Entered Text: %s\n", buffer);
        }
        ImGui::Text(buffer);
        float f = 100;
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::End();
    }

}



namespace PL0 {

    //std::vector<Symbol> symTable(1000);
    //std::vector<PCode> code(1000);
    Symbol symTable[1000];
    PCode code[1000];
    FILE* f;
    FILE* fword;
    FILE* fcode;
    FILE* fsymTable;
    FILE* fret;
    FILE* fwordOut;
    FILE* fretOut;
    const char CodeTable[8][4] = { "LIT","LOD","STO","CAL","INT","JMP","JPC","OPR" };


    int sym, num;
    char id[maxIdLength + 1];
    int pc = 1;
    int line = 1;

    void printErr(const char* err)
    {
        printf("Line %d:%s\n", line, err);
        fprintf(fret, " Line %d:%s\n", line, err);
        //exit(0);
    }

    int findKeyword(const char* str)
    {
        if (!strcmp(str, "const"))
            return CONSTSYM;
        if (!strcmp(str, "var"))
            return VARSYM;
        if (!strcmp(str, "procedure"))
            return PROCSYM;
        if (!strcmp(str, "begin"))
            return BEGINSYM;
        if (!strcmp(str, "end"))
            return ENDSYM;
        if (!strcmp(str, "odd"))
            return ODDSYM;
        if (!strcmp(str, "if"))
            return IFSYM;
        if (!strcmp(str, "then"))
            return THENSYM;
        if (!strcmp(str, "call"))
            return CALLSYM;
        if (!strcmp(str, "while"))
            return WHILESYM;
        if (!strcmp(str, "do"))
            return DOSYM;
        if (!strcmp(str, "write"))
            return WRITESYM;
        if (!strcmp(str, "read"))
            return READSYM;
        return -1;
    }

    int getSym(FILE* in)//词法分析 
    {
        extern int sym, num;
        extern char id[maxIdLength + 1];
        char buf[11];
        int pos = 0;
        char ch = ' ';
        while (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')//消除词法分析开始前的所有无意义符号 
        {
            if ((ch = fgetc(in)) == EOF)
            {
                return -1;
            }
            if (ch == '\n')
                line++;
        }
        if (ch == '{') {//消除注释
            while ((ch = fgetc(in)) != EOF && ch != '\n') {
                if (ch == '}') break;
            }
            if (ch == '\n') {
                printf("注释语句没有右括号\n");
                fputs("词法错误!注释语句没有右括号", fword);
                fprintf(fret, "词法错误!注释语句没有右括号");
                return -1;
            }
            ch = fgetc(in);
            ch = fgetc(in);
            line++;
        }
        if (isalpha(ch))//检测到以字母为起始的单词 
        {
            while (isalpha(ch) || isdigit(ch))//以字母为起始的词后续可以是字母或数字
            {
                if (pos >= maxIdLength)
                    return -1;
                buf[pos++] = ch;
                ch = fgetc(in);
            }
            ungetc(ch, in);//退出while循环，但是fgetc向前读入了一位，所以要把他退回输入流
            buf[pos++] = '\0';//给单词最后加上休止标志
            sym = findKeyword(buf);//查看该单词是否是关键词 
            printf("Line %d: (%d, %s)\n", line, sym, buf);
            fprintf(fword, "Line %d: (%d, %s)\n", line, sym, buf);
            if (sym < 0)//sym小于0则不是关键词，是变量名 
            {
                sym = IDENT;
                strcpy(id, buf);//将该单词拷贝到id数组里
                return 0;
            }
        }
        else if (isdigit(ch))//检测到以数字为起始的单词 
        {
            while (isdigit(ch))//以数字为起始的单词后续只能是数字 
            {
                if (pos >= maxIdLength)
                    return -1;
                buf[pos++] = ch;
                ch = fgetc(in);
            }
            ungetc(ch, in);//退出while循环，但是fgetc向前读入了一位，所以要把他退回输入流 
            buf[pos++] = '\0';//给关键词最后加上休止标志 
            sym = NUM;//该单词只能是数字 
            num = atoi(buf);//把该数字字符串转换为int型输出 
            printf("Line %d: (%d, %d)\n", line, sym, num);
            fprintf(fword, "Line %d: (%d, %d)\n", line, sym, num);
            return 0;
        }
        else if (ch == '(')//匹配特殊符号 
        {
            sym = LPAREN;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }

        else if (ch == ')')
        {
            sym = RPAREN;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }

        else if (ch == '=')
        {
            sym = EQ;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }

        else if (ch == '#')
        {
            sym = NEQ;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }
        else if (ch == '+')
        {
            sym = PLUS;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }
        else if (ch == '-')
        {
            sym = MINUS;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }
        else if (ch == '*')
        {
            sym = TIMES;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }
        else if (ch == '/')
        {
            sym = SPLASH;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }
        else if (ch == ',')
        {
            sym = COMMA;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }
        else if (ch == ';')
        {
            sym = SEMICOLON;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }
        else if (ch == '.')
        {
            sym = PERIOD;
            printf("Line %d: (%d, %c)\n", line, sym, ch);
            fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
        }

        else if (ch == ':')//检测到冒号时要判断是否是:=，还是只是单纯的冒号 
        {
            ch = fgetc(in);
            if (ch == '=')
            {
                sym = ASSIGN;
                printf("Line %d: (%d, %c)\n", line, sym, ch);
                fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
            }

            else
            {
                ungetc(ch, in);
                sym = COLON;
                printf("Line %d: (%d, %c)\n", line, sym, ch);
                fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
            }
        }
        else if (ch == '>')//检测到大于号时要判断是否是大于等于
        {
            ch = fgetc(in);
            if (ch == '=')
            {
                sym = GE;
                printf("Line %d: (%d, %c)\n", line, sym, ch);
                fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
            }

            else
            {
                ungetc(ch, in);
                sym = GT;
                printf("Line %d: (%d, %c)\n", line, sym, ch);
                fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
            }
        }
        else if (ch == '<')//检测到小于号时要判断是否是小于等于 
        {
            ch = fgetc(in);
            if (ch == '=')
            {
                sym = LE;
                printf("Line %d: (%d, %c)\n", line, sym, ch);
                fprintf(fword, "Line %d: (%d, %c)\n", line, sym, ch);
            }

            else
            {
                ungetc(ch, in);
                sym = LSS;
            }
        }
        else {
            printf("词法错误! 错误符号:%c\n", ch);
            fprintf(fword, "词法错误! 错误符号:%c\n", ch);
            fprintf(fret, "词法错误! 错误符号:%c\n", ch);
            return -1;
        }
        return 0;
        /*else return -1;
        if (ch == ' ')
            printf("Line %d: (%d,  )\n", line, SPACE);
        else if (ch == '\n')
            printf("Line %d: (%d, \\n)\n", line, ENTER);
        else if (ch == '\t')
            printf("Line %d: (%d, \\t)\n", line, TAB);
        return 0;*/
    }

    int block(int level, int index)//分程序的处理过程，level表示分程序层级，index是符号表索引 
    {
        int count = 0, dx = 0;
        int tpc = pc;
        genCode(JMP, 0, 0);
        bool flag = false;
        while (sym == CONSTSYM)
        {
            getSym();
            count += constDeclaration(index + count);
        }
        while (sym == VARSYM)
        {
            getSym();
            count += (dx = varDeclaration(level, index + count));
        }
        while (sym == PROCSYM)
        {
            flag = true;
            getSym();
            int px = count + index;
            count += procDeclaration(level + 1, px);
        }
        if (!flag)//如果是定义变量的话，那PC值就要回退，从定义变量那行开始执行 
            pc--;
        else//如果定义了过程的话，那么前面设定的JMP的偏移值就要等于定义完过程后的PC值，这样子才不会直接执行刚刚定义好的过程 
            code[tpc].a = pc;
        genCode(INT, 0, numLinkData + dx);
        statement(level, index + count);
        genCode(OPR, 0, OP_RET);
        return count;
    }

    int constDeclaration(int index)//CONST类型标识符常量定义语句处理  
    {
        Symbol sb;
        sb.type = CONST;
        int count = 0;
        do
        {
            if (sym != IDENT)
                printErr("CONST定义错误！声明CONST之后需要紧跟标识符名！");
            if (_find(0, index + count, id) >= 0)
                printErr("CONST定义错误！CONST类型标识符重复定义！");
            strcpy(sb.name, id);
            getSym();
            if (sym != EQ)
                printErr("CONST定义错误！赋值前缺少等号！");
            getSym();
            if (sym != NUM)
                printErr("CONST定义错误！未给CONST类型标识符赋初值！");
            sb.value = num;
            symTable[index + count++] = sb;
            getSym();
            if (sym != SEMICOLON)//PL0语法决定了CONST语句一行只能定义一个标识符常量 
                printErr("CONST定义错误！语句末缺少分号！");
        } while (sym != SEMICOLON);
        getSym();
        return count;
    }

    int varDeclaration(int level, int index)//VAR类型标识符变量定义语句处理 
    {
        Symbol sb;
        sb.type = VARIABLE;
        sb.level = level;
        sb.address = 0;
        int count = 0;
        int tsym = sym;
        do
        {
            if (sym != IDENT)
                printErr("VAR定义错误！声明VAR之后需要紧跟标识符名！");
            if (_find(0, index + count, id) >= 0)
                printErr("VAR定义错误！VAR类型标识符重复定义！");
            strcpy(sb.name, id);
            symTable[index + count++] = sb;
            sb.address++;
            getSym();
            if (!(sym == COMMA || sym == SEMICOLON))//PL0语法决定了VAR语句一行可以定义多个变量 
                printErr("VAR定义错误！语句末缺少逗号或分号！");
            tsym = sym;
            getSym();
        } while (tsym != SEMICOLON);//若出现分号，则定义语句结束，若为逗号，则定义语句继续，返回循环头部继续判断定义语句是否合法 
        return count;
    }

    int procDeclaration(int level, int index)//过程定义语句处理  
    {
        int count = 0;
        if (sym != IDENT)
            printErr("过程定义错误！声明procedure后必须紧跟过程名！");
        Symbol sb;
        strcpy(sb.name, id);
        sb.type = PROCEDURE;
        sb.level = level - 1;
        sb.address = pc;
        symTable[index + count++] = sb;
        getSym();
        if (sym != SEMICOLON)
            printErr("过程定义错误！语句末缺少分号！");
        getSym();
        block(level, index + count);
        if (sym != SEMICOLON)
            printErr("过程定义错误！语句末缺少分号！");
        getSym();
        if (sym == PROCSYM)//若过程中嵌套有过程，则递归调用过程定义语句 
        {
            getSym();
            count += procDeclaration(level, index + count);
        }
        return count + 1;
    }

    int _find(int from, int to, const char* name)
    {
        for (int i = to - 1; i >= from; i--) {
            if (!strcmp(name, symTable[i].name)) {
                return i;
            }
        }
            
        return -1;
    }

    void genCode(int op, int l, int a)
    {
        PCode pcode(op, l, a);
        code[pc++] = pcode;
    }

    int statement(int level, int index)//对语句的类型进行判断，对不同的语句类型进行分析和校验，生成中间代码  
    {
        if (sym == IFSYM)
        {
            getSym();
            ifStatement(level, index);
        }
        else if (sym == WHILESYM)
        {
            getSym();
            whileStatement(level, index);
        }
        else if (sym == CALLSYM)
        {
            getSym();
            callStatement(level, index);
        }
        else if (sym == WRITESYM)
        {
            getSym();
            writeStatement(level, index);
        }
        else if (sym == READSYM)
        {
            getSym();
            readStatement(level, index);
        }
        else if (sym == BEGINSYM)
        {
            getSym();
            compositeStatement(level, index);
        }
        else if (sym == IDENT)
        {
            assignStatement(level, index);
        }
        else
            return 0;
        return 0;
    }

    int ifStatement(int level, int index)//IF语句的分析和校验，生成中间代码  
    {
        condition(level, index);
        int cpc = pc;
        genCode(JPC, 0, 0);
        if (sym != THENSYM)
            printErr("IF句型定义出错！缺少THEN子句！");
        getSym();
        statement(level, index);
        code[cpc].a = pc;
        return 0;
    }

    int whileStatement(int level, int index)//while语句的分析和校验，生成中间代码   
    {
        int cpc = pc;
        condition(level, index);
        int jpc = pc;
        genCode(JPC, 0, 0);
        if (sym != DOSYM)
        {
            printErr("WHILE句型定义出错！缺少DO子句！");
        }
        getSym();
        statement(level, index);
        genCode(JMP, 0, cpc);
        code[jpc].a = pc;
        return 0;
    }

    int callStatement(int level, int index)//call语句的分析和校验，生成中间代码  
    {
        if (sym != IDENT)
            printErr("CALL语句语法错误！缺少调用的目标标识符！");
        int i = _find(0, index, id);
        if (i < 0)
            printErr("CALL语句语法错误！调用的标识符未定义！");
        if (symTable[i].type != PROCEDURE)
            printErr("CALL语句语法错误！所调用的标识符应当为procedure类型！");
        genCode(CAL, level - symTable[i].level, symTable[i].address);
        getSym();
        return 0;
    }

    int readStatement(int level, int index)//read语句的分析和校验，生成中间代码  
    {
        if (sym != LPAREN)
            printErr("READ语句语法错误！缺少左括号！");
        getSym();
        while (sym != RPAREN)
        {
            if (sym != IDENT)
                printErr("READ语句语法错误！缺少调用的标识符！");
            int i = _find(0, index, id);
            if (i < 0)
                printErr("READ语句语法错误！调用的标识符未定义！");
            if (symTable[i].type != VARIABLE)
                printErr("READ语句语法错误！调用的标识符应当为VAR类型！");
            genCode(OPR, 0, OP_READ);
            genCode(STO, level - symTable[i].level, symTable[i].address + numLinkData);
            getSym();
            if (sym != COMMA && sym != RPAREN)
                printErr("READ语句语法错误！缺少逗号或右括号！");
        }
        getSym();
        return 0;
    }

    int writeStatement(int level, int index)//write语句分析和校验，生成中间代码   
    {
        if (sym != LPAREN)
            printErr(" WRITE语句语法错误！缺少左括号！");
        getSym();
        while (sym != RPAREN)
        {
            expression(level, index);
            genCode(OPR, 0, OP_WRITE);
            if (sym != COMMA && sym != RPAREN)
                printErr("WRITE语句语法错误！缺少逗号或右括号！");
        }
        getSym();
        return 0;
    }

    int compositeStatement(int level, int index)//begin-end句型分析和校验，生成中间代码  
    {
        statement(level, index);
        while (sym == SEMICOLON)
        {
            getSym();
            statement(level, index);
        }
        if (sym != ENDSYM)
            printErr("BEGIN-END语句语法错误！缺少END!");
        getSym();
        return 0;
    }

    int assignStatement(int level, int index)
    {
        int i = _find(0, index, id);
        if (i < 0)
        {
            printErr("语法错误！VAR参数不存在！");
        }
        if (symTable[i].type == CONST)
            printErr("语法错误！参数类型不能为CONST!");
        getSym();
        if (sym != ASSIGN)
        {
            printErr("语法错误！缺少:=符号！");
        }
        getSym();
        expression(level, index);
        genCode(STO, level - symTable[i].level, numLinkData + symTable[i].address);
        return 0;
    }

    int condition(int level, int index)//条件语句分析和校验，生成中间代码 
    {
        if (sym == ODDSYM)
        {
            getSym();
            expression(level, index);
            genCode(LIT, 0, 0);
            genCode(OPR, 0, OP_NEQ);
        }
        else
        {
            expression(level, index);
            int op = sym;
            if (sym != NEQ && sym != EQ && sym != LSS && sym != LE && sym != GT && sym != GE)
                printErr("错误！");
            getSym();
            expression(level, index);
            switch (op)
            {
            case NEQ:
                genCode(OPR, 0, OP_NEQ); break;
            case EQ:
                genCode(OPR, 0, OP_EQ); break;
            case LSS:
                genCode(OPR, 0, OP_LSS); break;
            case LE:
                genCode(OPR, 0, OP_LE); break;
            case GT:
                genCode(OPR, 0, OP_GT); break;
            case GE:
                genCode(OPR, 0, OP_GE); break;
            }
        }
        return 0;
    }

    int expression(int level, int index)//表达式语句的分析和校验，生成中间代码 
    {
        int op = sym;
        if (sym == PLUS || sym == MINUS)
        {
            getSym();
        }
        factor(level, index);
        if (op == MINUS)
        {
            genCode(LIT, 0, 0);
            genCode(OPR, 0, OP_MINUS_HEAD);
        }
        while (sym == PLUS || sym == MINUS)
        {
            op = sym;
            getSym();
            factor(level, index);
            if (op == PLUS)
                genCode(OPR, 0, OP_ADD);
            else
                genCode(OPR, 0, OP_MINUS);
        }
        return 0;
    }

    int factor(int level, int index)//项的判断，生成中间代码 
    {
        term(level, index);
        int op = sym;
        while (sym == TIMES || sym == SPLASH)
        {
            op = sym;
            getSym();
            term(level, index);
            if (op == TIMES)
                genCode(OPR, 0, OP_TIMES);
            else
                genCode(OPR, 0, OP_DIV);
        }
        return 0;
    }

    int term(int level, int index)//因子的判断，生成中间代码 
    {
        if (sym == IDENT)
        {
            int i = _find(0, index, id);
            if (i < 0)
            {
                printErr("参数未定义！");
            }
            if (symTable[i].type == CONST)
                genCode(LIT, 0, symTable[i].value);
            else if (symTable[i].type == VARIABLE)
                genCode(LOD, level - symTable[i].level, numLinkData + symTable[i].address);
            else
            {
                printErr("错误！");
            }
            getSym();
        }
        else if (sym == NUM)
        {
            genCode(LIT, 0, num);
            getSym();
        }
        else
        {
            expression(level, index);
        }
        return 0;
    }

    void interprete()//编译出的中间代码的执行 
    {
        const static int ret_addr = 0, dynamic_link = 1, static_link = 2;//ret_addr是返回地址，dynamic_link是调用者的活动记录首地址，static_link是保存该过程直接外层的活动记录首地址 
        PCode ir;//当前指令 
        int ip = 1, sp = 0, bp = 0;//ip是指令指示器，sp是栈顶指示器，bp是栈底指示器 
        int stack[1000] = { 0 };//栈 
        int sp_stack[10];//用来保存原数据栈栈顶指针值 
        int sp_top = 0;//保存过的数据栈栈顶指针数量 
        while (ip < pc)
        {
            ir = code[ip++];
            switch (ir.op)
            {
            case LIT:
                stack[sp++] = ir.a; break;
            case LOD:
            {
                if (ir.l == 0)
                    stack[sp++] = stack[bp + ir.a];
                else
                {
                    int outer_bp = stack[bp + static_link];
                    while (--ir.l)
                        outer_bp = stack[outer_bp + static_link];
                    stack[sp++] = stack[outer_bp + ir.a];
                }
                break;
            }
            case STO:
            {
                if (ir.l == 0)
                    stack[bp + ir.a] = stack[sp - 1];
                else
                {
                    int outer_bp = stack[bp + static_link];
                    while (--ir.l)
                        outer_bp = stack[outer_bp + static_link];
                    stack[outer_bp + ir.a] = stack[sp - 1];
                }
                break;
            }
            case CAL:
            {
                stack[sp + ret_addr] = ip;
                stack[sp + dynamic_link] = bp;
                stack[sp + static_link] = bp;
                ip = ir.a;
                bp = sp;
                break;
            }
            case INT:
            {
                sp_stack[sp_top++] = sp;
                sp += ir.a;
                break;
            }
            case JMP:
            {
                ip = ir.a;
                break;
            }
            case JPC:
            {
                if (stack[sp - 1] == 0)
                    ip = ir.a;
                break;
            }
            case OPR:
            {
                switch (ir.a)
                {
                case OP_RET:
                {
                    ip = stack[bp + ret_addr];
                    bp = stack[bp + dynamic_link];
                    sp = sp_stack[--sp_top];
                    if (sp_top <= 0)
                    {
                        printf("程序正常结束！!\n");
                        return;
                    }
                    break;
                }
                case OP_ADD:
                {
                    stack[sp - 2] = stack[sp - 1] + stack[sp - 2];
                    sp--;
                    break;
                }
                case OP_MINUS:
                {
                    stack[sp - 2] = stack[sp - 2] - stack[sp - 1];
                    sp--;
                    break;
                }
                case OP_TIMES:
                {
                    stack[sp - 2] = stack[sp - 1] * stack[sp - 2];
                    sp--;
                    break;
                }
                case OP_DIV:
                {
                    stack[sp - 2] = stack[sp - 2] / stack[sp - 1];
                    sp--;
                    break;
                }
                case OP_NEQ:
                {
                    stack[sp - 2] = (stack[sp - 2] != stack[sp - 1]) ? 1 : 0;
                    sp--;
                    break;
                }
                case OP_EQ:
                {
                    stack[sp - 2] = (stack[sp - 2] == stack[sp - 1]) ? 1 : 0;
                    sp--;
                    break;
                }
                case OP_LSS:
                {
                    stack[sp - 2] = (stack[sp - 2] < stack[sp - 1]) ? 1 : 0;
                    sp--;
                    break;
                }
                case OP_LE:
                {
                    stack[sp - 2] = (stack[sp - 2] <= stack[sp - 1]) ? 1 : 0;
                    sp--;
                    break;
                }
                case OP_GT:
                {
                    stack[sp - 2] = (stack[sp - 2] > stack[sp - 1]) ? 1 : 0;
                    sp--;
                    break;
                }
                case OP_GE:
                {
                    stack[sp - 2] = (stack[sp - 2] >= stack[sp - 1]) ? 1 : 0;
                    sp--;
                    break;
                }
                case OP_READ:
                {
                    //std::cout << "请根据代码要求输入数字或字符:" << std::endl;
                    int temp = atoi(InputBuffer);
                    stack[sp++] = temp;
                    //std::cin >> stack[sp++];
                    break;
                }
                case OP_WRITE:
                {
                    std::cout << stack[sp - 1] << std::endl;
                    fprintf(fret, "%d\n", stack[sp - 1]);
                    itoa(stack[sp - 1], OutputBuffer,10);
                    return;
                    break;
                }
                case OP_MINUS_HEAD:
                {
                    stack[sp - 2] = stack[sp - 1] - stack[sp - 2];
                    sp--;
                    break;
                }
                default:
                {
                    printf("未知操作！\n"); return;
                }
                }
                break;
            }
            default:
                printf("未知指令！\n"); return;
            }
        }
    }


    void showWordAnalysis() {
        line = 1;
        f = fopen("test.pl0", "r");

        fword = fopen("wordOut.txt", "w");
        fcode = fopen("codeOut.txt", "w");
        fsymTable = fopen("symTableOut.txt", "w");
        fret = fopen("retOut.txt", "w");
        getSym();
        block(0, 0);
        fclose(f);
        fclose(fword);
        fclose(fcode);
        fclose(fsymTable);
        fclose(fret);
    }

    void showSourceCode() {
        ImGui::Begin("源码");
        f = fopen("test.pl0", "r");


        char wordBuffer[255];
        while (fgets(wordBuffer, 255, f) != nullptr) {
            ImGui::Text(wordBuffer);
        }
        fclose(f);
        ImGui::End();
    }

    void wordAnalysis() {
        ImGui::Begin("Generate");
        if (ImGui::Button("分析")) {
            PL0::showWordAnalysis();
        }
        ImGui::End();

        ImGui::Begin("词法分析");
        fwordOut = fopen("wordOut.txt", "r");
        char wordBuffer[255];
        while (fgets(wordBuffer, 255, fwordOut) != nullptr) {
            ImGui::Text(wordBuffer);
        }
        fclose(fwordOut);
        ImGui::End();
    }

    void showPcode() {
        ImGui::Begin("类Pcode虚拟机");
        for (int i = 1; i < pc; i++)
        {
            ImGui::Text("%d:\t %s\t %d\t %d\n", i, CodeTable[code[i].op], code[i].l, code[i].a);
        }
        ImGui::End();
    }



    void showSymTable() {
        ImGui::Begin("符号表");
        int j = 0;
        do {
            switch (symTable[j].type)
            {
            case 0:
                ImGui::Text("const %s := %d\n", symTable[j].name, symTable[j].value);
                break;
            case 1:
                ImGui::Text("var %s\n", symTable[j].name);
                break;
            case 2:
                ImGui::Text("proceduret %s\n", symTable[j].name);
                break;
            }
            j++;
        } while (symTable[j].value != 0);
        ImGui::End();
    }


    void dockSpace() {

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
                if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
                if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Begin("输入");
        ImGui::InputText(" ", InputBuffer, sizeof(InputBuffer));
        ImGui::End();

        ImGui::Begin("Run");
        if (ImGui::Button("编译运行"))
        { 
            PL0::interprete();
        }
        ImGui::End();

        ImGui::Begin("输出信息");
        fretOut = fopen("retOut.txt", "r");
        char retBuffer[255];
        while (fgets(retBuffer, 255, fretOut) != nullptr) {
            ImGui::Text(retBuffer);
        }
        fclose(fretOut);
        ImGui::Text(OutputBuffer);
        ImGui::End();

        ImGui::End();

    }
}
