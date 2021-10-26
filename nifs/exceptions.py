# -*- coding: utf-8 -*-

VER = "$Id: exceptions.py,v 1.1.1.1 2019/07/23 03:59:43 emo Exp $"


class DBException:
    """
    Exception raised for Database
    """

    def __init__(self, message=""):
        self.message = message


class TimeoutException:
    """
    Exception raised for Timeout
    """

    def __init__(self, message=""):
        self.message = message


class RetrieveException:
    """
    Exception raised for libretrieve
    """

    def __init__(self, message=""):
        self.message = message
