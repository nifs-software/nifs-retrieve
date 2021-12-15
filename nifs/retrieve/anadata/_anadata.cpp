#include <Python.h>
#include <anadata.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "numpy/arrayobject.h"

#define PyInt_FromSize_t(x) PyLong_FromSize_t(x)
#define PyString_Check(name) PyUnicode_Check(name)
#define PyString_FromString(x) PyUnicode_FromString(x)
#define PyString_FromStringAndSize(x, len) PyUnicode_FromStringAndSize(x, len)
#define PyString_Format(fmt, args) PyUnicode_Format(fmt, args)
#define PyString_AsString(str) PyBytes_AsString(str)
#define PyString_Size(str) PyBytes_Size(str)
#define PyInt_AsLong(x) PyLong_AsLong(x)
#define PyInt_FromLong(x) PyLong_FromLong(x)

//#define DEBUG 1

/*
 * libana の AnaData クラスのPython ラッパー
 *
 */
static const char *Id = "$Id: anadata.cpp,v 1.4 2020/07/15 03:01:36 yoshida Exp $";
static PyObject *ErrorObject; /* local exception */

static PyTypeObject AnaDataType;

#define onError(message)                   \
  {                                        \
    PyErr_SetString(ErrorObject, message); \
    return NULL;                           \
  }
using namespace eg;
struct EGAnaData
{
  PyObject_HEAD /* python header: ref-count + &typeobject */
      AnaData *ana;
};

extern "C"
{
  PyObject *init_anadata();
};

static PyObject *
EGAnaData_new(PyTypeObject *type,
              PyObject *args,
              PyObject *kwds);

static PyObject *
EGAnaData_alloc(PyTypeObject *type,
                Py_ssize_t nitems);

static void
deleteEGAnaData(EGAnaData *self);

static int
printEGAnaData(EGAnaData *self,
               FILE *fp,
               int flags);

static PyObject *
EGAnaData_retrieve(PyTypeObject *type,
                   PyObject *args);
static PyObject *
EGAnaData_getShotNo(PyObject *self,
                    PyObject *args);
static PyObject *
EGAnaData_getSubNo(PyObject *self,
                   PyObject *args);
static PyObject *
EGAnaData_getDiagnostics(PyObject *self,
                         PyObject *args);
static PyObject *
EGAnaData_getDate(PyObject *self,
                  PyObject *args);

static PyObject *
EGAnaData_getDimNo(PyObject *self,
                   PyObject *args);

static PyObject *
EGAnaData_getValNo(PyObject *self,
                   PyObject *args);

static PyObject *
EGAnaData_getDimSize(PyObject *self,
                     PyObject *args);

static PyObject *
EGAnaData_getValSize(PyObject *self,
                     PyObject *args);

static PyObject *
EGAnaData_getDimName(PyObject *self,
                     PyObject *args);
static PyObject *
EGAnaData_getDimUnit(PyObject *self,
                     PyObject *args);

static PyObject *
EGAnaData_getValName(PyObject *self,
                     PyObject *args);

static PyObject *
EGAnaData_getValUnit(PyObject *self,
                     PyObject *args);

static PyObject *
EGAnaData_getDimData(PyObject *self,
                     PyObject *args);

static PyObject *
EGAnaData_getDimData(PyObject *self,
                     PyObject *args);
static PyObject *
EGAnaData_getValData(PyObject *self,
                     PyObject *args);

static PyObject *
EGAnaData_getComment(PyObject *self,
                     PyObject *args);

static PyObject *
EGAnaData_setDiagnostics(PyObject *self,
                         PyObject *args);

static PyObject *
EGAnaData_setShotNo(PyObject *self,
                    PyObject *args);

static PyObject *
EGAnaData_setSubNo(PyObject *self,
                   PyObject *args);

static PyObject *
EGAnaData_setDate(PyObject *self,
                  PyObject *args);

static PyObject *
EGAnaData_addComment(PyObject *self,
                     PyObject *args);
static PyObject *
EGAnaData_setDim(PyObject *self,
                 PyObject *args);
static PyObject *
EGAnaData_setVal(PyObject *self,
                 PyObject *args);
static PyObject *
EGAnaData_clear(PyObject *self,
                PyObject *args);

