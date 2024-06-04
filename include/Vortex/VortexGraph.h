/*
 Copyright (c) 2024 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexGraph.h
//Header file for VortexGraph.

#ifndef Vortex_VortexGraph_h
#define Vortex_VortexGraph_h

#include <Vortex/VortexArgs.h>

#define VORTEX_GRAPH_START_TYPE_ENUM(offset) 100 + offset
#define VORTEX_GRAPH_START_ACTION_ENUM(offset) 100 + offset

#define VORTEX_GRAPH_VAS_SIZE 100000

#define VORTEX_GRAPH_EMPTY_TYPE 0
#define VORTEX_GRAPH_BYTE_TYPE 1
#define VORTEX_GRAPH_SBYTE_TYPE 2
#define VORTEX_GRAPH_SHORT_TYPE 3
#define VORTEX_GRAPH_USHORT_TYPE 4
#define VORTEX_GRAPH_INT_TYPE 5
#define VORTEX_GRAPH_UINT_TYPE 6
#define VORTEX_GRAPH_LONG_TYPE 7
#define VORTEX_GRAPH_ULONG_TYPE 8
#define VORTEX_GRAPH_FLOAT_TYPE 9
#define VORTEX_GRAPH_DOUBLE_TYPE 10
#define VORTEX_GRAPH_ANY_TYPE 11

#define GRAPH_ARGS(...) NewArgs(__VA_ARGS__)
#define GRAPH_TYPES(...) Ints(__VA_ARGS__)
#define GRAPH_INTS(...) Ints(__VA_ARGS__)
#define GRAPH_DOUBLES(...) Doubles(__VA_ARGS__)
#define GRAPH_ANY(...) VortexArgs_NewArgSet(VortexAny,__VA_ARGS__)
#define GRAPH_NODES(...) GRAPH_ANY(__VA_ARGS__)

#define VORTEX_GRAPH_ACTION_DESTROY_GRAPH 0
#define VORTEX_GRAPH_ACTION_DESTROY_NODE 1

#define VORTEX_GRAPH_DEFINE_ACTION(name)\
void name(VortexGraphNode node, vortex_int action_id)

typedef struct VortexGraph_s* VortexGraph;
typedef struct VortexGraphType_s* VortexGraphType;
typedef struct VortexGraphAction_s* VortexGraphAction;
typedef struct VortexGraphNode_s* VortexGraphNode;
typedef void (*VorextGraphCallBack)(VortexGraphNode node, vortex_int action_id);

VortexGraph VortexGraph_New(void);
void VortexGraph_Destroy(VortexGraph graph);
void VortexGraph_SetRoot(VortexGraph graph, VortexGraphNode node);
VortexGraphNode VortexGraph_GetRoot(VortexGraph graph);
VortexGraphNode VortexGraph_NewNode(VortexGraph graph, vortex_ulong type_id, VortexArgs elements);
VortexGraphNode VortexGraph_NewNodeNoElements(VortexGraph graph, vortex_ulong type_id);
void VortexGraph_DestroyNode(VortexGraphNode node);
VortexGraph VortexGraph_GetGraphFromNode(VortexGraphNode node);
vortex_ulong VortexGraph_GetNodeTypeId(VortexGraphNode node);
void VortexGraph_AllocNodeData(VortexGraphNode node);
void VortexGraph_AllocNodeDataFromSuperTypeId(VortexGraphNode node, vortex_int id);
VortexAny VortexGraph_GetNodeData(VortexGraphNode node);
void VortexGraph_NodeAddElements(VortexGraphNode node, VortexArgs elements);
vortex_int VortexGraph_AddNodeToGraph(VortexGraph graph, VortexGraphNode node_to_add);
vortex_int VortexGraph_AddNodeToNode(VortexGraphNode node, VortexGraphNode node_to_add);
void VortexGraph_AddType(VortexGraph graph, vortex_int id);
void VortexGraph_TypeInput(VortexGraph graph, vortex_int id, VortexArgs elements);
void VortexGraph_TypeExtends(VortexGraph graph, vortex_int id, VortexArgs extends);
void VortexGraph_SetBaseData(VortexGraph graph, vortex_int id, 
    VortexAny base_data, vortex_ulong base_data_size);
VortexAny VortexGraph_GetBaseData(VortexGraph graph, vortex_int id, 
    vortex_ulong* base_data_size_ptr);    
void VortexGraph_AddAction(VortexGraph graph, vortex_int type_id, vortex_int action_id,VorextGraphCallBack callback);
void VortexGraph_ApplyAction(VortexGraph graph, vortex_int action_id);
void VortexGraph_InvokeAction(VortexGraphNode node, VortexGraphAction action);
void VortexGraph_InvokeSuper(VortexGraphNode node, vortex_int action_id);
void VortexGraph_InvokeSuperWithIndex(VortexGraphNode node, vortex_ulong index, vortex_int action_id);
VortexGraphAction VortexGraph_GetActionfromSuper(VortexGraphType super, vortex_int action_id);
void VortexGraph_InvokeAllSupers(VortexGraphNode node, vortex_int action_id);
void VortexGraph_InvokeNode(VortexGraphNode node, vortex_int action_id);
void VortexGraph_InvokeNextNode(VortexGraphNode node, vortex_int action_id);
void VortexGraph_InvokeAllSubNodes(VortexGraphNode node, vortex_int action_id);
VortexGraphAction VortexGraph_GetActionfromNode(VortexGraphNode node, vortex_int action_id);
VortexGraphAction VortexGraph_GetActionfromSuper(VortexGraphType super, vortex_int action_id);
VortexGraphType VortexGraph_GetSuper(VortexGraphNode node, vortex_ulong index);
VortexGraphType VortexGraph_GetSuperWithTypeId(VortexGraphNode node, vortex_int id);
VortexAny VortexGraph_GetElement(VortexGraphNode node);
VortexAny VortexGraph_GetNextElement(VortexGraphNode node);
VortexAny VortexGraph_GetPreviousElement(VortexGraphNode node);
void VortexGraph_SetElement(VortexGraphNode node, VortexAny element);
VortexGraphNode VortexGraph_GetNode(VortexGraphNode node);
VortexGraphNode VortexGraph_GetNextNode(VortexGraphNode node);
VortexGraphNode VortexGraph_GetPreviousNode(VortexGraphNode node);
void VortexGraph_AdvanceNodeIndex(VortexGraphNode node);
void VortexGraph_RetreatNodeIndex(VortexGraphNode node);
void VortexGraph_SetNodeIndex(VortexGraphNode node, vortex_ulong index);
vortex_ulong VortexGraph_GetNodeIndex(VortexGraphNode node);
void VortexGraph_AdvanceElementIndex(VortexGraphNode node);
void VortexGraph_RetreatElementIndex(VortexGraphNode node);
void VortexGraph_SetElementIndex(VortexGraphNode node, vortex_ulong index);
vortex_ulong VortexGraph_GetElementIndex(VortexGraphNode node);


#endif /* Vortex_VortexGraph_h */
