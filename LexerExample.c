#include <stdio.h>
#include <stdlib.h>
#include <Vortex/VortexMem.h>
#include <Vortex/VortexFile.h>
#include <Vortex/VortexParser.h>

//((1+2)+3)

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

void Vortex_LexerExample(void) {
  VortexFile file = VortexFile_OpenFile("lexer_example",vortex_read_mode);
  VortexLexer lexer = VortexLexer_New(file);
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"(");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,")");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"+");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"-");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"*");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"/");
  VortexLexer_AddEvaluator(lexer,symbol_evaluator,"%");
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
  VortexLexer_Dispatch(lexer);
  VortexLexer_PrintTokenArray(lexer);
  VortexLexer_Destroy(lexer);
  VortexFile_CloseFile(file);
}