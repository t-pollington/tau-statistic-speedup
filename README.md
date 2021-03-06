# Tau statistic speedup
An optimised implementation of the tau statistic (relative prevalence ratio form), originally from R's `IDSpatialStats` package. Under the GPL-3.0 license please reference this cite-able resource DoI:[10.5281/zenodo.2552849](https://doi.org/10.5281/zenodo.2552849).

* [The statistic](#The-statistic)
* [How the speedup was done](#How-the-speedup-was-done)
* [What is the role of IDSpatialStats?](#What-is-the-role-of-IDSpatialStats)
* [Replication](#Replication)
* [Features not implemented](#Features-not-implemented)
* [References and credits](#References-and-credits)

## The statistic
I was evaluating the ['elevated prevalence' form](https://journals.plos.org/plosone/article/file?id=10.1371/journal.pone.0155249.s003&type=supplementary#page=6 "Lessler et al. Appendix 5, p6") of the tau statistic [[Lessler et al]](#References-and-credits) as we had data on the underlying population (i.e. non-cases as well as cases) containing months of disease onset *t<sub>i</sub>* and UTM coordinates of their household (*x<sub>i</sub>*,*y<sub>i</sub>*). I optimised the implementation of the tau statistic <a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\tau}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\tau}" title="\hat{\tau}" /></a> from the development repo of the `IDSpatialStats::get.tau()` function, leading to a ~**76x speedup**.

<a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\tau}(d_1,d_2)=\frac{\hat{\pi}(d_1,d_2)}{\hat{\pi}(0,\infty)}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\tau}(d_1,d_2)=\frac{\hat{\pi}(d_1,d_2)}{\hat{\pi}(0,\infty)}" title="\hat{\tau}(d_1,d_2)=\frac{\hat{\pi}(d_1,d_2)}{\hat{\pi}(0,\infty)}" /></a>

which is the ratio of the prevalence of any case *j* which is related (*z<sub>ij</sub> = 1, else 0*) to any case *i*, within an annulus region (radii *d<sub>1</sub>*,*d<sub>2</sub>*) around case *i*, versus the same but at any *d<sub>ij</sub>* distance separation (including none i.e. *d<sub>ij</sub>=0*).
Where the prevalence is estimated as:

<a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\pi}(d_1,d_2)=\frac{\sum_{i=1}^N\sum_{j=1,j\neq&space;i}^N\mathbf{1}(z_{ij}=1,d_{1}\leq&space;d_{ij}<d_2)}{\sum_{k=1}^N|\Omega_k(d_1,d_2)|}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\pi}(d_1,d_2)=\frac{\sum_{i=1}^N\sum_{j=1,j\neq&space;i}^N\mathbf{1}(z_{ij}=1,d_{1}\leq&space;d_{ij}<d_2)}{\sum_{k=1}^N|\Omega_k(d_1,d_2)|}" title="\hat{\pi}(d_1,d_2)=\frac{\sum_{i=1}^N\sum_{j=1,j\neq i}^N\mathbf{1}(z_{ij}=1,d_{1}\leq d_{ij}<d_2)}{\sum_{k=1}^N|\Omega_k(d_1,d_2)|}" /></a>

which is the proportion of related case pairs within a specified distance versus the same for any pairs (irrespective of relatedness or being a case), for *N* people with indicator function **1**(⋅). There is a minor correction here versus the original by [[Lessler et al]](#References-and-credits) is that we use half-closed distance intervals which was updated by @gilesjohnr on 17 Dec 2018 in their most recent [2782d6d](https://github.com/HopkinsIDD/IDSpatialStats/commit/2782d6dcc9ee4be9855b5e468ce789425b81d49a "Commit 2782d6d on 17 Dec 2018") commit.

The relatedness of a case pair *z<sub>ij</sub>*, is determined here using temporal information, if <a href="https://www.codecogs.com/eqnedit.php?latex=|t_j-t_i|<\text{mean&space;serial&space;interval}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?|t_j-t_i|<\text{mean&space;serial&space;interval}" title="|t_j-t_i|<\text{mean serial interval}" /></a> then *z_<sub>ij</sub>*=1 else 0.

## How the speedup was done
Rather than running `IDSpatialStats::get.tau()` function in an R script as described by `?get.tau()`, I isolated the C function responsible (`get_tau` on [line 403](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L403) from `spatialfuncs.c` source file for the [2782d6d](https://github.com/HopkinsIDD/IDSpatialStats/commit/2782d6dcc9ee4be9855b5e468ce789425b81d49a "Commit 2782d6d on 17 Dec 2018") commit), coded the four features summarised below, then ran it in an R script by sourcing it with `Rcpp::sourceCpp()` and then calling `getTau()` without needing `library(IDSpatialStats)` on [lines 6-10](https://github.com/t-pollington/tau-statistic-speedup/blob/master/run_get_tau.R#L6). I have provided commented R script file `run_get_tau.R` (in the root) and the C file `get_tau.cpp` (in the /src of the package) containing useful comments. 

1. **Stop calls to R within C** (~26x speedup)

*Previously*: Previously the R function `get.tau()` would call the `get_tau()` C function on lines [403-449](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L403
) (and internally `get_pi()` on [line 427](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L427
)). My `getTau()` function skips that step for easier reading here, not contributing to the speedup; so in essence the heart of the code isolated was described by `get_pi()` on [lines 64-148](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L64
). `get_pi()` has 3 nested loops: over distance windows, then a double loop for paired links between people. The slowdown occurs at [lines 126-129](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L126
) where the R function `Rfun` is called within C for **each** of the `i`x`j`x`k` loop evaluations.

*Change*: Formulate `Rfun` within `getTau`. I think this is relatively easy for an R user as if their `Rfun` was a simple if-else loop to choose between the cases {1,2,3} (for the three options of the numerator or denominator counts; 'cases' don't mean ill people here!), then it should be pretty similar in C.

2. **Stop repeat evaluations of undirected edges** (~2x speedup)

*Previously*: In the sum over all people the same link will be visited twice ie *i*->*j* and *j*->*i* but this isn't necessary as <a href="https://www.codecogs.com/eqnedit.php?latex=|t_j-t_i|<\text{mean&space;serial&space;interval}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?|t_j-t_i|<\text{mean&space;serial&space;interval}" title="|t_j-t_i|<\text{mean serial interval}" /></a> is symmetric to *i*,*j* switching due to the modulus function. In most temporally-related or serotype-shared scenarios the pairs are undirected and so the summation should really be 'upper triangular' style i.e. <a href="https://www.codecogs.com/eqnedit.php?latex=\sum_{i=1}^{N}\sum_{j=1,j\neq&space;i}^{\mathbf{j<i}}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\sum_{i=1}^{N}\sum_{j=1,j\neq&space;i}^{\mathbf{j<i}}" title="\sum_{i=1}^{N}\sum_{j=1,j\neq i}^{\mathbf{j<i}}" /></a>.

*Change*: Understandably by halving the loop we get a ~2x speedup.

3. **Explicitly force the compiler to use AVX2 CPU vectorisation** (~47% speedup)

*Previously*: The standard compiler flags are '-O2' and it is unclear if the loops were fully vectorised given the `if(...) continue;` statements within them. 

*Change*: Since the first version I have built the .cpp file into a minimal package using `Rcpp::Rcpp.package.skeleton()` with empty description and help files. The purpose is that the _configure_ file can set the 'g++' compiler flags to utilise '-O3' optimisation and '-mavx2' vectorisation which is common in CPU architectures; to tweak to your architecture please read [How to change and set Rcpp compile arguments](https://stackoverflow.com/questions/32586455/how-to-change-and-set-rcpp-compile-arguments). All `if()` statements in the inner loops have been replaced with boolean arithmetic. Also reduced the data types of some `int` and `double` types.

4. **Split the `posmat` data matrix into multiple vectors** (~20% speedup)

*Previously*: `posmat` is the matrix argument that gets fed into `get.tau()`. As the loops sequentially go through `i`, `j` & `k`, if posmat is large columnwise then the next row's value in memory may not be that close to the current location and this extra memory access time will add delays.

*Change*: Using vectors for each variable guarantees that the next observation for a variable will be next in memory. 

5. **Work with squared distances to avoid `sqrt()`** (negligible speedup)

*Previously*: To calculate the distance separation *d_<sub>ij</sub>* a Euclidean distance was calculated. 

*Change*: Working with squared distance ranges can make `sqrt()` redundant. 

## What is the role of IDSpatialStats?

Although I have found enormous speedup improvements (constructing 500 bootstrapped percentile confidence intervals can take tens of hours for a real dataset of 16,000 people) there is still an important role for the IDSpatialStats package in tau statistic calculations. The main speedup comes from writing the R `Rfun` function in C, but this requires additional user intervention and good understanding of the tau statistic. So one has to abandon IDSpatialStats entirely which completely defeats its purpose of ease and is not feasible for some users. Given the novelty of this recent statistic, the R package serves an important role for first-time users and those who need to easily compute the tau statistic for a relatively small dataset. This code is also for one form of the statistic while the R package offers much functionality besides.

## Replication
Unfortunately I can't share the dataset but can describe what you need:
* R v3.6.0
* library `Rcpp` for `sourceCpp()`. `IDSpatialStats` isn't required.
* data = R `matrix`-type object with columns named: "ORIG_ID"; "x"; "y"; "onset" and no missing data. For non-cases, the "onset" column should be numerically coded as -999.

## Features not implemented
* parallel computations across the `for(i){}` loop for *i* in `get_tau.cpp`. I tried to no avail using 'RcppArmadillo' to enable `#pragma omp parallel for` within `get_tau.cpp` as there are [concerns that `Rcpp` alone can't deal with OpenMP](https://stackoverflow.com/questions/48069990/multithreaded-simd-vectorized-mandelbrot-in-r-using-rcpp-openmp). Applying `#pragma omp simd` to the loops after the compiler optimisations above gave no speed improvement; there were difficulties applying it since although the for loop over *i* goes from 1 to N the nested *j* loop is of different length which depends on *i*.
* GPU computations. A good starting place for rapid code development would be MATLAB's `gpuArray` class or ArrayFire. My only reservation is the code would need adapting so the distance matrix is calculated on-the-fly only once rather than over the k-loop for the size of the radius intervals required. This would help reduce the amount of GPU device memory accesses from the GPU cores. 
* separating distance calculations from the Rfun code didn't lead to a speedup. Even though distances are needless calculated multiple times, the slowdown probably comes from storing these distances in slower-to-access caches, when a just-in-time method works faster.
* **Have you found an even faster way to do this? I'm open in principle to pull requests to this repo but message me to check.**
* **Found a bug or even a typo? I'd love to know! We're not perfect and we should all be open to criticism so we can do the best science for infectious disease modelling.**

## References and credits
1.  Lessler J, Salje H, Grabowski MK, Cummings DAT. *Measuring Spatial Dependence for Infectious Disease Epidemiology*. PLoS One 2016; 11: 5–1–13. doi: [10.1371/journal.pone.0155249](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0155249).
2. `HopkinsIDD/IDSpatialStats` (development repo for R's IDSpatialStats package) maintained by @jlessler and @gilesjohnr. Code from the [CRAN read-only mirror](https://github.com/cran/IDSpatialStats) hasn't been used as it is several months behind the development repo.
* Thanks to [CodeCogs](https://www.codecogs.com/latex/eqneditor.php "CodeCogs LaTeX equation editor, just copy+paste the HTML they provide") for renderring the mathematical formulae, let's hope they don't close down their site—Github **still** doesn't consider renderring LaTeX in README.md is a core functionality of code development!