static PyObject *
EGAnaData_regist(PyObject *self,
                 PyObject *args);

static PyObject *
EGAnaData_registfile(PyTypeObject *type,
                     PyObject *args);
static PyObject *
EGAnaData_delete(PyTypeObject *type,
                 PyObject *args);

static PyObject *
EGAnaData_search(PyTypeObject *type,
                 PyObject *args);

// Python メソッド
static PyMethodDef EGAnaDataMethods[] = {
    // データベース操作関連
    {"retrieve", (PyCFunction)EGAnaData_retrieve, METH_VARARGS | METH_CLASS,
     "引数:diag,shotno,subshotno\nデータを検索し、AnaDataオブジェクトを返す"},
    {"registfile", (PyCFunction)EGAnaData_registfile, METH_VARARGS | METH_CLASS,
     "引数:diag,shotno,subno,filename\n解析データのフォーマットで作成されたfilenameファイルをdiag,shotnob,subnoで登録する"},
    {"delete", (PyCFunction)EGAnaData_delete, METH_VARARGS | METH_CLASS,
     "引数:diag,shotno,subno\n登録された解析データを削除する"},
    {"search", (PyCFunction)EGAnaData_search, METH_VARARGS | METH_CLASS,
     "引数:diag,uppper,lower,subno,user,limit\n解析データの検索"},
    {"regist", (PyCFunction)EGAnaData_regist, METH_VARARGS,
     "引数:なし\n解析データオブジェクトの登録"},

    // 属性取得
    {"getShotNo", (PyCFunction)EGAnaData_getShotNo, METH_VARARGS,
     "引数:なし\nショット番号を返す"},
    {"getSubNo", (PyCFunction)EGAnaData_getSubNo, METH_VARARGS,
     "引数:なし\nサブショット番号を返す"},
    {"getDiagnostics", (PyCFunction)EGAnaData_getDiagnostics, METH_VARARGS,
     "引数:なし\n計測名を返す"},
    {"getComment", (PyCFunction)EGAnaData_getComment, METH_VARARGS,
     "引数:なし\nコメントを返す"},
    {"getDate", (PyCFunction)EGAnaData_getDate, METH_VARARGS,
     "引数:なし\n登録日時を返す"},
    {"getDimNo", (PyCFunction)EGAnaData_getDimNo, METH_VARARGS,
     "引数:なし\n次元の数を返す"},
    {"getValNo", (PyCFunction)EGAnaData_getValNo, METH_VARARGS,
     "引数:なし\n変数の数を返す"},
    {"getDimSize", (PyCFunction)EGAnaData_getDimSize, METH_VARARGS,
     "引数:dim(数値または文字列)\n次元dim(番号または名前)の配列大きさを返す"},
    {"getValSize", (PyCFunction)EGAnaData_getValSize, METH_VARARGS,
     "引数:なし\n変数の配列の大きさを返す"},
    {"getDimName", (PyCFunction)EGAnaData_getDimName, METH_VARARGS,
     "引数:dimNo(数値)\n番号dimNoの次元の名前を返す"},
    {"getDimUnit", (PyCFunction)EGAnaData_getDimUnit, METH_VARARGS,
     "引数:dim(数値または文字列)\n次元dim(番号またはの名前)の単位を返す"},
    {"getValName", (PyCFunction)EGAnaData_getValName, METH_VARARGS,
     "引数:valNo(数値)\n番号valNoの変数の名前を返す"},
    {"getValUnit", (PyCFunction)EGAnaData_getValUnit, METH_VARARGS,
     "引数:val(数値または文字列)\n変数val(番号またはの名前)の単位を返す"},
    {"getDimData", (PyCFunction)EGAnaData_getDimData, METH_VARARGS,
     "引数:dim(数値または文字列)\n次元dim(番号またはの名前)の配列を返す"},
    {"getValData", (PyCFunction)EGAnaData_getValData, METH_VARARGS,
     "引数:val(数値または文字列)\n配列val(番号またはの名前)の配列を返す"},

    // 属性セット
    {"setDiagnostics", (PyCFunction)EGAnaData_setDiagnostics, METH_VARARGS,
     "引数:diag(文字列)\n計測名をdiagでセットする"},
    {"setShotNo", (PyCFunction)EGAnaData_setShotNo, METH_VARARGS,
     "引数:shotNo(数値)\nショット番号をshotNoでセットする"},
    {"setSubNo", (PyCFunction)EGAnaData_setSubNo, METH_VARARGS,
     "引数:subNo(数値)\nサブショット番号をsubNoでセットする"},
    {"setDate", (PyCFunction)EGAnaData_setDate, METH_VARARGS,
     "引数:date(文字列 YYYY-MM-DD HH:MM:DD)\n日付をdateでセットする"},
    {"addComment", (PyCFunction)EGAnaData_addComment, METH_VARARGS,
     "引数:comment(文字列)\nコメントを追加する"},
    {"setDim", (PyCFunction)EGAnaData_setDim, METH_VARARGS,
     "引数:no(数値),name(文字列),unit(文字列),array(numpy.Array型)\n次元番号 no の次元を名前=name、単位=unit、データ=arrayでセットする"},
    {"setVal", (PyCFunction)EGAnaData_setVal, METH_VARARGS,
     "引数:no(数値),name(文字列),unit(文字列),array(numpy.Array型)\n変数番号 no の変数を名前=name、単位=unit、データ=arrayでセットする"},
    {NULL, NULL, NULL}};

