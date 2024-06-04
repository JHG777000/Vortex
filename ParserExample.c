#include <stdio.h>
#include <stdlib.h>
#include <Vortex/VortexMem.h>
#include <Vortex/VortexFile.h>
#include <Vortex/VortexParser.h>
#include <Vortex/VortexGraph.h>

typedef enum {
  EOL = 0,
  Assignment,
  Identifier,
  Integer,
  Double,
  String,
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
  Dot,
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
} symbols;

typedef struct lexer_data_s {
  vortex_ulong count;
  vortex_ulong is_double;
} *lexer_data;

typedef struct graph_data_s {
  VortexStore vars;
} *graph_data;

typedef struct atom_data_s {
  vortex_int type;
  vortex_int int_val;
  vortex_double double_val;
  VortexString string_val;
} *atom_data;

typedef enum {
 RootNode = VORTEX_GRAPH_START_TYPE_ENUM(0),
 AtomNode,
 GroupAtom,
 IntegerAtom,
 DoubleAtom,
 StringAtom,
 VarAtom,
 OperatorNode,
} NodeTypes;

typedef enum {
  EvalAction = VORTEX_GRAPH_START_ACTION_ENUM(0),
  PrintRoot,
  PrintAtom,
  AddAction,
  AssignmentAction
} ActionTypes;

VORTEX_GRAPH_DEFINE_ACTION(print_root) {
  puts("ROOT NODE: print_root");
  VortexGraph_InvokeAllSubNodes(node,PrintAtom);
  puts("ROOT NODE: print_root, done.");
}

VORTEX_GRAPH_DEFINE_ACTION(destroy_graph) {
  puts("ROOT NODE: destroy_graph");
  VortexGraph_InvokeAllSubNodes(node,VORTEX_GRAPH_ACTION_DESTROY_NODE);
  puts("ROOT NODE: destroy");
  VortexGraph_SetElementIndex(node,0);
  VortexGraph_SetElement(node,NULL);
  puts("ROOT NODE: destroy_graph, done.");
}

VORTEX_GRAPH_DEFINE_ACTION(destroy_node) {
  puts("NODE: destroy_node");
  VortexGraph_InvokeAllSubNodes(node,VORTEX_GRAPH_ACTION_DESTROY_NODE);
  if (VortexGraph_GetNodeData(node) != NULL) {
      atom_data node_data = VortexGraph_GetNodeData(node);
      if (node_data->type == VORTEX_GRAPH_ANY_TYPE) {
        if (node_data->string_val != NULL) {
          VortexString_Destroy(node_data->string_val);
        }
      }
  }
  VortexGraph_DestroyNode(node);
  puts("NODE: destroy_node, done.");
}

atom_data get_atom_data(VortexGraphNode node) {
  atom_data atom = VortexGraph_GetNodeData(node);
  if (VortexGraph_GetNodeTypeId(node) == VarAtom) {
     VortexGraph_SetElementIndex(node,0);
     VortexString var_name = VortexGraph_GetElement(node);
     VortexGraph graph = VortexGraph_GetGraphFromNode(node);
     VortexGraphNode root = VortexGraph_GetRoot(graph);
     graph_data root_data = VortexGraph_GetNodeData(root);
     VortexStore vars =  root_data->vars;
     atom = VortexStore_GetItem(vars,VortexString_GetBuffer(var_name));
  }
  return atom;
}

VORTEX_GRAPH_DEFINE_ACTION(print_atom) {
  puts("NODE: print_atom");
  atom_data atom = get_atom_data(node);
  switch (atom->type) {
    case VORTEX_GRAPH_INT_TYPE:
     printf("PRINT: INT is %d\n",atom->int_val);
    break;
    case VORTEX_GRAPH_DOUBLE_TYPE:
     printf("PRINT DOUBLE is %f\n",atom->double_val);
    break;
    case VORTEX_GRAPH_ANY_TYPE:
     printf("PRINT STRING is %s\n",VortexString_GetBuffer(atom->string_val));
    break;
  }
  puts("NODE: print_atom, done.");
}

