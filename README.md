# Tau statistic speedup
An optimised implementation of the tau statistic (relative prevalence ratio form), originally from R's `IDSpatialStats` package. Under the GPL-3.0 license please cite this resource (DoI: **xxxxxxxxxxxxxxxxx**).

## The statistic
I was evaluating the ['elevated prevalence' form](https://journals.plos.org/plosone/article/file?id=10.1371/journal.pone.0155249.s003&type=supplementary#page=6 "Lessler et al. Appendix 5, p6") of the tau statistic [[Lessler et al]](#References) as we had data for the underlying population (i.e. non-cases as well as cases) containing months of disease onset *t<sub>i</sub>* and UTM coordinates of their household (*x<sub>i</sub>*,*y<sub>i</sub>*). I optimised the implementation of the tau statistic from the development repo of the `IDSpatialStats::get.tau()` function, leading to ~**52x speedup**.

<a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\tau}(d_1,d_2)=\frac{\hat{\pi}(d_1,d_2)}{\hat{\pi}(0,\infty)}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\tau}(d_1,d_2)=\frac{\hat{\pi}(d_1,d_2)}{\hat{\pi}(0,\infty)}" title="\hat{\tau}(d_1,d_2)=\frac{\hat{\pi}(d_1,d_2)}{\hat{\pi}(0,\infty)}" /></a>

which is the ratio of the prevalence of any case *j* which is related (*z<sub>ij</sub> = 1, else 0*) to any case *i*, within an annulus region (radii *d<sub>1</sub>*,*d<sub>2</sub>*) around case *i*, versus the same but at any *d<sub>ij</sub>* distance separation (including none i.e. *d<sub>ij</sub>=0*).
Where the prevalence is estimated as:

<a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\pi}(d_1,d_2)=\frac{\sum_{i=1}^N\sum_{j=1,j\neq&space;i}^N\mathbf{1}(z_{ij}=1,d_{1}\leq&space;d_{ij}<d_2)}{\sum_{k=1}^N|\Omega_k(d_1,d_2)|}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\pi}(d_1,d_2)=\frac{\sum_{i=1}^N\sum_{j=1,j\neq&space;i}^N\mathbf{1}(z_{ij}=1,d_{1}\leq&space;d_{ij}<d_2)}{\sum_{k=1}^N|\Omega_k(d_1,d_2)|}" title="\hat{\pi}(d_1,d_2)=\frac{\sum_{i=1}^N\sum_{j=1,j\neq i}^N\mathbf{1}(z_{ij}=1,d_{1}\leq d_{ij}<d_2)}{\sum_{k=1}^N|\Omega_k(d_1,d_2)|}" /></a>

which is the proportion of related case pairs within a specified distance versus the same for any pairs (between cases and/or non-cases), for *N* people with indicator function **1**(⋅). Note a minor correction here versus the original by Lessler et al as we use half-closed distance intervals which was updated by @gilesjohnr on 17 Dec 2018.

The relatedness of a case pair *z<sub>ij</sub>*, is determined here using temporal information if <a href="https://www.codecogs.com/eqnedit.php?latex=|t_j-t_i|<\text{mean&space;serial&space;interval}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?|t_j-t_i|<\text{mean&space;serial&space;interval}" title="|t_j-t_i|<\text{mean serial interval}" /></a>.

