# powershell script called from appveyor.yml in the build_script: section
# This is needed because any write to stderr, which is used by gcc and cmake,
# will invoke a powershell error.

C:\msys64\usr\bin\bash.exe -lc "cd /c/projects/gdl &&. bash ./.ci/build_gdl_windows.msys test"