static PyTypeObject EGAnaDataType = {
    PyVarObject_HEAD_INIT(NULL, 0) "_anadata._AnaData", /*tp_name*/
    sizeof(EGAnaData),                                  /*tp_basicsize*/
    0,                                                  /*tp_itemsize*/
    (destructor)deleteEGAnaData,                        /*tp_dealloc*/
    (printfunc)printEGAnaData,                          /*tp_print*/
    (getattrfunc)0,                                     /*tp_getattr*/
    (setattrfunc)0,                                     /*tp_setattr*/
    0,                                                  /*tp_compare*/
    0,                                                  /*tp_repr*/
    0,                                                  /*tp_as_number*/
    0,                                                  /*tp_as_sequence*/
    0,                                                  /*tp_as_mapping*/
    0,                                                  /*tp_hash */
    0,                                                  /*tp_call*/
    0,                                                  /*tp_str*/
    0,                                                  /*tp_getattro*/
    0,                                                  /*tp_setattro*/
    0,                                                  /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,           /*tp_flags*/
    0,                                                  /*tp_doc*/
    0,                                                  /*tp_traverse*/
    0,                                                  /*tp_clear*/
    0,                                                  /*tp_richcompare*/
    0,                                                  /*tp_weaklistoffset*/
    0,                                                  /*tp_iter*/
    0,                                                  /*tp_iternext*/
    EGAnaDataMethods,                                   /*tp_methods*/
    0,                                                  /*tp_members*/
    0,                                                  /*tp_getsets*/
    0,                                                  /*tp_base*/
    0,                                                  /*tp_dict*/
    0,                                                  /*tp_descr_get*/
    0,                                                  /*tp_descr_set*/
    0,                                                  /*tp_dictoffset*/
    0,                                                  /*tp_init*/
    EGAnaData_alloc,                                    /*tp_alloc*/
    EGAnaData_new,                                      /*tp_new*/
};

#define is_EGAnaDataObject(v) (Py_TYPE(v) == &EGAnaDataType)

static PyObject *
EGAnaData_alloc(PyTypeObject *type,
                Py_ssize_t nitems)
{
#ifdef DEBUG
  printf("EGAnaData_alloc\n");
#endif
  EGAnaData *self;
  self = PyObject_NEW(EGAnaData, &EGAnaDataType);
  if (self == NULL)
    return NULL;
  return (PyObject *)self;
}

static void
deleteEGAnaData(EGAnaData *self)
{
#ifdef DEBUG
  printf("deleteEGAnaData\n");
#endif
  //  PyMem_DEL(self);
  if (self->ana)
    delete self->ana;
  self->ana = 0;

  if (is_EGAnaDataObject(self))
  {
    //printf("AnaData class\n");
    //    PyObject_DEL(self);
  }
  else
  {
    //printf("sub class\n");
    Py_TYPE(self)->tp_free((PyObject *)self);
  }
}

