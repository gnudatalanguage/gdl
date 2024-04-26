; this is a 'stub' waiting for a real PROFILER procedure written in C++ in GDL
pro profiler, module, clear=cler, code_coverage=code_coverage, data=data, filename=filename, output=output, report=report, reset= reset, system=system
  if ISA(report) and ISA(data) then Message,/informational,"PROFILER is not implemented in GDL, sorry."
end

