# -*- coding: utf-8 -*-
from nose.tools import *
from pykt import KyotoTycoon, KTException

@raises(IOError)
def test_err_status():
    db = KyotoTycoon()
    db.status()

def test_status():
    db = KyotoTycoon()
    db = db.open()
    ret = db.status()
    ok_(ret)
    db.close()

