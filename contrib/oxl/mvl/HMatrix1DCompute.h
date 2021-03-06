#ifndef HMatrix1DCompute_h_
#define HMatrix1DCompute_h_
//:
// \file
//
// Base class of classes to generate a line-to-line projectivity matrix from
// a set of matched points.
//
// \author fsm
//

class HMatrix1D;
#include <vgl/vgl_homg_point_1d.h>
#include <vcl_vector.h>

class HMatrix1DCompute
{
 public:
  //
  HMatrix1DCompute() : verbose_(false) { }
  virtual ~HMatrix1DCompute() { }

  //
  void verbose(bool); // set this to true for verbose run-time information

  //
  // Compute methods :
  //
  bool compute(const vcl_vector<vgl_homg_point_1d<double> >&,
               const vcl_vector<vgl_homg_point_1d<double> >&,
               HMatrix1D *);
  bool compute(const double [],
               const double [],
               int,
               HMatrix1D *);
 protected:
  bool verbose_;

  virtual bool compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> >&,
                                 const vcl_vector<vgl_homg_point_1d<double> >&,
                                 HMatrix1D *);
  virtual bool compute_array_dbl(const double [],
                                 const double [],
                                 int N,
                                 HMatrix1D *);
};

#endif // HMatrix1DCompute_h_
