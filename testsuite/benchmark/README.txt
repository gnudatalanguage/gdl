
Alain C., 2017-08-17

Suggestions and feedback welcome.

Some data sets are stored here :
http://aramis.obspm.fr/~coulais/IDL_et_GDL/Benchmark/


Benchmarking activities between IDL, GDL and FL
are running since years. Benchmarking does not adress
the quality and the exactness of the computations
(which are usually tested in testsuite/ files)

Now only 4 cases are publicaly available, based
on a common infrastructure (see below "Common files")

bench_fft.pro
bench_matrix_invert.pro
bench_matrix_multiply.pro
bench_median.pro


All these files do contain a related ploting procedure :

plot_bench_fft, plot_bench_matrix_invert, plot_bench_matrix_multiply,
plot_bench_median

All these pro. have some common keywords : path, svg, xrange & yrange ...

You can store XDR files from various computers in various sub-dir
and merge them in the plot using a list of path names
with keyword PATH=['.', path1, Path2 ...]


Common files : 

benchmark_compute_range.pro     : merging X and Y ranges from various XDR files
benchmark_file_search.pro       : looking for files with given prefix in various paths
benchmark_generate_cpuinfo.pro  : saving informations on the computer for intercomparison (i3, i5, i7 ...)
benchmark_generate_filename.pro : standardized ouptup files name (+date)
benchmark_graphic_style.pro     : GDL : green, IDL : red, FL : magenta
benchmark_plot_cartouche.pro    : ploting a "cartouche"
benchmark_svg.pro               : opening and closing SVG files (only in GDL)
