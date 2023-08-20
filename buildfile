project := "VortexProject".
project_version := "1.06".
buildfile_version := "1.0".
url_to_src := "https://github.com/JHG777000/Vortex/archive/main.zip".

build main_build.
 
 options.
  on test_enable("-t", "--test", "Enable Vortex test.").
  on toolchain_select("-s", "--select_toolchain=tool", "Select toolchain.").
 end options.
 
 get test_enable.
 get toolchain_select.

 if ( toolchain_select == nil ).
  var toolchain_select := "gcc".
 end if.

 message("Building Vortex...").
 
 make filepath threads_buildfile_path from "resources" to "threads/buildfile".
 files ThreadsBuildFile(threads_buildfile_path).
 subproject ThreadsProject("local",ThreadsBuildFile,"-d").
 return_output ThreadsProject.
 
 make filepath include_path from "resources" to "include".
 make filepath threads_include_path from "resources" to "source" from ThreadsProject.
 files VortexFiles("src.directory").
 
 grab Threads from ThreadsProject.
 
 sources VortexSource(Threads,VortexFiles).

 compiler VortexCompilerFlags("-Wall","-I " + threads_include_path, "-I " + include_path).
 toolchain VortexToolChain(toolchain_select,VortexCompilerFlags).
 output Vortex("library",VortexSource,VortexToolChain).

 if ( test_enable ).
  message("Running VortexTest...").
  files VortexTestFiles("Example.c","LexerExample.c").
  sources VortexTestSource(VortexTestFiles,Threads,Vortex).
  output VortexTest("application",VortexTestSource,VortexToolChain).
  launch(VortexTest).
  message("Ran VortexTest.").
 end if.

end build.

build clean_build.
 message("Cleaning Vortex...").
 clean("build").
end build.

default main_build.
