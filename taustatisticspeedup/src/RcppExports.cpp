// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// getTau
NumericVector getTau(const NumericVector ORIG_ID, const NumericVector x, const NumericVector y, const NumericVector onset, const NumericVector r, const NumericVector r_low, SEXP index);
RcppExport SEXP _taustatisticspeedup_getTau(SEXP ORIG_IDSEXP, SEXP xSEXP, SEXP ySEXP, SEXP onsetSEXP, SEXP rSEXP, SEXP r_lowSEXP, SEXP indexSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const NumericVector >::type ORIG_ID(ORIG_IDSEXP);
    Rcpp::traits::input_parameter< const NumericVector >::type x(xSEXP);
    Rcpp::traits::input_parameter< const NumericVector >::type y(ySEXP);
    Rcpp::traits::input_parameter< const NumericVector >::type onset(onsetSEXP);
    Rcpp::traits::input_parameter< const NumericVector >::type r(rSEXP);
    Rcpp::traits::input_parameter< const NumericVector >::type r_low(r_lowSEXP);
    Rcpp::traits::input_parameter< SEXP >::type index(indexSEXP);
    rcpp_result_gen = Rcpp::wrap(getTau(ORIG_ID, x, y, onset, r, r_low, index));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_taustatisticspeedup_getTau", (DL_FUNC) &_taustatisticspeedup_getTau, 7},
    {NULL, NULL, 0}
};

RcppExport void R_init_taustatisticspeedup(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
