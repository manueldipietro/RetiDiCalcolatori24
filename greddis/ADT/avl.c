#include "avl.h"

void print_T(void* e1){
    printf("%3d", *((int*) e1));
    return;
}

Avl* avl_new(){
    Avl* tree = malloc(sizeof(Avl));
    tree->root = NULL;
}

int avl_height_aux(Avl_node* node){
    if(node == NULL)
        return 0;
    int h_left = avl_height_aux(node->left);
    int h_right = avl_height_aux(node->right);
    return h_left > h_right ? h_left+1 : h_right+1;
}

int avl_height(Avl* tree){
    return avl_height_aux(tree->root);
}

int avl_get_balance_aux(Avl_node* node){
    if(node  == NULL)
        return 0;
    return avl_height_aux(node->left) - avl_height_aux(node->right);
}

int avl_get_balance(Avl* tree){
    return avl_get_balance_aux(tree->root);
}

Avl_node* avl_right_rotate(Avl_node* y){
    Avl_node* x = y->left;
    Avl_node* T2 = x->right;
    x->right = y;
    y->left = T2;
    return x;
}

Avl_node* avl_left_rotate(Avl_node* x){
    Avl_node* y = x->right;
    Avl_node* T2 = y->left;
    y->left = x;
    x->right = T2;
    return y;
}

Avl_node* avl_balance(Avl_node* root, int (*compar)(const void*, const void*)){
    int balance = avl_get_balance_aux(root);

    //Left Left Case
    if(balance > 1 && compar(root, root->left) > 0){
        return avl_right_rotate(root);
    }

    //Right Right Case
    if(balance < -1 && compar(root, root->right) < 0){
        return avl_left_rotate(root);
    }

    //Left Right Case
    if(balance > 1 && compar(root, root->left) > 0){
        root->left = avl_left_rotate(root->left);
        return avl_right_rotate(root);
    }

    //Right Left Case
    if(balance < -1 && compar(root, root->right) < 0){
        root->right = avl_right_rotate(root->right);
        return avl_left_rotate(root);
    }

    return root;
}

Avl_node* avl_insert_aux(Avl_node* root, Avl_node* to_insert, int (*compar)(const void*, const void*), void** finded){
    if(root==NULL){
        return to_insert;
    }
    int cmpRes = compar(root->T, to_insert->T);
    if( cmpRes == 0){
        *finded = root->T;
        return root;
    }
    else if( cmpRes < 0){
        root->right = avl_insert_aux( root->right, to_insert, compar, finded);
    }
    else root->left=avl_insert_aux(root->left, to_insert, compar, finded);
    
    return avl_balance(root, compar);
}

void* avl_insert(Avl* tree, void* T, int (*compar)(const void*, const void*)){
    void* finded = NULL;
    Avl_node* to_insert = malloc(sizeof(Avl_node));
    to_insert->left = NULL;
    to_insert->right = NULL;
    to_insert->T = T;
    tree->root = avl_insert_aux(tree->root, to_insert, compar, &finded);
    return finded;
}

void* avl_research_aux(Avl_node* root, void* T, int (*compar)(const void*, const void*)){
    if(root == NULL){
        return NULL;
    }
    int cmpRes = compar(root->T, T);
    if( cmpRes == 0){
        return root->T;
    }
    else if( cmpRes < 0){
        return avl_research_aux( root->right, T, compar);
    }
    else return avl_research_aux(root->left, T, compar);
}

void* avl_research(Avl* tree, void* T, int (*compar)(const void*, const void*)){
    return avl_research_aux(tree->root, T, compar);
}

void print2DUtil(Avl_node* root, int space);

//Find the maximum of minimum
Avl_node* avl_remove_aux_aux(Avl_node* root, void** T){
    if(root->right == NULL){
        *T = root->T;
        Avl_node* to_ret = root->left;
        free(root);
        return to_ret;        
    }
    root->right = avl_remove_aux_aux(root->right, T);
    return root;
}

