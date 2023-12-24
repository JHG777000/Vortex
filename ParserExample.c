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

/*
vortex_parser_processor(value);

vortex_parser_processor(identifier) {
  vortex_parser_expect(Identifier);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,vortex_parser_get_token);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_advance;
  return NULL;
}

vortex_parser_processor(literal) {
  vortex_int is_literal = vortex_parser_accept(Integer) 
  || vortex_parser_accept(Double) 
  || vortex_parser_accept(String);
  if (!is_literal) 
   vortex_parser_expect(Literal);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,vortex_parser_get_token);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_advance;
  return NULL;
}

vortex_parser_processor(unary_operator) {
   vortex_int is_unary_operator = vortex_parser_accept(Print) 
  || vortex_parser_accept(Not)
  || vortex_parser_accept(BNot);
  if (!is_unary_operator) 
   vortex_parser_expect(UnaryOperator);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,vortex_parser_get_token);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_advance;
  vortex_parser_process(value);
  return NULL;
}

vortex_parser_processor(call) {
  vortex_parser_expect(LParenthesis);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,NULL);
  VortexParseTreeNode_SetID(node,Call);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_advance;
  end:
  if (vortex_parser_accept(RParenthesis)) {
    vortex_parser_advance;
    return NULL;   
  }
  arg:
  vortex_parser_process(value);
  if (vortex_parser_accept(RParenthesis))
   goto end;
  if (vortex_parser_accept(Comma))
   goto arg; 
  return NULL;
}

vortex_parser_processor(index) {
  vortex_parser_expect(LSquareBracket);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,NULL);
  VortexParseTreeNode_SetID(node,Index);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_advance;
  end:
  if (vortex_parser_accept(RSquareBracket)) {
    vortex_parser_advance;
    return NULL;   
  }
  vortex_parser_process(value);
  if (vortex_parser_accept(RSquareBracket))
   goto end;
  vortex_parser_expect(RSquareBracket); 
  return NULL;  
}

vortex_parser_processor(increment) {
  vortex_parser_expect(Addition);
  vortex_parser_advance;
  vortex_parser_expect(Addition);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,NULL);
  VortexParseTreeNode_SetID(node,Increment);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_advance; 
  return NULL;   
}

vortex_parser_processor(decrement) {
  vortex_parser_expect(Subtraction);
  vortex_parser_advance;
  vortex_parser_expect(Subtraction);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,NULL);
  VortexParseTreeNode_SetID(node,Decrement);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_advance;
  return NULL;    
}

vortex_parser_processor(post_operator) {
   vortex_int is_post_operator = vortex_parser_accept(Addition) 
  || vortex_parser_accept(Subtraction)
  || vortex_parser_accept(LParenthesis)
  || vortex_parser_accept(LSquareBracket);
  if (!is_post_operator) 
   vortex_parser_expect(PostOperator);
  if (vortex_parser_accept(LParenthesis))
   return vortex_parser_process(call);
  if (vortex_parser_accept(LSquareBracket))
   return vortex_parser_process(index);
  if (vortex_parser_accept(Addition))
   return vortex_parser_process(increment);
  if (vortex_parser_accept(Subtraction))
   return vortex_parser_process(decrement);  
  return NULL;
}

vortex_parser_processor(binary_operator) {
   vortex_int is_binary_operator = vortex_parser_accept(Addition) 
  || vortex_parser_accept(Subtraction) 
  || vortex_parser_accept(Multiplication)
  || vortex_parser_accept(Division)
  || vortex_parser_accept(Modulus)
  || vortex_parser_accept(And)
  || vortex_parser_accept(Band)
  || vortex_parser_accept(Or)
  || vortex_parser_accept(Bor)
  || vortex_parser_accept(Xor)
  || vortex_parser_accept(GreaterThan)
  || vortex_parser_accept(GreaterThanOrEqual)
  || vortex_parser_accept(LessThan)
  || vortex_parser_accept(LessThanOrEqual)
  || vortex_parser_accept(Equal);
  if (!is_binary_operator) 
   vortex_parser_expect(BinaryOperator);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,vortex_parser_get_token);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  //fix parse tree
  VortexParseTreeNode_Swap(node,VortexParseTreeNode_GetSuper(node));
  vortex_parser_advance;
  return NULL;
}

vortex_parser_processor(assignment) {
  vortex_parser_expect(Identifier);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,vortex_parser_get_token);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  node = 
   VortexParseTreeNode_New(NULL,NULL);
  VortexParseTreeNode_SetID(node,Assignment);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  //fix parse tree
  puts("C");
  //VortexParseTreeNode_Swap(node,VortexParseTreeNode_GetSuper(node));
  vortex_parser_advance; 
}

vortex_parser_processor(post_operation) {
  if (vortex_parser_accept(Identifier))
   return 
    vortex_parser_process(assignment);
  return 
   vortex_parser_process(post_operator);
}

vortex_parser_processor(collection);

vortex_parser_processor(arguments) {
  VortexToken token = VortexToken_New();
  VortexToken_SetTokenID(token,Arguments);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,token);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_expect(LParenthesis);
   end:
  if (vortex_parser_accept(RParenthesis)) {
    vortex_parser_advance;
    return NULL;   
  }
  arg:
  vortex_parser_process(identifier);
  if (vortex_parser_accept(RParenthesis))
   goto end;
  if (vortex_parser_accept(Comma))
   goto arg; 
}    

vortex_parser_processor(lambda) {
  vortex_int is_lambda =
   Identifier == VortexToken_GetTokenID(vortex_parser_peek(1))
   && RParenthesis == VortexToken_GetTokenID(vortex_parser_peek(2))
   && LBracket == VortexToken_GetTokenID(vortex_parser_peek(3));
  if (!is_lambda)
   vortex_parser_expect(Lambda);
  VortexToken token = VortexToken_New();
  VortexToken_SetTokenID(token,Lambda);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,token);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_process(arguments);
  vortex_parser_process(collection);
  return NULL;
}

vortex_parser_processor(group) {
  vortex_parser_expect(LParenthesis);
  vortex_int is_lambda =
   Identifier == VortexToken_GetTokenID(vortex_parser_peek(1))
   && RParenthesis == VortexToken_GetTokenID(vortex_parser_peek(2))
   && LBracket == VortexToken_GetTokenID(vortex_parser_peek(3));
  if (is_lambda)
   return vortex_parser_process(lambda);
  VortexToken token = VortexToken_New();
  VortexToken_SetTokenID(token,Group);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,token);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_advance;
  vortex_int is_unary_operator = vortex_parser_accept(Print) 
  || vortex_parser_accept(Not)
  || vortex_parser_accept(BNot);
  if (is_unary_operator)
   return vortex_parser_process(unary_operator);
  vortex_parser_process(value);
  vortex_int is_post_operator = vortex_parser_accept(Addition) 
  || vortex_parser_accept(Subtraction)
  || vortex_parser_accept(LParenthesis)
  || vortex_parser_accept(LSquareBracket);
  if (is_post_operator)
   return vortex_parser_process(post_operator);
  vortex_parser_process(binary_operator);
  vortex_parser_process(value);
  return NULL;
}

vortex_parser_processor(collection_value) {
  if (vortex_parser_accept(RBracket)) 
   return NULL; 
  vortex_parser_process(value);
   vortex_int is_post_operator = vortex_parser_accept(Subtraction) 
  || vortex_parser_accept(Subtraction)
  || vortex_parser_accept(LParenthesis)
  || vortex_parser_accept(LSquareBracket);
  if (is_post_operator)
   vortex_parser_process(post_operator);
  if (vortex_parser_accept(Comma)
      || vortex_parser_accept(EOL)) {
    vortex_parser_advance;
    return 
     vortex_parser_process(collection_value);
  } 
  return NULL;
}

vortex_parser_processor(collection) {
  vortex_parser_expect(LBracket);
  VortexToken token = VortexToken_New();
  VortexToken_SetTokenID(token,Collection);
  VortexParseTreeNode node = 
   VortexParseTreeNode_New(NULL,token);
  VortexParseTree_Add(parse_tree,node);
  VortexParseTree_Advance(parse_tree);
  vortex_parser_advance;  
  vortex_parser_process(collection_value);
  vortex_parser_expect(RBracket);   
}    

vortex_parser_processor(value) {
  if (vortex_parser_accept(Integer))
   return vortex_parser_process(literal);
  if (vortex_parser_accept(Double))
   return vortex_parser_process(literal);
  if (vortex_parser_accept(String))
   return vortex_parser_process(literal);
  if (vortex_parser_accept(LParenthesis))
   return vortex_parser_process(group);
  if (vortex_parser_accept(Identifier))
   return vortex_parser_process(identifier);
  if (vortex_parser_accept(LBracket))
   return vortex_parser_process(collection);  
  vortex_parser_expect(Value); 
  return NULL;
}

vortex_parser_processor(if_operation) {
  vortex_parser_expect(If);
  vortex_parser_advance;
  vortex_parser_expect(LParenthesis);
  vortex_parser_process(value);
  vortex_parser_expect(RParenthesis);
  vortex_parser_advance;
  if (vortex_parser_accept(LBracket)) {
    vortex_parser_process(collection);
    vortex_parser_expect(EOL);
  }
  return NULL;
}

vortex_parser_processor(unary_operation) {
  puts("UnaryOperation!");
  vortex_ulong symbol = 
   VortexToken_GetTokenID(vortex_parser_peek(0));
  if (symbol == If) 
   return vortex_parser_process(if_operation);
  if (symbol == Print 
      || symbol == Not 
      || symbol == BNot)
    return vortex_parser_process(unary_operator);
  vortex_int is_post_operator = vortex_parser_accept(Addition) 
  || vortex_parser_accept(Subtraction)
  || vortex_parser_accept(LParenthesis)
  || vortex_parser_accept(LSquareBracket)
  || vortex_parser_accept(Identifier);
  if (is_post_operator)
   return vortex_parser_process(post_operation);
  puts("B"); 
  return NULL;
}
*/


/*
 Statement( :(value,900), Statement( :(text,"Hello World")))
*/
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
  //VortexParseTree_AddToTheRight(parse_tree,node); 
  //VortexParser_DispatchProcessor(parser,
    //VortexToken_GetTokenID(vortex_parser_peek(0)),line_number);
  //VortexParseTree_AdvanceToTheRight(parse_tree);
  //VortexParseTree_PrintWithIterator(parse_tree);
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
