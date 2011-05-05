# -*- coding: utf-8 -*-
from nose.tools import *
from pykt import KyotoTycoon, KTException

@raises(IOError)
def test_err_echo():
    db = KyotoTycoon()
    db.echo()

def test_echo():
    db = KyotoTycoon()
    db = db.open()
    ret = db.echo()
    db.close()
    ok_(ret) 