VORTEX_GRAPH_DEFINE_ACTION(eval_node) {  
  puts("NODE: eval_node");
  VortexGraph_InvokeAllSubNodes(node,action_id);
     atom_data atom = get_atom_data(node);
  if (VortexGraph_GetNodeTypeId(node) == DoubleAtom) {
     VortexGraph_SetElementIndex(node,0);
     VortexAny val_ptr = VortexGraph_GetElement(node);
     vortex_double double_val = vortex_get(vortex_double,val_ptr);
     atom_data atom = get_atom_data(node);
     atom->type = VORTEX_GRAPH_DOUBLE_TYPE;
     atom->double_val = double_val;
  }
  if (VortexGraph_GetNodeTypeId(node) == IntegerAtom) {
     VortexGraph_SetElementIndex(node,0);
     VortexAny val_ptr = VortexGraph_GetElement(node);
     vortex_int int_val = vortex_get(vortex_int,val_ptr);
     atom_data atom = get_atom_data(node);
     atom->type = VORTEX_GRAPH_INT_TYPE;
     atom->int_val = int_val;
  }
  if (VortexGraph_GetNodeTypeId(node) == StringAtom) {
     VortexGraph_SetElementIndex(node,0);
     VortexString string_val = VortexGraph_GetElement(node);
     atom_data atom = get_atom_data(node);
     atom->type = VORTEX_GRAPH_ANY_TYPE;
     atom->string_val = string_val;
  }
  if (VortexGraph_GetNodeTypeId(node) == GroupAtom) {
     VortexGraph_SetElementIndex(node,1);
     VortexGraphNode operator = VortexGraph_GetElement(node);
     if (operator == NULL) {
       puts("operator is null");
       abort();
     }
     VortexGraph_SetElementIndex(operator,0);
     VortexAny val_ptr = VortexGraph_GetElement(operator);
     if (val_ptr == NULL) {
       puts("val_ptr is null");
       abort();
     }
     vortex_int action = vortex_get(vortex_int,val_ptr);
     VortexGraph_InvokeNode(node,action);
     atom_data atom = get_atom_data(node);
  }
  puts("NODE: eval_node, done.");
}

VORTEX_GRAPH_DEFINE_ACTION(assignment_action) {
  puts("OPERATOR: assignment_action");
  atom_data atom = get_atom_data(node);
  VortexGraph_SetElementIndex(node,0);
  VortexGraphNode lhs = VortexGraph_GetElement(node);
  VortexGraph_SetElementIndex(node,2);
  VortexGraphNode rhs = VortexGraph_GetElement(node);
  if (lhs == NULL) puts("lhs NULL");
  if (rhs == NULL) puts("rhs NULL");
  atom_data rhs_atom = get_atom_data(rhs);
  if (rhs_atom == NULL) {
    puts("Type error, ATOM is null.");
    abort();
  }
  
  if (VortexGraph_GetNodeTypeId(lhs) == VarAtom) {
     VortexGraph_SetElementIndex(lhs,0);
     VortexString var_name = VortexGraph_GetElement(lhs);
     VortexGraph graph = VortexGraph_GetGraphFromNode(lhs);
     VortexGraphNode root = VortexGraph_GetRoot(graph);
     graph_data root_data = VortexGraph_GetNodeData(root);
     VortexStore vars =  root_data->vars;
     atom_data rhs_copy = VortexMem_Copy(rhs_atom,sizeof(struct atom_data_s));
     if (rhs_atom->type == VORTEX_GRAPH_ANY_TYPE) {
       rhs_copy->string_val = VortexString_Copy(rhs_atom->string_val);
     }
     VortexStore_AddItem(vars,rhs_copy,VortexString_GetBuffer(var_name));
  } else {
    puts("LHS is not an Identifier");
    abort();
  }
  
  atom->type = rhs_atom->type;
  atom->int_val = rhs_atom->int_val;
  atom->double_val = rhs_atom->double_val;
  if (rhs_atom->type == VORTEX_GRAPH_ANY_TYPE) {
       atom->string_val = VortexString_Copy(rhs_atom->string_val);
  }
  puts("OPERATOR: assignment_action, done.");
}

