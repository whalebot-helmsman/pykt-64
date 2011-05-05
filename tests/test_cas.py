# -*- coding: utf-8 -*-
from nose.tools import *
from pykt import KyotoTycoon, KTException

def clear():
    db = KyotoTycoon()
    db = db.open()
    db.clear()
    db.close()

@raises(IOError)
def test_err_cas():
    db = KyotoTycoon()
    db.cas("A")

@with_setup(setup=clear)
def test_cas():
    db = KyotoTycoon()
    db = db.open()
    db.set("A", "B")
    ret = db.cas("A", oval="B", nval="C")
    ok_(ret == True)
    ret = db.get("A")
    ok_(ret == "C")
    db.close()

@with_setup(setup=clear)
@raises(KTException)
def test_cas_few_param1():
    db = KyotoTycoon()
    db = db.open()
    db.set("A", "B")
    db.cas("A", nval="C")

@with_setup(setup=clear)
def test_cas_few_param2():
    db = KyotoTycoon()
    db = db.open()
    db.set("A", "B")
    ret = db.cas("A", oval="B")
    ok_(ret == True)
    ret = db.get("A")
    ok_(ret == None)
    db.close()

#TODO utf8 test
