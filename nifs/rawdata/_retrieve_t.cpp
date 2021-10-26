#include "stdio.h"
#include "retrieve.h"

#if defined(_WIN32) || defined(WIN64)
#pragma warning(disable : 4786)
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef short int16_t;
#endif

#if defined(__linux__) || defined(__MACH__)
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#endif

#if defined(WIN32) || defined(WIN64)
#pragma warning(disable : 4996)
#include <winsock2.h>
#endif // 2011-06-16 s.i

#ifndef INDEX_ENV_UNDEFINED
#define INDEX_ENV_UNDEFINED -101
#endif

#ifndef RETRIEVE_T_VERSION
#define RETRIEVE_T_VERSION "15.0.1"
#endif

#if defined(__linux__) || defined(__MACH__)
#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif
#ifndef _MAX_PATH // 2011-06-16 s.i
#define _MAX_PATH 256
#endif
#ifndef NAME_SEPARATOR
#define NAME_SEPARATOR '/'
#endif
#else
#ifndef NAME_SEPARATOR
#define NAME_SEPARATOR '\\'
#endif
#endif

#define RAW_DATA 1
#define ANA_DATA 2
#define PRM_DATA 3
#define INDEX_ENV_UNDEFINED -101

extern "C"
{
#include <Python.h>
  //#include "numpy/arrayobject.h"
  void init_retrieve_t();
};

static PyObject *ErrorObject;

const int bSilent = 1;

static int ret_t_error_print(int ret, const char *opt24, const char *opt25)
{
  switch (ret)
  {
  case 10:
    printf("Parameter is illegal.\n");
    break;
  case 20:
    printf("Memory allocation error!\n");
    break;
  case 24:
    printf("Fail to connect Index Server from %s!\n", opt24);
    break;
  case 25:
    printf("Fail to connect Trans Server from %s!\n", opt25);
    break;
  case 29:
    printf("Index protocole version disagreement!\n");
    break;
  case 30:
    printf("Arc shot data retrieve on DATABASE but not found!\n");
    break;
  case 31:
    printf("Socket time out!\n");
    break;
  case 32:
    printf("Trans protocole version disagreement!\n");
    break;
  case 33:
    printf("Fail to get Data Location from Index Server!\n");
    break;
  case 34:
    printf("Fast channel number within extent!\n");
    break;
  case 35:
    printf("Last channel number within extent!\n");
    break;
  case 36:
    printf("Can not extract of compressed data!\n");
  case 37:
    printf("Arc shot data retrieve on O2 data base but not found!(Abnomal porosess of Transd)\n");
    break;
  case 40:
    printf("Arc shot channel data transed error!\n");
    break;
  case 42:
    printf("Unmatch Attribute\n");
    break;
  case 53:
    printf("Not supported DTS info\n");
    break;
  case 54:
    printf("No DTS link Data. Please check setup for Diagnostics\n");
    break;
  case 55:
    printf("No Diagnostics name\n");
    break;
  case 56:
    printf("No Host ID\n");
    break;
  case 57:
    printf("No DTS Data.\n");
    break;
  default:
    printf("Error Code:%d", ret);
    if (ret < 0)
    {
      printf("\t[ %s ]", retrieveErrorMessage(ret));
    }
    printf("\n");
    break;
  }

  return ret;
}

