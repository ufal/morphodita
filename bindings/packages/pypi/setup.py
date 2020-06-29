from sys import platform
from distutils.core import setup, Extension

with open('README') as file:
    readme = file.read()

extra_link_args = []
extra_compile_args = ['-std=c++11', '-fvisibility=hidden', '-w']
if platform == "darwin":
    extra_compile_args += ['-stdlib=libc++']
    extra_link_args += ['-stdlib=libc++']

setup(
    name             = 'ufal.morphodita',
    version          = '',
    description      = 'Bindings to MorphoDiTa library',
    long_description = readme,
    author           = 'Milan Straka',
    author_email     = 'straka@ufal.mff.cuni.cz',
    url              = 'http://ufal.mff.cuni.cz/morphodita',
    license          = 'MPL 2.0',
    py_modules       = ['ufal.morphodita'],
    ext_modules      = [Extension(
        'ufal._morphodita',
        ['morphodita/morphodita.cpp', 'morphodita/morphodita_python.cpp'],
        language = 'c++',
        include_dirs = ['morphodita/include'],
        extra_compile_args = extra_compile_args,
        extra_link_args = extra_link_args)],
    classifiers = [
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: Mozilla Public License 2.0 (MPL 2.0)',
        'Programming Language :: C++',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Libraries'
    ]
)
