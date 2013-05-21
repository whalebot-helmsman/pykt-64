from pykt import KyotoTycoon

def clear():
    db = KyotoTycoon()
    db = db.open()
    db.clear()
    db.close()

