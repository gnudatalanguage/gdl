;+
; NAME: ALLANVAR
;
; PURPOSE: compute Allan Variance for a
;
; CATEGORY: statistic
;
; CALLING SEQUENCE: ALLANVAR, input, clu
;
; INPUTS:
;   - input : a 1D array
;
; OPTIONAL INPUTS:
;
; KEYWORD PARAMETERS:
;   - /test : stop at the end, before returning
;   - /help : plot a short help and return without computation
;   - /demo : super basic plot
;   - n_clusters : number of clusters
;   - min_size
;   - max_size
;
; OUTPUTS:
;   - avar : the computed values for the Allan Var
;   - cluster_sizes : the length on which the computation is done
;
; OPTIONAL OUTPUTS: non
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS: none
;
; RESTRICTIONS:
; -- The input array is assumed to be 1D
; -- no test on the presence of Infinity or NaN
;
; PROCEDURE: a lot of doc. & pseudo on the Web.
;
; EXAMPLE:
;    - ALLANVAR, randomu(seed, 100000), /demo
;    - nb=1e6 & ALLANVAR, RANDOMU(seed, nb)+0.01*sin(350.*findgen(nb)/nb),/de
;
; MODIFICATION HISTORY: intitial import in GDL project by AC 2024, Jan 30
;
; This work is a derived work from Nikolay Mayorov
; https://github.com/nmayorov/allan-variance/tree/master
;
; The transposition in IDL/GDL/FL syntax was done by René Gastaud
; 2024-january-25
;
; This code is under MIT licence (same licence thant the original one)
; and with written agreement from the orignal author. This licence is
; compatible with GNU GPL v2+ we use in GDL. Authorship should remain. 
;
; --------------------------
;
; This code was packaged in GDL by Alain C., 30 Jan 2024.
; No equivalent function was found in IDL or associated mainstream
; libs (idlastro, Coyote, ....) If an equivalent does exist, please report.
; This code was tested on diffents signals.
;
; --------------------------
;-
function COMPUTE_CLUSTER_SIZES, nn, min_size=min_size, max_size=max_size, $
                                n_clusters=n_clusters, test=test
;
if ~KEYWORD_SET(min_size) then min_size = 1L
if ~KEYWORD_SET(max_size) then max_size = nn/10
if ~KEYWORD_SET(n_clusters) then n_clusters=100
;;;;
i0 = FINDGEN(n_clusters)/(n_clusters-1)*(alog(max_size)/alog(2))
i0 = i0 + ALOG(min_size)/ALOG(2)
i1 = 2.^i0
i2 = ROUND(i1)
index  = UNIQ(i2)
result = i2[index]
;
if KEYWORD_SET(test) then STOP
;
return, result
;
end
;
; --------------------------
;
pro ALLANVAR, input, avar, cluster_sizes, $
              min_size=min_size, max_size=max_size, n_clusters=n_clusters, $
              test=test, help=help, demo=demo
;
nx = N_ELEMENTS(input)
cluster_sizes = COMPUTE_CLUSTER_SIZES(nx, min_size=min_size, $
                                      max_size=max_size, n_clusters=n_clusters)
xx = TOTAL(DOUBLE(input), /cumulative)
nn = N_ELEMENTS(cluster_sizes)
avar = FLTARR(nn)
for i=0,nn-1 do begin
   k = cluster_sizes[i]
   c = xx[2*k:*] - 2*xx[k:-k-1] + xx[0:-2*k-1]
   avar[i] = MEAN(c*c)/k/k/2
endfor
;
if KEYWORD_SET(demo) then PLOT_OO, cluster_sizes , avar, psym=-2
;
if KEYWORD_SET(test) then STOP
;
end
