from distutils.core import setup, Extension

with open('README') as file:
    readme = file.read()

setup(
    name             = 'ufal.morphodita',
    version          = '',
    description      = 'Bindings to MorphoDiTa library',
    long_description = readme,
    author           = 'Milan Straka',
    author_email     = 'straka@ufal.mff.cuni.cz',
    url              = 'http://ufal.mff.cuni.cz/morphodita',
    license          = 'LGPL',
    py_modules       = ['ufal.morphodita'],
    ext_modules      = [Extension(
        'ufal_morphodita',
        [MORPHODITA_SOURCES],
        language = 'c++',
        include_dirs = ['morphodita/include'],
        extra_compile_args = ['-std=c++11', '-fvisibility=hidden'],
        extra_link_args = ['-s'])],
    classifiers = [
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
        'Programming Language :: C++',
        'Programming Language :: Python',
        'Topic :: Software Development :: Libraries'
    ]
)
