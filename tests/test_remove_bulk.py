# -*- coding: utf-8 -*-
from setup_teardown import clear, stop_db
from nose.tools import *
from pykt import KyotoTycoon, KTException

d = dict(A="B", C="D")
d2 = {
    "あいうえお": "ABC",
    "かきくけこ": "てすと2",
    }

@raises(IOError)
def test_err_open():
    db = KyotoTycoon()
    ret = db.remove_bulk(d.keys())


@with_setup(setup = clear, teardown = stop_db)
@raises(TypeError)
def test_err_type():
    db = KyotoTycoon()
    db.open()
    ret = db.remove_bulk(d)

@with_setup(setup = clear, teardown = stop_db)
def test_remove_bulk():
    db = KyotoTycoon()
    db.open()
    ret = db.set_bulk(d)
    ok_(ret == 2)
    ret = db.remove_bulk(d.keys())
    ok_(ret == 2)
    ret = db.get("A")
    ok_(ret == None)
    db.close()

@raises(KTException)
@with_setup(setup = clear, teardown = stop_db)
def test_remove_bulk_with_db():
    db = KyotoTycoon("test")
    db.open()
    db.remove_bulk(d.keys())
    ok_(False)

@with_setup(setup = clear, teardown = stop_db)
def test_remove_bulk_utf8():
    db = KyotoTycoon()
    db.open()
    ret = db.set_bulk(d2)
    ok_(ret == 2)
    ret = db.remove_bulk(d2.keys())
    ok_(ret == 2)
    ret = db.get("あいうえお")
    ok_(ret == None)
    db.close()


@with_setup(setup = clear, teardown = stop_db)
def test_remove_bulk_atomic():
    db = KyotoTycoon()
    db.open()
    ret = db.set_bulk(d2, atomic=True)
    ok_(ret == 2)
    ret = db.remove_bulk(d2.keys(), atomic=True)
    ok_(ret == 2)
    ret = db.get("あいうえお")
    ok_(ret == None)
    db.close()




