from nifs.retrieve.anadata import AnaData
import matplotlib.pyplot as plt


# instatiate AnaData class at shotNo: 80000, sub-shotNo: 1
ana = AnaData(80000, 1)

# retrieve datset of "thomson" diagnostics
ds = ana.retrieve("thomson")

# shot dataset information
print(ds)

# plot Te as function of R varied between 1000 to 1100 [ms].
fig1, ax1 = plt.subplots()
ds.Te.sel(Time=slice(1000, 1100)).plot.line(x="R", ax=ax1)
ax1.set_title("Observed Te by thomson scattering meseurement")

# plot Te in 2-dimension (R - Time)
fig2, ax2 = plt.subplots()
ds.Te.plot(x="Time", y="R", ax=ax2)
ax2.set_title("Observed Te by thomson scattering meseurement")

plt.show()
