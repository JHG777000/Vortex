/*
 Copyright (c) 2023 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexParser.h
//Header file for VortexParser.

#ifndef Vortex_VortexParser_h
#define Vortex_VortexParser_h

typedef enum {
 EvalModeHold,
 EvalModeRelease
} VortexLexerEvalMode;

typedef struct VortexParserToken_s* VortexParserToken;
typedef struct VortexParserTreeNode_s* VortexParserTreeNode;
typedef struct VortexLexer_s* VortexLexer;
typedef struct VortexParser_s* VortexParser;

typedef void (*VortexLexerEvaluator)(VortexLexer lexer, vortex_int character);
typedef void (*VortexLexerTokenizer)(VortexLexer lexer);

#define vortex_lexer_evaluator(name) void name(VortexLexer lexer, vortex_int character)
#define vortex_lexer_tokenizer(name) void name(VortexLexer lexer)

VortexParserToken VortexParserToken_New(void);
VortexParserToken VortexParserToken_NewFromCharacters(VortexLexer lexer);
void VortexParserToken_Destroy(VortexParserToken token);
void VortexParserToken_SetTokenData(VortexParserToken token,
  VortexAny token_data);
VortexAny VortexParserToken_GetTokenData(VortexParserToken token);
void VortexParserToken_SetTokenString(VortexParserToken token,
  VortexString token_string);
void VortexParserToken_SetTokenStringFromCharacterArray(VortexParserToken token,
  vortex_int* character_array, vortex_ulong array_size);
VortexString VortexParserToken_GetTokenString(VortexParserToken token);      
VortexLexer VortexLexer_New(VortexFile file);
void VortexLexer_Destroy(VortexLexer lexer);
void VortexLexer_SetLexerData(VortexLexer lexer, VortexAny lexer_data);
VortexAny VortexLexer_GetLexerData(VortexLexer lexer);
void VortexLexer_AddToken(VortexLexer lexer, VortexParserToken token);
void VortexLexer_PrintTokenArray(VortexLexer lexer);
vortex_int* VortexLexer_NewCharacterArray(vortex_ulong* array_size);
vortex_int* VortexLexer_AddCharacterToCharacterArray(vortex_int character,
 vortex_int* character_array, vortex_ulong* array_size);
void VortexLexer_AddCharacter(VortexLexer lexer, vortex_int character);
void VortexLexer_RemoveAllCharacters(VortexLexer lexer); 
void VortexLexer_SetTokenizer(VortexLexer lexer, VortexLexerTokenizer tokenizer); 
vortex_int VortexLexer_AddTokenizerFilter(VortexLexer lexer,
  const char* filter_name);
vortex_int VortexLexer_RemoveTokenizerFilter(VortexLexer lexer,
  const char* filter_name);
vortex_int VortexLexer_AddFilterToTokenizerFilter(VortexLexer lexer,
  const char* filter_name, const char* filter);
vortex_int VortexLexer_ApplyTokenizerFilter(VortexLexer lexer,
  const char* filter_name, const char* filter);
vortex_int VortexLexer_AddEvaluator(VortexLexer lexer,
  VortexLexerEvaluator evaluator, const char* character);
void VortexLexer_SetEvalMode(VortexLexer lexer,
  VortexLexerEvalMode eval_mode);
VortexLexerEvalMode VortexLexer_GetEvalMode(VortexLexer lexer);
void VortexLexer_Dispatch(VortexLexer lexer);             
void VortexLexer_Redispatch(VortexLexer lexer, vortex_int character);

#endif /* Vortex_VortexParser_h */ 