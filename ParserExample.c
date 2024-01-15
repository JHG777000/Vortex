#include <stdio.h>
#include <stdlib.h>
#include <Vortex/VortexMem.h>
#include <Vortex/VortexFile.h>
#include <Vortex/VortexParser.h>

/*
value: 900.
text: "Hello World".
array: {1,2,3,4,5}.
struct: {a: 1, b: 2}.
main: () {
 #text.   
}.

var value : int = 900;
var text : string = "Hello World";
var my_array : int[5] = {1,2,3,4,5};
struct {
 var test : int;
} my_struct_def;

var my_struct : my_struct_def = {0};

function main {
 statement #text;
 if true {
  
 } else {
  
 };
};

*/

typedef enum {
  Literal,
  Value,
  BinaryOperator,
  UnaryOperator,
  PostOperator,
} high_level_symbols;

typedef enum {
  EOL,
  Assignment,
  Arguments,
  Identifier,
  Integer,
  Double,
  String,
  Lambda,
  Collection,
  Group,
  LParenthesis,
  RParenthesis,
  LSquareBracket,
  RSquareBracket,
  LBracket,
  RBracket,
  Comma,
  Addition,
  Subtraction,
  Multiplication,
  Division,
  Modulus,
  Print,
  If,
  Not,
  BNot,
  And,
  Band,
  Or,
  Bor,
  Xor,
  GreaterThan,
  GreaterThanOrEqual,
  LessThan,
  LessThanOrEqual,
  Equal,
  Call,
  Index,
  Increment,
  Decrement
} symbols;

typedef struct lexer_data_s {
  vortex_ulong count;
  vortex_ulong eol_mode;
  vortex_ulong is_double;
} *lexer_data;

vortex_parser_processor(main) {
  printf("LINE# %d\n",line_number);
  VortexLexer_PrintTokenArray(VortexParser_GetLexer(parser));
  puts("EOL");
  //VortexParser_SetProcessorDispatch(parser,unary_operation_processor,If);
  //VortexParser_SetProcessorDispatch(parser,unary_operation_processor,Not);
  //VortexParser_SetProcessorDispatch(parser,unary_operation_processor,BNot);
  //VortexParser_SetProcessorDispatch(parser,unary_operation_processor,Print);
  //VortexParser_SetProcessorDispatch(parser,unary_operation_processor,Identifier);
  VortexParseTreeNode node = VortexParseTreeNode_New(NULL,NULL);
  VortexParseTreeNode_SetParsedString(node,vortex_str("Statement"));
  //VortexParser_DispatchProcessor(parser,
    //VortexToken_GetTokenID(vortex_parser_peek(0)),line_number);
  //VortexParseTree_Print(parse_tree);
  //printf("NUM_OF_NODES: %d\n",VortexParseTree_GetNumOfNodes(parse_tree));  
  return NULL;  
}

vortex_lexer_determiner(end_of_line_determiner) {
  lexer_data data = VortexLexer_GetLexerData(lexer);
  if (character == '{') data->eol_mode++;
  if (character == '}') data->eol_mode--;
  if (character == '(') data->eol_mode++;
  if (character == ')') data->eol_mode--;
  printf("EOL MODE: %d\n",data->eol_mode);
  if (character == '.' && !data->eol_mode)
   return 1;
  return 0; 
}

vortex_lexer_evaluator(string_evaluator) {
  lexer_data data = VortexLexer_GetLexerData(lexer);
  if (character == '"') data->count++;
  if (data->count < 2) {
    if (character != '"')
     VortexLexer_AddCharacter(lexer,character);
    VortexLexer_SetEvalMode(lexer,EvalModeHold);
    VortexLexer_Dispatch(lexer);
    return;
  }
  data->count = 0;
  VortexToken token = VortexToken_NewFromCharacters(lexer);
  VortexLexer_AddToken(lexer,token);
  VortexToken_SetTokenID(token,String);
  VortexLexer_RemoveAllCharacters(lexer);
  VortexLexer_SetEvalMode(lexer,EvalModeRelease);
  VortexLexer_Dispatch(lexer);
}

vortex_lexer_evaluator(identifier_evaluator) {
  if (isalpha(character) 
    || isdigit(character)
    || character == '_') {
    VortexLexer_AddCharacter(lexer,character);
    VortexLexer_SetEvalMode(lexer,EvalModeHold);
    VortexLexer_Dispatch(lexer);
    return;
  }
  VortexToken token = VortexToken_NewFromCharacters(lexer);
  VortexLexer_AddToken(lexer,token);
  VortexToken_SetTokenID(token,Identifier);
  VortexLexer_RemoveAllCharacters(lexer);
  VortexLexer_SetEvalMode(lexer,EvalModeRelease);
  VortexLexer_Redispatch(lexer,character);
}

vortex_lexer_evaluator(symbol_evaluator) {
  VortexLexer_AddCharacter(lexer,character);
  VortexToken token = VortexToken_NewFromCharacters(lexer);
  VortexLexer_AddToken(lexer,token);
   VortexToken_SetTokenID(token,
     VortexLexer_GetFilterIDWithCharacter(lexer,"symbols",character));
  VortexLexer_RemoveAllCharacters(lexer);
  VortexLexer_Dispatch(lexer);
}
 
vortex_lexer_evaluator(number_evaluator) {
  lexer_data data = VortexLexer_GetLexerData(lexer);
  if (isdigit(character)
    || character == '.') {
    if (character == '.') {
      if (!data->eol_mode)
       goto skip; 
      data->is_double = 1;
    }
    VortexLexer_AddCharacter(lexer,character);
    VortexLexer_SetEvalMode(lexer,EvalModeHold);
    VortexLexer_Dispatch(lexer);
    return;
  }
  skip:
  VortexToken token = VortexToken_NewFromCharacters(lexer);
  VortexLexer_AddToken(lexer,token);
  VortexToken_SetTokenID(token,Integer);
  if (data->is_double)
   VortexToken_SetTokenID(token,Double);
  VortexLexer_RemoveAllCharacters(lexer);
  VortexLexer_SetEvalMode(lexer,EvalModeRelease);
  data->is_double = 0;
  VortexLexer_Redispatch(lexer,character);
}

