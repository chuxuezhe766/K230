from machine import FPIOA, Pin

class YbKey1:
    def __init__(self):
        self._fpioa = FPIOA()
        self._pin_num = 42
        self._fpioa.set_function(self._pin_num, FPIOA.GPIO0 + self._pin_num, ie=1, oe=0)
        self._key = Pin(self._pin_num, Pin.IN, pull=Pin.PULL_UP, drive=7)

    def value(self):
        return self._key.value()

    def is_pressed(self):
        return self._key.value() == 0

class YbKey2:
    def __init__(self):
        self._fpioa = FPIOA()
        self._pin_num = 33
        self._fpioa.set_function(self._pin_num, FPIOA.GPIO0 + self._pin_num, ie=1, oe=0)
        self._key = Pin(self._pin_num, Pin.IN, pull=Pin.PULL_UP, drive=7)

    def value(self):
        return self._key.value()

    def is_pressed(self):
        return self._key.value() == 0

class YbKey3:
    def __init__(self):
        self._fpioa = FPIOA()
        self._pin_num = 43
        self._fpioa.set_function(self._pin_num, FPIOA.GPIO0 + self._pin_num, ie=1, oe=0)
        self._key = Pin(self._pin_num, Pin.IN, pull=Pin.PULL_UP, drive=7)

    def value(self):
        return self._key.value()

    def is_pressed(self):
        return self._key.value() == 0
