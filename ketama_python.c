#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "src/ketama.h"


typedef struct {
    PyObject_HEAD
    PyObject* filename;
    char* cfilename;
    ketama_continuum continuum;
} Ketama;


static void
Ketama_dealloc(Ketama *self) {
    if (self->continuum != NULL) {
        ketama_smoke(self->continuum);
        self->continuum = NULL;
    }

    if (self->filename != NULL) {
        Py_XDECREF(self->filename);
        self->filename = NULL;
        self->cfilename = NULL;
    }

    Py_TYPE(self)->tp_free((PyObject *) self);
}



/*
    Ketama.__new__ classmethod definition
*/
static PyObject *
Ketama_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Ketama *self;
    self = (Ketama *) type->tp_alloc(type, 0);
    return (PyObject *) self;
}


/*
    Ketama.__init__() method
*/
static int
Ketama_init(Ketama *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"filename", NULL};
    PyObject *filename;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &filename)) {
        return -1;
    }

    if (filename == NULL) {
        PyErr_SetString(PyExc_ValueError, "First argument must be passed");
        return -1;
    }

    char* cfilename;

    if (PyBytes_Check(filename)) {
        if(PyBytes_GET_SIZE(filename) <= 0) {
            PyErr_SetString(PyExc_ValueError, "Value must be non empty byte string");
            return -1;
        }
        cfilename = (char*) PyBytes_AsString(filename);
    } else if (PyUnicode_Check(filename)) {
        if(PyUnicode_GET_SIZE(filename) <= 0) {
            PyErr_SetString(PyExc_ValueError, "Value must be non empty string");
            return -1;
        }
        cfilename = (char*) PyUnicode_AsUTF8(filename);
    } else {
        PyErr_SetString(PyExc_ValueError, "Value must be str or bytes");
        return -1;
    }

    if (!ketama_roll(&self->continuum, cfilename)) {
        char* err = ketama_error();
        if (err != NULL) {
            PyErr_Format(PyExc_RuntimeError, "%s", err);
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Unknown Ketama error");
        }

        return -1;
    }

    self->filename = filename;
    self->cfilename = cfilename;
    Py_INCREF(self->filename);

    return 0;
}


static PyObject* Ketama_repr(Ketama *self) {
    if (self->continuum == NULL) {
        PyErr_SetNone(PyExc_RuntimeError);
        return NULL;
    }

    return PyUnicode_FromFormat(
        "<%s as %p: filename=\"%s\">",
        Py_TYPE(self)->tp_name, self, self->cfilename
    );
}


PyDoc_STRVAR(Ketama_get_server_docstring,
    "Maps a key onto a server in the continuum. \n\n"
    "    Ketama('example.conf').get_server(key) -> Tuple[int, str]"
);

static PyObject*
Ketama_get_server(Ketama* self, PyObject *args, PyObject *kwds) {
    PyObject *key;

    if (!PyArg_ParseTuple(args, "O", &key)) return NULL;

    char* ckey;
    size_t length;

    if (PyBytes_Check(key)) {
        ckey = (char*) PyBytes_AsString(key);
        length = PyBytes_GET_SIZE(key);
    } else if (PyUnicode_Check(key)) {
        ckey = (char*) PyUnicode_AsUTF8(key);
        length = PyUnicode_GET_LENGTH(key);
    } else {
        PyErr_SetString(PyExc_ValueError, "Value must be non empty string");
        return NULL;
    }

    mcs *result = ketama_get_server(ckey, self->continuum, length);
    return Py_BuildValue("Is", result->point, result->ip);
}


/*
    Ketama.get_points() method
*/
PyDoc_STRVAR(Ketama_get_points_docstring,
    "return all continuum points.\n\n"
    "    Ketama.get_points() -> List[Tuple[int, str]]"
);

static PyObject*
Ketama_get_points(Ketama* self, PyObject *args, PyObject *kwds) {

    mcs (*mcsarr)[self->continuum->numpoints] = self->continuum->array;
    PyObject* ret = PyList_New(self->continuum->numpoints);

    for (int i = 0; i < self->continuum->numpoints; i++) {
        PyList_SET_ITEM(
            ret, i, Py_BuildValue("Is", (*mcsarr)[i].point, (*mcsarr)[i].ip)
        );
    }

    return ret;
}


/*
    Ketama properties
*/
static PyMemberDef Ketama_members[] = {
    {
        "filename",
        T_OBJECT,
        offsetof(Ketama, filename),
        READONLY,
        "filename"
    },
    {NULL}  /* Sentinel */
};


static PyMethodDef Ketama_methods[] = {
    {
        "get_server",
        (PyCFunction) Ketama_get_server, METH_VARARGS,
        Ketama_get_server_docstring
    },
    {
        "get_points",
        (PyCFunction) Ketama_get_points, METH_NOARGS,
        Ketama_get_points_docstring
    },
    {NULL}  /* Sentinel */
};


static PyTypeObject
KetamaType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Ketama",
    .tp_doc = "Ketama instance",
    .tp_basicsize = sizeof(Ketama),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = Ketama_new,
    .tp_init = (initproc) Ketama_init,
    .tp_dealloc = (destructor) Ketama_dealloc,
    .tp_members = Ketama_members,
    .tp_methods = Ketama_methods,
    .tp_repr = (reprfunc) Ketama_repr
};


static PyModuleDef ketama_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "ketama",
    .m_doc = "Ketama library python bindings.",
    .m_size = -1,
};


PyMODINIT_FUNC PyInit_ketama(void) {
    PyObject *m;

    m = PyModule_Create(&ketama_module);

    if (m == NULL) return NULL;

    if (PyType_Ready(&KetamaType) < 0) return NULL;

    Py_INCREF(&KetamaType);

    if (PyModule_AddObject(m, "Ketama", (PyObject *) &KetamaType) < 0) {
        Py_XDECREF(&KetamaType);
        Py_XDECREF(m);
        return NULL;
    }

    return m;
}
