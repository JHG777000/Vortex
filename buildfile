
project := "RKLibProject".

project_version := "1.0".

buildfile_version := "1.0".

url_to_src := "https://github.com/JHG777000/RKLib/archive/master.zip".

build RKLibBuild.

 options.

  on test_enable("-t", "--test", "Enable RKLib test.").

 end options.

 get test_enable.

 message("Building RKLib...\n").

 make filepath include_path from "resources" to "include".

 files RKLibFiles("src.directory").

 sources RKLibSource(RKLibFiles).

 compiler RKLibCompilerFlags("-Wall","-I " + include_path).

 toolchain RKLibToolChain("clang",RKLibCompilerFlags).

 output RKLib("library",RKLibSource,RKLibToolChain).

 if ( test_enable ).

  files RKLibTestFiles("Example.c").

  sources RKLibTestSource(RKLibTestFiles,RKLib).

  output RKLibTest("application",RKLibTestSource,RKLibToolChain).

  launch(RKLibTest).

 end if.

end build.

build clean_build.

 message("Cleaning RKLib...\n").

 clean("build").

end build.

default RKLibBuild.



