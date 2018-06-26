
project := "RKLibProject".

project_version := "1.0".

buildfile_version := "1.0".

url_to_src := "https://github.com/JHG777000/RKLib/archive/master.zip".

build RKLibBuild.

 options.

  on test_enable("-t", "--test", "Enable RKLib test.").

  on toolchain_select("-s", "--select_toolchain=tool", "Select toolchain.").

 end options.

 get test_enable.

 get toolchain_select.

 if ( toolchain_select == nil ).

  var toolchain_select := "gcc".

 end if.

 message("Building RKLib...").

 make filepath include_path from "resources" to "include".

 if (!is_win).

  library_names RKLibLibraries("pthreads").

 end if.

 files RKLibFiles("src.directory").

 sources RKLibSource(RKLibFiles,RKLibLibraries).

 compiler RKLibCompilerFlags("-Wall","-I " + include_path).

 toolchain RKLibToolChain(toolchain_select,RKLibCompilerFlags).

 output RKLib("library",RKLibSource,RKLibToolChain).

 if ( test_enable ).

  message("Running RKLibTest...").

  files RKLibTestFiles("Example.c").

  sources RKLibTestSource(RKLibTestFiles,RKLib).

  output RKLibTest("application",RKLibTestSource,RKLibToolChain).

  launch(RKLibTest).

  message("Ran RKLibTest.").

 end if.

end build.

build clean_build.

 message("Cleaning RKLib...").

 clean("build").

end build.

default RKLibBuild.



