'''
const module: Allows to define constant variables
'''
class _Const:
    '''
    _Const class
    '''
    class ConstError(TypeError): pass

    def __setattr__(self, name, value):
        if self.__dict__.has_key(name):
            raise self.ConstError, "Can't rebind const(%s)" % name
        self.__dict__[name]=value
    '''
    def __getattr__(self, name):
        if self.__dict__.has_key(name) :
            return self.__dict__[name]
        else:
            raise self.ConstError, "Unspecified attribute const(%s)" % name
    '''

import sys
sys.modules[__name__]=_Const()