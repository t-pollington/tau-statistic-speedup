//Credit: Inspired and amended from https://github.com/HopkinsIDD/IDSpatialStats/commit/2782d6dcc9ee4be9855b5e468ce789425b81d49a by @gilesjohnr @jlessler
//Author: @t-pollington
//Date: 6 Aug 2019
#include <cstdlib>
#include <iostream>
#include <Rcpp.h>
#define NTYPE unsigned short //fine if N<=65535
using namespace std;
using namespace Rcpp;
// [[Rcpp::export]]

NumericVector getTau3(const NumericVector ORIG_ID, const NumericVector x, const NumericVector y, const NumericVector onset, const NumericVector r, const NumericVector r_low, SEXP index){ //see how multiple vector arguments are parsed in here rather than the single posmat matrix
  IntegerVector ORIG_IDint = as<IntegerVector>(ORIG_ID);
  IntegerVector onsetint = as<IntegerVector>(onset);
  NTYPE R,i,j,k;
  float distij2 = 0;
  float distik2 = 0;
  float r2 = 0;
  float r2_low = 0;
  unsigned long long num_cnt = 0, denom_cnt = 0; //counters for those filling conditions//
  unsigned short serialintvl = 7; //mean serial interval of the disease
  unsigned short r_size = r.size();
  NTYPE N = ORIG_ID.size();
  int *inds = INTEGER(index);
  NumericVector tau3(r_size, NULL);
  float piInf = 0;
  bool check = 1;
  bool bstrapconflict = 0;
  bool sameperson = 0;
  bool iscaseij = 0;
  bool iscaseijk = 0;
  bool temprelij = 0;
  bool temprelijk = 0;
  bool withindist = 0;
  
  if(*inds==-1){ //if index was set to -1 then it means we can turn off bootstrapping checks 
    check = 0; 
  }
  
  //calc piInf
  for (i=0;i<N;i++) {
    for (j=0;j<N;j++) {
      if(i==j) continue;
      if((inds[i] == inds[j]) && check) continue; //do not compare someone with themself if bootstrapping*/
      if(ORIG_IDint[i]==ORIG_IDint[j]) continue; //ie the person migrated to a different place in the study
      iscaseij = (onsetint[i]!=-999) && (onsetint[j]!=-999);
      temprelij = ((onsetint[j]-onsetint[i]) <= serialintvl) && ((onsetint[j]-onsetint[i]) >= 0);
      for (k=0;k<N;k++) {
        bstrapconflict = (j==k || i==k)||((inds[j] == inds[k]) && check)||((inds[i] == inds[k]) && check);
        sameperson = ((ORIG_IDint[j]==ORIG_IDint[k])||(ORIG_IDint[i]==ORIG_IDint[k]));
        denom_cnt = denom_cnt + (!bstrapconflict && !sameperson);
        iscaseijk = iscaseij && (onsetint[k]!=-999);
        temprelijk = temprelij && ((onsetint[k]-onsetint[j]) <= serialintvl) && ((onsetint[k]-onsetint[j]) >= 0);
        num_cnt = num_cnt + (!bstrapconflict && !sameperson && iscaseijk && temprelijk);
      }
    }
  }
  piInf = (float)num_cnt/denom_cnt;
  
  //calc pi(r2_low,r2)
  for (R=0;R<r_size;R++) {
    num_cnt = 0;
    denom_cnt = 0;
    r2  = r[R]*r[R]; //transformation to squared distances to make sqrt() redundant in this line and the one below
    r2_low = r_low[R]*r_low[R];
    
    for (i=0;i<N;i++) {
      for (j=0;j<N;j++) {
        if(i==j) continue;
        if((inds[i] == inds[j]) && check) continue;
        if(ORIG_IDint[i]==ORIG_IDint[j]) continue; 
        iscaseij = (onsetint[i]!=-999) && (onsetint[j]!=-999);
        temprelij = ((onsetint[j]-onsetint[i]) <= serialintvl) && ((onsetint[j]-onsetint[i]) >= 0);
        distij2 = (x[i] - x[j])*(x[i] - x[j]) + (y[i] - y[j])*(y[i] - y[j]); //calculate the distance
        withindist = ((distij2 >= r2_low) && (distij2 < r2));
        if(!withindist) continue;
        for (k=0;k<N;k++) {
          distik2 = (x[i] - x[k])*(x[i] - x[k]) + (y[i] - y[k])*(y[i] - y[k]); //calculate the distance
          withindist = ((distik2 >= r2_low) && (distik2 < r2));
          bstrapconflict = (j==k || i==k)||((inds[j] == inds[k]) && check)||((inds[i] == inds[k]) && check);
          sameperson = ((ORIG_IDint[j]==ORIG_IDint[k])||(ORIG_IDint[i]==ORIG_IDint[k]));
          denom_cnt = denom_cnt + (!bstrapconflict && !sameperson && withindist);
          iscaseijk = iscaseij && (onsetint[k]!=-999);
          temprelijk = temprelij && ((onsetint[k]-onsetint[j]) <= serialintvl) && ((onsetint[k]-onsetint[j]) >= 0);
          num_cnt = num_cnt + (!bstrapconflict && !sameperson && iscaseijk && temprelijk && withindist);
        }
      }
    }  
    tau3[R] = (float)num_cnt/denom_cnt; // pi(r.min,r.max)
    tau3[R] = (float)tau3[R]/piInf;
  }
  return(tau3);
}