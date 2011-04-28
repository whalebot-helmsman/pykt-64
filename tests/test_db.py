from pykt import KyotoTycoon

def test_init():
    db = KyotoTycoon()
    assert(db != None)

def test_open():
    db = KyotoTycoon()
    db = db.open()
    assert db != None

def test_close():
    db = KyotoTycoon()
    db = db.open()
    ret = db.close()
    assert ret 

def test_non_connect_close():
    db = KyotoTycoon()
    ret = db.close()
    assert ret == False


