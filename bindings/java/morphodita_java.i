%include "../morphodita.i"

%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("morphodita_java");
  }
%}