void Vortex_ParserExample(void) {
  VortexFile file = VortexFile_OpenFile("parser_example",vortex_read_mode);
  VortexLexer lexer = VortexLexer_New(file);
  lexer_data data = vortex_new_mem_of_type(struct lexer_data_s);
  data->count = 0;
  data->eol_mode = 0;
  data->is_double = 0;
  VortexLexer_SetLexerData(lexer,data);
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"{");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"}");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"[");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"]");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"(");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,")");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"+");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"-");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"*");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"/");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"%");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"#");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,":");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,".");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,",");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"?");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"!");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"~");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"@");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"&");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,";");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"|");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"^");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,">");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"<");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"$");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"µ");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"=");
  VortexLexer_AddEvaluator(lexer,number_evaluator,"0");
  VortexLexer_AddEvaluator(lexer,number_evaluator,"1");
  VortexLexer_AddEvaluator(lexer,number_evaluator,"2");
  VortexLexer_AddEvaluator(lexer,number_evaluator,"3");
  VortexLexer_AddEvaluator(lexer,number_evaluator,"4");
  VortexLexer_AddEvaluator(lexer,number_evaluator,"5");
  VortexLexer_AddEvaluator(lexer,number_evaluator,"6");
  VortexLexer_AddEvaluator(lexer,number_evaluator,"7");
  VortexLexer_AddEvaluator(lexer,number_evaluator,"8");
  VortexLexer_AddEvaluator(lexer,number_evaluator,"9");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"_");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"a");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"b");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"c");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"d");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"e");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"f");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"g");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"h");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"i");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"j");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"k");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"l");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"m");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"n");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"o");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"p");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"q");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"r");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"s");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"t");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"u");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"v");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"w");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"x");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"y");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"z");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"A");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"B");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"C");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"D");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"E");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"F");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"G");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"H");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"I");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"J");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"K");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"L");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"M");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"N");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"O");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"P");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"Q");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"R");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"S");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"T");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"U");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"V");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"W");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"X");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"Y");
  VortexLexer_AddEvaluator(lexer,identifier_evaluator,"Z");
  VortexLexer_AddEvaluator(lexer,string_evaluator,"\"");
  VortexLexer_AddTokenizerFilter(lexer,"symbols");
  VortexLexer_AddIDToFilter(lexer,"symbols",":",Assignment);
  VortexLexer_AddIDToFilter(lexer,"symbols",".",EOL);
  VortexLexer_AddIDToFilter(lexer,"symbols","(",LParenthesis);
  VortexLexer_AddIDToFilter(lexer,"symbols",")",RParenthesis);
  VortexLexer_AddIDToFilter(lexer,"symbols","[",LSquareBracket);
  VortexLexer_AddIDToFilter(lexer,"symbols","]",RSquareBracket);
  VortexLexer_AddIDToFilter(lexer,"symbols","{",LBracket);
  VortexLexer_AddIDToFilter(lexer,"symbols","}",RBracket);
  VortexLexer_AddIDToFilter(lexer,"symbols",",",Comma);
  VortexLexer_AddIDToFilter(lexer,"symbols","+",Addition);
  VortexLexer_AddIDToFilter(lexer,"symbols","-",Subtraction);
  VortexLexer_AddIDToFilter(lexer,"symbols","*",Multiplication);
  VortexLexer_AddIDToFilter(lexer,"symbols","/",Division);
  VortexLexer_AddIDToFilter(lexer,"symbols","%",Modulus);
  VortexLexer_AddIDToFilter(lexer,"symbols","#",Print);
  VortexLexer_AddIDToFilter(lexer,"symbols","?",If);
  VortexLexer_AddIDToFilter(lexer,"symbols","!",Not);
  VortexLexer_AddIDToFilter(lexer,"symbols","~",BNot);
  VortexLexer_AddIDToFilter(lexer,"symbols","@",And);
  VortexLexer_AddIDToFilter(lexer,"symbols","&",Band);
  VortexLexer_AddIDToFilter(lexer,"symbols",";",Or);
  VortexLexer_AddIDToFilter(lexer,"symbols","|",Bor);
  VortexLexer_AddIDToFilter(lexer,"symbols","^",Xor);
  VortexLexer_AddIDToFilter(lexer,"symbols",">",GreaterThan);
  VortexLexer_AddIDToFilter(lexer,"symbols","$",GreaterThanOrEqual);
  VortexLexer_AddIDToFilter(lexer,"symbols","<",LessThan);
  VortexLexer_AddIDToFilter(lexer,"symbols","µ",LessThanOrEqual);
  VortexLexer_AddIDToFilter(lexer,"symbols","=",Equal);
  VortexLexer_SetDeterminer(lexer,end_of_line_determiner);
  VortexParser parser = VortexParser_New(lexer);
  VortexToken token = VortexToken_New();
  VortexToken_SetTokenString(token,vortex_str("."));
   VortexToken_SetTokenID(token,EOL);
  VortexParser_SetDefaultToken(parser,token);
  VortexParser_SetMainProcessor(parser,main_processor);
  VortexLexer_SetParser(lexer,parser);
  VortexLexer_Dispatch(lexer);
  VortexLexer_PrintTokenArray(lexer);
  VortexParser_Destroy(parser);
  VortexLexer_Destroy(lexer);
  VortexFile_CloseFile(file);
}
