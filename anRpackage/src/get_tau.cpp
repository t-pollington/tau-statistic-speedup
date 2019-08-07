//Credit: Inspired and amended from https://github.com/HopkinsIDD/IDSpatialStats/commit/2782d6dcc9ee4be9855b5e468ce789425b81d49a by @gilesjohnr @jlessler
//Author: @t-pollington
//Date: 29 Jan 2019
#include <cstdlib>
#include <iostream>
#include <Rcpp.h>
#define NTYPE unsigned short //fine if N<=65535
using namespace std;
using namespace Rcpp;
// [[Rcpp::export]]

NumericVector getTau(const NumericVector ORIG_ID, const NumericVector x, const NumericVector y, const NumericVector onset, const NumericVector r, const NumericVector r_low, SEXP index){ //see how multiple vector arguments are parsed in here rather than the single posmat matrix
  NTYPE i,j,k;
  double dist2 = 0;
  double r2 = 0;
  double r2_low = 0;
  long long num_cnt, denom_cnt; //counters for those filling conditions//
  unsigned short serialintvl = 7; //mean serial interval of the disease
  unsigned short r_size = r.size();
  NTYPE N = ORIG_ID.size();
  int *inds = INTEGER(index);
  NumericVector tau(r_size, NULL);
  double piInf = 0;
  bool check = 1;
  bool bstrapconflict = 0;
  bool sameperson = 0;
  bool iscasepair = 0;
  bool temprelated = 0;
  bool withindist = 0;

if(*inds==-1){ //if index was set to -1 then it means we can turn off bootstrapping checks 
  check = 0; 
}

num_cnt = 0;
denom_cnt = 0;

//calc piInf
for (i=0;i<N;i++) {
  for (j=0; j<i;j++) {
    bstrapconflict = (inds[i] == inds[j]) && check; //do not compare someone with themself if bootstrapping*/
    sameperson = (ORIG_ID[i]==ORIG_ID[j]); //ie the person migrated to a different place in the study
    denom_cnt = denom_cnt + (!(bstrapconflict)*!(sameperson));
    iscasepair = (onset[i]!=-999) && (onset[j]!=-999);
    temprelated = (abs(onset[i]-onset[j]) <= serialintvl); //could add temporal restrictions here that the pair be within the start/end dates of the study
    num_cnt = num_cnt + (!(bstrapconflict)*!(sameperson)*iscasepair*temprelated);
  }
}
piInf = (double)num_cnt/denom_cnt;

//calc pi(r2_low,r2)
for (k=0;k<r_size;k++) {
  num_cnt = 0;
  denom_cnt = 0;
  r2  = pow(r[k],2); //transformation to squared distances to make sqrt() redundant in this line and the one below
  r2_low = pow(r_low[k],2); 

  for (i=0;i<N;i++) {
    for (j=0; j<i;j++) { //lower triangular access only as undirected pairs assumed
      bstrapconflict = (inds[i] == inds[j]) && check; //do not compare someone with themself if bootstrapping
      sameperson = (ORIG_ID[i]==ORIG_ID[j]);
      dist2 = pow(x[i] - x[j],2) + pow(y[i] - y[j],2); //calculate the distance
      withindist = ((dist2 >= r2_low) && (dist2 < r2));
      if(bstrapconflict||!withindist||sameperson) continue;
      denom_cnt = denom_cnt + 1;
      iscasepair = (onset[i]!=-999) && (onset[j]!=-999);
      temprelated = (abs(onset[i]-onset[j]) <= serialintvl);
      num_cnt = num_cnt + (iscasepair*temprelated);
    }
  }
  tau[k] = (double)num_cnt/denom_cnt; // pi(r.min,r.max)
  tau[k] = (double)tau[k]/piInf;
}
return(tau);
}