# -*- coding: utf-8 -*-
from setup_teardown import clear, stop_db
from nose.tools import *
from pykt import KyotoTycoon, KTException

d = dict(A="B", C="D")
d2 = {
    "あいうえお": "ABC",
    "かきくけこ": "てすと2",
    }

@with_setup(setup = clear, teardown = stop_db)
@raises(IOError)
def test_err_set_bulk():
    db = KyotoTycoon()
    ret = db.set_bulk(d)

@with_setup(setup = clear, teardown = stop_db)
def test_set_bulk():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set_bulk(d)
    ok_(ret == 2)
    ret = db.get("A")
    ok_(ret == "B")
    ret = db.get("C")
    ok_(ret == "D")
    db.close()

@raises(KTException)
@with_setup(setup = clear, teardown = stop_db)
def test_set_bulk_with_db():
    db = KyotoTycoon("test")
    db = db.open()
    ret = db.set_bulk(d)
    ok_(False)

@with_setup(setup = clear, teardown = stop_db)
def test_set_bulk_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set_bulk(d2)
    ok_(ret == 2)
    ret = db.get("あいうえお")
    ok_(ret == "ABC")
    ret = db.get("かきくけこ")
    ok_(ret == "てすと2")
    db.close()

@with_setup(setup = clear, teardown = stop_db)
def test_set_bulk_atomic():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set_bulk(d, atomic=True)
    ok_(ret == 2)
    ret = db.get("A")
    ok_(ret == "B")
    ret = db.get("C")
    ok_(ret == "D")
    db.close()


