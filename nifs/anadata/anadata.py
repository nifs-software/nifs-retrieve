# -*- coding: utf-8 -*-
import re
from ._anadata import _AnaData

VER = "$Id: anadata.py,v 1.2 2020/07/15 02:59:48 yoshida Exp $"


class _AnaDataInfo:

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
    AnaData
    =======

    class to hold analysis data.
    Main methods are implemeted by the base class _AnaData.

    Attributes
    -----------
    retrieve
    """

    def __new__(cls, *av):
        return super(AnaData, cls).__new__(cls, *av)

    #    def __init__(self, *av):
    #        print "AnaData.__init__"
    #        _AnaData.__init__(self,*av)

    def getUserDict(self):
        """
        convert the comments to dict type valueables.
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
    def search(cls, diagnostics, upper=1, lower=0, subno=1, userid=None, limit=100):
        """Search for registered data that maches the arguments.

        Parameters
        ----------
        diagnostics : list
            list of diagnostics name.
        upper : int
            upper shot number, by default 1
        lower : int
            lower shot number, by default 0
        subno : int, optional
            sub-shotnumber, by default 1
        userid : int, optional
            ID number of the registrant, by default None
        limit : int, optional
            maximum nuber of searches, by default 100

        Returns
        -------
        list
            containing _AnaDataInfo instances which have shot information.

        Example
        --------
        .. prompt:: python >>> auto

            >>> from nifs.anadata import Anadata
            >>> AnaData.search("Bolometer", upper=10010, lower=10000)
            [_AnaDataInfo]
        """
        r = _AnaData.search(diagnostics, upper, lower, subno, userid, limit)
        return [_AnaDataInfo(e) for e in r]


#    @classmethod
#    def retrieve(self, *av):
#        ana = _AnaData.retrieve(*av)
#        ana.__class__ = AnaData
#        return ana
