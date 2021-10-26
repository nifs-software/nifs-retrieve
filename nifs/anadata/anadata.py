# -*- coding: utf-8 -*-
import re
from ._anadata import _AnaData

VER = "$Id: anadata.py,v 1.2 2020/07/15 02:59:48 yoshida Exp $"


class _AnaDataInfo:
    """

    """

    def __init__(self, ary):
        (
            self.diagnostics,
            self.server,
            self.directory,
            self.filename,
            self.userid,
            self.summary,
            self.shotno,
            self.subshotno,
            self.filelen,
        ) = ary


class AnaData(_AnaData):
    """
    [クラス名] AnaData
    <メンバー>
    <説明>
      解析データを保持するクラス
      コードの大部分はベースクラスの _AnaData で実装している。
    """

    def __new__(cls, *av):
        return super(AnaData, cls).__new__(cls, *av)

    #    def __init__(self, *av):
    #        print "AnaData.__init__"
    #        _AnaData.__init__(self,*av)

    def getUserDict(self):
        """
        [メソッド名]getUserDict
        <説明>
         コメントの キー = 値の部分を検索し、Dict 型に格納する
        """
        comment = self.getComment()
        rgx = re.compile("^\s*(.*)=(.*)\s*$")
        ret = {}
        for line in comment.splitlines():
            m = rgx.match(line)
            if m:
                key = m.group(1).strip().upper()
                val = m.group(2).strip()
                ret[key] = val
        return ret

    @classmethod
    def search(cls, diagnostics, upper, lower, subno=1, userid=None, limit=100):
        """
        [メソッド名]search
        <引数>
          diagnostics (list): 計測名のリスト
          upper (int):  ショット番号上限
          lower (int):　ショット番号下限
          subno (int):  サブショット番号
          userid (string) : 登録者
          limit : 検索数上限
        <説明>
         引数にマッチする登録済データの検索を行う。

        """
        r = _AnaData.search(diagnostics, upper, lower, subno, userid, limit)
        return [_AnaDataInfo(e) for e in r]


#    @classmethod
#    def retrieve(self, *av):
#        ana = _AnaData.retrieve(*av)
#        ana.__class__ = AnaData
#        return ana
