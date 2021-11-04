# -*- coding: utf-8 -*-
import traceback
import xarray as xr
from ._anadata import _AnaData
from nifs.database import get_connection

VER = "$Id: anadata.py,v 1.2 2020/07/15 02:59:48 yoshida Exp $"


class AnaData:
    """
    AnaData
    =======

    class to handle analyzed data stored in analysis (kaiseki) server.
    When retrieving or registering data to kaiseki server,
    wrapper class :py:class:`_AnaData` which is implemeted in libana2 is used.
    Retrieved data is reconstructed as one dataset
    using `xarray package <http://xarray.pydata.org/>`_

    Parameters
    ----------
    shot : int, required
        shot number
    subshot : int, optional
        sub-shot number, by default 1

    Example
    --------
    .. prompt:: python >>> auto

        >>> from nifs.anadata import AnaData
        >>>
        >>> ana = AnaData(80000, 1)
    """

    def __init__(self, shot: int, subshot=1) -> None:
        # set initial values to prevent undefined behaviour.
        self._shot = shot
        self._subshot = subshot
        # open database
        self.database = get_connection()

    def __del__(self) -> None:
        # close database
        self.database.close()

    @property
    def shot(self) -> int:
        """Shot number.

        Returns
        -------
        int
        """
        return self._shot

    def retrieve(self, diagnostics: str) -> xr.Dataset:
        """
        Retrieving analyzed data from kaiseki-server.
        This methods returns a xarray.Dataset object.
        If you want to know about xarray, please see the xarray HP:
        http://xarray.pydata.org/.


        Parameters
        ----------
        diagnostics : str
            name of diagnostics which is registred in the kaiseki-server.

        Returns
        -------
        xarray.Dataset
            dataset of a xarray object.

        Examples
        --------
        .. prompt :: python >>> auto

            >>> from nifs.anadata import AnaData
            >>>
            >>> ana = AnaData(80000, 1)
            >>> # Retrieving dataset of thomson
            >>> ds = ana.retrieve("thomson")
            >>> ds
            <xarray.Dataset>
            Dimensions:       (Time: 96, R: 137)
            Coordinates:
            * Time          (Time) float64 0.0 33.0 66.0 ... 3.1e+03 3.133e+03 3.166e+03
            * R             (R) float64 2.525e+03 2.55e+03 ... 4.884e+03 4.895e+03
            Data variables:
                Te            (Time, R) float64 84.0 0.0 38.0 47.0 ... 5.0 73.0 5.0 125.0
                dTe           (Time, R) float64 2.178e+04 0.0 40.0 ... 2.178e+04 2.178e+04
                n_e           (Time, R) float64 9.0 138.0 16.0 4.0 ... 38.0 11.0 0.0 16.0
                dn_e          (Time, R) float64 8.0 63.0 7.0 6.0 7.0 ... 13.0 8.0 11.0 7.0
                laser         (Time, R) float64 5.209e+03 5.209e+03 ... 4.373e+03 4.373e+03
                laser number  (Time, R) float64 5.0 5.0 5.0 5.0 5.0 ... 1.0 1.0 1.0 1.0 1.0
            Attributes:
                diagnostics:  thomson
                description:  density (ne) is only for very rough information. Do not use...
        """
        ana = _AnaData.retrieve(diagnostics, self.shot, self._subshot)

        dim_num = ana.getDimNo()
        val_num = ana.getValNo()

        coords = {ana.getDimName(i): ana.getDimData(i) for i in range(dim_num)}
        data_vars = {
            ana.getValName(i): ([key for key in coords.keys()], ana.getValData(i), {"units": ana.getValUnit(i)})
            for i in range(val_num)
        }
        attrs = {"diagnostics": diagnostics, "description": ana.getComment(), "shot_number": self.shot}

        ds = xr.Dataset(data_vars=data_vars, coords=coords, attrs=attrs)
        for i in range(dim_num):
            ds[ana.getDimName(i)].attrs["units"] = ana.getDimUnit(i)

        return ds

    def diagnostics_list(self) -> dict:
        """Registered diagnostics names at a shot number.

        Returns
        -------
        dict
            {name: comment}

        Examples
        --------
        .. prompt :: python >>> auto

            >>> from nifs.anadata import AnaData
            >>> ana = AnaData(80000, 1)
            >>> ana.diagnostics_list()
            {'ha1': 'Time behaviors of visible lines of Halpha and HeI',
             'ech': 'Wave form of ECH pulse',
             'nb1pwr': 'NBI power of BL1',
             'imp01': 'Line intensities of Lyman-alpha and some impurity ions',
             'wp': 'Plasma stored energy estimated by diamagnetic flux measurement',...
            }
        """
        try:
            cursor = self.database.cursor()
            sql = (
                "SELECT d.name, d.diagcomment "
                "FROM diagnostics AS d "
                "JOIN datalocation AS dl ON dl.did = d.id "
                f"WHERE dl.shotno = {self.shot}"
            )
            cursor.execute(sql)
            diag_list = cursor.fetchall()
            return {name: comment for name, comment in diag_list}

        except Exception:
            traceback.print_exc()
