import numpy as np
import matplotlib.pyplot as plt
from nifs.retrieve.rawdata import RawData


# Bolomgeter の ショット番号=48000, サブショット番号=1, チャンネル番号=1 のデータを取得
data = RawData.retrieve("Bolometer", 48000, 1, 1)

range = data.params["Range"]  # レンジ取得
rangefactor = data.params["RangeFactor"]  # 倍率
resolution = data.params["Resolution(bit)"]  # 解像度(bit数)
volt = (range * rangefactor * 2) / (2 ** resolution)  # 生データを電圧に変換する際の係数

val = data.get_val()  # 生データ (デジタイザーのイメージデータを int の配列にしたもの)
voltdata = (val - ((2 ** resolution) / 2)) * volt  # オフセットを引いて先にもとめた係数をかけ、電圧の配列を得る

clk = data.params["ClockSpeed"]  # サンプリングクロックの取得
timedata = np.arange(0.0, len(val)) / clk  # 電圧データと同じ長さの配列を作り、サンプリングクロックで割る(サンプリング時刻の配列を得る)

plt.title("Bolometer")
plt.xlabel("t [sec]")
plt.ylabel("v [V]")
plt.plot(timedata, voltdata)
plt.show()
