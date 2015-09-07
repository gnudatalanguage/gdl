pro LEVEL4

txt='LEVEL4'
message, /continue, txt, level=1
message, /continue, txt, level=0
message, /continue, txt, level=-1
message, /continue, txt, level=-2
end

pro LEVEL3

txt='LEVEL3'
message, /continue, txt, level=1
message, /continue, txt, level=0
message, /continue, txt, level=-1
message, /continue, txt, level=-2
LEVEL4
end

pro LEVEL2

txt='LEVEL2'
message, /continue, txt, level=1
message, /continue, txt, level=0
message, /continue, txt, level=-1
message, /continue, txt, level=-2
LEVEL3
end

pro LEVEL1

txt='LEVEL1'
message, /continue, txt, level=1
message, /continue, txt, level=0
message, /continue, txt, level=-1
message, /continue, txt, level=-2
LEVEL2
end

pro test_message

level1
end

