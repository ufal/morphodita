import setuptools
import sys

with open('README') as file:
    readme = file.read()

extra_link_args = []
extra_compile_args = ['-std=c++11', '-w']
if sys.platform == "darwin":
    extra_compile_args += ['-stdlib=libc++']
    extra_link_args += ['-stdlib=libc++']

setuptools.setup(
    name             = 'ufal.morphodita',
    version          = '',
    description      = 'Bindings to MorphoDiTa library',
    long_description = readme,
    author           = 'Milan Straka',
    author_email     = 'straka@ufal.mff.cuni.cz',
    url              = 'https://ufal.mff.cuni.cz/morphodita',
    license          = 'MPL 2.0',
    packages         = ['ufal', 'ufal.morphodita'],
    package_data     = {'ufal.morphodita': ['__init__.pyi', 'py.typed']},
    ext_modules      = [setuptools.Extension(
        'ufal.morphodita._morphodita',
        ['ufal/morphodita/morphodita.cpp', 'ufal/morphodita/morphodita_python.cpp'],
        language = 'c++',
        extra_compile_args = extra_compile_args,
        extra_link_args = extra_link_args)],
    classifiers      = [
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: Mozilla Public License 2.0 (MPL 2.0)',
        'Programming Language :: C++',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Libraries'
    ]
)
