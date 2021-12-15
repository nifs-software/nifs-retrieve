import numpy as np
from ._retrieve import retrieve as _retrieve
from .rawdata import RawData
from nifs.retrieve.exceptions import RetrieveException

VER = "$Id: voltdata.py,v 1.2 2020/07/15 02:59:48 yoshida Exp $"


class VoltData:
    """
    [クラス名] VoltData
    <メンバー>
    val (numpy.array(ndtype=float32) : 電圧データの配列(numpy.array)
    params (dict) : 収集パラメタ
    <説明>
    電圧データを保持するクラス
    """

    def __init__(self, val, params):
        """
        [コンストラクタ]
        <引数>
        val (string) : 電圧データ float32 の配列の文字列表現
        params (dict) : 収集パラメタ
        """
        self.val = np.fromstring(val, np.float32)
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
        return self.val

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
          電圧データ(VolData オブジェクト)
        <説明>
          引数で指定されたショットを検索し、電圧データオブジェクトを返す
        """
        list = []
        try:
            val = _retrieve(diag, shotno, subno, channel, timeout, True, list)
            params = RawData.build_hash(list)
            data = VoltData(val, params)
            return data
        except Exception:
            raise RetrieveException("Error occurred in _rerieve.retrieve")


if __name__ == "__main__":
    data = VoltData.retrieve("Bolometer", 80000, 1, 1)
    print((data.get_val()))
