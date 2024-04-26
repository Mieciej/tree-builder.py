#define PY_SSIZE_T_CLEAN
#include <python3.11/Python.h>
#include <iostream>
#include "attribute.h"
#include <vector>
#include "tree.h"


int build_branch(PyObject *list, bitmask_t* rows,Py_ssize_t n_rows,
        bitmask_t* columns, Py_ssize_t n_columns ) {
    std::vector<Attribute*> * attributes = new std::vector<Attribute*>();
    PyObject *item, *number, *target;

    long *classes = new long [n_rows];
    size_t i=0;
    for (Py_ssize_t i = 0; i < n_rows; i++) {

        for ( Py_ssize_t j = 0; j < n_columns-1; j++) {

            item = PyList_GetItem(list, i);
            Py_ssize_t l = PyList_Size(item);
            if( l < 0) {
                return (int)NULL;
            }
            number = PyList_GetItem(item,j);
            long t = PyLong_AsLong(number);
            target = PyList_GetItem(item,n_columns-1);
            long c = PyLong_AsLong(target);
            classes[i] =c;
            std::pair<long,long> * pair = new std::pair<long,long>(t,c) ;
            if(j < attributes->size()){
                attributes->at(j)->values->push_back(pair);
                continue;
            }
            Attribute* new_attribute = new Attribute(n_rows);

            new_attribute->values->push_back(pair);
            attributes->push_back(new_attribute);
        }
    }
    bitmask_t n[1];
    n[0] = ~0;
    std::cout << "INFO: Finished reading features." <<std::endl;
    for ( size_t i = 0; i<n_columns-1;  i++){
        Attribute *feat = attributes->at(i);
        float ent = feat->get_entropy(n);
        std::cout<<"Feature " << i  <<": "<< ent<< std::endl;
    }   
    Branch b(n_rows, *attributes, n, classes);
    std::cout << b.get_entropy() << std::endl;
    b.split((*attributes)[0]);
    return 0;
}


static PyObject * tree_build(PyObject * self,PyObject *args){
    PyObject * list, *item, *number; 
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
    bitmask_t *columns = new bitmask_t[l/64+1];
    columns[0] = 0;
    columns[0] = columns[0] | (1<<1);
    build_branch(list,rows,n,columns,l);
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
PyMODINIT_FUNC PyInit_tree_builder(void){
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
    Py_SetProgramName(program);
    Py_Initialize();
    PyMem_RawFree(program);
}
