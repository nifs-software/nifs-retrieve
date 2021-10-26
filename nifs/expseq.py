# -*- coding: utf-8 -*-
import socket
import traceback
import struct
import select

MC_ADDR = "225.1.1.3"
MC_PORT = 7000


class ExpSeq:
    """
    [クラス名] ExpSeq
    <メンバ>
      seqNo : シーケンス番号
      shotNo : ショット番号
      subNo : サブショット番号
    <説明>
      実験シーケンスのマルチキャストパケットを扱うクラス
    """

    def __init__(self, seqNo, shotNo, subNo):
        self.seqNo = seqNo
        self.shotNo = shotNo
        self.subNo = subNo

    @classmethod
    def get_next_packet(cls, timeout=-1):
        """
        [関数名] get_next_packet(timeout=-1)
        <引数>
          timeout (int ) : タイムアウト秒 (in)
        <説明>
          実験シーケンスのマルチキャストパケットを受信し、ExpSeq オブジェクトを返す。
          timeout が正の場合は timeout 秒だけ待ち、この間にパケットを受信できなかった場合None を返す
        """
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(("", MC_PORT))
        mreq = struct.pack("4sl", socket.inet_aton(MC_ADDR), socket.INADDR_ANY)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

        try:
            if timeout >= 0:
                ins, outs, errs = select.select([sock], [], [], timeout)
            else:
                ins, outs, errs = select.select([sock], [], [])

            if ins:
                message, address = ins[0].recvfrom(MC_PORT)
                length = len(message) - 4 * 5
                fmt = "<iiiii%ds" % length
                # id,size,seqNo,shotNo,subNo,rest = struct.unpack('<iiiiis100',message)
                id, size, seqNo, shotNo, subNo, rest = struct.unpack(fmt, message)
                return ExpSeq(seqNo, shotNo, subNo)
        except KeyboardInterrupt:
            raise
        except Exception:
            traceback.print_exc()

        return None


if __name__ == "__main__":
    expSeq = ExpSeq.get_next_packet(10)
    if expSeq:
        print(("Exp No=", expSeq.shotNo))
    else:
        print("no packet arrived")
