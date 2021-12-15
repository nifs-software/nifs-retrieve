import numpy
from ._retrieve import retrieve as _retrieve
from nifs.retrieve.database import _dbstore
from nifs.retrieve.exceptions import RetrieveException, DBException

VER = "$Id: rawdata.py,v 1.2 2020/07/15 02:59:48 yoshida Exp $"

DBSTORE_TYPE_STRING = 1
DBSTORE_TYPE_BYTE = 2
DBSTORE_TYPE_SHORT = 3
DBSTORE_TYPE_INT = 4
DBSTORE_TYPE_FLOAT = 5
DBSTORE_TYPE_DOUBLE = 6
DBSTORE_RAW_DATA = 1
DBSTORE_ANA_DATA = 2


class RawData:
    """
    <メンバー>
      val (string) : 生データをstringで表現したもの
      params (dict) : 収集パラメタ
    <説明>
      生データを保持するクラス
    """

    def __init__(self, val, params):
        """
        [コンストラクタ]
        <引数>
          val (string) : 生データを文字列で表現したもの
          params (dict) : 収集パラメタ
        """
        self.val = val
        self.params = params

    def get_val(self):
        """
        [関数名] get_val
        <引数>
          なし
        <返値>
          numpy.array の配列
        <説明>
          データを int の配列(numpy.array)に変換して返す
        """

        params = self.params
        val = self.val
        bits = 0
        if "Resolution(bit)" in params:
            bits = int(params["Resolution(bit)"])
        elif "ImageType" in params:
            if params["ImageType"] == "INT8":
                bits = 8
            elif params["ImageType"] == "INT16":
                bits = 16
            elif params["ImageType"] == "INT32":
                bits = 32

        if bits <= 8:
            ary = numpy.fromstring(val, numpy.int8)
        elif bits <= 16:
            ary = numpy.fromstring(val, numpy.int16)
        else:
            ary = numpy.fromstring(val, numpy.int32)

        if "ModuleType" in params:
            type = params["ModuleType"]
            if type == "PXI6133":
                ary >> 2

        return ary

    @classmethod
    def dbsopen(cls, emailAddress, diagName, shotNo, subNo):
        """
        [関数名] dbsopen
        <引数>
          emailAddress : (string) メールアドレス
          diagName : (string) 計測名
          shotNo : (int ) ショット番号
          subNo : (int )サブショット番号
          obj : (RawDataオブジェクトまたは RawData オブジェクトの配列)
        <返値>
          書き込み用ハンドル
        <説明>
          dbstore で保存するため、セッションをオープンする。
        """
        try:
            d = _dbstore.open(emailAddress, diagName, shotNo, subNo, DBSTORE_RAW_DATA)
            return d
        except Exception:
            raise RetrieveException("_dbstore.dbopen(%s,%d,%d)" % (diagName, shotNo, subNo))

    @classmethod
    def dbsclose(cls, handle):
        """
        [関数名] dbsclose
        <引数>
          handle : (int) dbstore 用ハンドル
        <返値>
          なし
        <説明>
          dbstore のセッションをクローズする
        """
        try:
            _dbstore.close(handle)
        except Exception:
            raise DBException("Error occurred in _dbstore.close")

    @classmethod
    def dbsabort(cls, handle):
        """
        [関数名] dbsabort
        <引数>
          handle : (int) dbstore 用ハンドル
        <返値>
          なし
        <説明>
          dbstore のセッションを中断する
        """
        try:
            _dbstore.abort(handle)
        except Exception:
            raise RetrieveException("Error occurred in _dbstore.close")

    @classmethod
    def dbswrite(cls, handle, ch, obj):
        """
        [関数名] store
        <引数>
          ch : (int )チャンネル番号
          obj : (RawDataオブジェクトまたは RawData オブジェクトの配列)
        <返値>
          True / False
        <説明>
          handle で結びつけられたセッションを使って dbstore で保存する。
          objs が RawData オブジェクトの場合、そのオブジェクトを チャンネル番号 ch に登録する。
          objs が list オブジェクトの場合、配列の各メンバー(RawData オブジェクト)をチャンネル番号 ch から順に登録する
        """
        try:
            if isinstance(obj, RawData):
                data = obj
                param_list = data.__build_param_list(data.params)
                image_type = data.__get_image_type(data.val)
                ret = _dbstore.write(handle, ch, data.val, param_list, image_type)
            elif isinstance(obj, list):
                chNo = ch
                for data in obj:
                    if isinstance(data, RawData):
                        param_list = data.__build_param_list(data.params)
                        image_type = data.__get_image_type(data.val)
                        ret = _dbstore.write(handle, chNo, data.val, param_list, image_type)
                        if not ret:
                            break
                    chNo += 1
            return ret
        except Exception:
            raise RetrieveException("Error occurred in _dbstore")

    def __get_image_type(self, v):
        if isinstance(v, numpy.ndarray):
            typeName = v.dtype.name
            if typeName == "int32" or typeName == "uint32":
                return "INT32"
            elif typeName == "int16" or typeName == "u int16":
                return "INT16"
            elif typeName == "int8" or typeName == "uint8":
                return "INT8"
        else:
            return "INT8"

    def __get_typecode(self, o):
        t = type(o)
        if t == numpy.int8 or t == numpy.uint8:
            return DBSTORE_TYPE_BYTE
        elif t == numpy.int16 or t == numpy.uint16:
            return DBSTORE_TYPE_SHORT
        elif t == int or t == numpy.int32 or t == numpy.uint32:
            return DBSTORE_TYPE_INT
        elif t == numpy.float32:
            return DBSTORE_TYPE_FLOAT
        elif t == "float" or t == numpy.float64:
            return DBSTORE_TYPE_DOUBLE
        else:
            return DBSTORE_TYPE_STRING

    def __build_param_list(self, hash):
        ret = []
        keys = list(hash.keys())
        keys.sort()
        for key in keys:
            val = hash[key]
            t = self.__get_typecode(val)
            ret.append([key, str(val), t])
        return ret

    @classmethod
    def build_hash(cls, list):
        h = {}
        for n, t, v in list:
            if t == DBSTORE_TYPE_STRING:
                h[n] = v
            elif t == DBSTORE_TYPE_BYTE:
                h[n] = numpy.int8(v)
            elif t == DBSTORE_TYPE_SHORT:
                h[n] = numpy.int16(v)
            elif t == DBSTORE_TYPE_INT:
                h[n] = numpy.int32(v)
            elif t == DBSTORE_TYPE_FLOAT:
                h[n] = numpy.float32(v)
            elif t == DBSTORE_TYPE_DOUBLE:
                h[n] = numpy.float64(v)
        return h

    @classmethod
    def retrieve(cls, diag, shotno, subno, channel, timeout=0):
        """
        [関数名] retrieve(diag,shotno,subno,chNo,timeout=0)
        <引数>
          diag (string) :  計測名 (in)
         shotno (int) :  ショット番号 (in)
          subno (int) : サブショット番号 (in)
          chNo (int) : チャンネル番号 (in)
          timeout (int ) : タイムアウト秒 (in)
        <返値>
          生データ(RawData オブジェクト)
        <説明>
          引数で指定されたショットを検索し、生データオブジェクトを返す
        """
        list = []
        try:
            val = _retrieve(diag, shotno, subno, channel, timeout, False, list)
            params = RawData.build_hash(list)
            data = RawData(val, params)
            return data
        except Exception:
            raise RetrieveException(
                "Error occurred in _rerieve.retrieve({:%s}, {:%d}, {:%d}, {:%d})".format(diag, shotno, subno, channel)
            )


if __name__ == "__main__":
    data = RawData.retrieve("Bolometer", 80000, 1, 1)
    print((data.get_val()))
