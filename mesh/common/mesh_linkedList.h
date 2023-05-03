#ifndef MESH_LL_H
#define MESH_LL_H

struct mesh_node {
    struct process *data;
    struct mesh_node* next;
};

struct process *get_process_by_id(struct mesh_node* head, int id);
void add_node(struct mesh_node** head_ref, struct process *new_data);
void remove_node(struct mesh_node** head_ref, int index);

#endif