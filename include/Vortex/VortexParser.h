/*
 Copyright (c) 2023-2024 Jacob Gordon. All rights reserved.

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

#include <ctype.h>

typedef enum {
 EvalModeHold,
 EvalModeRelease
} VortexLexerEvalMode;

typedef struct VortexToken_s* VortexToken;
typedef struct VortexParseTree_s* VortexParseTree;
typedef struct VortexParseTreeNode_s* VortexParseTreeNode;
typedef struct VortexLexer_s* VortexLexer;
typedef struct VortexParser_s* VortexParser;

typedef void (*VortexLexerEvaluator)(VortexLexer lexer,
   vortex_int character, vortex_ulong dispatch_id);
typedef void (*VortexLexerTokenizer)(VortexLexer lexer,
   vortex_ulong dispatch_id);
typedef vortex_int (*VortexLexerDeterminer)(VortexLexer lexer,
   vortex_int character, vortex_ulong dispatch_id);

#define vortex_lexer_evaluator(name)\
 static void name(VortexLexer lexer, vortex_int character, vortex_ulong dispatch_id)
#define vortex_lexer_tokenizer(name)\
 static void name(VortexLexer lexer, vortex_ulong dispatch_id)
#define vortex_lexer_determiner(name)\
 static vortex_int name(VortexLexer lexer, vortex_int character, vortex_ulong dispatch_id) 

typedef VortexAny (*VortexParserProcessor)
 (VortexParser parser,
   VortexParseTree parse_tree, 
    vortex_ulong line_number);
typedef VortexAny (*VortexTokenDataToNodeData)
 (VortexToken token,
   VortexAny token_data);   

#define vortex_parser_processor(name)\
 static VortexAny name##_##processor(VortexParser parser, VortexParseTree parse_tree, vortex_ulong line_number)
#define vortex_parser_process(name)\
 name##_##processor(parser,parse_tree,line_number)
#define vortex_parser_accept(symbol)\
 VortexParser_AcceptOrExpect(VortexParser_GetToken(parser),#symbol,symbol,0,line_number)
#define vortex_parser_expect(symbol)\
 VortexParser_AcceptOrExpect(VortexParser_GetToken(parser),#symbol,symbol,1,line_number)  
#define vortex_parser_accept_with_offset(symbol,offset)\
 VortexParser_AcceptOrExpect(vortex_parser_peek(offset),#symbol,symbol,0,line_number)
#define vortex_parser_expect_with_offset(symbol,offset)\
 VortexParser_AcceptOrExpect(vortex_parser_peek(offset),#symbol,symbol,1,line_number)
#define vortex_parser_peek(offset)\
 VortexParser_GetTokenWithIndex(parser,VortexParser_GetTokenIndex(parser)+offset)
#define vortex_parser_advance\
 VortexParser_AdvanceTokenIndex(parser)
#define vortex_parser_retreat\
 VortexParser_RetreatTokenIndex(parser)
#define vortex_parser_advance_with_offset(offset)\
 VortexParser_AdvanceTokenIndexByOffset(parser,offset)
#define vortex_parser_retreat_with_offset\
 VortexParser_RetreatTokenIndexByOffset(parser,offset)
#define vortex_parser_reset\
 VortexParser_SetTokenIndex(parser,0)
#define vortex_parser_reset_with_index(index)\
 VortexParser_SetTokenIndex(parser,index)
#define vortex_parser_get_token\
 VortexParser_GetToken(parser)
#define vortex_parser_get_token_with_index(index)\
 VortexParser_GetTokenWithIndex(parser,index)

 
VortexParser VortexParser_New(VortexLexer lexer);
void VortexParser_Destroy(VortexParser parser);
VortexParseTree VortexParser_GetParseTree(VortexParser parser);
void VortexParser_SetDefaultToken(VortexParser parser,
  VortexToken default_token); 
void VortexParser_SetMainProcessor(VortexParser parser,
  VortexParserProcessor main_processor);  
void VortexParser_SetTokenIndex(VortexParser parser,
  vortex_ulong token_index);
void VortexParser_AdvanceTokenIndex(VortexParser parser);
void VortexParser_RetreatTokenIndex(VortexParser parser);
void VortexParser_AdvanceTokenIndexByOffset(VortexParser parser,
  vortex_ulong offset);    
void VortexParser_RetreatTokenIndexByOffset(VortexParser parser,
  vortex_ulong offset);
vortex_ulong VortexParser_GetTokenIndex(VortexParser parser);  
VortexToken VortexParser_GetTokenWithIndex(VortexParser parser,
  vortex_ulong index);    
VortexToken VortexParser_GetToken(VortexParser parser);
VortexLexer VortexParser_GetLexer(VortexParser parser);
vortex_int VortexParser_AcceptOrExpect(VortexToken token,
  const char* symbol_name, vortex_ulong symbol_id, 
  vortex_int expect, vortex_ulong line_number);  
void VortexParser_SetProcessorDispatch(VortexParser parser, 
  VortexParserProcessor processor, vortex_ulong dispatch_id);
VortexAny VortexParser_DispatchProcessor(VortexParser parser, 
  vortex_ulong dispatch_id, vortex_ulong line_number);    
void VortexParser_ParseLine(VortexParser parser,
  vortex_ulong line_number);
VortexParseTree VortexParseTree_New(void);
void VortexParseTree_Destroy(VortexParseTree tree);
void VortexParseTree_SetRootNode(VortexParseTree tree,
  VortexParseTreeNode node);
VortexParseTreeNode VortexParseTree_GetRootNode(VortexParseTree tree);  
void VortexParseTree_PushActiveNode(VortexParseTree tree,
  VortexParseTreeNode node);
VortexParseTreeNode VortexParseTree_PopActiveNode(VortexParseTree tree);
VortexParseTreeNode VortexParseTree_GetActiveNode(VortexParseTree tree);  
void VortexParseTree_SetActiveNode(VortexParseTree tree,
  VortexParseTreeNode node);  
VortexStack VortexParseTree_GettActiveStack(VortexParseTree tree); 
VortexParseTreeNode VortexParseTreeNode_New(
  VortexTokenDataToNodeData token_data_to_node_data,
  VortexToken token);
void VortexParseTreeNode_Destroy(VortexParseTreeNode node);
void VortexParseTreeNode_AddToArray(VortexParseTreeNode node,
  VortexParseTreeNode node_to_add);
VortexArray VortexParseTreeNode_GetArray(VortexParseTreeNode node);
void VortexParseTreeNode_AddToStore(VortexParseTreeNode node,
  VortexParseTreeNode node_to_add, char* label);
VortexStore VortexParseTreeNode_GetStore(VortexParseTreeNode node);  
void VortexParseTreeNode_SetData(VortexParseTreeNode node,
  VortexAny data);
VortexAny VortexParseTreeNode_GetData(VortexParseTreeNode node);
void VortexParseTreeNode_SetParsedString(VortexParseTreeNode node,
  VortexString parsed_string);
VortexString VortexParseTreeNode_GetParsedString(VortexParseTreeNode node); 
void VortexParseTreeNode_SetID(VortexParseTreeNode node,
  vortex_ulong id);
vortex_ulong VortexParseTreeNode_GetID(VortexParseTreeNode node);
VortexParseTreeNode VortexParseTreeNode_GetSuper(VortexParseTreeNode node);
void VortexParseTreeNode_IterateWith(
  VortexMemIteratorFuncType iterator,
  VortexParseTreeNode node);
void VortexParseTreeNode_ReiterateWith(
  VortexMemIteratorFuncType iterator,
  VortexParseTreeNode node);             
VortexToken VortexToken_New(void);
VortexToken VortexToken_NewFromCharacters(VortexLexer lexer);
void VortexToken_Destroy(VortexToken token);
void VortexToken_SetTokenID(VortexToken token,
  vortex_ulong token_id);
vortex_ulong VortexToken_GetTokenID(VortexToken token);
void VortexToken_SetTokenData(VortexToken token,
  VortexAny token_data);
VortexAny VortexToken_GetTokenData(VortexToken token);
void VortexToken_SetTokenString(VortexToken token,
  VortexString token_string);
void VortexToken_SetTokenStringFromCharacterArray(VortexToken token,
  vortex_int* character_array, vortex_ulong array_size);
VortexToken VortexToken_SetTokenStringFromCharacters(VortexLexer lexer,
   VortexToken token);   
VortexString VortexToken_GetTokenString(VortexToken token);      
VortexLexer VortexLexer_New(VortexFile file);
void VortexLexer_Destroy(VortexLexer lexer);
void VortexLexer_ResetLexer(VortexLexer lexer);
void VortexLexer_SetLexerData(VortexLexer lexer, VortexAny lexer_data);
VortexAny VortexLexer_GetLexerData(VortexLexer lexer);
void VortexLexer_SetParser(VortexLexer lexer, VortexParser parser) ;
vortex_ulong VortexLexer_GetLineNumber(VortexLexer lexer);
void VortexLexer_AddToken(VortexLexer lexer, VortexToken token);
void VortexLexer_PrintTokenArray(VortexLexer lexer);
VortexArray VortrxLexer_GetTokenArray(VortexLexer lexer);
vortex_int* VortexLexer_NewCharacterArray(vortex_ulong* array_size);
vortex_int* VortexLexer_AddCharacterToCharacterArray(vortex_int character,
 vortex_int* character_array, vortex_ulong* array_size);
void VortexLexer_AddCharacter(VortexLexer lexer, vortex_int character);
void VortexLexer_RemoveAllCharacters(VortexLexer lexer);
vortex_int* VortexLexer_GetCharacterArray(VortexLexer lexer);
vortex_ulong VortexLexer_GetCharacterArrayIndex(VortexLexer lexer);
void VortexLexer_SetTokenizer(VortexLexer lexer, VortexLexerTokenizer tokenizer); 
vortex_int VortexLexer_AddTokenizerFilter(VortexLexer lexer,
  const char* filter_name);
vortex_int VortexLexer_RemoveTokenizerFilter(VortexLexer lexer,
  const char* filter_name);
vortex_int VortexLexer_AddFilterToTokenizerFilter(VortexLexer lexer,
  const char* filter_name, const char* filter);
vortex_int VortexLexer_AddIDToFilter(VortexLexer lexer,
  const char* filter_name, const char* filter, vortex_ulong id);  
vortex_int VortexLexer_ApplyTokenizerFilter(VortexLexer lexer,
  const char* filter_name, const char* filter);
vortex_ulong VortexLexer_GetFilterID(VortexLexer lexer,
  const char* filter_name, const char* filter);
vortex_ulong VortexLexer_GetFilterIDWithCharacter(VortexLexer lexer,
  const char* filter_name, vortex_int character);    
vortex_int VortexLexer_AddEvaluator(VortexLexer lexer,
  VortexLexerEvaluator evaluator, const char* character);
void VortexLexer_SetEvalMode(VortexLexer lexer,
  VortexLexerEvalMode eval_mode);
VortexLexerEvalMode VortexLexer_GetEvalMode(VortexLexer lexer);
void VortexLexer_SetEndOfLineCharacter(VortexLexer lexer,
  vortex_int character);
vortex_int VortexLexer_GetEndOfLineCharacter(VortexLexer lexer);
void VortexLexer_SetDeterminer(VortexLexer lexer,
  VortexLexerDeterminer determiner);
VortexLexerDeterminer VortexLexer_GetDeterminer(VortexLexer lexer);  
void VortexLexer_Dispatch(VortexLexer lexer);             
void VortexLexer_Redispatch(VortexLexer lexer, vortex_int character);

#endif /* Vortex_VortexParser_h */ 