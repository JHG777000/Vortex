/*
 Copyright (c) 2024 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexGraph.c
//VortexGraph

#include <stdio.h>
#include <stdlib.h>
#include <Vortex/VortexMem.h>
#include <Vortex/VortexGraph.h>

struct VortexGraph_s {
  VortexArrayStore types;
  VortexGraphNode root;
};

struct VortexGraphType_s {
  vortex_int id;
  VortexArray element_types; //ptr
  VortexAny base_data;
  vortex_ulong base_data_size;
  VortexArrayStore actions;
  VortexArray extends;//ptrs
};

struct VortexGraphAction_s {
  vortex_int id;
  VorextGraphCallBack callback;
};

struct VortexGraphNode_s {
  VortexDataStructureFlag flag;
  VortexGraph graph;   
  VortexGraphType type;
  VortexAny data;
  VortexArray elements;
  vortex_ulong element_index;
  vortex_ulong node_index;
  vortex_ulong num_of_nodes;
};

VortexGraph VortexGraph_New(void) {
    VortexGraph graph = vortex_new_mem_of_type(struct VortexGraph_s);
    graph->types = VortexArrayStore_New(VORTEX_GRAPH_VAS_SIZE);
    graph->root = NULL;
    return graph;
}
static void Vortex_DestroyGraphType(VortexGraphType type);

void VortexGraph_Destroy(VortexGraph graph) { 
    VortexGraph_InvokeNode(graph->root,VORTEX_GRAPH_ACTION_DESTROY_GRAPH);
    VortexGraph_DestroyNode(graph->root);
    vortex_array_store_foreach(node,graph->types) {
        Vortex_DestroyGraphType(VortexList_GetItem(node));
    }
    VortexArrayStore_Destroy(graph->types);
    free(graph);
}

void VortexGraph_SetRoot(VortexGraph graph, VortexGraphNode node) {
    graph->root = node;
}

VortexGraphNode VortexGraph_GetRoot(VortexGraph graph) {
    return graph->root;
}

VortexGraphNode VortexGraph_NewNode(VortexGraph graph, vortex_ulong type_id, VortexArgs elements) {
    VortexGraphNode node = VortexGraph_NewNodeNoElements(graph,type_id);
    VortexGraph_NodeAddElements(node,elements);
    return node;
}

VortexGraphNode VortexGraph_NewNodeNoElements(VortexGraph graph, vortex_ulong type_id) {
    VortexGraphType type = VortexArrayStore_GetItem(graph->types,type_id);
    if (type == NULL) return NULL;
    VortexGraphNode node = vortex_new_mem_of_type(struct VortexGraphNode_s);
    node->flag = VortexGraphNodeFlag;
    node->graph = graph;
    node->type = type;
    node->data = NULL;
    node->elements = VortexArray_New();
    node->element_index = 0;
    node->node_index = 0;
    node->num_of_nodes = 0;
    return node;
}

void VortexGraph_DestroyNode(VortexGraphNode node) {
    VortexArray_Destroy(node->elements);
    free(node->data);
    free(node);
}

VortexGraph VortexGraph_GetGraphFromNode(VortexGraphNode node) {
    return node->graph;
}

vortex_ulong VortexGraph_GetNodeTypeId(VortexGraphNode node) {
    return node->type->id;
}

void VortexGraph_AllocNodeData(VortexGraphNode node) {
    node->data = 
     VortexMem_Copy(node->type->base_data,node->type->base_data_size);
}

void VortexGraph_AllocNodeDataFromSuperTypeId(VortexGraphNode node, vortex_int id) {
    VortexGraphType super = VortexGraph_GetSuperWithTypeId(node,id);
    node->data = 
     VortexMem_Copy(super->base_data,super->base_data_size);
}

VortexAny VortexGraph_GetNodeData(VortexGraphNode node) {
    return node->data;
}

void VortexGraph_NodeAddElements(VortexGraphNode node, VortexArgs elements) {
    VortexArgs_UseArgs(elements);
    vortex_ulong index = 0;
    vortex_int element_type = 0;
    vortex_int* element_type_ptr = NULL;
    vortex_ulong num_of_elements = VortexArray_GetNumofItems(node->type->element_types);
   add_element_to_node:
    element_type_ptr = VortexArray_GetItem(node->type->element_types,index);
    if (element_type_ptr == NULL) return;
    element_type = vortex_get(vortex_int,element_type_ptr);
    #define type_check(type,type_id,type_string)\
        case type_id:\
            if (CanGetNextArg(elements,type)) {\
                type type##_var = GetNextArg(elements,type);\
                VortexArray_AddItem(node->elements,vortex_any(type##_var));\
            } else {\
                printf("VORTEX GRAPH: Type error when adding element to node. Expected %s\n",type_string);\
                abort();\
            }\
        break;
       
    switch (element_type) {
        type_check(vortex_byte,VORTEX_GRAPH_BYTE_TYPE,"vortex_byte.")
        type_check(vortex_sbyte,VORTEX_GRAPH_SBYTE_TYPE,"vortex_sbyte.")
        type_check(vortex_short,VORTEX_GRAPH_SHORT_TYPE,"vortex_short.")
        type_check(vortex_ushort,VORTEX_GRAPH_USHORT_TYPE,"vortex_ushort.")
        type_check(vortex_int,VORTEX_GRAPH_INT_TYPE,"vortex_int.")
        type_check(vortex_uint,VORTEX_GRAPH_UINT_TYPE,"vortex_uint.")
        type_check(vortex_long,VORTEX_GRAPH_LONG_TYPE,"vortex_long.")
        type_check(vortex_ulong,VORTEX_GRAPH_ULONG_TYPE,"vortex_ulong.")
        type_check(vortex_float,VORTEX_GRAPH_FLOAT_TYPE,"vortex_float.")
        type_check(vortex_double,VORTEX_GRAPH_DOUBLE_TYPE,"vortex_double.")
        case VORTEX_GRAPH_ANY_TYPE:
            if (CanGetNextArg(elements,VortexAny)) {
                VortexAny var = GetNextArg(elements,VortexAny);
                VortexArray_AddItem(node->elements,var);
            } else {
                printf("VORTEX GRAPH: Type error when adding element to node. Expected VortexAny.\n");
                abort();
            }
        break;
        default:
           if (element_type >= 100) {
               if (CanGetNextArg(elements,VortexAny)) {
                VortexAny var = GetNextArg(elements,VortexAny);
                VortexArray_AddItem(node->elements,var);
                if (var != NULL) node->num_of_nodes++;
            } else {
                printf("VORTEX GRAPH: Type error when adding element to node. Expected Node.\n");
                abort();
            }
           }
         break;
    }
    num_of_elements--;
    index++;
    if (num_of_elements > 0) goto add_element_to_node;
}

vortex_int VortexGraph_AddNodeToGraph(VortexGraph graph, VortexGraphNode node_to_add) {
    return VortexGraph_AddNodeToNode(graph->root,node_to_add);
}

static vortex_int Vortex_IsNodeOfNodeType(VortexGraphNode node, vortex_int type_id) {
    if (node->type->id == type_id) {
        return 1;
    }
    vortex_array_foreach(index,node->type->extends) {
        VortexGraphType type = VortexArray_GetItem(node->type->extends,index);
        if (type->id == type_id) {
         return 1;
        }
    }
    return 0;
}

vortex_int VortexGraph_AddNodeToNode(VortexGraphNode node, VortexGraphNode node_to_add) {
    vortex_int state = 0;
    vortex_int element_type = 0;
    vortex_int* element_type_ptr = NULL;
    vortex_array_foreach(index,node->type->element_types) {
        element_type_ptr = VortexArray_GetItem(node->type->element_types,index);
        if (element_type_ptr == NULL) return 0;
        element_type = vortex_get(vortex_int,element_type_ptr);
        if (Vortex_IsNodeOfNodeType(node_to_add,element_type) && 
            VortexArray_GetItem(node->elements,index) == NULL) {   
            VortexArray_SetItem(node->elements,index,node_to_add);
            node->num_of_nodes++;
            return 1;
        }
    }
    vortex_array_foreach(index,node->type->element_types) {
        element_type_ptr = VortexArray_GetItem(node->type->element_types,index);
        if (element_type_ptr == NULL) return 0;
        element_type = vortex_get(vortex_ulong,element_type_ptr);
        if (element_type >= 100 &&
             VortexArray_GetItem(node->elements,index) != NULL) 
             state = VortexGraph_AddNodeToNode(VortexArray_GetItem(node->elements,index),node_to_add);   
        if (state) return 1;  
    }
    return 0;
}

static VortexGraphType Vortex_NewGraphType(vortex_int id) {
    VortexGraphType type = vortex_new_mem_of_type(struct VortexGraphType_s);
    type->id = id;
    type->element_types = VortexArray_New();
    type->base_data = NULL;
    type->base_data_size = 0;
    type->actions = VortexArrayStore_New(VORTEX_GRAPH_VAS_SIZE);
    type->extends = VortexArray_New();
    return type;
}

static void Vortex_DestroyGraphAction(VortexGraphAction action);

static void Vortex_DestroyGraphType(VortexGraphType type) {
    vortex_array_foreach(index,type->element_types) {
        free(VortexArray_GetItem(type->element_types,index));
    }
    VortexArray_Destroy(type->element_types);
    vortex_array_store_foreach(node,type->actions) {
        Vortex_DestroyGraphAction(VortexList_GetItem(node));
    }
    VortexArrayStore_Destroy(type->actions);
    VortexArray_Destroy(type->extends);
    free(type->base_data);
    free(type);
}

void VortexGraph_AddType(VortexGraph graph, vortex_int id) {
    VortexGraphType type = VortexArrayStore_GetItem(graph->types,id);
    if (type != NULL) return;
    type = Vortex_NewGraphType(id);
    VortexArrayStore_SetItem(graph->types,type,id);
}

void VortexGraph_TypeInput(VortexGraph graph, vortex_int id, VortexArgs elements) {
    VortexArgs_UseArgs(elements);
    VortexGraphType type = VortexArrayStore_GetItem(graph->types,id);
    if (type == NULL) return;
    while (CanGetNextArg(elements,vortex_int)) {
        vortex_int id = GetNextArg(elements,vortex_int);
        VortexArray_AddItem(type->element_types,vortex_any(id));
    }
}

void VortexGraph_TypeExtends(VortexGraph graph, vortex_int id, VortexArgs extends) {
    VortexArgs_UseArgs(extends);
    VortexGraphType type = VortexArrayStore_GetItem(graph->types,id);
    if (type == NULL) return;
    while (CanGetNextArg(extends,vortex_int)) {
        vortex_int id = GetNextArg(extends,vortex_int);
        VortexArray_AddItem(type->extends,VortexArrayStore_GetItem(graph->types,id));
    }
}

void VortexGraph_SetBaseData(VortexGraph graph, vortex_int id, 
    VortexAny base_data, vortex_ulong base_data_size) {
    VortexGraphType type = VortexArrayStore_GetItem(graph->types,id);
    if (type == NULL) return;
    type->base_data = base_data;
    type->base_data_size = base_data_size;
}

VortexAny VortexGraph_GetBaseData(VortexGraph graph, vortex_int id, 
    vortex_ulong* base_data_size_ptr) {
    VortexGraphType type = VortexArrayStore_GetItem(graph->types,id);
    if (type == NULL) return NULL;
    *base_data_size_ptr = type->base_data_size;
    return type->base_data;
}

static VortexGraphAction Vortex_NewGraphAction(vortex_int id, VorextGraphCallBack callback) {
    VortexGraphAction action = vortex_new_mem_of_type(struct VortexGraphAction_s);
    action->id = id;
    action->callback = callback;
    return action;
}

static void Vortex_DestroyGraphAction(VortexGraphAction action) {   
    free(action);
}

void VortexGraph_AddAction(VortexGraph graph, vortex_int type_id, 
      vortex_int action_id,
      VorextGraphCallBack callback) {
    VortexGraphType type = VortexArrayStore_GetItem(graph->types,type_id);
    if (type == NULL) return;
    VortexGraphAction action = VortexArrayStore_GetItem(type->actions,action_id);
    if (action != NULL) return;
    action = Vortex_NewGraphAction(action_id,callback);   
    VortexArrayStore_SetItem(type->actions,action,action_id);
}

void VortexGraph_ApplyAction(VortexGraph graph, vortex_int action_id) {
    if (graph->root == NULL) return;
    VortexGraph_InvokeNode(graph->root,action_id);
}

void VortexGraph_InvokeAction(VortexGraphNode node, VortexGraphAction action) {
    if (action->callback != NULL) action->callback(node,action->id);
}

void VortexGraph_InvokeSuper(VortexGraphNode node, vortex_int action_id) {
    VortexGraph_InvokeSuperWithIndex(node,0,action_id);
}

void VortexGraph_InvokeSuperWithIndex(VortexGraphNode node, vortex_ulong index, vortex_int action_id) {
    VortexGraphType super = VortexGraph_GetSuper(node,index);
    VortexGraphAction action = VortexGraph_GetActionfromSuper(super,action_id);
    VortexGraph_InvokeAction(node,action);
}

void VortexGraph_InvokeAllSupers(VortexGraphNode node, vortex_int action_id) {
    vortex_array_foreach(index,node->type->extends) {
        VortexGraph_InvokeSuperWithIndex(node,index,action_id);  
    }
}

void VortexGraph_InvokeNode(VortexGraphNode node, vortex_int action_id) {
    VortexGraphAction action = VortexGraph_GetActionfromNode(node,action_id);
    if (action == NULL) return;
    VortexGraph_InvokeAction(node,action);
}

void VortexGraph_InvokeNextNode(VortexGraphNode node, vortex_int action_id) {
    VortexGraphNode next_node = VortexGraph_GetNextNode(node);
    VortexGraph_InvokeNode(next_node,action_id);
}

void VortexGraph_InvokeAllSubNodes(VortexGraphNode node, vortex_int action_id) {
    VortexGraphNode next_node = NULL;
    vortex_ulong num_of_nodes = node->num_of_nodes;
    VortexGraph_SetNodeIndex(node,0);
  next_node:
    while ( (next_node = VortexGraph_GetNode(node)) ) {
        if (num_of_nodes <= 0) break;
        VortexGraph_InvokeNode(next_node,action_id);
        VortexGraph_AdvanceNodeIndex(node);
        num_of_nodes--;   
    }
    if (num_of_nodes > 0) {
        VortexGraph_AdvanceNodeIndex(node);
        num_of_nodes--;
        goto next_node;
    }
    VortexGraph_SetNodeIndex(node,0);
}

VortexGraphAction VortexGraph_GetActionfromNode(VortexGraphNode node, vortex_int action_id) {
    return VortexArrayStore_GetItem(node->type->actions,action_id);
}

VortexGraphAction VortexGraph_GetActionfromSuper(VortexGraphType super, vortex_int action_id) {
    return VortexArrayStore_GetItem(super->actions,action_id);
}

VortexGraphType VortexGraph_GetSuper(VortexGraphNode node, vortex_ulong index) {
    VortexGraphType type = node->type;
    return VortexArray_GetItem(type->extends,index);
}

VortexGraphType VortexGraph_GetSuperWithTypeId(VortexGraphNode node, vortex_int id) {
    vortex_array_foreach(index,node->type->extends) {
        VortexGraphType super = VortexArray_GetItem(node->type->extends,index);
        if (super->id == id) return super;
    }
    return NULL;
}

VortexAny VortexGraph_GetElement(VortexGraphNode node) {
    return VortexArray_GetItem(node->elements,node->element_index);
}

VortexAny VortexGraph_GetNextElement(VortexGraphNode node) {
    VortexGraph_AdvanceElementIndex(node);
    return VortexGraph_GetElement(node);
}
VortexAny VortexGraph_GetPreviousElement(VortexGraphNode node) {
   VortexGraph_RetreatElementIndex(node);
   return VortexGraph_GetElement(node); 
}

void VortexGraph_SetElement(VortexGraphNode node, VortexAny element) {
    VortexArray_SetItem(node->elements,node->element_index,element);
}

VortexGraphNode VortexGraph_GetNode(VortexGraphNode node) {
    vortex_ulong element_type = 0;
    vortex_ulong* element_type_ptr = NULL;
    vortex_ulong index = node->node_index;
  get_element_type: 
    element_type_ptr = VortexArray_GetItem(node->type->element_types,index);
    if (element_type_ptr == NULL) return NULL;
    element_type = vortex_get(vortex_ulong,element_type_ptr);
    if (element_type < 100) goto get_next_element_type; //make sure node
    node->node_index = index;
    return VortexArray_GetItem(node->elements,index);
  get_next_element_type:
    index++;
    goto get_element_type;
}

VortexGraphNode VortexGraph_GetNextNode(VortexGraphNode node) {
    VortexGraph_AdvanceNodeIndex(node);
    return VortexGraph_GetNode(node);
}

VortexGraphNode VortexGraph_GetPreviousNode(VortexGraphNode node) {
    VortexGraph_RetreatNodeIndex(node);
    return VortexGraph_GetNode(node);
}

void VortexGraph_AdvanceNodeIndex(VortexGraphNode node) {
    node->node_index++;
}

void VortexGraph_RetreatNodeIndex(VortexGraphNode node) {
    node->node_index--;
}

void VortexGraph_SetNodeIndex(VortexGraphNode node, vortex_ulong index) {
    node->node_index = index;
}

vortex_ulong VortexGraph_GetNodeIndex(VortexGraphNode node) {
    return node->node_index;
}

void VortexGraph_AdvanceElementIndex(VortexGraphNode node) {
    node->element_index++;
}

void VortexGraph_RetreatElementIndex(VortexGraphNode node) {
    node->element_index--;
}

void VortexGraph_SetElementIndex(VortexGraphNode node, vortex_ulong index) {
    node->element_index = index;
}

vortex_ulong VortexGraph_GetElementIndex(VortexGraphNode node) {
    return node->element_index;
}