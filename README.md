# Tau statistic speedup
An optimised implementation of the tau statistic (relative prevalence ratio form), originally from R's `IDSpatialStats` package.

## The statistic
I was evaluating the ['elevated prevalence' form](https://journals.plos.org/plosone/article/file?id=10.1371/journal.pone.0155249.s003&type=supplementary#page=6 "Lessler et al. Appendix 5, p6") of the tau statistic [[Lessler et al]](#References) as we had data for the underlying population (i.e. non-cases as well as cases) containing months of disease onset *t<sub>i</sub>* and UTM coordinates of their household (*x<sub>i</sub>*,*y<sub>i</sub>*). I optimised the implementation of the tau statistic from the development repo of the `IDSpatialStats::get.tau()` function, leading to ~**52x speedup**.

<a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\tau}(d_1,d_2)=\frac{\hat{\pi}(d_1,d_2)}{\hat{\pi}(0,\infty)}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\tau}(d_1,d_2)=\frac{\hat{\pi}(d_1,d_2)}{\hat{\pi}(0,\infty)}" title="\hat{\tau}(d_1,d_2)=\frac{\hat{\pi}(d_1,d_2)}{\hat{\pi}(0,\infty)}" /></a>

which is the ratio of the prevalence of any case *j* which is related (*z<sub>ij</sub> = 1, else 0*) to any case *i*, within an annulus region (radii *d<sub>1</sub>*,*d<sub>2</sub>*) around case *i*, versus the same but at any *d<sub>ij</sub>* distance separation (including none i.e. *d<sub>ij</sub>=0*).
Where the prevalence is estimated as:

<a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\pi}(d_1,d_2)=\frac{\sum_{i=1}^N\sum_{j=1,j\neq&space;i}^N\mathbf{1}(z_{ij}=1,d_{1}\leq&space;d_{ij}<d_2)}{\sum_{k=1}^N|\Omega_k(d_1,d_2)|}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\pi}(d_1,d_2)=\frac{\sum_{i=1}^N\sum_{j=1,j\neq&space;i}^N\mathbf{1}(z_{ij}=1,d_{1}\leq&space;d_{ij}<d_2)}{\sum_{k=1}^N|\Omega_k(d_1,d_2)|}" title="\hat{\pi}(d_1,d_2)=\frac{\sum_{i=1}^N\sum_{j=1,j\neq i}^N\mathbf{1}(z_{ij}=1,d_{1}\leq d_{ij}<d_2)}{\sum_{k=1}^N|\Omega_k(d_1,d_2)|}" /></a>

which is the proportion of related case pairs within a specified distance versus the same for any pairs (between cases and/or non-cases), for *N* people with indicator function **1**(⋅). Note a minor correction here versus the original by Lessler et al as we use half-closed distance intervals which was updated by @gilesjohnr on 17 Dec 2018.

The relatedness of a case pair *z<sub>ij</sub>*, is determined here using temporal information if <a href="https://www.codecogs.com/eqnedit.php?latex=|t_j-t_i|<\text{mean&space;serial&space;interval}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?|t_j-t_i|<\text{mean&space;serial&space;interval}" title="|t_j-t_i|<\text{mean serial interval}" /></a>.

## How the speedup was done
Rather than running `IDSpatialStats::get.tau()` function in an R script as described by `?get.tau()`, I isolated the C function responsible (`get_tau` on [line 403](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c) from `spatialfuncs.c` source file for the[2782d6d](https://github.com/HopkinsIDD/IDSpatialStats/commit/2782d6dcc9ee4be9855b5e468ce789425b81d49a "Commit 2782d6d on 17 Dec 2018") commit), applied the four features summarised below, then ran it in an R script by sourcing it with `Rcpp::sourceCpp()` and then calling `get_tau()` without needing `library(IDSpatialStats)`. I have provided both the R script file `run_get_tau.R` and the C file `get_tau.cpp` containing comments.

1. Stop calls to R within C (~26x speedup)
*Description of previous implementation*: Previously the R function `get.tau()` would call the `get_tau()` C function on lines [403-449](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L403
) (and internally `get_pi()` on [line 427](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L427
)). My `get_tau()` function skips that step for easier reading here; so in essence the heart of the code was described by `get_pi()` on [lines 64-148](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L64
). `get_pi()` has 3 nested loops: over distance windows, then a double loop for paired links between people. The slowdown occurs at [lines 126-129](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L126
) where the R function `Rfun` is called within C for **each** of the `i`x`j`x`k` loop evaluations.

*Change*: Formulate `Rfun` within `get_tau`. I think this is relatively easy for an R user as if their `Rfun` was a simple if-else loop to choose between cases {1,2,3} (for the three options of the numerator or denominator counts; 'cases' doesn't mean ill people here but options!) then it should be pretty similar in C.

**LINK R SCRIPT FILE**
2. Stop repeat evaluations of undirected edges (~2x speedup)
**explain**
3. Split the `posmat` data matrix into multiple vectors (~20% speedup)
**explain**
4. Work with squared distances to avoid `sqrt()` (negligible speedup)
**explain**

##DIFFICULTY UPDATED IDSPATIALSTATS FOR SOME OF THESE AND GIVE REASON FOR R FUNCTION UTILITY##

## Replication
Unfortunately I can't share the dataset for replication but can describe what is needed:
* R v3.5.1
* library `Rcpp` for `sourceCpp()`. Note that `IDSpatialStats` isn't required.
* data = R `matrix`-type object with named columns: "ORIG_ID"; "x"; "y"; "onset" and no missing data. For non-cases onset should be numerically coded as "-999".

## Features not implemented
* parallel computations across the `for(i){}` loop for *i* in `get_tau.cpp`. I tried using parallel packages in R and C's `#pragma omp parallel for` with `#include <omp.h>` but to no avail.
* GPU computations. A good starting place for rapid code development is MATLAB's `gpuArray` class.
* **Have you found an even faster way to do this? I'm open in principle to pull requests to this repo but message me to check.**

## References
*  Lessler J, Salje H, Grabowski MK, Cummings DAT. *Measuring Spatial Dependence for Infectious Disease Epidemiology*. PLoS One 2016; 11: 5–1–13. doi: [10.1371/journal.pone.0155249](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0155249).
* `HopkinsIDD/IDSpatialStats` (development repo for R's IDSpatialStats package) maintained by @jlessler and @gilesjohnr. Note that we have not used code from the [CRAN read-only mirror](https://github.com/cran/IDSpatialStats) as it is several months behind the development repo.

## Credits
Thanks to [CodeCogs](https://www.codecogs.com/latex/eqneditor.php "CodeCogs LaTeX equation editor, just copy+paste the HTML they provide") for renderring the mathematical formulae, let's hope they don't close down their domain. Github **still** doesn't seem to think renderring LaTeX in README.md is part of core functionality in developing/describing code!