## How the speedup was done
Rather than running `IDSpatialStats::get.tau()` function in an R script as described by `?get.tau()`, I isolated the C function responsible (`get_tau` on [line 403](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c) from `spatialfuncs.c` source file for the [2782d6d](https://github.com/HopkinsIDD/IDSpatialStats/commit/2782d6dcc9ee4be9855b5e468ce789425b81d49a "Commit 2782d6d on 17 Dec 2018") commit), applied the four features summarised below, then ran it in an R script by sourcing it with `Rcpp::sourceCpp()` and then calling `getTau()` without needing `library(IDSpatialStats)` [lines 6-10](https://github.com/t-pollington/tau-statistic-speedup/blob/master/run_get_tau.R#L6). I have provided both the R script file `run_get_tau.R` and the C file `get_tau.cpp` containing comments.

1. Stop calls to R within C (~26x speedup)
*Description of previous implementation*: Previously the R function `get.tau()` would call the `get_tau()` C function on lines [403-449](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L403
) (and internally `get_pi()` on [line 427](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L427
)). My `get_tau()` function skips that step for easier reading here; so in essence the heart of the code was described by `get_pi()` on [lines 64-148](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L64
). `get_pi()` has 3 nested loops: over distance windows, then a double loop for paired links between people. The slowdown occurs at [lines 126-129](https://github.com/HopkinsIDD/IDSpatialStats/blob/master/src/spatialfuncs.c#L126
) where the R function `Rfun` is called within C for **each** of the `i`x`j`x`k` loop evaluations.

*Change*: Formulate `Rfun` within `get_tau`. I think this is relatively easy for an R user as if their `Rfun` was a simple if-else loop to choose between cases {1,2,3} (for the three options of the numerator or denominator counts; 'cases' doesn't mean ill people here but options!) then it should be pretty similar in C.

2. Stop repeat evaluations of undirected edges (~2x speedup)
*Description of previous implementation*: In the sum over all people the same link will be visited twice ie *i*->*j* and *j*->*i* but this isn't necessary as <a href="https://www.codecogs.com/eqnedit.php?latex=|t_j-t_i|<\text{mean&space;serial&space;interval}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?|t_j-t_i|<\text{mean&space;serial&space;interval}" title="|t_j-t_i|<\text{mean serial interval}" /></a> is symmetric to *i* and *j* switching due to the modulus function. In most temporally-related or serotype-shared scenarios the pairs are undirected and so the summation is really 'upper triangular' style i.e. <a href="https://www.codecogs.com/eqnedit.php?latex=\sum_{i=1}^{N}\sum_{j=1,j\neq&space;i}^{j<i}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\sum_{i=1}^{N}\sum_{j=1,j\neq&space;i}^{j<i}" title="\sum_{i=1}^{N}\sum_{j=1,j\neq i}^{j<i}" /></a>.

*Change*: So we apply this change and understandably get a ~2x speedup.

3. Split the `posmat` data matrix into multiple vectors (~20% speedup)
*Description of previous implementation*: As the loops sequentially go through `i`, `j` & `k` if posmat is large then the next rows value in memory may not be that close to the current location and this extra memory access time will make things slow.

*Change*: Using vectors for each variable guarantees that the next observation for a variable will be next in memory. 

4. Work with squared distances to avoid `sqrt()` (negligible speedup)
*Description of previous implementation*: To calculate the distance separation *d_<sub>ij</sub>* a Euclidean distance needed to be calculated. 

*Change*: Working with squared distance ranges can then mean that `sqrt` is made redundant. 

## What is the role of IDSpatialStats?

Although I have found enormous speedup improvements (constructing 500 bootstrapped percentile confidence intervals can take tens of hours for a real dataset of 16,000 people) there is still an important role for the IDSpatialStats package in tau statistic calculations. The main speedup by writing the R `Rfun` function in C requires additional user intervention and good understanding of the tau statistic. This then means one has to abandon IDSpatialStats entirely which completely defeats its ease and is not feasible for some users. Given the novelty of this recently introduced statistic, the R package serves an important role for first-time users and those who need to easily compute the tau statistic for a relatively small dataset.

## Replication
Unfortunately I can't share the dataset for replication but can describe what is needed:
* R v3.5.1
* library `Rcpp` for `sourceCpp()`. Note that `IDSpatialStats` isn't required.
* data = R `matrix`-type object with columns named: "ORIG_ID"; "x"; "y"; "onset" and no missing data. For non-cases, the "onset" column should be numerically coded as "-999".

## Features not implemented
* parallel computations across the `for(i){}` loop for *i* in `get_tau.cpp`. I tried using parallel packages in R and C's `#pragma omp parallel for` with `#include <omp.h>` but to no avail.
* GPU computations. A good starting place for rapid code development is MATLAB's `gpuArray` class.
* separating distance calculations from the Rfun code didn't lead to a speedup. Even though distances are needless calculated multiple times, the slowdown probably comes from storing these distances in slower-to-access caches, when a just-in-time method works faster.
* **Have you found an even faster way to do this? I'm open in principle to pull requests to this repo but message me to check.**
* **Found a bug or even a typo? I'd love to know! No I'm not one of those scientists who gets irrationally offended when things get criticised. We're not perfect and we should all be open to criticism so we can do the best science for infectious disease modelling.**

## References
*  Lessler J, Salje H, Grabowski MK, Cummings DAT. *Measuring Spatial Dependence for Infectious Disease Epidemiology*. PLoS One 2016; 11: 5–1–13. doi: [10.1371/journal.pone.0155249](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0155249).
* `HopkinsIDD/IDSpatialStats` (development repo for R's IDSpatialStats package) maintained by @jlessler and @gilesjohnr. Note that we have not used code from the [CRAN read-only mirror](https://github.com/cran/IDSpatialStats) as it is several months behind the development repo.

## Credits
Thanks to [CodeCogs](https://www.codecogs.com/latex/eqneditor.php "CodeCogs LaTeX equation editor, just copy+paste the HTML they provide") for renderring the mathematical formulae, let's hope they don't close down their domain. Github **still** doesn't seem to think renderring LaTeX in README.md is part of core functionality in developing/describing code!
