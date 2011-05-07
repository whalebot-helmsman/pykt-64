from pykt import set_wait_callback

def test_wait_callback_fail():
    try:
        set_wait_callback(1)
        assert False
    except TypeError:
        assert True
    except:
        assert False



def test_wait_callback():
    def callback(fd, state):
        pass
    set_wait_callback(callback)
    

