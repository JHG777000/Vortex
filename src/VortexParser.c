/*
 Copyright (c) 2023 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexParser.c

#include <stdio.h>
#include <stdlib.h>

#include <Vortex/VortexMem.h>
#include <Vortex/VortexFile.h>
#include <Vortex/VortexParser.h>

struct VortexParserToken_s {
 VortexString token_string;
 VortexAny token_data;
};

struct VortexParserTreeNode_s {
 VortexString parsed_string;
 VortexAny node_data;
 VortexAny A;
 VortexAny B;
};

struct VortexLexer_s {
 VortexAny lexer_data;
 VortexFile file;
 vortex_int* character_array;
 vortex_ulong num_of_characters;
 VortexArray token_array;
 VortexLexerTokenizer tokenizer;
 VortexStore tokenizer_filters;
 vortex_ulong line_number;
 VortexStore evaluator_store;
 VortexLexerEvaluator last_evaluator;
 VortexLexerEvalMode eval_mode;
};

struct VortexParser_s {
 VortexAny parser_data;
};

VortexParserToken VortexParserToken_New(void) {
  VortexParserToken token = vortex_new_mem_of_type(struct VortexParserToken_s);
  token->token_string = NULL;
  token->token_data = NULL;
  return token;
}

VortexParserToken VortexParserToken_NewFromCharacters(VortexLexer lexer) {
  VortexParserToken token = VortexParserToken_New();
  VortexParserToken_SetTokenStringFromCharacterArray(token,
    lexer->character_array,lexer->num_of_characters);
  return token;  
}

void VortexParserToken_Destroy(VortexParserToken token) {
 if (token->token_string != NULL) vortex_strfree(token->token_string);
 free(token);
}

void VortexParserToken_SetTokenData(VortexParserToken token,
  VortexAny token_data) {
  token->token_data = token_data;
}

VortexAny VortexParserToken_GetTokenData(VortexParserToken token) {
  return token->token_data;
}

void VortexParserToken_SetTokenString(VortexParserToken token,
  VortexString token_string) {
  token->token_string = token_string;
}

void VortexParserToken_SetTokenStringFromCharacterArray(VortexParserToken token,
  vortex_int* character_array, vortex_ulong array_size) {
  token->token_string = VortexString_NewFromUTF32(character_array,array_size);
}

VortexString VortexParserToken_GetTokenString(VortexParserToken token) {
  return token->token_data;
}

VortexLexer VortexLexer_New(VortexFile file) {
  VortexLexer lexer = vortex_new_mem_of_type(struct VortexLexer_s);
  lexer->lexer_data = NULL;
  lexer->file = file;
  lexer->character_array = NULL;
  lexer->num_of_characters = 0;
  lexer->token_array = NULL;
  lexer->tokenizer = NULL;
  lexer->tokenizer_filters = NULL;
  lexer->line_number = 0;
  lexer->evaluator_store = NULL;
  lexer->last_evaluator = NULL;
  lexer->eval_mode = EvalModeRelease;
  return lexer;
}

void VortexLexer_Destroy(VortexLexer lexer) {
  VortexLexer_RemoveAllCharacters(lexer);
  if (lexer->token_array != NULL) {
   vortex_array_foreach(index,lexer->token_array) {
     VortexParserToken token = 
     VortexArray_GetItem(lexer->token_array,index);
     VortexParserToken_Destroy(token);
   }
    VortexArray_Destroy(lexer->token_array);
  }
  if (lexer->tokenizer_filters != NULL)
   VortexStore_Destroy(lexer->tokenizer_filters);
  if (lexer->evaluator_store != NULL)
    VortexStore_Destroy(lexer->evaluator_store);
  free(lexer);
}

void VortexLexer_SetLexerData(VortexLexer lexer, void* lexer_data) {
  lexer->lexer_data = lexer_data;
}

void* VortexLexer_GetLexerData(VortexLexer lexer) {
  return lexer->lexer_data;
}

void VortexLexer_AddToken(VortexLexer lexer, VortexParserToken token) {
  if (lexer->token_array == NULL) lexer->token_array = VortexArray_New();
  VortexArray_AddItem(lexer->token_array,token);
}

void VortexLexer_PrintTokenArray(VortexLexer lexer) {
  if (lexer->token_array == NULL) {
    printf("No tokens. Token array is empty.\n"); 
    return;
  }
  vortex_array_foreach(index,lexer->token_array) {
    VortexParserToken token = 
     VortexArray_GetItem(lexer->token_array,index);
    printf("Token: "); 
    vortex_strprint(token->token_string);
    printf("\n"); 
  }
}

vortex_int* VortexLexer_NewCharacterArray(vortex_ulong* array_size) {
  vortex_int* array = vortex_c_array(1,vortex_int);
  *array_size = 1;
  return array;
}

vortex_int* VortexLexer_AddCharacterToCharacterArray(vortex_int character,
  vortex_int* character_array, vortex_ulong* array_size) {
  if (*array_size == 0) {
   character_array = VortexLexer_NewCharacterArray(array_size);
   character_array[0] = character;
   return character_array;
  }
  vortex_int* array = vortex_realloc(character_array,
  (*array_size)+1,*array_size,vortex_int,1);
  array[*array_size] = character;
  (*array_size)++;
  return array;
}

void VortexLexer_AddCharacter(VortexLexer lexer, vortex_int character) {
 lexer->character_array = 
  VortexLexer_AddCharacterToCharacterArray(character,
   lexer->character_array,&lexer->num_of_characters);
}

void VortexLexer_RemoveAllCharacters(VortexLexer lexer) {
  lexer->num_of_characters = 0;
  free(lexer->character_array);
  lexer->character_array = NULL;
}

void VortexLexer_SetTokenizer(VortexLexer lexer,VortexLexerTokenizer tokenizer) {
  lexer->tokenizer = tokenizer;
}

vortex_int VortexLexer_AddTokenizerFilter(VortexLexer lexer,
  const char* filter_name) {
  if (lexer->tokenizer_filters == NULL) 
   lexer->tokenizer_filters = VortexStore_New();
  if (VortexStore_ItemExists(lexer->tokenizer_filters,filter_name)) 
   return 0;
  return 
   VortexStore_AddItem(lexer->tokenizer_filters,VortexStore_New(),filter_name);
}

vortex_int VortexLexer_RemoveTokenizerFilter(VortexLexer lexer,
  const char* filter_name) {
  if (lexer->tokenizer_filters == NULL) return 0;
  VortexStore filter_strore = VortexStore_GetItem(lexer->tokenizer_filters,filter_name);
  if (filter_strore == NULL) return 0;
  VortexStore_Destroy(filter_strore);
  return VortexStore_RemoveItem(lexer->tokenizer_filters,filter_name);
}

vortex_int VortexLexer_AddFilterToTokenizerFilter(VortexLexer lexer,
  const char* filter_name, const char* filter) {
  if (lexer->tokenizer_filters == NULL) return 0;
  VortexStore filter_strore = VortexStore_GetItem(lexer->tokenizer_filters,filter_name);
  if (filter_strore == NULL) return 0;
   return 
  VortexStore_AddItem(filter_strore,NULL,filter);
}

vortex_int VortexLexer_ApplyTokenizerFilter(VortexLexer lexer,
  const char* filter_name, const char* filter) {
  if (lexer->tokenizer_filters == NULL) return 0;
  VortexStore filter_store = VortexStore_GetItem(lexer->tokenizer_filters,filter_name);
  if (filter == NULL) return 0;
  return VortexStore_ItemExists(filter_store,filter);   
}

vortex_int VortexLexer_AddEvaluator(VortexLexer lexer,
  VortexLexerEvaluator evaluator, const char* character) {
  if (lexer->evaluator_store == NULL)
   lexer->evaluator_store = VortexStore_New();
  if (VortexStore_ItemExists(lexer->evaluator_store,character))
   return 0; 
  VortexStore_AddItem(lexer->evaluator_store,evaluator,character);
  return 1;
}

void VortexLexer_SetEvalMode(VortexLexer lexer,
  VortexLexerEvalMode eval_mode) {
  lexer->eval_mode = eval_mode;
}

VortexLexerEvalMode VortexLexer_GetEvalMode(VortexLexer lexer) {
  return lexer->eval_mode;
}

void VortexLexer_Dispatch(VortexLexer lexer) {
  vortex_int character = VortexFile_GetUTF32Character(lexer->file);
  VortexLexer_Redispatch(lexer,character);
}

void VortexLexer_Redispatch(VortexLexer lexer, vortex_int character) {
  if (character == EOF) {
   if (lexer->tokenizer != NULL)
    lexer->tokenizer(lexer);
   return;
  }
  if (character == '\n') lexer->line_number++;
  VortexLexerEvaluator evaluator = NULL;
  if (lexer->eval_mode == EvalModeRelease) {
    const vortex_int* text = &character;
    VortexString string = VortexString_NewFromUTF32(text,1);
    const char* c_string = VortexString_ConvertToCString(string);
    evaluator = VortexStore_GetItem(lexer->evaluator_store,c_string);
    free(c_string);
  }
  if (lexer->eval_mode == EvalModeHold) {
   evaluator = lexer->last_evaluator;
  }
  if (evaluator != lexer->last_evaluator)
   if (lexer->tokenizer != NULL) lexer->tokenizer(lexer);
  lexer->last_evaluator = evaluator;
  lexer->tokenizer = NULL;
  if (evaluator != NULL) evaluator(lexer,character);
  if (evaluator == NULL) VortexLexer_Dispatch(lexer);
}