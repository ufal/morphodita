#!/bin/sh

set -e

src=`dirname $0`/../../src

# Copy needed files
files="include/morphodita.h `make -s -C $src lib_sources`"
while [ "$ori_files" != "$files" ]; do
  ori_files="$files"
  for source in $files; do
    for f in $source `perl -ne '/^\s*#include\s+"([^"]+)"/&&print"$1\n"' $src/$source`; do
      if [ -f $src/`dirname $source`/$f ]; then f=`dirname $source`/$f; fi
      if [ ! -f $f ]; then
        mkdir -p `dirname $f`
        cp $src/$f `dirname $f`
        files="$files $f"
      fi
    done
  done
done

# Change local includes to relative
for f in $files; do
  (cd `dirname $f` && perl -pe 's@^\s*#include\s+"([^"]+)"@"#include \"".(-f $1 ? $1 : -f "../$1" ? "../$1" : -f "../../$1" ? "../../$1" : die "Cannot find include $1")."\""@e;' -i `basename $f`)
done

# Modify export and import attributes
perl -e '
  $old=<<EOF;
// Export attributes
#ifdef _WIN32
  #ifdef BUILDING_DLL
    #define EXPORT_ATTRIBUTES __attribute__ ((dllexport))
  #else
    #define EXPORT_ATTRIBUTES
  #endif
#else
  #define EXPORT_ATTRIBUTES __attribute__ ((visibility ("default")))
#endif
EOF
  $new=<<EOF;
// Export attributes
#define EXPORT_ATTRIBUTES
EOF
  $old_quoted = quotemeta $old;
  undef $/;
  $file = <>;
  $file =~ s/$old_quoted/$new/ or die "Cannot find the following block in common.h.\n\n$old\nTerminating";
  print $file;
' -i common.h

perl -e '
  $old=<<EOF;
// Import attributes
#if defined(_WIN32) && !defined(MORPHODITA_STATIC)
  #define MORPHODITA_IMPORT __declspec(dllimport)
#else
  #define MORPHODITA_IMPORT
#endif
EOF
  $new=<<EOF;
// Import attributes
#define MORPHODITA_IMPORT
EOF
  $old_quoted = quotemeta $old;
  undef $/;
  $file = <>;
  $file =~ s/$old_quoted/$new/ or die "Cannot find the following block in morphodita.h.\n\n$old\nTerminating";
  print $file;
' -i include/morphodita.h
