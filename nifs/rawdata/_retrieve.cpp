#if defined(WIN32) || defined(WIN64)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT (0x0400)
#endif
#pragma warning(disable : 4786)
#else
#include <string>
#endif // WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "retrieve.h"

#define RAW_DATA 1
#define ANA_DATA 2

extern "C"
{
#include <Python.h>
  //#include "numpy/arrayobject.h"
  void init_retrieve();
};

#define PyInt_FromSize_t(x) PyLong_FromSize_t(x)
#define PyString_Check(name) PyBytes_Check(name)
#define PyString_FromString(x) PyUnicode_FromString(x)
#define PyString_FromStringAndSize(x, len) PyUnicode_FromStringAndSize(x, len)
#define PyString_Format(fmt, args) PyUnicode_Format(fmt, args)
#define PyString_AsString(str) PyBytes_AsString(str)
#define PyString_Size(str) PyBytes_Size(str)
#define PyInt_AsLong(x) PyLong_AsLong(x)
#define PyInt_FromLong(x) PyLong_FromLong(x)

static const char Id[] = "$Id: retrieve.cpp,v 1.1.1.1 2019/07/23 03:59:43 emo Exp $";
static PyObject *ErrorObject;

static PyObject *
retrieve_retrieve_main(const char *Diag_Name,
                       int shot,
                       int sub,
                       int ch,
                       int timeoutSec,
                       int voltconv,
                       PyObject *params)
{
  unsigned int AShot_Number = shot;
  unsigned short ASub_Shot_Number = static_cast<unsigned short>(sub);
  short chno = static_cast<short>(ch);
  long ret;
  int rd = -1;
  char *IndexServer = getenv("INDEXSERVERNAME");

  if (IndexServer == 0)
  {
    PyErr_SetString(ErrorObject, "Enviroment Variable INDEXSERVERNAME is not set");
    return 0;
  }

  rd = retrieveOpenWait(Diag_Name, IndexServer, AShot_Number, ASub_Shot_Number, timeoutSec);
  if (0 > rd)
  {
    //    jclass jc = env->FindClass("retrieve/RetrieveException");
    //    env->ThrowNew(jc, "Error while calling retrieveOpenWait");
    return NULL;
  }

  unsigned int n_channel = 1;
  short year, month, day, hour, min, sec;
  char management[32];
  char comment[128];
  int comment_size = 128;
  char cserver[32];

  ret = retrieveShotInfo(rd, &n_channel, &year, &month, &day, &hour, &min, &sec,
                         management, comment, comment_size, cserver);

  if (0 > ret)
  {
    PyErr_SetString(ErrorObject, "Error while calling retrieveShotInfo");
    retrieveClose(rd);
    return 0;
  }

  /*
  printf("---Arc Shot Infomation---\n");
  printf("Index Server Name  = %s\n",IndexServer);
  printf("Trans Server Name  = %s\n",cserver);
  printf("Shot Number        = %10u\n",AShot_Number);
  printf("Sub Shot Number    = %10hu\n",ASub_Shot_Number);
  printf("DATE               = %4d-%2d-%2d\n",year,month,day);
  printf("TIME               = %4d:%2d:%2d\n",hour,min,sec);
  */

  unsigned int data_length;
  unsigned int comp_length;
  unsigned short param_count;
  short data_type;
  char image_type[32];
  unsigned short value_len;
  int is_nframe;
  char *data_type_name;
  char *struct_str;

  ret = retrieveChInfo(rd, chno,
                       &data_length, &comp_length, &param_count, &data_type, image_type, &value_len, &is_nframe,
                       management, comment, comment_size);
  if (0 > ret)
  {
    PyErr_SetString(ErrorObject, "Error while calling retrieveChInfo");
    retrieveClose(rd);
    return 0;
  }

  switch (data_type)
  {
  case RAW_DATA:
    data_type_name = "RAW";
    break;
  case ANA_DATA:
    data_type_name = "ANA";
    break;
  default:
    data_type_name = "???";
    break;
  }
  if (0 == is_nframe)
  {
    struct_str = "None Frame";
  }
  else if (0 < is_nframe)
  {
    struct_str = "Frame";
  }
  else
  {
    struct_str = "?????";
  }

  //printf("---Channel Information---\n");
  //printf("Channel Number     = %d\n"	,chno);
  //printf("Data Type          = %s\n"	,data_type_name);
  //printf("Data Size          = %u\n"	,data_length);
  //printf("Comp Size          = %u\n"	,comp_length);
  //
  //printf("Resolution(bit)    = %hu\n"	,value_len);
  //printf("Image Type         = %s\n"	,image_type);
  //printf("Data Struct        = %s\n"	,struct_str);
  //if( 0 != is_nframe )
  //  printf("Number of Frame    = %d\n"	,is_nframe);
  //
  //  printf("Comment            = %s\n"	,comment);

  if (0 < param_count)
  {
    char **Key = new char *[param_count];
    char **Val = new char *[param_count];
    int *Typ = new int[param_count];
    int j;
    for (j = 0; j < param_count; j++)
    {
      Key[j] = new char[128];
      Val[j] = new char[128];
    }
    ret = retrieveChParams(rd, chno, Key, Val, Typ);
    if (0 > ret)
    {
      PyErr_SetString(ErrorObject, "Error while calling retrieveChParams");
      retrieveClose(rd);
      return 0;
    }
    char *module_type = "unkown";
    for (j = 0; j < param_count; j++)
    {
      if (0 == strcmp("ModuleType", Key[j]))
      {
        module_type = Val[j];
        break;
      }
    }
    for (j = 0; j < param_count; j++)
    {
      if (1 == (int)Typ[j] && retrieve_check_string(Val[j]))
      {
        //		printf("%s,%s,\"%s\",%d\n" ,module_type ,Key[j] ,Val[j] ,(int)Typ[j]);
      }
      else
      {
        //		printf("%s,%s,%s,%d\n" ,module_type ,Key[j] ,Val[j] ,(int)Typ[j]);
      }
    }

    if (params != Py_None && PyList_Check(params))
    {
      for (j = 0; j < param_count; j++)
      {
        PyObject *item = PyList_New(0);
        PyObject *Pkey = PyString_FromString(Key[j]);
        PyObject *Pval = PyString_FromString(Val[j]);
        PyObject *Ptyp = PyInt_FromLong(Typ[j]);

        PyList_Append(item, Pkey);
        PyList_Append(item, Ptyp);
        PyList_Append(item, Pval);
        PyList_Append(params, item);

        Py_DECREF(Pkey);
        Py_DECREF(Ptyp);
        Py_DECREF(Pval);
        Py_DECREF(item);

        delete[] Key[j];
        delete[] Val[j];
      }

      //{
      //char buf[10];
      //PyObject* Pkey = PyString_FromString("Resolution(bit)");
      //sprintf(buf,"%d",value_len);
      //PyObject* Pval = PyString_FromString(buf);
      //Py_DECREF(Pkey);
      //Py_DECREF(Pval);
      //}
    }

    delete[] Typ;
    delete[] Val;
    delete[] Key;
  }

  char *data = 0;

  PyObject *retVal = 0;

  if (0 < data_length)
  {
    if (voltconv)
    {
      unsigned int volts_buf_size = 0;
      if (9 > value_len)
      {
        volts_buf_size = data_length;
      }
      else if (17 > value_len)
      {
        volts_buf_size = data_length / 2;
      }
      else if (33 > value_len)
      {
        volts_buf_size = data_length / 4;
      }

      unsigned int ret_length;
      float *data = new float[volts_buf_size];
      ret = retrieveChVolts(rd, chno, data, volts_buf_size, &ret_length);
      if (0 > ret)
      {
        PyErr_SetString(ErrorObject, "Error while calling retrieveChVolts");
        delete[] data;
        retrieveClose(rd);
        return 0;
      }
      retVal = PyBytes_FromStringAndSize((const char *)data, (int)volts_buf_size * sizeof(float));
    }
    else
    {
      unsigned int ret_length;
      data = new char[data_length];
      ret = retrieveChData(rd, chno, data, data_length, &ret_length);

      if (0 > ret)
      {
        PyErr_SetString(ErrorObject, "Error while calling retrieveChData");
        delete[] data;
        retrieveClose(rd);
        return 0;
      }
      retVal = PyBytes_FromStringAndSize(data, (int)data_length);
    }
  }
  retrieveClose(rd);
  return retVal;
}

