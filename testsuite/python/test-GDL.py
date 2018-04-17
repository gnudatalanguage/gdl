#!/usr/bin/py.test -vv

import math
import numpy
import pytest
import os
import warnings

if 'ADTTMP' in os.environ:
    os.chdir(os.environ['ADTTMP'])
import GDL

class GDLFile (object):
    '''Temporary GDL test source file. The file name is taken from the
    procedure/function definition which has to be in the first line.

    This class is to be used in a `with` statement, which returns the function
    resp. procedure name. After leaving the with block, the file is removed.

    Example:

        code = """
        function myfun
          return, 1
        end
        """
        with GDLFile(code) as name:
            assert GDL.function(name) == 1

    '''
    def __init__(self, code):
        defline = code.strip().split('\n')[0].split()
        if len(defline) > 0 and defline[0].lower() in ('pro', 'function'):
            self.name = defline[1].lower().replace(',','')
            self.fname = self.name + '.pro'
        else:
            self.name = None
            with warnings.catch_warnings():
                warnings.simplefilter("ignore")
                self.fname = os.tmpnam()
        self.code = code

    def __enter__(self):
        fp = file(self.fname, 'w')
        fp.write(self.code)
        fp.close()
        return self.name or self.fname

    def __exit__(self, t, value, trace):
        os.unlink(self.fname)


def test_function_internal():
    '''Call the internal sin() function'''
    
    assert GDL.function('sin', 1.0) == math.sin(1.0)


def test_function_user():
    '''Call the sinus function via a user defined function'''

    code = '''
    function MYSIN, arg
      return, sin(arg)
    end
    '''
    with GDLFile(code) as name:
        assert GDL.function(name, 1.0) == math.sin(1.0)


@pytest.mark.parametrize('arg', [
    'Hello, world', '', '\n',
#    u'Hello, world',
    -1.2, 1e-39, 0.0, 0.05, 1.0, 1e128, float('inf'), float('-inf'),
    -1, 0, 1, 1000, 2**31-1, -2**31,
    -1L, 0L, 1L, 2L**31-1, -2L**31,
#    2**45-1,
#    complex(1.,1.), complex(0,0),
#    numpy.arange(0, 259, dtype=int),
#    numpy.arange(0, 259, dtype=long),
#    numpy.arange(-255, 255, dtype=numpy.int8),
    numpy.arange(0, 259, dtype=numpy.uint8),
    numpy.arange(-255, 259, dtype=numpy.int16),
#    numpy.arange(0, 259, dtype=numpy.uint16),
    numpy.arange(-255, 259, dtype=numpy.int32),
    numpy.arange(0, 259, dtype=numpy.uint32),
    numpy.arange(-1, 1., 0.1, dtype=float),
    numpy.arange(-1, 1., 0.1, dtype=numpy.float32),
    numpy.arange(-1, 1., 0.1, dtype=numpy.float64),
    numpy.arange(-1, 1., 0.1, dtype=numpy.complex64),
    numpy.arange(-1, 1., 0.1, dtype=numpy.complex128),
#    [1,2,3,4,5],
#    (1,2,3,4,5),
#    {'1':2},
])
def test_function_arg_pass_return(arg):
    '''Call a function that just returns its argument, with different data types'''

    code = '''
    function ARG_PASS_RETURN, arg
      return, arg
    end
    '''
    with GDLFile(code) as name:
        assert numpy.all(GDL.function(name, arg) == arg)


def test_pro_internal():
    '''Call the internal setenv procedure'''
    
    s = 'blabla'
    GDL.pro('setenv', 'T1=' + s)


def test_pro_user():
    '''Call the setenv procedure via a user defined procedure'''

    code = '''
    pro USER_SETENV, key, val
      SETENV, key + '=' + val
      print, GETENV(key)
    end
    '''
    s = 'blabla'
    with GDLFile(code) as name:
        GDL.pro(name, 'T1', s)


@pytest.mark.parametrize('arg', [
    'Hello, world', '',
#    u'Hello, world',
    -1.2, 1e-39, 0.0, 0.05, 1.0, 1e128, float('inf'), float('-inf'),
    -1, 0, 1, 1000, 2**31-1, -2**31,
    -1L, 0L, 1L, 2L**31-1, -2L**31,
#    2**45-1,
#    complex(1.,1.),
])
def test_pro_arg_pass(arg):
    '''Call a user defined procedure that stores the value for different
    data types in a file'''
    
    code = '''
    PRO pro_arg_pass, fname, arg
      openw, 5, fname
      printf, 5, arg
      close, 5
    end
    '''
    with GDLFile(code) as name:
        with warnings.catch_warnings():
            warnings.simplefilter("ignore")
            fname = os.tmpnam()
        GDL.pro(name, fname, arg)
        ret = open(fname).read().strip()
        os.unlink(fname)
        assert arg == arg.__class__(ret)


@pytest.mark.xfail
@pytest.mark.parametrize('dtype', [
    numpy.uint8, numpy.int16, numpy.int32, numpy.float32, numpy.float64,
    numpy.complex64, numpy.complex128, numpy.uint32, float])
def test_pro_setvalue(dtype):
    '''Call a user defined procedure that replaces the values of an array with
    its sinus.

    Note that GDL does not support changes in the arguments, so this test is
    expected to fail'''

    
    code = '''pro SETVALUE, arg
    arg[*] = sin(arg)
    end
    '''
    with GDLFile(code) as name:
        arg = numpy.arange(-10, 10., 1.1).astype(dtype)
        ret = numpy.copy(arg)
        GDL.pro(name, ret)
        for a, r in zip(numpy.sin(arg), ret):
            assert a == r


def test_script():
    '''Call a simple script that writes a short file'''
    
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        fname = os.tmpnam()
    arg = 'Hello, world!'
    code = '''openw, 5, '{0}'
    printf, 5, '{1}'
    close, 5
    '''
    with GDLFile(code.format(fname, arg)) as script:
        GDL.script(script)
        ret = open(fname).read().strip()
        os.unlink(fname)
        assert arg == ret


@pytest.mark.skipif(True, reason='This will errornously abort the test suite')
def test_invalid_code():
    '''Call a function with some invalid GDL code'''
    
    code = '''
    PRO invalid_code
       this_is_invalid
    end
    '''
    with GDLFile(code) as name:
        with pytest.raises(GDL.error):
            GDL.pro(name)