static int
printEGAnaData(EGAnaData *self,
               FILE *fp,
               int flags)
{
#ifdef DEBUG
  printf("printEGAnaData\n");
#endif
  AnaData *p = self->ana;
  if (p)
  {
    try
    {
      fprintf(fp, "_AnaData(%s, %d, %d)", p->getDiagnostics(), p->getShotNo(), p->getSubNo());
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    fprintf(fp, "EGAnaData(null)");
  }
  return 0;
}

static PyObject *
EGAnaData_getShotNo(PyObject *self,
                    PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getShotNo\n");
#endif
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      return Py_BuildValue("i", p->getShotNo());
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getSubNo(PyObject *self,
                   PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getSubNo\n");
#endif
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
    return Py_BuildValue("i", p->getSubNo());
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getDiagnostics(PyObject *self,
                         PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getDiagnostics\n");
#endif
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      return Py_BuildValue("s", p->getDiagnostics());
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getDate(PyObject *self,
                  PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getDate\n");
#endif
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      return Py_BuildValue("s", p->getDate());
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getComment(PyObject *self,
                     PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getComment\n");
#endif
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      return Py_BuildValue("s", p->getComment());
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}
static PyObject *
EGAnaData_getDimNo(PyObject *self,
                   PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getDimNo\n");
#endif
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      return Py_BuildValue("i", p->getDimNo());
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getValNo(PyObject *self,
                   PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getValNo\n");
#endif
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      return Py_BuildValue("i", p->getValNo());
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getDimSize(PyObject *self,
                     PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getDimSize\n");
#endif
  PyObject *o;
  if (!PyArg_ParseTuple(args, "O", &o))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      if (PyString_Check(o))
      {
        const char *s = PyUnicode_AsUTF8(o);
        return Py_BuildValue("i", p->getDimSize(s));
      }
      else
      {
        int no = (int)PyInt_AsLong(o);
        return Py_BuildValue("i", p->getDimSize(no));
      }
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getValSize(PyObject *self,
                     PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getValSize\n");
#endif
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      return Py_BuildValue("i", p->getValSize());
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getDimName(PyObject *self,
                     PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getDimName\n");
#endif
  int no;
  if (!PyArg_ParseTuple(args, "i", &no))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      return Py_BuildValue("s", p->getDimName(no));
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getDimUnit(PyObject *self,
                     PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getDimUnit\n");
#endif
  PyObject *o;
  if (!PyArg_ParseTuple(args, "O", &o))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      if (PyString_Check(o))
      {
        const char *s = PyUnicode_AsUTF8(o);
        return Py_BuildValue("s", p->getDimUnit(s));
      }
      else
      {
        int no = (int)PyInt_AsLong(o);
        return Py_BuildValue("s", p->getDimUnit(no));
      }
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getValName(PyObject *self,
                     PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getValName\n");
#endif
  int no;
  if (!PyArg_ParseTuple(args, "i", &no))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      return Py_BuildValue("s", p->getValName(no));
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getValUnit(PyObject *self,
                     PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getValUnit\n");
#endif
  PyObject *o;
  if (!PyArg_ParseTuple(args, "O", &o))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
    try
    {
      if (PyString_Check(o))
      {
        const char *s = PyUnicode_AsUTF8(o);
        return Py_BuildValue("s", p->getValUnit(s));
      }
      else
      {
        int no = (int)PyInt_AsLong(o);
        return Py_BuildValue("s", p->getValUnit(no));
      }
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_getDimData(PyObject *self,
                     PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getDimData\n");
#endif
  PyObject *o;
  npy_intp dimensions[1];
  if (!PyArg_ParseTuple(args, "O", &o))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      const double *data;
      int len;
      if (PyString_Check(o))
      {
        //  	const char* s = PyString_AsString(o);
        const char *s = PyUnicode_AsUTF8(o);
        data = p->getDimData(s);
        len = p->getDimSize(s);
      }
      else
      {
        int no = (int)PyInt_AsLong(o);
        data = p->getDimData(no);
        len = p->getDimSize(no);
      }
      double *_data = new double[len];
      memcpy(_data, data, len * sizeof(double));
      dimensions[0] = len;
      PyArrayObject *ary = (PyArrayObject *)PyArray_SimpleNewFromData(1, dimensions, NPY_DOUBLE, _data);
      return PyArray_Return(ary);
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    return Py_None;
  }
}

static PyObject *
EGAnaData_getValData(PyObject *self,
                     PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_getValData\n");
#endif
  PyObject *o;
  PyObject *hash = NULL;
  int no;
  if (!PyArg_ParseTuple(args, "O|O", &o, &hash))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      const double *data;
      int len;
      if (PyString_Check(o))
      {
        const char *s = PyUnicode_AsUTF8(o);
        no = p->getValIndex(s);
      }
      else
      {
        no = (int)PyInt_AsLong(o);
      }
      // 全データ取得
      if (hash == NULL)
      {
        int dimNo = p->getDimNo();
        npy_intp *dimensions = new npy_intp[dimNo];
        int i;
        for (i = 0; i < dimNo; i++)
        {
          dimensions[i] = p->getDimSize(i);
        }
        data = p->getValData(no);
        len = p->getValSize();
        double *_data = new double[len];
        memcpy(_data, data, len * sizeof(double));

        PyArrayObject *ary = (PyArrayObject *)PyArray_SimpleNewFromData(dimNo, dimensions, NPY_DOUBLE, _data);
        delete[] dimensions;
        return PyArray_Return(ary);
        // 特定面で切断
      }
      else if (PyDict_Check(hash))
      {
        npy_intp dimensions[1];
        int dimNo = p->getDimNo();
        int *idx = new int[dimNo];
        bool *b = new bool[dimNo];
        int i;
        for (i = 0; i < dimNo; i++)
        {
          b[i] = true;
        }

        // 次元のインデックスをidx[] にセット
        for (i = 0; i < dimNo - 1; i++)
        {
          PyObject *key, *val;
          Py_ssize_t pos = 0;
          while (PyDict_Next(hash, &pos, &key, &val))
          {
            //	    const char* dimName = PyString_AsString(key);
            const char *dimName = PyUnicode_AsUTF8(key);
            int dimVal = (int)PyInt_AsLong(val);
            int m = p->getDimIndex(dimName);
            idx[m] = dimVal;
            b[m] = false;
          }
        }

        // セットされていない次元を探す
        int dno = -1;
        for (i = 0; i < dimNo; i++)
        {
          if (b[i])
          {
            dno = i;
            break;
          }
        }

        int vsize = p->getDimSize(dno);
        double *_data = new double[vsize];
        p->getValData(idx, dno, no, _data);
        dimensions[0] = vsize;
        PyArrayObject *ary = (PyArrayObject *)PyArray_SimpleNewFromData(1, dimensions, NPY_DOUBLE, _data);
        delete[] idx;
        delete[] b;
        return PyArray_Return(ary);
      }
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_setDiagnostics(PyObject *self,
                         PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_setDiagnostics\n");
#endif
  const char *s;
  if (!PyArg_ParseTuple(args, "s", &s))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      p->setDiagnostics(s);
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
EGAnaData_setShotNo(PyObject *self,
                    PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_setShotNo\n");
#endif
  int no;
  if (!PyArg_ParseTuple(args, "i", &no))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      p->setShotNo(no);
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
EGAnaData_setSubNo(PyObject *self,
                   PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_setSubNo\n");
#endif
  int no;
  if (!PyArg_ParseTuple(args, "i", &no))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      p->setSubNo(no);
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
EGAnaData_setDate(PyObject *self,
                  PyObject *args)
{
  const char *s;
  if (!PyArg_ParseTuple(args, "s", &s))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      p->setDate(s);
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
EGAnaData_addComment(PyObject *self,
                     PyObject *args)
{
  const char *s;
  if (!PyArg_ParseTuple(args, "s", &s))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      p->addComment(s);
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
EGAnaData_setDim(PyObject *self,
                 PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_setDim\n");
#endif
  const char *name;
  const char *unit;
  int no;
  PyArrayObject *o;

  if (!PyArg_ParseTuple(args, "issO", &no, &name, &unit, &o))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      int n = PyArray_ITEMSIZE(o);
      double *data = (double *)PyArray_DATA(o);
      p->setDim(no, name, unit, n, data);
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
EGAnaData_setVal(PyObject *self,
                 PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_setVal\n");
#endif
  const char *name;
  const char *unit;
  int no;
  PyArrayObject *o;

  if (!PyArg_ParseTuple(args, "issO", &no, &name, &unit, &o))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      int n = PyArray_ITEMSIZE(o);
      double *data = (double *)PyArray_DATA(o);
      p->setVal(no, name, unit, n, data);
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
EGAnaData_clear(PyObject *self,
                PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_clear\n");
#endif
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  if (p)
  {
    try
    {
      p->clear();
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
EGAnaData_retrieve(PyTypeObject *type,
                   PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_retrieve : 0\n");
#endif
  const char *diag;
  int shotNo;
  int subNo;
  if (!PyArg_ParseTuple(args, "sii", &diag, &shotNo, &subNo))
    return NULL;
  //EGAnaData* p = (EGAnaData *)EGAnaData_alloc(type,0);
  EGAnaData *p = (EGAnaData *)type->tp_alloc(type, 0);
  p->ana = new AnaData();
  try
  {
    *(p->ana) = AnaData::retrieve(diag, shotNo, subNo);
    return (PyObject *)p;
  }
  catch (AnaDataException ex)
  {
    printf("ERROR: %s\n", ex.getMessage().c_str());
    onError("Failed to retrieve data")
  }
}

static PyObject *
EGAnaData_registfile(PyTypeObject *type,
                     PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_registfile\n");
#endif
  const char *diag;
  const char *fileName;
  int shotNo;
  int subNo;
  bool b = true;
  if (!PyArg_ParseTuple(args, "siis", &diag, &shotNo, &subNo, &fileName))
    return NULL;
  try
  {
    AnaData::regist(diag, shotNo, subNo, fileName);
  }
  catch (AnaDataException ex)
  {
    b = false;
    onError("Failed to retrieve data")
  }
  if (b)
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

static PyObject *
EGAnaData_delete(PyTypeObject *type,
                 PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_delete\n");
#endif
  const char *diag;
  int shotNo;
  int subNo;
  bool b = true;
  if (!PyArg_ParseTuple(args, "sii", &diag, &shotNo, &subNo))
    return NULL;
  try
  {
    AnaData::erase(diag, shotNo, subNo);
  }
  catch (AnaDataException ex)
  {
    b = false;
    onError("Failed to delete data")
  }
  if (b)
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

static PyObject *
EGAnaData_search(PyTypeObject *type,
                 PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_search\n");
#endif
  PyObject *o;
  const char *user;
  int lower;
  int upper;
  int subno;
  int limit;
  PyObject *ret = 0;

  bool b = true;
  if (!PyArg_ParseTuple(args, "Oiiizi", &o, &lower, &upper, &subno, &user, &limit))
    return NULL;
  if (!PyList_Check(o))
  {
    onError("Object must be List");
    Py_INCREF(Py_None);
    return Py_None;
  }
  try
  {
    int n = PyList_Size(o);
    int i;
    vector<string> diagAry;
    for (i = 0; i < n; i++)
    {
      PyObject *item = PyList_GetItem(o, i);
      //      const char* diag = PyString_AsString(item);
      const char *diag = PyUnicode_AsUTF8(item);
      diagAry.push_back(string(diag));
    }
    vector<KaisekiInfo> ary;
    AnaData::search(diagAry, lower, upper, subno, user, limit, ary);
    ret = PyList_New(ary.size());
    for (i = 0; i < ary.size(); i++)
    {
      PyObject *_diag = PyString_FromString(ary[i].diagnostics.c_str());
      PyObject *_server = PyString_FromString(ary[i].server.c_str());
      PyObject *_directory = PyString_FromString(ary[i].directory.c_str());
      PyObject *_filename = PyString_FromString(ary[i].filename.c_str());
      PyObject *_userid = PyString_FromString(ary[i].userid.c_str());
      PyObject *_summary = PyString_FromString(ary[i].userid.c_str());
      PyObject *_shotno = PyInt_FromLong(ary[i].shotno);
      PyObject *_subshotno = PyInt_FromLong(ary[i].subshotno);
      PyObject *_filelen = PyInt_FromLong(ary[i].filelen);

      PyObject *item = PyList_New(9);
      PyList_SetItem(item, 0, _diag);
      PyList_SetItem(item, 1, _server);
      PyList_SetItem(item, 2, _directory);
      PyList_SetItem(item, 3, _filename);
      PyList_SetItem(item, 4, _userid);
      PyList_SetItem(item, 5, _summary);
      PyList_SetItem(item, 6, _shotno);
      PyList_SetItem(item, 7, _subshotno);
      PyList_SetItem(item, 8, _filelen);

      PyList_SetItem(ret, i, item);
    }
  }
  catch (AnaDataException ex)
  {
    b = false;
    onError("Failed to retrieve data")
  }
  if (b)
  {
    return ret;
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject *
EGAnaData_regist(PyObject *self,
                 PyObject *args)
{
#ifdef DEBUG
  printf("EGAnaData_regist\n");
#endif
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  AnaData *p = ((EGAnaData *)self)->ana;
  bool b = false;
  if (p)
  {
    try
    {
      b = p->regist();
    }
    catch (AnaDataException ex)
    {
      onError("error occurred in libana");
    }
  }
  if (b)
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

/*
static int
EGAnaData_init(PyObject* self,
	      PyObject* args,
	      PyObject* kwds)
{
  int dimNo;
  int valNo;
  printf("EGAnaData_init\n");
  if (!PyArg_ParseTuple(args,"ii",&dimNo,&valNo))
    return -1;
  EGAnaData* p = createEGAnaData();
  p->ana = new AnaData(dimNo, valNo);
  return 0;
}
*/

static PyObject *
EGAnaData_subtype_new(PyTypeObject *type,
                      PyObject *args,
                      PyObject *kwds)
{
#ifdef DEBUG
  printf("EGAnaData_subtype_new\n");
#endif
  PyObject *tmp, *newobj;

  tmp = EGAnaData_new(&EGAnaDataType, args, kwds);
  if (tmp == NULL)
    return NULL;

  newobj = type->tp_alloc(type, 0);
  if (newobj == NULL)
  {
    Py_DECREF(tmp);
    return NULL;
  }

  ((EGAnaData *)newobj)->ana = new AnaData(*((EGAnaData *)tmp)->ana);
  Py_DECREF(tmp);

  return newobj;
}

static PyObject *
EGAnaData_new(PyTypeObject *type,
              PyObject *args,
              PyObject *kwds)
{
#ifdef DEBUG
  printf("EGAnaData_new\n");
#endif
  int dimNo;
  int valNo;
  if (type != &EGAnaDataType)
    return EGAnaData_subtype_new(type, args, kwds);

  if (!PyArg_ParseTuple(args, "ii", &dimNo, &valNo))
    return NULL;

  EGAnaData *p = (EGAnaData *)EGAnaData_alloc(type, 0);
  //EGAnaData* p = (EGAnaData *)type->tp_alloc(type,0); //2010.06.03

  p->ana = new AnaData(dimNo, valNo);
  return (PyObject *)p;
}

// static PyMethodDef EGAnaData_methods[] = {
//     //  {"EGAnaData",EGAnaData_new,METH_VARARGS},
//     {NULL, NULL}};

static struct PyModuleDef EGAnaDataModule = {
    PyModuleDef_HEAD_INIT,
    "_anadata", /* name of module */
    NULL,       /* module documentation, may be NULL */
    -1,         /* size of per-interpreter state of the module,
	       or -1 if the module keeps state in global variables. */
    NULL, NULL, NULL, NULL, NULL};

/* Add a type to a module */
int PyModule_AddType(PyObject *module, const char *name, PyTypeObject *type)
{
  if (PyType_Ready(type))
  {
    return -1;
  }
  Py_INCREF(type);
  if (PyModule_AddObject(module, name, (PyObject *)type))
  {
    Py_DECREF(type);
    return -1;
  }
  return 0;
}

/* Module */
PyMODINIT_FUNC
PyInit__anadata(void)
{
  import_array();
  PyObject *m;

  m = PyModule_Create(&EGAnaDataModule);
  if (m == NULL)
    return NULL;

  PyModule_AddType(m, "_AnaData", &EGAnaDataType);
  return m;
}
