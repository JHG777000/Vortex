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
main(): {
 #text.   
}.
*/

typedef struct lexer_data_s {
  vortex_ulong count;
} *lexer_data;

vortex_parser_processor(main) {
  VortexLexer_PrintTokenArray(VortexParser_GetLexer(parser));
  puts("EOL");
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
  VortexLexer_RemoveAllCharacters(lexer);
  VortexLexer_SetEvalMode(lexer,EvalModeRelease);
  VortexLexer_Redispatch(lexer,character);
}

vortex_lexer_evaluator(symbol_evaluator) {
  VortexLexer_AddCharacter(lexer,character);
  VortexToken token = VortexToken_NewFromCharacters(lexer);
  VortexLexer_AddToken(lexer,token);
  VortexLexer_RemoveAllCharacters(lexer);
  VortexLexer_Dispatch(lexer);
}

vortex_lexer_tokenizer(number_tokenizer) {
  VortexToken token = VortexToken_NewFromCharacters(lexer);
  VortexLexer_AddToken(lexer,token);
  VortexLexer_RemoveAllCharacters(lexer);
}

vortex_lexer_evaluator(number_evaluator) {
  VortexLexer_AddCharacter(lexer,character);
  VortexLexer_SetTokenizer(lexer,number_tokenizer);  
  VortexLexer_Dispatch(lexer);
}

void Vortex_ParserExample(void) {
  VortexFile file = VortexFile_OpenFile("parser_example",vortex_read_mode);
  VortexLexer lexer = VortexLexer_New(file);
  lexer_data data = vortex_new_mem_of_type(struct lexer_data_s);
  data->count = 0;
  VortexLexer_SetLexerData(lexer,data);
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"{");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"}");
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
  VortexLexer_SetEndOfLineCharacter(lexer,'.');
  VortexParser parser = VortexParser_New(lexer);
  VortexParser_SetMainProcessor(parser,main_processor);
  VortexLexer_SetParser(lexer,parser);
  VortexLexer_Dispatch(lexer);
  VortexLexer_PrintTokenArray(lexer);
  VortexParser_Destroy(parser);
  VortexLexer_Destroy(lexer);
  VortexFile_CloseFile(file);
}