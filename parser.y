%{
#include <iostream>
#include <fstream>
#include <vector>

#include"../main.h"
#include"../Express.h"

extern int yylex();
void yyerror(const char*);
%}

%union
{
  char* str_val;
  int int_val;
  char char_val;
  Express* expr_val;
  std::vector<std::string>* vect_str;
  std::vector<Express*>* vect_expr;
  int* type_val;
}

%error-verbose
%token ARRAYSY 
%token ASSIGNSY 
%token BEGINSY 
%token CHARCONSTSY 
%token CHRSY 
%token COLONSY 
%token COMMASY 
%token CONSTSY 
%token DIVSY 
%token DOSY 
%token DOTSY 
%token DOWNTOSY 
%token ELSEIFSY 
%token ELSESY 
%token ENDSY 
%token EQSY 
%token FORSY 
%token FORWARDSY 
%token FUNCTIONSY 
%token GTESY 
%token GTSY 
%token IDENTSY
%token IFSY 
%token INTSY 
%token LBRACKETSY 
%token LPARENSY 
%token LTESY 
%token LTSY 
%token MINUSSY 
%token MODSY 
%token MULTSY
%token NOTSY 
%token OFSY
%token ORDSY 
%token PLUSSY 
%token PREDSY 
%token PROCEDURESY 
%token RBRACKETSY
%token READSY 
%token RECORDSY 
%token REFSY 
%token REPEATSY 
%token RETURNSY 
%token RPARENSY 
%token SCOLONSY 
%token STOPSY 
%token STRINGSY 
%token SUCCSY
%token THENSY 
%token TOSY 
%token TYPESY 
%token UNTILSY 
%token VARSY 
%token WHILESY 
%token WRITESY

%left ANDSY ORSY
%right NOTSY
%nonassoc EQSY LTESY GTESY GTSY LTSY NEQSY 
%left PLUSSY MINUSSY 
%left DIVSY MODSY MULTSY
%right UMINUSSY 

%type <char_val> CHARCONSTSY
%type <int_val> Arguments 
%type <int_val> ArrayType 
%type <int_val> Assignment
%type <int_val> Block 
%type <int_val> Body  
%type <int_val> ElseClause 
%type <int_val> ElseIfHead 
%type <int_val> ElseIfList 
%type <expr_val> Expression 
%type <int_val> FSignature 
%type <int_val> FieldDecl 
%type <int_val> FieldDecls
%type <int_val> ForHead 
%type <int_val> ForStatement 
%type <int_val> FormalParameter
%type <int_val> FormalParameters  
%type <int_val> FunctionCall 
%type <int_val> INTSY 
%type <vect_str> IdentList 
%type <int_val> OptVar 
%type <int_val> IfHead 
%type <int_val> IfStatement 
%type <str_val> LValue 
%type <int_val> OptArguments 
%type <int_val> OptFormalParameters  
%type <int_val> PSignature 
%type <int_val> ProcedureCall
%type <int_val> ReadArgs
%type <int_val> ReadStatement 
%type <int_val> RecordType 
%type <int_val> RepeatStatement 
%type <int_val> ReturnStatement 
%type <type_val> SimpleType 
%type <int_val> Statement 
%type <int_val> StatementList 
%type <int_val> StopStatement 
%type <int_val> ThenPart 
%type <int_val> ToHead 
%type <type_val> Type 
%type <int_val> WhileHead 
%type <int_val> WhileStatement 
%type <vect_expr> WriteArgs 
%type <int_val> WriteStatement  
%type <str_val> IDENTSY 
%type <str_val> STRINGSY 

%%
Program : ProgramHead Block DOTSY {}
				;

ProgramHead : OptConstDecls OptTypeDecls OptVarDecls PFDecls
            ;
OptConstDecls : CONSTSY ConstDecls
							|
							;

ConstDecls : ConstDecls ConstDecl
					 | ConstDecl
					 ;

ConstDecl : IDENTSY EQSY Expression SCOLONSY {}
					;

PFDecls : PFDecls ProcedureDecl
        | PFDecls FunctionDecl
        |
        ;

ProcedureDecl : PSignature SCOLONSY FORWARDSY SCOLONSY {}
              | PSignature SCOLONSY Body SCOLONSY {}
				    	;

PSignature : PROCEDURESY IDENTSY LPARENSY OptFormalParameters RPARENSY {}
           ;

FunctionDecl : FSignature SCOLONSY FORWARDSY SCOLONSY {}
						 | FSignature SCOLONSY Body SCOLONSY {}
						 ;

FSignature : FUNCTIONSY IDENTSY LPARENSY OptFormalParameters RPARENSY COLONSY Type {}
           ;

OptFormalParameters : FormalParameters {}
                    | {}
                    ;

FormalParameters : FormalParameters SCOLONSY FormalParameter {}
                 | FormalParameter {}
                 ;

FormalParameter : OptVar IdentList COLONSY Type {}
                ;

OptVar : VARSY {}
       | REFSY {}
       | {}
       ;


Body : OptConstDecls OptTypeDecls OptVarDecls Block {}
     ;

Block : BEGINSY StatementList ENDSY { outBlock(); }
      ;

StatementList : StatementList SCOLONSY Statement {}
              | Statement {}
              ;

OptTypeDecls : TYPESY TypeDecls
             |
             ;

