#define PY_SSIZE_T_CLEAN
#include <python3.11/Python.h>
typedef unsigned long long int bitmask_t ;


int build_branch(PyObject *list, bitmask_t* rows,Py_ssize_t n_rows,
        bitmask_t* columns, Py_ssize_t n_columns ) {


    PyObject *item, *number;
    printf("The list is of size %ld\n", n_rows);

    for (Py_ssize_t i = 0; i < n_rows; i++){
        if(!(rows[i/64] & (1 << i))){
            continue;
        }
        printf("I chose the row %ld\n", i);

        for ( Py_ssize_t j = 0; j < n_columns; j++){
            if(!(columns[j/64] & (1 << j))){
                continue;
            }
            printf("I chose the column %ld\n", j);
            item = PyList_GetItem(list, i);
            Py_ssize_t l = PyList_Size(item);
            if( l < 0) {
                return NULL;
            }
            number = PyList_GetItem(item,j);
            long t = PyLong_AsLong(number);
            printf("I have found value: %ld!\n", t);
        }
    }
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
    bitmask_t *rows = (bitmask_t *) malloc(sizeof(bitmask_t) * (n/64 + 1));
    rows[0] = -1;


    item = PyList_GetItem(list, 0);
    Py_ssize_t l = PyList_Size(item);
    if( l < 0) {
        return NULL;
    }
    bitmask_t *columns = (bitmask_t *) malloc(sizeof(bitmask_t) * (l/64 + 1));
    columns[0] = 0;
    columns[0] = columns[0] | (1<<1);
    build_branch(list,rows,n,columns,l);
    //for(Py_ssize_t i = 0; i < n; i++) {
        //item = PyList_GetItem(list, i);
        //Py_ssize_t l = PyList_Size(item);
        //if( l < 0) {
            //return NULL;
        //}
        //for ( Py_ssize_t j = 0; j < l; j++){
           //number = PyList_GetItem(item, j);
           //long t = PyLong_AsLong(number);
        //}
    //}
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
