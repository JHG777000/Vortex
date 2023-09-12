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
#include <Vortex/VortexMath.h>
#include <Vortex/VortexFile.h>
#include <Vortex/VortexParser.h>

struct VortexToken_s {
  VortexString token_string;
  vortex_ulong id;
  VortexAny data;
};

struct VortexParseTree_s {
  VortexParseTreeNode root_node;
  VortexParseTreeNode active_node;
  vortex_ulong index;
  vortex_ulong num_of_nodes;
};

struct VortexParseTreeNode_s {
  VortexString parsed_string;
  VortexAny data;
  vortex_ulong id;
  VortexParseTreeNode super;
  VortexParseTreeNode left;
  VortexParseTreeNode right;
};

struct VortexLexer_s {
  VortexAny data;
  VortexFile file;
  VortexParser parser;
  vortex_int* character_array;
  vortex_ulong num_of_characters;
  VortexArray token_array;
  VortexLexerTokenizer tokenizer;
  VortexStore tokenizer_filters;
  vortex_ulong line_number;
  VortexStore evaluator_store;
  VortexLexerEvaluator last_evaluator;
  VortexLexerEvalMode eval_mode;
  vortex_int eol_character;
  vortex_int eol_lock;
};

struct VortexParser_s {
  VortexAny data;
  VortexParseTree parse_tree;
  VortexToken default_token;
  VortexArray token_array;
  vortex_ulong token_index;
  VortexArrayStore dispatch_array;
  VortexLexer lexer;
  VortexParserProcessor main_processor;
};

VortexParser VortexParser_New(VortexLexer lexer) {
  VortexParser parser = vortex_new_mem_of_type(struct VortexParser_s);
  parser->parse_tree = VortexParseTree_New();
  parser->default_token = NULL;
  parser->token_array = lexer->token_array;
  parser->token_index = 0;
  parser->dispatch_array = VortexArrayStore_New(VortexMath_GetInt64MaxVal());
  parser->lexer = lexer;
  parser->main_processor = NULL;
  return parser;
}

void VortexParser_Destroy(VortexParser parser) {
  VortexParseTree_Destroy(parser->parse_tree);
  if (parser->default_token != NULL)
   VortexToken_Destroy(parser->default_token);
  VortexArrayStore_Destroy(parser->dispatch_array); 
  free(parser);
}

VortexParseTree VortexParser_GetParseTree(VortexParser parser) {
  return parser->parse_tree;
}

void VortexParser_SetDefaultToken(VortexParser parser,
  VortexToken default_token) {
  parser->default_token = default_token;
}

void VortexParser_SetMainProcessor(VortexParser parser,
  VortexParserProcessor main_processor) {
  parser->main_processor = main_processor;
}

void VortexParser_SetTokenIndex(VortexParser parser,
  vortex_ulong token_index) {
  parser->token_index = token_index;
}

void VortexParser_AdvanceTokenIndex(VortexParser parser) {
  parser->token_index++;
}

void VortexParser_RetreatTokenIndex(VortexParser parser) {
  parser->token_index--;
}

void VortexParser_AdvanceTokenIndexByOffset(VortexParser parser,
  vortex_ulong offset) {
  parser->token_index+=offset;
}

void VortexParser_RetreatTokenIndexByOffset(VortexParser parser,
  vortex_ulong offset) {
  parser->token_index-=offset;
}

vortex_ulong VortexParser_GetTokenIndex(VortexParser parser) {
  return parser->token_index;
}

VortexToken VortexParser_GetTokenWithIndex(VortexParser parser,
  vortex_ulong index) {
  VortexToken token = 
   VortexArray_GetItem(parser->token_array,index);
  if ( token == NULL ) 
   token = parser->default_token;
  return token;      
} 

VortexToken VortexParser_GetToken(VortexParser parser) {
  return VortexParser_GetTokenWithIndex(parser,parser->token_index);
}

VortexLexer VortexParser_GetLexer(VortexParser parser) {
  return parser->lexer;
}

vortex_int VortexParser_AcceptOrExpect(VortexToken token,
  const char* symbol_name, vortex_ulong symbol_id, 
  vortex_int expect, vortex_ulong line_number) {
    if (expect) if ( symbol_id != token->id ) {
        printf("On line: %d, error: expected %s, %s is not %s.\n",line_number,
               symbol_name,VortexString_GetBuffer(token->token_string),symbol_name);
        exit(EXIT_FAILURE);
    }
    if ( symbol_id == token->id ) return 1;
    return 0;
}

void VortexParser_SetProcessorDispatch(VortexParser parser, 
  VortexParserProcessor processor, vortex_ulong dispatch_id) {
  VortexArrayStore_SetItem(parser->dispatch_array,
    processor,dispatch_id);
}

