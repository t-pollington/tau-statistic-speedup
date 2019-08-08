rm(list = ls()) #clean up from before
library(Rcpp) #sourceCpp()
setwd(FolderWhereYouStoreYourData)
load(file = "YourData.RData") #say you have called your matrix object "X"
setwd("/taustatisticspeedup")
#'Install and restart'

r.max = seq(50,500,50) #upper bound of each bin, in metres
r.min = rep.int(0,length(r.max)) #lower bound of each bin
ptm <- proc.time()
for (i in 1:10) {
  taustatisticspeedup::getTau2(X.NW[,"ORIG_IDenum"],X.NW[,"x"],X.NW[,"y"],X.NW[,"KA"],r.max,r.min,as.integer(-1)) #note that as we want the point estimate of tau we set the last argument to -1. You could adapt this function for bootstrapping by parsing in a vector of indices for the last argument.
}
timing = (proc.time() - ptm)
timing/10 #2.0578s for commit 5dceac9 get_tau.cpp
r.max = seq(50,500,50) #upper bound of each bin, in metres
r.min = rep.int(0,length(r.max)) #lower bound of each bin
ptm <- proc.time()
for (i in 1:10) {
  taustatisticspeedup::getTau3(X.NW[,"ORIG_IDenum"],X.NW[,"x"],X.NW[,"y"],X.NW[,"KA"],r.max,r.min,as.integer(-1)) #note that as we want the point estimate of tau we set the last argument to -1. You could adapt this function for bootstrapping by parsing in a vector of indices for the last argument.
}
timing = (proc.time() - ptm)
timing/10 #2.0578s for commit 5dceac9 get_tau.cpp