TypeDecls    : TypeDecls TypeDecl
             | TypeDecl
             ;

TypeDecl : IDENTSY EQSY Type SCOLONSY {}
         ;

Type : SimpleType {$$ = $1; }
     | RecordType {}
     | ArrayType {}
     ;

SimpleType : IDENTSY {$$ = symbol_table.findType($1); }
           ;

RecordType : RECORDSY FieldDecls ENDSY {}
           ;

FieldDecls : FieldDecls FieldDecl {}
           | {}
           ;

FieldDecl : IdentList COLONSY Type SCOLONSY {}
          ;

IdentList : IdentList COMMASY IDENTSY {$1->push_back($3); $$ = $1;}
          | IDENTSY {$$ = new std::vector<std::string>(1, $1);}
          ;

ArrayType : ARRAYSY LBRACKETSY Expression COLONSY Expression RBRACKETSY OFSY Type {}
          ;

OptVarDecls : VARSY VarDecls
            |
            ;

VarDecls    : VarDecls VarDecl
            | VarDecl
            ;

VarDecl : IdentList COLONSY Type SCOLONSY {for(unsigned int i = 0; i < $1->size(); i++){ Express a($3, 0); symbol_table.addExpr($1->at(i), a);} }
        ;

Statement : Assignment {}
          | IfStatement {}
          | WhileStatement {}
          | RepeatStatement {}
          | ForStatement {}
          | StopStatement {}
          | ReturnStatement {}
          | ReadStatement {}
          | WriteStatement {}
          | ProcedureCall {}
          | {}
          ;

Assignment : LValue ASSIGNSY Expression { outAssignment($1, $3); }
           ;

IfStatement : IfHead ThenPart ElseIfList ElseClause ENDSY {}
            ;

IfHead : IFSY Expression {}
       ;

ThenPart : THENSY StatementList {}
         ;

ElseIfList : ElseIfList ElseIfHead ThenPart {}
           |{}
           ;

ElseIfHead : ELSEIFSY Expression {}
           ;

ElseClause : ELSESY StatementList {}
           | {}
           ;

WhileStatement : WhileHead DOSY StatementList ENDSY {}
               ;

WhileHead : WHILESY Expression {}
          ;

RepeatStatement : REPEATSY StatementList UNTILSY Expression {}

ForStatement : ForHead ToHead DOSY StatementList ENDSY{}
             ;

ForHead : FORSY IDENTSY ASSIGNSY Expression {}
        ;

ToHead : TOSY Expression {}
       | DOWNTOSY Expression {}
       ;

StopStatement : STOPSY {}
              ;

ReturnStatement : RETURNSY Expression {}
                | RETURNSY {}
                ;


ReadStatement : READSY LPARENSY ReadArgs RPARENSY {}
              ;

ReadArgs : ReadArgs COMMASY LValue {}
         | LValue                  {}
         ;

WriteStatement : WRITESY LPARENSY WriteArgs RPARENSY {outWriteStatement($3);}
               ;

WriteArgs : WriteArgs COMMASY Expression {$1->push_back($3); $$ = $1; }
          | Expression                   {$$ = new std::vector<Express*>(1, $1);}
          ;

ProcedureCall : IDENTSY LPARENSY OptArguments RPARENSY {}
              ;
OptArguments : Arguments {}
             |           {}
             ;
Arguments : Arguments COMMASY Expression {}
          | Expression                   {}
          ;

Expression : CHARCONSTSY                         {$$ = new Express(type_char, $1);}
           | CHRSY LPARENSY Expression RPARENSY  {if($3->type_ptr == type_int){$$ = new Express(type_char, $3->raw_val);}else{std::cerr << "Error: chr is undefined for non-integer parameters" <<std::endl;} }
           | Expression ANDSY Expression         {if($1->raw_val > 0 && $3->raw_val > 0){$$ = new Express(type_bool, 1);}else{$$ = new Express(type_bool, 0);}}
           | Expression DIVSY Expression         {}
           | Expression EQSY Expression          {}
           | Expression GTESY Expression         {}
           | Expression GTSY Expression          {}
           | Expression LTESY Expression         {}
           | Expression LTSY Expression          {}
           | Expression MINUSSY Expression       {}
           | Expression MODSY Expression         {}
           | Expression MULTSY Expression        {}
           | Expression NEQSY Expression         {}
           | Expression ORSY Expression          {}
           | Expression PLUSSY Expression        {}
           | FunctionCall                        {}
           | INTSY                               {}
           | LPARENSY Expression RPARENSY        {}
           | LValue                              {$$ = symbol_table.findExpr($1);}
           | MINUSSY Expression %prec UMINUSSY   {}
           | NOTSY Expression                    {}
           | ORDSY LPARENSY Expression RPARENSY  {}
           | PREDSY LPARENSY Expression RPARENSY {}
           | STRINGSY                            {$$ = new Express(type_string, string_list.size()); string_list.push_back($1); }
           | SUCCSY LPARENSY Expression RPARENSY {}
           ;

FunctionCall : IDENTSY LPARENSY OptArguments RPARENSY {}
             ;

LValue : LValue DOTSY IDENTSY {}
       | LValue LBRACKETSY Expression RBRACKETSY {}
       | IDENTSY {$$ = $1;}
       ;
%%

void yyerror(const char* msg)
{
  std::cerr << msg;
}