void VortexParser_DispatchProcessor(VortexParser parser, 
  vortex_ulong dispatch_id, vortex_ulong line_number) {
  VortexParserProcessor processor = 
   VortexArrayStore_GetItem(parser->dispatch_array,dispatch_id);
  processor(parser,parser->parse_tree,line_number); 
}

void VortexParser_ParseLine(VortexParser parser,
  vortex_ulong line_number) {
  parser->main_processor(parser,parser->parse_tree,line_number);
  parser->token_index = 0;
  VortexLexer_ResetLexer(parser->lexer);
}

VortexParseTree VortexParseTree_New(void) {
  VortexParseTree tree = 
   vortex_new_mem_of_type(struct VortexParseTree_s);
  tree->root_node = NULL;
  tree->active_node = NULL;
  tree->index = 0;
  tree->num_of_nodes = 0;
  return tree;
}

void VortexParseTree_Destroy(VortexParseTree tree) {
  free(tree);
}

vortex_ulong VortexParseTree_AddToTheLeft(VortexParseTree tree,
  VortexParseTreeNode node) {
  if ( tree->root_node == NULL ) {
    tree->root_node = node;
    tree->active_node = node;
    return 1;
  } 
  if ( tree->active_node->left != NULL )
   return 0;  
  tree->active_node->left = node;
  node->super = tree->active_node;
  tree->num_of_nodes++;
  return 1;
}

vortex_ulong VortexParseTree_AdvanceToTheLeft(VortexParseTree tree) {
  if ( tree->active_node == NULL )
   return 0;
  if ( tree->active_node->left == NULL )
   return 0;  
  tree->active_node = tree->active_node->left;
  return 1;
}

vortex_ulong VortexParseTree_AddToTheRight(VortexParseTree tree,
  VortexParseTreeNode node) {
  if ( tree->root_node == NULL ) {
    tree->root_node = node;
    tree->active_node = node;
    return 1;
  }   
  if ( tree->active_node->right != NULL )
   return 0;   
  tree->active_node->right = node;
  node->super = tree->active_node;
  tree->num_of_nodes++;
  return 1; 
}

vortex_ulong VortexParseTree_AdvanceToTheRight(VortexParseTree tree) {
  if ( tree->active_node == NULL )
   return 0;
  if ( tree->active_node->right == NULL )
   return 0;  
  tree->active_node = tree->active_node->right;
  return 1;
}

void VortexParseTree_SetActiveNode(VortexParseTree tree,
  VortexParseTreeNode node) {
  tree->active_node = node;
}

VortexParseTreeNode VortexParseTree_GetActiveNode(VortexParseTree tree) {
  return tree->active_node;
}

VortexParseTreeNode VortexParseTree_GetRootNode(VortexParseTree tree) {
  return tree->root_node;
}

vortex_ulong VortexParseTree_GetNumOfNodes(VortexParseTree tree) {
  return tree->num_of_nodes;
}

VortexParseTreeNode VortexParseTree_GetNodeWithIndex(
  VortexParseTree tree,
  vortex_ulong index) {
  return 
   VortexParseTreeNode_GetNodeWithIndex(tree->root_node,index);
}

VortexParseTreeNode VortexParseTree_GetNextNode(VortexParseTree tree) {
  if (tree->index == tree->num_of_nodes)
   tree->index = 0;
  tree->index++;
  if (tree->index == tree->num_of_nodes) return NULL;
  return VortexParseTree_GetNodeWithIndex(tree,tree->index); 
}

void VortexParseTree_ResetIndex(VortexParseTree tree) {
  tree->index = 0;
}

vortex_ulong VortexParseTree_GetIndex(VortexParseTree tree) {
  return tree->index;
}

VortexParseTreeNode VortexParseTreeNode_New(
  VortexTokenDataToNodeData token_data_to_node_data,
  VortexToken token) {
  VortexParseTreeNode node =
   vortex_new_mem_of_type(struct VortexParseTreeNode_s);
  node->parsed_string = VortexString_Copy(token->token_string);
  node->id = token->id;
  node->data = NULL;
  if (token_data_to_node_data != NULL)
   node->data = token_data_to_node_data(token,token->data);
  node->super = NULL; 
  node->left = NULL;
  node->right = NULL; 
  return node;
}

void VortexParseTreeNode_Destroy(VortexParseTreeNode node) {
  VortexString_Destroy(node->parsed_string);
  free(node);
}

void VortexParseTreeNode_SetData(VortexParseTreeNode node,
  VortexAny data) {
  node->data = data;  
}