static PyObject *
retrieve_t_retrieve_main(const char *Diag_Name,
                         int AShot_Number,
                         int ASub_Shot_Number,
                         int ch,
                         int TimeoutSec)
{
#ifdef _DEBUG
  //	char *IndexServer="DasIndex::indextest";
  char *IndexServer = "DasIndex";
#else
  char *IndexServer = getenv("INDEXSERVERNAME");
#endif
  char Server[32] = {'\0'};
  int ret;
  int rd = -1;
  short bSilent = 1;
  short bDouble = 1;

  if (!IndexServer)
  {
    printf("Environment variable INDEXSERVERNAME unknown!\n");
    return NULL;
  }

  uint32_t retShot;
  uint16_t retSubShot;
  char DTSsource[64], DTShostID[64], DTSmoduleID[64], CLKsource[64], CLKhostID[64], CLKmoduleID[64];
  short DTStriggerChannel, DTSclockChannel;
  char strDTStriggerChannel[64], strDTSclockChannel[64], ExtOrInt[64], InternalClock[64], SamplingInterval[64], PreSampling[64];
  int32_t DTSuserDefine;
  int64_t DTStimeArraySize;
  int iPreSamplings, iSamplingInterval, ClkDTSid;
  char ClkDTSmodule[128];

  int64_t LastCount;
  short t_ch = (short)ch;
  int type_channel = sizeof(t_ch);
  const char *p_channel = (char *)&(t_ch);
  int real_channel;
  unsigned int last_ch = 0;

  void *timeArray = NULL;
  void *tmp_clock = NULL;
  void *tmp_start = NULL;

  PyObject *retVal = 0;
  int needDTSlink = 1;
  char emsg[256];

  if (AShot_Number < 56221)
  { // DTS LINK INFO. from DTSinfo table.
    needDTSlink = 0;
  }

  try
  {
    rd = retrieveOpenWait(Diag_Name, IndexServer, AShot_Number, ASub_Shot_Number, TimeoutSec);
    if (0 > rd)
    {
      sprintf(emsg, "Error %d [ %s ] !\n", rd, retrieveErrorMessage(rd));
      throw emsg;
    }
    char management[32], comment[128], cserver[128];
    int comment_size = 128;
    short retYear, retMonth, retDay, retHour, retMinute, retSecond;

    ret = retrieveShotInfo(rd, &last_ch, &retYear, &retMonth, &retDay, &retHour, &retMinute, &retSecond,
                           management, comment, comment_size, cserver);
    if (0 > ret)
    {
      sprintf(emsg, "Error %d [ %s ] !\n", rd, retrieveErrorMessage(rd));
      ret_t_error_print(ret, IndexServer, Server);
      throw emsg;
    }
    ret = 0;
    if (AShot_Number < 56221)
    { // DTS LINK INFO. from DTSinfo table.
      int32_t array_size;
      ret = retrieveGetDTSinfox(Diag_Name, AShot_Number, ASub_Shot_Number, t_ch, t_ch, IndexServer,
                                DTSsource, DTShostID, DTSmoduleID,
                                &DTStriggerChannel, &DTSclockChannel, &DTSuserDefine, &array_size,
                                ExtOrInt, &iPreSamplings, &iSamplingInterval, &ClkDTSid, ClkDTSmodule);
      if (ret)
      {
        printf("Error from DLL( retrieveGetDTSinfox )! %d\n", ret);
        ret_t_error_print(ret, IndexServer, Server);
        return 0;
      }
      DTStimeArraySize = array_size;
      // DTStriigerホストとDTSclockホストは同一である。
      strcpy(CLKsource, DTSsource);
      strcpy(CLKhostID, DTShostID);
      strcpy(CLKmoduleID, DTSmoduleID);
      sprintf(strDTStriggerChannel, "%d", DTStriggerChannel);
      sprintf(strDTSclockChannel, "%d", DTSclockChannel);
    }
    //	Retrieveからパラメータ文字列を取得	パラメータからDTSリンク情報を取り出す
    ret = retrieveGetDTSinfoFromRetrieve_ex(rd, type_channel, p_channel, needDTSlink, &real_channel,
                                            &retShot, &retSubShot,
                                            DTSsource, DTShostID, DTSmoduleID, strDTStriggerChannel,
                                            CLKsource, CLKhostID, CLKmoduleID, strDTSclockChannel,
                                            &DTSuserDefine, &DTStimeArraySize, ExtOrInt,
                                            InternalClock, SamplingInterval, PreSampling, &LastCount);
    //	DTSリンク情報から時間軸配列を作成
    if (0 != ret)
    {
      sprintf(emsg, "Error from DLL( retrieveGetDTSinfoFromRetrieve )! %d\n", ret);
      ret_t_error_print(ret, IndexServer, Server);
      throw emsg;
    }

    timeArray = new double[DTStimeArraySize];

    ret = retrieveGetDTSdatax2_ex(IndexServer,
                                  DTSsource, DTShostID, DTSmoduleID, strDTStriggerChannel,
                                  CLKsource, CLKhostID, CLKmoduleID, strDTSclockChannel,
                                  &DTSuserDefine, DTStimeArraySize, retShot, retSubShot,
                                  ExtOrInt, InternalClock, SamplingInterval, PreSampling, LastCount, bSilent,
                                  bDouble, timeArray, tmp_clock, tmp_start);
    if (ret)
    {
      sprintf(emsg, "Error from DLL( retrieveGetDTSdatax2 )! %d\n", ret);
      ret_t_error_print(ret, IndexServer, Server);
      throw emsg;
    }

    retVal = PyBytes_FromStringAndSize((const char *)timeArray, DTStimeArraySize * sizeof(double));
  }
  catch (const char *mes)
  {
    printf(emsg);
    retVal = 0;
  }

  if (rd >= 0)
    retrieveClose(rd);
  return retVal;
}

static PyObject *
retrieve_t_retrieve(PyObject *self,
                    PyObject *args)
{
  char *diag;
  int shotno;
  int subno;
  int ch;
  int timeoutSec;

  if (!PyArg_ParseTuple(args, "siiii", &diag, &shotno, &subno, &ch, &timeoutSec))
  {
    return NULL;
  }

  PyObject *result = retrieve_t_retrieve_main(diag, shotno, subno, ch, timeoutSec);

  if (result == NULL)
  {
    return NULL;
  }

  return result;
}

static PyMethodDef retrieve_methods[] = {
    {"retrieve", retrieve_t_retrieve, METH_VARARGS},
    {NULL},
};

static struct PyModuleDef RetrieveTModule = {
    PyModuleDef_HEAD_INIT,
    "_retrieve_t", /* name of module */
    NULL,          /* module documentation, may be NULL */
    -1,            /* size of per-interpreter state of the module,
	       or -1 if the module keeps state in global variables. */
    retrieve_methods};

PyMODINIT_FUNC
PyInit__retrieve_t(void)
{
  PyObject *m;

  m = PyModule_Create(&RetrieveTModule);
  if (m == NULL)
    return NULL;
  PyObject *d = PyModule_GetDict(m);
  ErrorObject = Py_BuildValue("s", "_retrieve_t.error");
  PyDict_SetItemString(d, "error", ErrorObject);
  return m;
}