static PyObject *
retrieve_retrieve(PyObject *self,
                  PyObject *args)
{
  char *diag;
  int shotno;
  int subno;
  int ch;
  int timeout;
  int voltconv;
  PyObject *params;

  if (!PyArg_ParseTuple(args, "siiiiiO", &diag, &shotno, &subno, &ch, &timeout, &voltconv, &params))
  {
    return NULL;
  }

  PyObject *result = retrieve_retrieve_main(diag, shotno, subno, ch, timeout, voltconv, params);

  if (result == NULL)
  {
    return NULL;
  }

  return result;
}

static PyMethodDef retrieve_methods[] = {
    {"retrieve", retrieve_retrieve, METH_VARARGS, NULL},
    {NULL},
};

/*
void init_retrieve() 
{
  //  import_array();
  PyObject *m,*d;
  
  m = Py_InitModule("_retrieve",retrieve_methods);
  d = PyModule_GetDict(m);
  ErrorObject = Py_BuildValue("s","_retrieve.error");
  PyDict_SetItemString(d,"error",ErrorObject);
}
*/

static struct PyModuleDef RetrieveModule = {
    PyModuleDef_HEAD_INIT,
    "_retrieve", /* name of module */
    NULL,        /* module documentation, may be NULL */
    -1,          /* size of per-interpreter state of the module,
	       or -1 if the module keeps state in global variables. */
    retrieve_methods};

PyMODINIT_FUNC
PyInit__retrieve(void)
{
  PyObject *m;

  m = PyModule_Create(&RetrieveModule);
  if (m == NULL)
    return NULL;
  PyObject *d = PyModule_GetDict(m);
  ErrorObject = Py_BuildValue("s", "_retrieve.error");
  PyDict_SetItemString(d, "error", ErrorObject);
  return m;
}
