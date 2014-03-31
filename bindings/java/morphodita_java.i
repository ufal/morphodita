%module morphodita_java

%include "../common/morphodita.i"

%pragma(java) jniclasscode=%{
  static {
    java.io.File localMorphodita = new java.io.File(System.mapLibraryName("morphodita_java"));

    if (localMorphodita.exists())
      System.load(localMorphodita.getAbsolutePath());
    else
      System.loadLibrary("morphodita_java");
  }
%}
