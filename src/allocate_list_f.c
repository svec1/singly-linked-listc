#include "allocate_list_f.h"

#include <string.h>
#include <stdlib.h>

data** allocate_mem_fdata_node(node** nod, size_t data_struct_byte_size){
    (*nod) = malloc(sizeof(node));
    (*nod)->data_alloc = malloc(sizeof(data));
    (*nod)->data_alloc->allocate_data = malloc(data_struct_byte_size*MAX_SIZE_NODE_DATA);
    memset((*nod)->data_alloc->allocate_data, NULL, data_struct_byte_size*MAX_SIZE_NODE_DATA);
    (*nod)->data_alloc->count_elem = 0;
    (*nod)->data_alloc->ros = 0;
    (*nod)->next_node = NULL;
    return &(*nod)->data_alloc;
}
data** try_allocate_mem_fdata_node(node** nod, size_t data_struct_byte_size){
    if(!(*nod)) return allocate_mem_fdata_node(nod, data_struct_byte_size);
    else if((*nod)->data_alloc->count_elem < MAX_SIZE_NODE_DATA) return &(*nod)->data_alloc;
    return try_allocate_mem_fdata_node(&(*nod)->next_node, data_struct_byte_size);
}
void try_free_mem_fdata_node(node** nod){
    if(!(*nod)) return;
    else if((*nod)->data_alloc->count_elem) try_free_mem_fdata_node(&(*nod)->next_node);
    free((*nod)->data_alloc->allocate_data);
    free((*nod));
}
void allocate_mem_fdata(list** root_l){
    allocate_mem_fdata_node(&(*root_l)->node, (*root_l)->data_struct_byte_size);
}

node** find_node_wfree_allocate_mem(node** nod_last, node** nod){
    if(!(*nod)) return nod_last;
    else if((*nod)->data_alloc->count_elem < MAX_SIZE_NODE_DATA) return nod;
    return find_node_wfree_allocate_mem(nod, &(*nod)->next_node);
}

node** find_node_index_allocate_mem(node** nod, size_t index, size_t* curr_index){
    if(!(*nod)) return NULL;
    else if(index<(*curr_index)+MAX_SIZE_NODE_DATA && index-(*curr_index)<=(*nod)->data_alloc->count_elem){
        return nod;
    }
    (*curr_index)+=5;
    return find_node_index_allocate_mem(&(*nod)->next_node, index, curr_index);
}

void copy_data_byte_to_data_alloc_allocate_mem(data** data_alloc, void* data, size_t data_struct_byte_size){
    if(!(*data_alloc) || !(*data_alloc)->allocate_data){
        printf("It's not possible to copy data to unallocated memory\n");
        exit(1);
    }
    for(size_t i = (*data_alloc)->count_elem*data_struct_byte_size, j = 0; i < (*data_alloc)->count_elem*data_struct_byte_size+data_struct_byte_size; ++i, ++j){
        ((unsigned char*)(*data_alloc)->allocate_data)[i] = ((unsigned char*)data)[j];
    }
    ++(*data_alloc)->count_elem;
    ++(*data_alloc)->ros;
}
void erase_data_byte_last_in_allocate_mem(data** data_alloc, size_t data_struct_byte_size){
    if(!(*data_alloc) || !(*data_alloc)->allocate_data){
        printf("It's not possible to copy data to unallocated memory\n");
        exit(1);
    }
    for(size_t i = (*data_alloc)->count_elem*data_struct_byte_size; i < (*data_alloc)->count_elem*data_struct_byte_size+data_struct_byte_size; ++i){
        ((unsigned char*)(*data_alloc)->allocate_data)[i] = NULL;
    }
    --(*data_alloc)->count_elem;
    --(*data_alloc)->ros;
}
void erase_data_byte_in_allocate_mem(data** data_alloc, size_t data_struct_byte_size, size_t offset){
    if(!(*data_alloc) || !(*data_alloc)->allocate_data){
        printf("It's not possible to copy data to unallocated memory\n");
        exit(1);
    }
    for(size_t i = offset*data_struct_byte_size; i > offset*data_struct_byte_size+data_struct_byte_size; ++i){
        ((unsigned char*)(*data_alloc)->allocate_data)[i] = NULL;
    }
    --(*data_alloc)->count_elem;
}
void* copy_data_byte_from_allocate_mem(data** data_alloc, size_t data_struct_byte_size, size_t offset){
    if(!(*data_alloc) || !(*data_alloc)->allocate_data){
        printf("It's not possible to copy data to unallocated memory\n");
        exit(1);
    }
    void* data_copy = malloc(data_struct_byte_size);
    for(size_t i = offset*data_struct_byte_size, j = 0; i < offset*data_struct_byte_size+data_struct_byte_size; ++i, ++j){
        ((unsigned char*)data_copy)[j] = ((unsigned char*)(*data_alloc)->allocate_data)[i];
    }
    return data_copy;
}

void add_element_to_list(list** root_l, void* data, size_t data_struct_byte_size){
    if(!(*root_l)){
        printf("Passed to function - copy_el_data_to_allocate_mem_fdata(), the list does not exist\n");
        exit(1);
    }
    else if(data_struct_byte_size!=(*root_l)->data_struct_byte_size){
        printf("The size of the list item structure is not equal to the size passed to the function: add_element_to_list()\n");
        exit(1);
    }
    
    copy_data_byte_to_data_alloc_allocate_mem(
        try_allocate_mem_fdata_node(
            &(*root_l)->node,
            data_struct_byte_size),
        data,
        data_struct_byte_size);
}
void del_last_element_to_list(list** root_l){
    if(!(*root_l)){
        printf("Passed to function - copy_el_data_to_allocate_mem_fdata(), the list does not exist\n");
        exit(1);
    }
    erase_data_byte_last_in_allocate_mem(
        &(*find_node_wfree_allocate_mem(
            NULL,
            &(*root_l)->node))->data_alloc,
        (*root_l)->data_struct_byte_size);
}
void del_index_element_to_list(list** root_l, size_t index){
    if(!(*root_l)){
        printf("Passed to function - copy_el_data_to_allocate_mem_fdata(), the list does not exist\n");
        exit(1);
    }
    size_t index_count_el_nodes = 0;
    node* nod;
    if((nod = find_node_index_allocate_mem(&(*root_l)->node, index, &index_count_el_nodes) == NULL)){
        printf("Element with index not found - %d\n", index);
        exit(1);
    }
    erase_data_byte_in_allocate_mem(&nod->data_alloc, (*root_l)->data_struct_byte_size, index_count_el_nodes-index);
}
void* get_element(list** root_l, size_t index){
    if (!(*root_l)) {
        printf("Passed to function - copy_el_data_to_allocate_mem_fdata(), the list does not exist\n");
        exit(1);
    }
    size_t index_count_el_nodes = 0;
    node** nod = find_node_index_allocate_mem(&(*root_l)->node, index, &index_count_el_nodes);
    if(nod == NULL){
        printf("Element with index not found - %d\n", index);
        exit(1);
    }
    return copy_data_byte_from_allocate_mem(&(*nod)->data_alloc, (*root_l)->data_struct_byte_size, (index>= index_count_el_nodes) ? index-index_count_el_nodes : index);
}