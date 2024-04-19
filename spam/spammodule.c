#define PY_SSIZE_T_CLEAN
#include <python3.11/Python.h>

static PyObject * spam_system(PyObject * self,PyObject *args){
    PyObject * list, *item, *number; 
    if (!PyArg_ParseTuple(args, "O", &list)) {
        return NULL;
    }
    Py_ssize_t n = PyList_Size(list);
    if(n < 0){
        return NULL;
    }
    for(Py_ssize_t i = 0; i < n; i++) {
        item = PyList_GetItem(list, i);
        Py_ssize_t l = PyList_Size(item);
        if( l < 0) {
            return NULL;
        }

        for ( Py_ssize_t j = 0; j < l; j++){
           number = PyList_GetItem(item, j);
           long t = PyLong_AsLong(number);
           printf("Element[%ld, %ld] = %ld\n",i,j, t);
        }
    }
    return PyLong_FromLong(0);
}


static PyMethodDef SpamMethods[] = {
    {"system",spam_system,METH_VARARGS,"Execute a shell command."},
    {NULL,NULL,0,NULL}
};
static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    SpamMethods
};
PyMODINIT_FUNC PyInit_spam(void){
    return PyModule_Create(&spammodule);
}

int main(int argc, char *argv[]){
    wchar_t *program = Py_DecodeLocale(argv[0],NULL);
    if(program == NULL){
        fprintf(stderr,"Fatal error: cannot decode[0]\n");
        exit(1);
    }
    if(PyImport_AppendInittab("spam",PyInit_spam)==-1){
        fprintf(stderr, "Error: could not extend in-built modules table\n");
        exit(1);
    }
    Py_SetProgramName(program);
    Py_Initialize();
    PyMem_RawFree(program);
}