VortexAny VortexParseTreeNode_GetData(VortexParseTreeNode node) {
  return node->data;  
}

VortexString VortexParseTreeNode_GetParsedString(VortexParseTreeNode node) {
  return node->parsed_string;  
}

void VortexParseTreeNode_SetID(VortexParseTreeNode node,
  vortex_ulong id) {
  node->id = id;  
}

vortex_ulong VortexParseTreeNode_GetID(VortexParseTreeNode node) {
  return node->id;  
}

void VortexParseTreeNode_SetLeftSubNode(VortexParseTreeNode node,
  VortexParseTreeNode left) {
  node->left = left;
  left->super = node;  
}

VortexParseTreeNode VortexParseTreeNode_GetLeftSubNode(VortexParseTreeNode node) {
  return node->left;  
}

void VortexParseTreeNode_SetRightSubNode(VortexParseTreeNode node,
  VortexParseTreeNode right) {
  node->right = right;
  right->super = node;  
}

VortexParseTreeNode VortexParseTreeNode_GetRightSubNode(VortexParseTreeNode node) {
  return node->right;  
}

VortexParseTreeNode VortexParseTreeNode_GetNodeWithIndex(
  VortexParseTreeNode active_node,
  vortex_ulong index) {
  VortexParseTreeNode node = NULL;   
  if (index <= 0) return active_node;  
  if (active_node->left != NULL)
   node = VortexParseTreeNode_GetNodeWithIndex(active_node->left,index-1);
  if (active_node->right != NULL && (index-1) > 0)
   node = VortexParseTreeNode_GetNodeWithIndex(active_node->right,index-1);
  return node;
}

VortexToken VortexToken_New(void) {
  VortexToken token = vortex_new_mem_of_type(struct VortexToken_s);
  token->token_string = NULL;
  token->id = 0;
  token->data = NULL;
  return token;
}

VortexToken VortexToken_NewFromCharacters(VortexLexer lexer) {
  VortexToken token = VortexToken_New();
  VortexToken_SetTokenStringFromCharacterArray(token,
    lexer->character_array,lexer->num_of_characters);
  return token;  
}

void VortexToken_Destroy(VortexToken token) {
 if (token->token_string != NULL) vortex_strfree(token->token_string);
 free(token);
}

void VortexToken_SetTokenID(VortexToken token,
  vortex_ulong token_id) {
  token->id = token_id;
}

vortex_ulong VortexToken_GetTokenID(VortexToken token) {
  return token->id;
}

void VortexToken_SetTokenData(VortexToken token,
  VortexAny token_data) {
  token->data = token_data;
}

VortexAny VortexToken_GetTokenData(VortexToken token) {
  return token->data;
}

void VortexToken_SetTokenString(VortexToken token,
  VortexString token_string) {
  token->token_string = token_string;
}

void VortexToken_SetTokenStringFromCharacterArray(VortexToken token,
  vortex_int* character_array, vortex_ulong array_size) {
  token->token_string = VortexString_NewFromUTF32(character_array,array_size);
}

VortexToken VortexToken_SetTokenStringFromCharacters(VortexLexer lexer, VortexToken token) {
  VortexToken_SetTokenStringFromCharacterArray(token,
    lexer->character_array,lexer->num_of_characters);
  return token;  
}

VortexString VortexToken_GetTokenString(VortexToken token) {
  return token->data;
}

VortexLexer VortexLexer_New(VortexFile file) {
  VortexLexer lexer = vortex_new_mem_of_type(struct VortexLexer_s);
  lexer->data = NULL;
  lexer->file = file;
  lexer->parser = NULL;
  lexer->character_array = NULL;
  lexer->num_of_characters = 0;
  lexer->token_array = NULL;
  lexer->tokenizer = NULL;
  lexer->tokenizer_filters = NULL;
  lexer->line_number = 1;
  lexer->evaluator_store = NULL;
  lexer->last_evaluator = NULL;
  lexer->eval_mode = EvalModeRelease;
  lexer->eol_character = 0;
  lexer->eol_lock = 0;
  return lexer;
}

void VortexLexer_Destroy(VortexLexer lexer) {
  VortexLexer_ResetLexer(lexer);
  if (lexer->tokenizer_filters != NULL) {
    vortex_store_foreach(tokenizer_filter,
        lexer->tokenizer_filters) {
        VortexStore tokenizer_filter_Store =
         VortexList_GetItem(tokenizer_filter);
        vortex_store_foreach(filter,
            tokenizer_filter_Store) {
          vortex_ulong* id_ptr = 
           VortexList_GetItem(filter);
          free(id_ptr);        
        }
        VortexStore_Destroy(tokenizer_filter_Store); 
    }
  }
  if (lexer->tokenizer_filters != NULL)
   VortexStore_Destroy(lexer->tokenizer_filters);
  if (lexer->evaluator_store != NULL)
    VortexStore_Destroy(lexer->evaluator_store);
  free(lexer);
}

