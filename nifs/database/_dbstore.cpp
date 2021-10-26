#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "dbstore.h"

#if defined(__linux__) || defined(__MACH__)
#include <strings.h>
#define _stricmp(x1, x2) strcasecmp(x1, x2)
#endif

extern "C"
{
#include <Python.h>
  //#include "numpy/arrayobject.h"
  void init_dbstore();
};

static const char Id[] = "$Id: dbstore.cpp,v 1.1.1.1 2019/07/23 03:59:43 emo Exp $";
static PyObject *ErrorObject;

static PyObject *
dbstore_open(PyObject *self,
             PyObject *args)
{
  char *mailAddress;
  char *diagName;
  int shotNo;
  int subNo;
  int dataType;
  if (!PyArg_ParseTuple(args, "ssiii", &mailAddress, &diagName, &shotNo, &subNo, &dataType))
  {
    return 0;
  }
  int desc = dbsOpen(mailAddress, diagName, (unsigned int)shotNo, (short)subNo, dataType);
  if (desc < 0)
  {
    Py_INCREF(Py_None);
    return Py_None;
  }

  PyObject *retVal = Py_BuildValue("i", desc);
  return retVal;
}

static PyObject *
dbstore_close(PyObject *self,
              PyObject *args)
{
  int desc;
  if (!PyArg_ParseTuple(args, "i", &desc))
  {
    return 0;
  }
  dbsClose(desc);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
dbstore_abort(PyObject *self,
              PyObject *args)
{
  int desc;
  if (!PyArg_ParseTuple(args, "i", &desc))
  {
    return 0;
  }
  dbsAbort(desc);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
dbstore_write(PyObject *self,
              PyObject *args)
{
  int desc;
  int chNo;
  char *data;
  int data_len;
  PyObject *paramList;
  char *dataType;
  int ret = 0;

  if (!PyArg_ParseTuple(args, "iis#Os", &desc, &chNo, &data, &data_len, &paramList, &dataType))
  {
    return 0;
  }

  if (!PyList_Check(paramList))
    return 0;

  int list_len = PyList_Size(paramList);
  int i;

  int *types = new int[list_len];
  char **keys = new char *[list_len];
  char **vals = new char *[list_len];
  int max_len = 1;

  for (i = 0; i < list_len; i++)
  {
    PyObject *item = PyList_GetItem(paramList, i);
    if (!PyList_Check(item))
      return 0;
    PyObject *k = PyList_GetItem(item, 0);
    PyObject *v = PyList_GetItem(item, 1);
    PyObject *t = PyList_GetItem(item, 2);
    /*
    keys[i] = PyString_AsString(k);
    vals[i] = PyString_AsString(v);
    types[i] = PyInt_AsLong(t);
*/

    PyArg_ParseTuple(k, "s", &keys[i]);
    PyArg_ParseTuple(v, "s", &vals[i]);
    PyArg_ParseTuple(t, "i", &types[i]);

    if (strlen(keys[i]) > max_len)
      max_len = strlen(keys[i]);
    if (strlen(vals[i]) > max_len)
      max_len = strlen(vals[i]);
  }

  int _param_len = max_len * 3 * list_len;
  char *_param = new char[_param_len];
  memset(_param, 0, _param_len);
  char *p = _param;
  for (i = 0; i < list_len; i++)
  {
    strcpy(p, keys[i]);
    p += max_len;
    strcpy(p, vals[i]);
    p += max_len;
    sprintf(p, "%d", types[i]);
    p += max_len;
  }
  delete[] keys;
  delete[] vals;
  delete[] types;

  ret = dbsWrite(desc, chNo, (unsigned int)list_len * 3, (unsigned int)max_len, (unsigned char *)_param,
                 (unsigned int)data_len, (unsigned char *)data, dataType);

  delete[] _param;

  if (ret == 0)
  {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else
  {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyMethodDef dbstore_methods[] = {
    {"open", dbstore_open, METH_VARARGS},
    {"close", dbstore_close, METH_VARARGS},
    {"abort", dbstore_abort, METH_VARARGS},
    {"write", dbstore_write, METH_VARARGS},
    {NULL},
};

static struct PyModuleDef DBstoreModule = {
    PyModuleDef_HEAD_INIT,
    "_dbstore", /* name of module */
    NULL,        /* module documentation, may be NULL */
    -1,          /* size of per-interpreter state of the module,
	       or -1 if the module keeps state in global variables. */
    dbstore_methods};

PyMODINIT_FUNC
PyInit__dbstore(void)
{
  PyObject *m;

  m = PyModule_Create(&DBstoreModule);
  if (m == NULL)
    return NULL;
  PyObject *d = PyModule_GetDict(m);
  ErrorObject = Py_BuildValue("s", "_dbstore.error");
  PyDict_SetItemString(d, "error", ErrorObject);
  return m;
}