VORTEX_GRAPH_DEFINE_ACTION(add_action) {
  puts("OPERATOR: add_action");
  atom_data atom = get_atom_data(node);
  VortexGraph_SetElementIndex(node,0);
  VortexGraphNode lhs = VortexGraph_GetElement(node);
  VortexGraph_SetElementIndex(node,2);
  VortexGraphNode rhs = VortexGraph_GetElement(node);
  if (lhs == NULL) puts("lhs NULL");
  if (rhs == NULL) puts("rhs NULL");
  atom_data lhs_atom = get_atom_data(lhs);
  atom_data rhs_atom = get_atom_data(rhs);
  if (lhs_atom == NULL || rhs_atom == NULL) {
    puts("Type error, ATOM is null.");
    abort();
  }
  switch (lhs_atom->type) {
    case VORTEX_GRAPH_INT_TYPE:
    if (rhs_atom->type != VORTEX_GRAPH_INT_TYPE) {
      puts("Type error, RHS is not int.");
      abort();
    }
    atom->type = lhs_atom->type;
    atom->int_val = lhs_atom->int_val + rhs_atom->int_val;
    break;
    case VORTEX_GRAPH_DOUBLE_TYPE:
    if (rhs_atom->type != VORTEX_GRAPH_DOUBLE_TYPE) {
      puts("Type error, RHS is not double.");
      abort();
    }
    atom->type = lhs_atom->type;
    atom->double_val = lhs_atom->double_val + rhs_atom->double_val;
    break;
    case VORTEX_GRAPH_ANY_TYPE:
    if (rhs_atom->type != VORTEX_GRAPH_ANY_TYPE) {
      puts("Type error, RHS is not string.");
      abort();
    }
    atom->type = lhs_atom->type;
    atom->string_val = VortexString_AddStrings(lhs_atom->string_val, rhs_atom->string_val);
    break;
    defualt:
     puts("Type error, LHS is not a valid type.");
     abort();
    break;
  }
  
  puts("OPERATOR: add_action, done.");
}

VortexGraph setup_graph(void) {
  VortexGraph graph = VortexGraph_New();
  
  VortexGraph_AddType(graph,RootNode);
  VortexGraph_TypeInput(graph,RootNode,GRAPH_ARGS(GRAPH_TYPES(AtomNode)));
  graph_data data = vortex_new_mem_of_type(struct graph_data_s);
  data->vars = VortexStore_New();
  VortexGraph_SetBaseData(graph,RootNode,data,sizeof(struct graph_data_s));
  
  VortexGraph_AddType(graph,AtomNode);
  atom_data atom = vortex_new_mem_of_type(struct atom_data_s);
  atom->type = 0;
  atom->int_val = 0;
  atom->double_val = 0;
  atom->string_val = NULL;
  VortexGraph_SetBaseData(graph,AtomNode,atom,sizeof(struct atom_data_s));
  
  VortexGraph_AddType(graph,GroupAtom);
  VortexGraph_TypeExtends(graph,GroupAtom,GRAPH_ARGS(GRAPH_TYPES(AtomNode)));
  VortexGraph_TypeInput(graph,GroupAtom,GRAPH_ARGS(GRAPH_TYPES(AtomNode,OperatorNode,AtomNode)));
   
  VortexGraph_AddType(graph,IntegerAtom);
  VortexGraph_TypeExtends(graph,IntegerAtom,GRAPH_ARGS(GRAPH_TYPES(AtomNode)));
  VortexGraph_TypeInput(graph,IntegerAtom,GRAPH_ARGS(GRAPH_TYPES(VORTEX_GRAPH_INT_TYPE)));
  
  VortexGraph_AddType(graph,DoubleAtom);
  VortexGraph_TypeExtends(graph,DoubleAtom,GRAPH_ARGS(GRAPH_TYPES(AtomNode)));
  VortexGraph_TypeInput(graph,DoubleAtom,GRAPH_ARGS(GRAPH_TYPES(VORTEX_GRAPH_DOUBLE_TYPE)));
  
  VortexGraph_AddType(graph,StringAtom);
  VortexGraph_TypeExtends(graph,StringAtom,GRAPH_ARGS(GRAPH_TYPES(AtomNode)));
  VortexGraph_TypeInput(graph,StringAtom,GRAPH_ARGS(GRAPH_TYPES(VORTEX_GRAPH_ANY_TYPE)));
  
  VortexGraph_AddType(graph,VarAtom);
  VortexGraph_TypeExtends(graph,VarAtom,GRAPH_ARGS(GRAPH_TYPES(AtomNode)));
  VortexGraph_TypeInput(graph,VarAtom,GRAPH_ARGS(GRAPH_TYPES(VORTEX_GRAPH_ANY_TYPE)));
  
  VortexGraph_AddType(graph,OperatorNode);
  VortexGraph_TypeInput(graph,OperatorNode,GRAPH_ARGS(GRAPH_TYPES(VORTEX_GRAPH_INT_TYPE)));
  
  int i = RootNode;
  while (i < VarAtom) {
     VortexGraph_AddAction(graph,i,EvalAction,eval_node);
     i++;
  }
  
  VortexGraph_AddAction(graph,GroupAtom,AddAction,add_action);
  VortexGraph_AddAction(graph,GroupAtom,AssignmentAction,assignment_action);

  VortexGraph_AddAction(graph,RootNode,VORTEX_GRAPH_ACTION_DESTROY_GRAPH,destroy_graph);
  VortexGraph_AddAction(graph,RootNode,PrintRoot,print_root);
  
  i = AtomNode;
  while (i < OperatorNode) {
     VortexGraph_AddAction(graph,i,PrintAtom,print_atom);
     i++;
  }

  i = AtomNode;
  while (i <= OperatorNode) {
     VortexGraph_AddAction(graph,i,VORTEX_GRAPH_ACTION_DESTROY_NODE,destroy_node);
     i++;
  }
  return graph;
}

