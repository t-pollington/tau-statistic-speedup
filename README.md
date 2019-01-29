# Tau statistic speedup
An optimised implementation of the tau statistic (relative prevalence ratio form), originally from R's `IDSpatialStats` package.

I was evaluating the 'elevated prevalence' form of the tau statistic as we had data for the underlying population (i.e. non-cases as well as cases) containing months of disease onset and UTM coordinates of their household. I optimised the implementation of the tau statistic from the development repo of the `IDSpatialStats::get.tau()` function, leading to ~**52x speedup**. 

<a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\tau}(d_1,d_2)&space;=&space;\frac{\hat{\theta}(d_1,d_2)}{\hat{\theta}(0,\infty)}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\tau}(d_1,d_2)&space;=&space;\frac{\hat{\theta}(d_1,d_2)}{\hat{\theta}(0,\infty)}" title="\hat{\tau}(d_1,d_2) = \frac{\hat{\theta}(d_1,d_2)}{\hat{\theta}(0,\infty)}" /></a>
which is the ratio of the odds of finding any case *j* which is related (*z_ij = 1, else 0*) to any case *i*, within an annulus region (radii *d_1*,*d_2*) around case *i*, versus the same but at any *d_ij* distance separation (including *d_ij=0*).
Where the odds are estimates as:
<a href="https://www.codecogs.com/eqnedit.php?latex=\hat{\theta}(d_1,d_2)&space;=&space;\frac{\sum_i^N\sum_{j,j\neq&space;i}^N&space;\mathbf{1}(z_{ij}=1,d_{1}\leq&space;d_{ij}<d_2)}{\sum_i^N\sum_{j,j\neq&space;i}^N&space;\mathbf{1}(z_{ij}=0,d_{1}\leq&space;d_{ij}<d_2)}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\hat{\theta}(d_1,d_2)&space;=&space;\frac{\sum_i^N\sum_{j,j\neq&space;i}^N&space;\mathbf{1}(z_{ij}=1,d_{1}\leq&space;d_{ij}<d_2)}{\sum_i^N\sum_{j,j\neq&space;i}^N&space;\mathbf{1}(z_{ij}=0,d_{1}\leq&space;d_{ij}<d_2)}" title="\hat{\theta}(d_1,d_2) = \frac{\sum_i^N\sum_{j,j\neq i}^N \mathbf{1}(z_{ij}=1,d_{1}\leq d_{ij}<d_2)}{\sum_i^N\sum_{j,j\neq i}^N \mathbf{1}(z_{ij}=0,d_{1}\leq d_{ij}<d_2)}" /></a>
which is the ratio of the number of related case pairs within a specified distance versus the same for unrelated case pairs, for *N* cases with indicatory function **1**(⋅). 

The relatedness of a case pair *z_ij*, is determine here using temporal information.

Thank you to [CodeCogs](https://www.codecogs.com/latex/eqneditor.php "CodeCogs LaTeX equation editor, just copy+paste in the HTML they provide") for renderring the mathematical formulae, let's hope they don't close down their domain. Github **still** doesn't seem to think renderring LaTeX in README.md is part of core functionality in developing/describing code!
