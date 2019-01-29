# Tau statistic speedup
An optimised implementation of the tau statistic (relative prevalence ratio form), originally from R's `IDSpatialStats` package.

## Introduction
I was evaluating the ['elevated prevalence' form](https://journals.plos.org/plosone/article/file?id=10.1371/journal.pone.0155249.s003&type=supplementary#page=7 "Lessler et al. Appendix 6, p7") of the tau statistic as we had data for the underlying population (i.e. non-cases as well as cases) containing months of disease onset and UTM coordinates of their household. I optimised the implementation of the tau statistic from the development repo of the `IDSpatialStats::get.tau()` function, leading to ~**52x speedup**. 

<a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\tau}(d_1,d_2)&space;=&space;\frac{\hat{\theta}(d_1,d_2)}{\hat{\theta}(0,\infty)}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\tau}(d_1,d_2)&space;=&space;\frac{\hat{\theta}(d_1,d_2)}{\hat{\theta}(0,\infty)}" title="\hat{\tau}(d_1,d_2) = \frac{\hat{\theta}(d_1,d_2)}{\hat{\theta}(0,\infty)}" /></a>

which is the ratio of the odds of finding any case *j* which is related (*z<sub>ij</sub> = 1, else 0*) to any case *i*, within an annulus region (radii *d<sub>1</sub>*,*d<sub>2</sub>*) around case *i*, versus the same but at any *d<sub>ij</sub>* distance separation (including *d<sub>ij</sub>=0*).
Where the odds are estimated as:

<a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\theta}(d_1,d_2)&space;=&space;\frac{\sum_i^N\sum_{j,j\neq&space;i}^N&space;\mathbf{1}(z_{ij}=1,d_{1}\leq&space;d_{ij}<d_2)}{\sum_i^N\sum_{j,j\neq&space;i}^N&space;\mathbf{1}(z_{ij}=0,d_{1}\leq&space;d_{ij}<d_2)}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\theta}(d_1,d_2)&space;=&space;\frac{\sum_i^N\sum_{j,j\neq&space;i}^N&space;\mathbf{1}(z_{ij}=1,d_{1}\leq&space;d_{ij}<d_2)}{\sum_i^N\sum_{j,j\neq&space;i}^N&space;\mathbf{1}(z_{ij}=0,d_{1}\leq&space;d_{ij}<d_2)}" title="\hat{\theta}(d_1,d_2) = \frac{\sum_i^N\sum_{j,j\neq i}^N \mathbf{1}(z_{ij}=1,d_{1}\leq d_{ij}<d_2)}{\sum_i^N\sum_{j,j\neq i}^N \mathbf{1}(z_{ij}=0,d_{1}\leq d_{ij}<d_2)}" /></a>

which is the ratio of the number of related case pairs within a specified distance versus the same for unrelated case pairs, for *N* cases with indicatory function **1**(⋅). 

The relatedness of a case pair *z<sub>ij</sub>*, is determined here using temporal information if <a href="https://www.codecogs.com/eqnedit.php?latex=|t_j-t_i|<\text{mean&space;serial&space;interval}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?|t_j-t_i|<\text{mean&space;serial&space;interval}" title="|t_j-t_i|<\text{mean serial interval}" /></a>.

## How the speedup was done
1. Stop calls to R within C (~26x speedup)
2. Stop repeat evaluations of undirected edges (~2x speedup)
3. Split the `posmat` data matrix into multiple vectors (~20% speedup)
4. Work with squared distances to avoid `sqrt()` (negligible speedup)
**explain**

## Replication
Unfortunately I can't share the dataset for replication but can describe what is needed:
* R v3.5.1
* library `Rcpp` for `sourceCpp()`
* data = R `matrix`-type object with named columns: "index"; "xcoord"; "ycoord"; "onset"

## Features not implemented
* parallel computations across the `for(i){}` loop for *i* in `get_tau.cpp`. I tried using parallel packages in R and C's `#pragma omp parallel for` with `#include <omp.h>` but to no avail.
* GPU computations. A good starting place for rapid code development is MATLAB's `gpuArray` class.

## References
*  Lessler J, Salje H, Grabowski MK, Cummings DAT. *Measuring Spatial Dependence for Infectious Disease Epidemiology*. PLoS One 2016; 11: 5–1–13. doi: [10.1371/journal.pone.0155249](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0155249).
* `HopkinsIDD/IDSpatialStats` (development repo for R's IDSpatialStats package) maintained by @jlessler and @gilesjohnr

## Credits
Thanks to [CodeCogs](https://www.codecogs.com/latex/eqneditor.php "CodeCogs LaTeX equation editor, just copy+paste in the HTML they provide") for renderring the mathematical formulae, let's hope they don't close down their domain. Github **still** doesn't seem to think renderring LaTeX in README.md is part of core functionality in developing/describing code!
