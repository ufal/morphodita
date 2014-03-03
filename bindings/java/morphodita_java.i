%module morphodita_java

%include "../common/morphodita.i"

%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("morphodita_java");
  }
%}
