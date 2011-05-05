# -*- coding: utf-8 -*-
from nose.tools import *
from pykt import KyotoTycoon, KTException

def test_init():
    db = KyotoTycoon()
    ok_(db != None)

def test_open():
    db = KyotoTycoon()
    db = db.open()
    ok_(db != None)

def test_close():
    db = KyotoTycoon()
    db = db.open()
    ret = db.close()
    ok_(ret)

def test_non_connect_close():
    db = KyotoTycoon()
    ret = db.close()
    ok_(ret == False)

