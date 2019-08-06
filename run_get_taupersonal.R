rm(list = ls()) #clean up from before
library(Rcpp) #sourceCpp()
setwd(FolderWhereYouStoreYourData)
load(file = "YourData.RData") #say you have called your matrix object "X"
setwd(FolderWhereYouStoreYourTauCFile)
sourceCpp("get_tau.cpp")

r.max = seq(50,500,50) #upper bound of each bin, in metres
r.min = rep.int(0,length(r.max)) #lower bound of each bin
ptm <- proc.time()
for (i in 1:10) {
  getTau(X.NW[,"ORIG_IDenum"],X.NW[,"x"],X.NW[,"y"],X.NW[,"KA"],r.max,r.min,as.integer(-1)) #note that as we want the point estimate of tau we set the last argument to -1. You could adapt this function for bootstrapping by parsing in a vector of indices for the last argument.
}
timing = (proc.time() - ptm)
timing/10 #2.0578s for commit 5dceac9 get_tau.cpp
#2.03s for NTYPE change to unsigned short

ORIG_IDenum <- file("ORIG_IDenum.bin", "wb")
writeBin(X.NW[,"ORIG_IDenum"], ORIG_IDenum,size = integer())
close(ORIG_IDenum)
x <- file("x.bin", "wb")
writeBin(X.NW[,"x"], x, size = double())
close(x)
y <- file("y.bin", "wb")
writeBin(X.NW[,"y"], y, size = double())
close(y)
KA <- file("KA.bin", "wb")
writeBin(X.NW[,"KA"], KA, size = integer())
close(KA)
r.maxbin <- file("r.max.bin", "wb")
writeBin(r.max, r.maxbin, size = integer())
close(r.maxbin)
r.minbin <- file("r.min.bin", "wb")
writeBin(r.min, r.minbin, size = integer())
close(r.minbin)
indx <- file("indx.bin", "wb")
indxvector = as.numeric(sample.int(16221,16221,replace = T))
writeBin(indxvector, indx, size = integer())
close(indx)
indx <- file("indx.bin", "rb")
readBin(con = indx,what = 4,n=16221)
close(indx)
