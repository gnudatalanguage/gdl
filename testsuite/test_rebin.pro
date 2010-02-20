; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro test_rebin
  ; testing the two ways of specifying new dimensions:
  a = randomn(seed,2,3,4,5)
  if $
    ~array_equal(size(a), size(rebin(a,2,3,4,5))) || $
    ~array_equal(size(a), size(rebin(a,[2,3,4,5]))) || $
    ~array_equal([4,6,8,10], size(rebin(a,4,6,8,10), /dim)) || $
    ~array_equal([4,6,8,10], size(rebin(a,[4,6,8,10]), /dim)) $
  then exit, status=1
end