Avl_node* avl_remove_aux(Avl_node* root, void* T, int(*compare) (const void*, const void*), void** T_removed){
    if(root == NULL){
        return NULL;
    }
    
    int cmp = compare(root->T, T);
    if( cmp > 0)
        root->left = avl_remove_aux(root->left, T, compare, T_removed);
    else if( cmp < 0)
        root->right = avl_remove_aux(root->right, T, compare, T_removed);
    else{
        *T_removed = root->T;
        if(root->left == NULL && root->right == NULL){
            free(root);
            return NULL;
        }
        else if(root->left == NULL){
            Avl_node* to_ret = root->right;
            free(root);
            return root->right;
        }
        else if(root->right == NULL){
            Avl_node* to_ret = root->left;
            free(root);
            return root->left;
        }  
        else{
            void* substitute;      
            T_removed = root->T;
            root->left = avl_remove_aux_aux(root->left, &substitute);
            root->T = substitute;
            return root;
        }
    }

    root = avl_balance(root, compare);

    return root;
}

void* avl_remove(Avl* tree, void* T_toremove, int(*compare) (const void*, const void*)){
    void* T_removed;
    tree->root = avl_remove_aux(tree->root, T_toremove, compare, &T_removed);    
    return T_removed;
}

void avl_delete_aux(Avl_node* root){
    if(root == NULL)
        return;
    avl_delete_aux(root->left);
    avl_delete_aux(root->right);
    free(root);
    return;
}

void avl_delete(Avl* tree){
    avl_delete_aux(tree->root);
    free(tree);
    return;
}

void avl_delete_aux_andfreeinfo(Avl_node* root, void (*T_destroyer) (void*)){
    if(root == NULL)
        return;
    T_destroyer(root->T);
    avl_delete_aux(root->left);
    avl_delete_aux(root->right);
    free(root);
    return;

}

void avl_delete_andfreeinfo(Avl* tree, void (*T_destroyer) (void*)){
    avl_delete_aux_andfreeinfo(tree->root, T_destroyer);
    free(tree);
    return;

}



// Function to print binary tree in 2D
// It does reverse inorder traversal
void print2DUtil(Avl_node* root, int space)
{
    // Base case
    if (root == NULL)
        return;
 
    // Increase distance between levels
    space += 10;
 
    // Process right child first
    print2DUtil(root->right, space);
 
    // Print current node after space
    // count
    printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    print_T(root->T);
    // Process left child
    print2DUtil(root->left, space);
}
 
// Wrapper over print2DUtil()
void print2D(Avl* tree)
{
    // Pass initial space count as 0
    print2DUtil(tree->root, 0);
}
 



int compar_int(const void* e1, const void* e2){
    if((*(int*)e1) == (*(int*)e2))
        return 0;
    if((*(int*)e1) > (*(int*)e2))
        return 1;
    return -1;

}

/*
int main(){
    Avl* tree = avl_new();
    
    int n0 = 0;
    int n1 = 1;
    int n2 = 2;
    int n3 = 3;
    int n4 = 4;
    int n5 = 5;
    int n6 = 6;
    int n7 = 7;
    int n8 = 8;
    int n9 = 9;
    int n10 = 10;
    int n11 = 10;


    avl_insert(tree, &n0, compar_int);
    avl_insert(tree, &n1, compar_int);
    avl_insert(tree, &n2, compar_int);
    avl_insert(tree, &n3, compar_int);
    avl_insert(tree, &n4, compar_int);
    avl_insert(tree, &n5, compar_int);
    avl_insert(tree, &n6, compar_int);
    avl_insert(tree, &n7, compar_int);
    avl_insert(tree, &n8, compar_int);
    avl_insert(tree, &n9, compar_int);
    avl_insert(tree, &n10, compar_int);
    printf("Ret: %p, Main:%p\n", (int*)avl_insert(tree, &n11, compar_int), &n10);
    printf("Coefficente bilanciamento: %d\n", avl_get_balance(tree));
    print2D(tree);
    printf("\n");

    printf("Rimozione:\n");
    avl_remove(tree, &n0, compar_int);
    printf("Coefficente bilanciamento: %d\n", avl_get_balance(tree));
    print2D(tree);
    printf("\n");

    avl_remove(tree, &n1, compar_int);

    printf("Coefficente bilanciamento: %d\n", avl_get_balance(tree));
    print2D(tree);
    printf("\n");

    return 0;
}*/
