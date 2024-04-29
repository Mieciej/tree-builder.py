#define PY_SSIZE_T_CLEAN
#include <python3.11/Python.h>
#include <iostream>
#include "attribute.h"
#include <vector>
#include "tree.h"
#include <unordered_map>

extern "C" void expand_tree(Branch* b);

extern "C" void printBT(const Branch* node, long value,
                        std::unordered_map<long, std::string> &dictionary);
extern "C" void print_tree_helper(const std::string& prefix, const Branch* node,
                                  bool not_last, long value,
                                  std::unordered_map<long, std::string> &dictionary,
                                  bool first);
int parse_value(PyObject * value, std::string& result){

    PyObject* str = PyObject_Str(value);
    const char* bytes = PyUnicode_AsUTF8(str);
    result =std::string(bytes);
    return 0;
}
int encode_value(std::string key_value, 
                 std::unordered_map<std::string, long> & encoding,
                 std::unordered_map<long, std::string>* & decoding,
                 long&  encoder){
    if(!encoding.count(key_value)) {
        (encoding)[key_value] = encoder++;
        (*decoding)[encoder-1] = key_value;
        std::cout <<"Value: " <<key_value <<
        ", encoding: "<<(encoding)[key_value] <<std::endl;
    }
    return 0;
}
int parse_attributes(PyObject *list, Py_ssize_t n_rows,
                     Py_ssize_t n_columns, std::vector<Attribute*>* &attributes,
                     std::unordered_map<long, std::string> * &dictionary,
                     long * &classes){

    attributes = new std::vector<Attribute*>();
    PyObject *item, *value, *target;
    std::unordered_map<std::string,long> encoding; 
    dictionary = new std::unordered_map<long, std::string>;
    classes = new long [n_rows];
    long encoder = 0;
    for (Py_ssize_t row = 0; row < n_rows; row++) {

        for ( Py_ssize_t column = 0; column < n_columns-1; column++) {

            item = PyList_GetItem(list, row);
            Py_ssize_t l = PyList_Size(item);
            if( l < 0) {
                return (int)NULL;
            }
            value = PyList_GetItem(item,column);
            std::string key_value;
            parse_value(value, key_value);
        
            encode_value(key_value, encoding, 
                         dictionary, encoder);

            target = PyList_GetItem(item,n_columns-1);
            std::string key_target;
            parse_value(target, key_target);
            encode_value(key_target, encoding, 
                         dictionary, encoder);
            classes[row] = encoding[key_target];
            std::pair<long,long> * pair = new std::pair<long,long>(encoding[key_value],encoding[key_target]) ;
            Attribute* new_attribute;
            if(row == 0){
                new_attribute = new Attribute(n_rows,column);
                attributes->push_back(new_attribute);
            }
            else {
                new_attribute = (*attributes)[column];
            }

            new_attribute->values->push_back(pair);
        }
    }
    std::cout << "INFO: Finished reading features." <<std::endl;
    return 0;
}

extern "C" int build_branch(PyObject *list, 
                 Py_ssize_t n_rows, Py_ssize_t n_columns ) {
    std::vector<Attribute*> * attributes;
    long * classes;
    std::unordered_map<long,std::string> *dictionary;
    parse_attributes(list, n_rows, n_columns,
                     attributes, dictionary,
                     classes);
    bitmask_t *n  =new bitmask_t [n_rows/sizeof(bitmask_t)+1];
    for (size_t i = 0; i<n_rows/sizeof(bitmask_t)+1; i++){
        n[i] = ~0;
    }
    Branch root(n_rows, *attributes, n, classes);
    expand_tree(&root);
    std::cout << "INFO: Finished constructing a tree. "<<std::endl;
    printBT(&root, 0, *dictionary);

    for(auto a: *attributes){
        delete(a);
    }
    delete(attributes);
    delete(classes);
    delete(dictionary);
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
extern "C" void print_tree_helper(const std::string& prefix, const Branch* node,
                                  bool not_last,
                                  long value,std::unordered_map<long,std::string> &dictionary,
                                  bool first)
{
    if (node != nullptr)
    {
        std::cout << prefix;

        std::cout << (not_last ? "├": "└");
        std::string print_value;
        if(!first) print_value= dictionary[value];
        else print_value="─";
        std::cout << print_value <<"──";
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
            std::string bonus = "";
            for (size_t i = 0; i<print_value.size()-1; i++) {
                bonus+=" ";
            }
            print_tree_helper(prefix + (not_last ? "│    " : "     ") + bonus,  c.second,nl , c.first,dictionary, false);
            ++i;
        }
    }
}

extern "C" void printBT(const Branch* node, long value, std::unordered_map<long,std::string> &dictionary)
{
    print_tree_helper("", node, false, value, dictionary,true);
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


    item = PyList_GetItem(list, 0);
    Py_ssize_t l = PyList_Size(item);
    if( l < 0) {
        return NULL;
    }
    build_branch(list,n,l);
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
