#!/usr/bin/env python
# -*- coding: utf-8 -*-
import socket
import traceback
import struct
import select
import time
import sys
from nifs.database import chk_exist

MC_ADDR = "225.1.1.1"
MC_PORT = 7001
DIAG_LEN = 36
EXPECTED_SIZE = 4 * 4 + DIAG_LEN


class NewAnaData:
    """
    [クラス名] NewAnaData
    <メンバ>
      diagnostics : 計測名
      shotNo : ショット番号
      subNo : サブショット番号
    <説明>
      解析データの新規登録通知のマルチキャストパケットを扱うクラス
    """

    @classmethod
    def strip(cls, s):
        # idx = s.find(b"\x00")
        # return s[:idx]
        idx = s.find(b"\x00")
        return s[:idx].decode()

    def __init__(self, diagnostics, shotNo, subNo):
        self.diagnostics = diagnostics
        self.shotNo = shotNo
        self.subNo = subNo

    @classmethod
    def get_next_packet(cls, timeout):
        """
        [関数名] get_next_packet(timeout)
        <引数>
          timeout (int ) : タイムアウト秒 (in)
    　　<説明>
          解析データ登録通知のマルチキャストパケットをtimeout秒待って受信し、
          NewAnaData オブジェクトを返す。timeout 秒内にパケットを受信できなかった場合None を返す
        """
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(("", MC_PORT))
        mreq = struct.pack("4sl", socket.inet_aton(MC_ADDR), socket.INADDR_ANY)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

        try:
            while timeout >= 0:
                starttime = time.time()
                ins, outs, errs = select.select([sock], [], [], timeout)
                endtime = time.time()

                if ins:
                    message, address = ins[0].recvfrom(MC_PORT)
                    fmt = "<i"
                    (id,) = struct.unpack(fmt, message[:4])
                    if id == 2:
                        length = len(message) - EXPECTED_SIZE
                        fmt = "<ii%dsii%ds" % (DIAG_LEN, length)
                        id, size, diagnostics, shotNo, subNo, rest = struct.unpack(fmt, message)
                        diagnostics = NewAnaData.strip(diagnostics)
                        return NewAnaData(diagnostics, shotNo, subNo)
                timeout -= endtime - starttime
        except KeyboardInterrupt:
            raise
        except Exception:
            traceback.print_exc()
        finally:
            sock.close()

        return None

    @classmethod
    def wait(cls, diagnostics, shotNo, timeout):
        """
        [関数名] wait(diagnosticcs, shotNo, timeout)
        <引数>
          diagnstics(string) :計測名
          shotno(int) : ショット番号
          timeout (int ) : タイムアウト秒 (in)
        <説明>
          計測名 diagnostics の解析データ登録通知のマルチキャストパケットをtimeout秒待って受信し、
          NewAnaData オブジェクトを返す。timeout 秒内にパケットを受信できなかった場合None を返す
        """
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(("", MC_PORT))
        mreq = struct.pack("4sl", socket.inet_aton(MC_ADDR), socket.INADDR_ANY)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

        try:
            #
            # 変数が参照前に定義されていないと例外となるため定義する
            _diagnostics = ""
            _shotNo = 0
            _subNo = 1
            while timeout >= 0:
                starttime = time.time()
                ins, outs, errs = select.select([sock], [], [], timeout)
                endtime = time.time()

                if ins:
                    message, address = ins[0].recvfrom(MC_PORT)
                    fmt = "<i"
                    (id,) = struct.unpack(fmt, message[:4])
                    if id == 2:
                        length = len(message) - EXPECTED_SIZE
                        fmt = "<ii%dsii%ds" % (DIAG_LEN, length)
                        id, size, _diagnostics, _shotNo, _subNo, rest = struct.unpack(fmt, message)
                        _diagnostics = NewAnaData.strip(_diagnostics)
                        if (diagnostics == _diagnostics) and (shotNo == _shotNo):
                            return NewAnaData(_diagnostics, _shotNo, _subNo)
                timeout -= endtime - starttime

            # check
            if chk_exist(_diagnostics, _shotNo, _subNo):
                print(
                    ("TimeOut(packet lost) Diagnostics='%s' shotno=%d subshotno=%d" % (_diagnostics, _shotNo, _subNo))
                )
                return NewAnaData(_diagnostics, _shotNo, _subNo)

        except Exception:
            traceback.print_exc()
        finally:
            sock.close()

        return None


if __name__ == "__main__":
    import nifs.database

    no = nifs.database.get_current_shot_no()
    print(("no=", no))
    packet = NewAnaData.wait("thomson", no, 180)
    if packet:
        print(("Diagnostics='%s' shotno=%d" % (packet.diagnostics, packet.shotNo)))
    else:
        print("no packet arrived")

    packet = NewAnaData.get_next_packet(30)
    if packet:
        print(("Diagnostics='%s' shotno=%d" % (packet.diagnostics, packet.shotNo)))
    else:
        print("no packet arrived")
