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
#include <setjmp.h>
#include <Vortex/VortexMem.h>
#include <Vortex/VortexGraph.h>

struct VortexGraphProfile_s {
  VortexStore elements;
  VortexJumpBuffer jump_buffer;
};

static vortex_int is_method = 0;

static VortexAny Vortex_GraphArgsCallBack(VortexAny callback_object) {
   longjmp(((VortexGraphProfile)callback_object)->jump_buffer.env,1);
   return NULL;
}

VortexAny VortexGraph_GetArg(VortexGraphProfile profile, VortexArgs args, const char* typestring) {
   return VortexArgs_GetNextArgWithCallback(args,typestring,0,profile,Vortex_GraphArgsCallBack);
}

VortexJumpBufferPtr VortexGraph_GetJumpBuffer(VortexGraphProfile profile) {
   return &(profile->jump_buffer);
}

VortexGraphProfile VortexGraph_NewProfile(VortexGraphProfileDefinition definition) {
    VortexGraphProfile profile = vortex_new_mem_of_type(struct VortexGraphProfile_s);
    profile->elements = VortexStore_New();
    definition(profile);
    return profile;
}

void VortexGraph_AddArrayToProfile(const char* name, VortexGraphProfile profile) {
    VortexStore_AddItem(profile->elements,VortexArray_New(),name);
}

vortex_int VortexGraph_AddMethod(VortexGraphProfile profile, const char* name, VortexAny method) {
    if (!VortexStore_ItemExists(profile->elements,name)) {
       VortexList list = VortexList_New();
       VortexList_AddToList(list,&is_method);
       VortexStore_AddItem(profile->elements,list,name);
    }
    VortexList list = VortexStore_GetItem(profile->elements,name);
    if (VortexList_GetItem(VortexList_GetFirtstNode(list)) != &is_method)
     return 0;
    VortexList_AddToList(list,method);
    return 1;
}

VortexAny VortexGraph_LookUpMethod(VortexGraphProfile profile, const char* name, vortex_ulong index) {
    VortexDataStructure ds = VortexStore_GetItem(profile->elements,name); 
    if (ds->flag != VortexListNodeFlag) return NULL;
    VortexList list = ds;
    if (VortexList_GetItem(VortexList_GetFirtstNode(list)) != &is_method)
     return NULL;
    VortexListNode node = VortexList_GetNode(list,index+1);
    if (node == NULL) return NULL;
    return VortexList_GetItem(node);
}




/*

vortex_graph_method(my_setter,void,arg(input,int)) {
 
}

vortex_graph_method(my_getter,int,noargs) {
 
}

vortex_graph_method(visit_literal,void,arg(literal,Literal)) {
 
}

vortex_graph_method(visit_addition,void,arg(addition,Addition)) {
 
}

vortex_graph_profile(profile_1) {
    vortex_graph_add_array("array_1");
    vortex_graph_add_method(my_setter);
    vortex_graph_add_method(my_getter);
    vortex_graph_add_methods("visit",
     visit_literal,visit_addition);
    vortex_graph_add_self("before");
    vortex_graph_add_self("after");
}

VortexGraphProfile profile = VortexGraph_NewProfile(profile_1);
vortex_graph_invoke(profile,visit,arg(literal,Literal));

vortex_graph_method(test,int,noargs) {
 return 0;
}

int vortex_graph_method_test_callable0(VortexGraphProfile profile, bool is_lookup, VortexArgs args) {
  int index = 0;
  if (is_lookup) {
   //var = SETJMP
   if (var) index++;
   if (index > max_index) {
    puts("TYPE ERROR!!!!");
   }
   return vortex_graph_lookup_method(profile,"test",index)(profile,false,args);//CAST
  } else {
   return vortex_graph_method_test_callable1(profile,args);
  }
}

int vortex_graph_method_test_callable1(VortexGraphProfile profile, VortexArgs args) {
  int retvalue;
  vortex_graph_method_test_callable2(profile,arg(&retvalue,int*),args);
  return retvalue;
}

void vortex_graph_method_test_callable2(VortexGraphProfile profile,VortexArgs retarg, VortexArgs args) {
  int* retvalue = vortex_graph_getarg(retarg);
  *retvalue = vortex_graph_method_test_callable3(profile,arg(&retvalue,int*),args);
}

int vortex_graph_method_test_callable3(VortexGraphProfile profile, VortexArgs args) {
 return 0;
}

struct node {
 struct node* before;
 struct node* after;
 int value;
}

VortexGraphProfile profile = VortexGraphProfileNew();
VortexGraph_SetProgram(profile,"Builder",0,graph_builder);
VortexGraph_RunProgram(profile,"Builder",0,noargs);
VortexGraph_TraverseNext(profile,this_node,"Next",NEXT_NODE_STATE,noargs);

void graph_builder(VortexGraphProfile profile, VortexArgs args) {
 struct node* root_node = VortexGraph_GetGraph(profile);
 if (root_node == NULL) root_node = new(struct node);
 struct node* a = new(struct node);
 a->value++;
 root_node->after = a;
 VortexGraph_AddNewNodeToList(profile,a);
 
}

*/