import numpy as np
from ._retrieve_t import retrieve as _retrieve
from nifs.retrieve.exceptions import RetrieveException

VER = "$Id: timedata.py,v 1.2 2020/07/15 02:59:48 yoshida Exp $"


class TimeData:
    """
    [クラス名] TimeData
    <メンバー>
    val (numpy.array(ndtype=float32) : 電圧データの配列(numpy.array)
    <説明>
    収集タイミングの配列データを保持するクラス
    """

    def __init__(self, val):
        """
        [コンストラクタ]
        <引数>
        val (string) : 時刻 float32 の配列の文字列表現
        """
        self.val = np.fromstring(val, np.float64)

    def get_val(self):
        """
        [関数名] get_val
        <引数>
          なし
        <返値>
          numpy.array の配列
        <説明>
          データを float32 の配列(numpy.array)に変換して返す
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
          時刻配列データ(VolData オブジェクト)
        <説明>
          引数で指定されたショットを検索し、電圧データオブジェクトを返す
        """
        try:
            val = _retrieve(diag, shotno, subno, channel, timeout)
            data = TimeData(val)
            return data
        except Exception:
            raise RetrieveException("Error occurred in _rerieve_t.retrieve")


if __name__ == "__main__":
    data = TimeData.retrieve("Bolometer", 80000, 1, 1)
    print((data.val))
