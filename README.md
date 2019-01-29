# Tau statistic speedup
An optimised implementation of the tau statistic (relative prevalence ratio form), originally from R's `IDSpatialStats` package.

I was evaluating the 'elevated prevalence' form of the tau statistic as we had data for the underlying population (i.e. non-cases as well as cases) containing months of disease onset and UTM coordinates of their household. I optimised the implementation of the tau statistic from the development repo of the `IDSpatialStats::get.tau()` function, leading to ~**52x speedup**. 

\hat{\tau}(d_1,d_2) = \frac{\hat{\theta}(d_1,d_2)}{\hat{\theta}(0,\infty)}
where
\hat{\theta}(d_1,d_2) = \frac{\sum_i^N\sum_{j,j\neq i}^N \mathbf{1}(z_{ij}=1,d_{1}\leq d_{ij}<d_2)}{\sum_i^N\sum_{j,j\neq i}^N \mathbf{1}(z_{ij}=0,d_{1}\leq d_{ij}<d_2)}

Thank you to CodeCogs for renderring the mathematical formulae, let's hope they don't close down their domain. Github **still** doesn't seem to think renderring LaTeX in README.md is part of core functionality in developing/describing code!
