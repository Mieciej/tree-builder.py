#define PY_SSIZE_T_CLEAN
#include <python3.11/Python.h>
#include <iostream>
#include "attribute.h"
#include <vector>
#include "tree.h"

extern "C" void expand_tree(Branch* b);

extern "C" void printBT(const Branch* node, long value,
                        std::unordered_map<long, std::string> &dictionary);
extern "C" void print_tree_helper(const std::string& prefix, const Branch* node,
                                  bool not_last, long value,std::unordered_map<long, std::string> &dictionary);
extern "C" int build_branch(PyObject *list, bitmask_t* rows,
                 Py_ssize_t n_rows, Py_ssize_t n_columns ) {
    std::vector<Attribute*> * attributes
        = new std::vector<Attribute*>();
    PyObject *item, *value, *target;
    std::unordered_map<std::string,long> encoding; 
    std::unordered_map<long, std::string> dictionary;
    long *classes = new long [n_rows];
    long encoder = 0;
    for (Py_ssize_t row = 0; row < n_rows; row++) {

        for ( Py_ssize_t column = 0; column < n_columns-1; column++) {

            item = PyList_GetItem(list, row);
            Py_ssize_t l = PyList_Size(item);
            if( l < 0) {
                return (int)NULL;
            }
            value = PyList_GetItem(item,column);
            PyObject* str = PyObject_Str(value);
            const char* bytes = PyUnicode_AsUTF8(str);
            std::string key_value (bytes);
            if(!encoding.count(key_value)) {
                encoding[key_value] = encoder++;
                dictionary[encoder-1] = key_value;
                std::cout <<"Value: " <<key_value <<
                ", encoding: "<<encoding[key_value] <<std::endl;
            }

            target = PyList_GetItem(item,n_columns-1);
            str = PyObject_Str(target);
            bytes = PyUnicode_AsUTF8(str);
            std::string key_target (bytes);
            if(!encoding.count(key_target)) {
                encoding[key_target] = encoder++;
                dictionary[encoder-1] = key_target;
                std::cout <<"Target class: " <<key_target <<
                ", encoding: "<<encoding[key_target] <<std::endl;
            }

            classes[row] = encoding[key_target];
            std::pair<long,long> * pair = new std::pair<long,long>(encoding[key_value],encoding[key_target]) ;
            if((unsigned)column < attributes->size()){
                attributes->at(column)->values->push_back(pair);
                continue;
            }
            Attribute* new_attribute = new Attribute(n_rows,column);

            new_attribute->values->push_back(pair);
            attributes->push_back(new_attribute);
        }
    }
    bitmask_t n[1];
    n[0] = ~0;
    std::cout << "INFO: Finished reading features." <<std::endl;
    Branch root(n_rows, *attributes, n, classes);
    expand_tree(&root);
    std::cout << "INFO: Finished constructing a tree. "<<std::endl;
    printBT(&root, 0, dictionary);
    return 0;
}

extern "C" void expand_tree(Branch * b){
    Branch *curr = b;
    if (!(curr->split())){
        for(auto child : *curr->children){
            expand_tree(child.second);
        }
    }
    if(curr->is_leaf){
        return;
    }
}
extern "C" void print_tree_helper(const std::string& prefix, const Branch* node, bool not_last, long value,std::unordered_map<long,std::string> &dictionary)
{
    if (node != nullptr)
    {
        std::cout << prefix;

        std::cout << (not_last ? "├": "└");
        std::cout << dictionary[value] <<"──";
        if(!node->is_leaf) {
            std::cout << node->split_attribute->label << std::endl;
        } else {
            std::cout << dictionary[node->decision]<<std::endl;
            return;
        }
        size_t i =0;
        for (auto c: *node->children)
        {
            bool nl =  i != node->children->size()-1;
            print_tree_helper(prefix + (not_last ? "│    " : "     "), c.second,nl , c.first,dictionary);
            ++i;
        }
    }
}

extern "C" void printBT(const Branch* node, long value, std::unordered_map<long,std::string> &dictionary)
{
    print_tree_helper("", node, false, value, dictionary);
}


static PyObject * tree_build(PyObject * self,PyObject *args){
    PyObject * list, *item;
    if (!PyArg_ParseTuple(args, "O", &list)) {
        return NULL;
    }
    Py_ssize_t n = PyList_Size(list);
    if(n < 0){
        return NULL;
    }
    bitmask_t *rows = new bitmask_t[n/64+1];
    rows[0] = -1;


    item = PyList_GetItem(list, 0);
    Py_ssize_t l = PyList_Size(item);
    if( l < 0) {
        return NULL;
    }
    build_branch(list,rows,n,l);
    return PyLong_FromLong(0);
}


static PyMethodDef TreeBuilderMethods[] = {
    {"build",tree_build,METH_VARARGS,"Build decision tree."},
    {NULL,NULL,0,NULL}
};
static struct PyModuleDef treebuildermodule = {
    PyModuleDef_HEAD_INIT,
    "tree_builder",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    TreeBuilderMethods
};
extern "C" PyMODINIT_FUNC PyInit_tree_builder(void){
    return PyModule_Create(&treebuildermodule);
}

int main(int argc, char *argv[]){
    wchar_t *program = Py_DecodeLocale(argv[0],NULL);
    if(program == NULL){
        fprintf(stderr,"Fatal error: cannot decode[0]\n");
        exit(1);
    }
    if(PyImport_AppendInittab("tree_builder",PyInit_tree_builder)==-1){
        fprintf(stderr, "Error: could not extend in-built modules table\n");
        exit(1);
    }
    Py_Initialize();
    PyMem_RawFree(program);
}
