  # powershell script called from appveyor.yml in the build_script: section
  # This is needed because any write to stderr, which is used by gcc and cmake,
  # will invoke a powershell error.  
  # evidently, the appveyor PS environment is carried through:
    echo $env:WXNAME $env:GMNAME
  # those two env. variables determine how the build proceeds.
      if ( $env:platform -Match "mingw64630x8664") 
      {
      $env:WXNAME=""
      $env:GMNAME=""
      cd c:\projects\gdl
      cmd /E:ON /K scripts\mingw-bldgdl
      }
      else
      {
      C:\msys64\usr\bin\bash.exe -lc "cd /c/projects/gdl &&. ./scripts/appveyor_gdl.msys"
      }
