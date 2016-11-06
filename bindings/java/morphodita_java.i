%module morphodita_java

%include "../common/morphodita.i"

%pragma(java) jniclasscode=%{
  static {
    java.io.File libraryFile = new java.io.File(morphodita_java.libraryPath);

    if (libraryFile.exists())
      System.load(libraryFile.getAbsolutePath());
    else
      System.loadLibrary("morphodita_java");
  }
%}

%pragma(java) modulecode=%{
  static String libraryPath;

  static {
    libraryPath = System.mapLibraryName("morphodita_java");
  }

  public static void setLibraryPath(String path) {
    libraryPath = path;
  }
%}
