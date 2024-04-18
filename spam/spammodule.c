#define PY_SSIZE_T_CLEAN
#include <python3.11/Python.h>
#include <python3.11/numpy/arrayobject.h>
#include <stdlib.h>
#include <stdio.h>

static PyObject * spam_system(PyObject * self,PyObject *args){
	//PyArrayObject* arr = (PyArrayObject*) PyArray_FromAny(args, NULL, 0,0, 
	//		NPY_ARRAY_IN_ARRAY,NULL);
	//
	int ** dataptr;
	npy_intp multi_index[3];

    PyArrayObject *arr = (PyArrayObject *)PyArray_FromAny(args, NULL, 0, 0,
			NPY_ARRAY_IN_ARRAY, NULL);	
	NpyIter *iter = NpyIter_New(arr,NPY_ITER_READONLY | NPY_ITER_MULTI_INDEX |
			NPY_ITER_REFS_OK , NPY_CORDER, NPY_NO_CASTING, NULL);
	NpyIter_IterNextFunc *iternext = NpyIter_GetIterNext(iter, NULL);
	NpyIter_GetMultiIndexFunc *get_multi_index = 
		NpyIter_GetGetMultiIndex(iter, NULL);

	dataptr = (int**)NpyIter_GetDataPtrArray(iter);

	do{
		get_multi_index(iter, multi_index);
		printf("Element[%" NPY_INTP_FMT "][%" NPY_INTP_FMT "] = %d\n", 
				multi_index[1], multi_index[2],**dataptr);
	} while(iternext(iter));
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
	import_array();
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
