pro test_device_properties
  properties=[$
"Device has scalable pixel size (e.g., PostScript)."$
,"Device can output text at an arbitrary angle using hardware."$
,"Device can control line thickness with hardware."$
,"Device can display images."$
,"Device supports color."$
,"Device supports polygon filling with hardware."$
,"Device hardware characters are monospace."$
,"Device can read pixels (i.e., it supports TVRD)."$
,"Device supports windows."$
,"Device prints black on a white background (e.g., printers are plotters)."$
,"Device has no hardware characters."$
,"Device does line-fill style polygon filling in hardware."$
,"Device will apply Hershey-style embedded formatting commands to device fonts."$
,"Device is a pen plotter."$
,"Device can transfer 16-bit pixels."$
,"Device supports Kanji characters."$
,"Device supports widgets."$
,"Device has Z-buffer."$
,"Device supports TrueType fonts."$
]
  f=reverse(byte(string(!d.flags,format='(b19)')))
  w=where(f eq 49b, count)
  if (count gt 0) then print, properties[w], format='(a)'
end