VortexGraph setup_root(void) {
  static VortexGraph graph = NULL;
  if (graph != NULL) return graph;
  graph = setup_graph();
  VortexGraphNode root = VortexGraph_NewNode(graph,RootNode,GRAPH_ARGS(GRAPH_NODES(NULL)));
  VortexGraph_AllocNodeData(root);
  VortexGraph_SetRoot(graph,root);
  return graph;
}

vortex_parser_processor(main) {
  printf("LINE# %d\n",line_number);
  VortexLexer_PrintTokenArray(VortexParser_GetLexer(parser));
  vortex_ulong balance = 0;
  VortexGraph graph = setup_root();
  VortexString token_string = NULL;
  VortexGraphNode node = NULL;
  puts("EOL");
  VortexArray token_array = VortrxLexer_GetTokenArray(VortexParser_GetLexer(parser));
  vortex_array_foreach(index,token_array) {
    vortex_ulong id = VortexToken_GetTokenID(VortexArray_GetItem(token_array,index));
    switch (id) {
      case LParenthesis:
      balance++;
      puts("LParenthesis");
      node = VortexGraph_NewNode(graph,GroupAtom,GRAPH_ARGS(GRAPH_NODES(NULL,NULL,NULL)));
      VortexGraph_AllocNodeDataFromSuperTypeId(node,AtomNode);
      VortexGraph_AddNodeToGraph(graph,node);
      printf("balance: %d\n",balance);
      puts("LParenthesis end");
      break;
      case RParenthesis:
       balance--;
       puts("RParenthesis");
       printf("balance: %d\n",balance);
       puts("RParenthesis end");
      break;
      case Integer:
       puts("Integer");
       token_string = VortexToken_GetTokenString(VortexArray_GetItem(token_array,index));
       vortex_int val = atoi(VortexString_GetBuffer(token_string));
       node = VortexGraph_NewNode(graph,IntegerAtom,GRAPH_ARGS(GRAPH_INTS(atoi(VortexString_GetBuffer(token_string)))));
       VortexGraph_AllocNodeDataFromSuperTypeId(node,AtomNode);
       VortexGraph_AddNodeToGraph(graph,node);
       printf("int: %d\n",val);
       puts("Integer end");
      break;
      case Double:
      puts("Double");
       token_string = VortexToken_GetTokenString(VortexArray_GetItem(token_array,index));
       node = VortexGraph_NewNode(graph,DoubleAtom,GRAPH_ARGS(GRAPH_DOUBLES(atof(VortexString_GetBuffer(token_string)))));
       VortexGraph_AllocNodeDataFromSuperTypeId(node,AtomNode);
       VortexGraph_AddNodeToGraph(graph,node);
       puts("Double end");
      break;
      case String:
       puts("String");
       token_string = VortexString_Copy(VortexToken_GetTokenString(VortexArray_GetItem(token_array,index)));
       node = VortexGraph_NewNode(graph,StringAtom,GRAPH_ARGS(GRAPH_ANY(token_string)));
       VortexGraph_AllocNodeDataFromSuperTypeId(node,AtomNode);
       VortexGraph_AddNodeToGraph(graph,node);
       puts("String end");
      break;
      case Identifier:
       puts("Identifier");
       token_string = VortexString_Copy(VortexToken_GetTokenString(VortexArray_GetItem(token_array,index)));
       node = VortexGraph_NewNode(graph,VarAtom,GRAPH_ARGS(GRAPH_ANY(token_string)));
       VortexGraph_AddNodeToGraph(graph,node);
       puts("Identifier end");
      break;
      case Addition:
       puts("Addition");
       node = VortexGraph_NewNode(graph,OperatorNode,GRAPH_ARGS(GRAPH_INTS(AddAction)));
       VortexGraph_AddNodeToGraph(graph,node);
       printf("AddAction: %d\n",AddAction);
       puts("Addition end");
      break;
      case Assignment:
       puts("Assignment");
       node = VortexGraph_NewNode(graph,OperatorNode,GRAPH_ARGS(GRAPH_INTS(AssignmentAction)));
       VortexGraph_AddNodeToGraph(graph,node);
       printf("Assignment: %d\n",AssignmentAction);
       puts("Assignment end");
      break;
      case EOL:
       puts("EOL");
       if (balance != 0) {
          puts("Parenthesis not balanced.");
          abort();
       }
       VortexGraph_InvokeNode(VortexGraph_GetRoot(graph),EvalAction);
       VortexGraph_InvokeNode(VortexGraph_GetRoot(graph),PrintRoot);
       puts("EOL end");  
      break;
    }
  }
  VortexGraph_InvokeNode(VortexGraph_GetRoot(graph),VORTEX_GRAPH_ACTION_DESTROY_GRAPH);
  return NULL;
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
  VortexToken_SetTokenID(token, Identifier);
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
      data->is_double = 1;
    }
    VortexLexer_AddCharacter(lexer,character);
    VortexLexer_SetEvalMode(lexer,EvalModeHold);
    VortexLexer_Dispatch(lexer);
    return;
  }
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
  VortexLexer_AddIDToFilter(lexer,"symbols",";",EOL);
  VortexLexer_AddIDToFilter(lexer,"symbols",".",Dot);
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
  VortexLexer_AddIDToFilter(lexer,"symbols","!",Not);
  VortexLexer_AddIDToFilter(lexer,"symbols","~",BNot);
  VortexLexer_AddIDToFilter(lexer,"symbols","@",And);
  VortexLexer_AddIDToFilter(lexer,"symbols","&",Band);
  VortexLexer_AddIDToFilter(lexer,"symbols","?",Or);
  VortexLexer_AddIDToFilter(lexer,"symbols","|",Bor);
  VortexLexer_AddIDToFilter(lexer,"symbols","^",Xor);
  VortexLexer_AddIDToFilter(lexer,"symbols",">",GreaterThan);
  VortexLexer_AddIDToFilter(lexer,"symbols","$",GreaterThanOrEqual);
  VortexLexer_AddIDToFilter(lexer,"symbols","<",LessThan);
  VortexLexer_AddIDToFilter(lexer,"symbols","µ",LessThanOrEqual);
  VortexLexer_AddIDToFilter(lexer,"symbols","=",Assignment);
  VortexLexer_SetEndOfLineCharacter(lexer,';');
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
  VortexGraph_Destroy(setup_root());
  puts("Hello");
  VortexFile_CloseFile(file);
}
