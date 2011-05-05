# -*- coding: utf-8 -*-
from nose.tools import *
from pykt import KyotoTycoon, KTException

@raises(IOError)
def test_err_report():
    db = KyotoTycoon()
    db.report()

def test_report():
    db = KyotoTycoon()
    db = db.open()
    ret = db.report()
    ok_(ret)
    db.close()