void VortexLexer_ResetLexer(VortexLexer lexer) {
  VortexLexer_RemoveAllCharacters(lexer);
  if (lexer->token_array != NULL) {
   vortex_array_foreach(index,lexer->token_array) {
     VortexToken token = 
      VortexArray_GetItem(lexer->token_array,index);
     VortexToken_Destroy(token);
   }
    VortexArray_Destroy(lexer->token_array);
    lexer->token_array = NULL;
  }
}

void VortexLexer_SetLexerData(VortexLexer lexer, void* lexer_data) {
  lexer->data = lexer_data;
}

VortexAny VortexLexer_GetLexerData(VortexLexer lexer) {
  return lexer->data;
}

void VortexLexer_SetParser(VortexLexer lexer, VortexParser parser) {
  lexer->parser = parser;
}

vortex_ulong VortexLexer_GetLineNumber(VortexLexer lexer) {
  return lexer->line_number;
}

void VortexLexer_AddToken(VortexLexer lexer, VortexToken token) {
  if (lexer->token_array == NULL) lexer->token_array = VortexArray_New();
  VortexArray_AddItem(lexer->token_array,token);
}

void VortexLexer_PrintTokenArray(VortexLexer lexer) {
  if (lexer->token_array == NULL) {
    printf("No tokens. Token array is empty.\n"); 
    return;
  }
  vortex_array_foreach(index,lexer->token_array) {
    VortexToken token = 
     VortexArray_GetItem(lexer->token_array,index);
    printf("Token: "); 
    vortex_strprint(token->token_string);
    printf("\n"); 
  }
}

VortexArray VortrxLexer_GetTokenArray(VortexLexer lexer) {
  return lexer->token_array;
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

vortex_int VortexLexer_AddIDToFilter(VortexLexer lexer,
  const char* filter_name, const char* filter, vortex_ulong id) {
  if (lexer->tokenizer_filters == NULL) return 0;
  VortexStore filter_strore = VortexStore_GetItem(lexer->tokenizer_filters,filter_name);
  if (filter_strore == NULL) return 0;
   return 
  VortexStore_AddItem(filter_strore,vortex_any(id),filter);
}

vortex_int VortexLexer_ApplyTokenizerFilter(VortexLexer lexer,
  const char* filter_name, const char* filter) {
  if (lexer->tokenizer_filters == NULL) return 0;
  VortexStore filter_store = VortexStore_GetItem(lexer->tokenizer_filters,filter_name);
  if (filter == NULL) return 0;
  return VortexStore_ItemExists(filter_store,filter);   
}

vortex_ulong VortexLexer_GetFilterID(VortexLexer lexer,
  const char* filter_name, const char* filter) {
  if (lexer->tokenizer_filters == NULL) return 0;
  VortexStore filter_store = VortexStore_GetItem(lexer->tokenizer_filters,filter_name);
  if (filter == NULL) return 0;
  return vortex_get(vortex_ulong,VortexStore_GetItem(filter_store,filter));   
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

void VortexLexer_SetEndOfLineCharacter(VortexLexer lexer,
  vortex_int character) {
  lexer->eol_character = character;
}

vortex_int VortexLexer_GetEndOfLineCharacter(VortexLexer lexer) {
  return lexer->eol_character;
}

void VortexLexer_Dispatch(VortexLexer lexer) {
  vortex_int character = VortexFile_GetUTF32Character(lexer->file);
  VortexLexer_Redispatch(lexer,character);
}

void VortexLexer_Redispatch(VortexLexer lexer, vortex_int character) {
  if (lexer->eol_lock) {
    if (lexer->parser != NULL)
     VortexParser_ParseLine(lexer->parser,lexer->line_number);
    lexer->eol_lock = 0;
  }
  if (character == EOF) {
   if (lexer->tokenizer != NULL)
    lexer->tokenizer(lexer);
   return;
  }
  if (character == '\n') 
   lexer->line_number++;
  VortexLexerEvaluator evaluator = NULL;
  if (lexer->eval_mode == EvalModeRelease) {
    const vortex_int* text = &character;
    VortexString string = VortexString_NewFromUTF32(text,1);
    char* c_string = VortexString_ConvertToCString(string);
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
  if (lexer->eol_character == character)
   lexer->eol_lock++;
  if (evaluator != NULL) evaluator(lexer,character);
  if (evaluator == NULL) VortexLexer_Dispatch(lexer);
}