%module morphodita_java

%include "../morphodita_lcc_methods.i"

%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("morphodita_java");
  }
%}
