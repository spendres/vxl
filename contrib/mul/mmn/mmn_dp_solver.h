#ifndef mmn_dp_solver_h_
#define mmn_dp_solver_h_
//:
// \file
// \brief Solve restricted class of Markov problems (trees and tri-trees)
// \author Tim Cootes

#include <mmn/mmn_arc.h>
#include <mmn/mmn_dependancy.h>
#include <mmn/mmn_solver.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>

//: Solve restricted class of Markov problems (trees and tri-trees)
//  Find choice of values at each node which minimises Markov problem.
//  Minimises F() = sum node_costs + sum pair_costs
//
//  Assumes graph defining relationships can be fully decomposed by
//  repeatedly removing any nodes with two or fewer neighbours.
//  Global optimum is found using a series of sequential exhaustive
//  optimisations.
class mmn_dp_solver : public mmn_solver
{
 private:
  //: Workspace for incremental costs of each node
  vcl_vector<vnl_vector<double> > nc_;

  //: Workspace for incremental costs of each arc
  vcl_vector<vnl_matrix<double> > pc_;

  //: index1[i][j] = optimal choice for i if other node is j
  vcl_vector<vcl_vector<unsigned> > index1_;

  //: index2[i](j,k) = optimal choice of i if two other nodes are (j,k)
  vcl_vector<vnl_matrix<int> > index2_;

  //: Dependencies
  vcl_vector<mmn_dependancy> deps_;

  //: Compute optimal choice for node dep.v0 for each possible v1
  //  Uses pair costs for v0-v1 (in pc_) and the node cost nc_(v0)
  void process_dep1(const mmn_dependancy& dep);

  //: Compute optimal choice for dep.v0 given v1 and v2
  //  Uses only pairwise and node costs (in nc_ and pc_)
  void process_dep2(const mmn_dependancy& dep);

  //: Compute optimal choice for dep.v0 given v1 and v2
  //  Includes cost depending on (v0,v1,v2) as well as pairwise and
  //  node costs.
  // tri_cost(i,j,k) is cost of associating smallest node index
  // with i, next with j and largest node index with k.
  void process_dep2t(const mmn_dependancy& dep,
                    const vil_image_view<double>& tri_cost);

 public:
  //: Default constructor
  mmn_dp_solver();

  //: Index of root node
  unsigned root() const;

  //: Input the arcs that define the graph
  virtual void set_arcs(unsigned num_nodes,
                        const vcl_vector<mmn_arc>& arcs);

  //: Define dependencies
  void set_dependancies(const vcl_vector<mmn_dependancy>& deps,
                        unsigned n_nodes, unsigned max_n_arcs);

  //: Read access to dependencies
  const vcl_vector<mmn_dependancy>& deps() const { return deps_; }

  //: Find values for each node with minimise the total cost
  //  \param node_cost: node_cost[i][j] is cost of selecting value j for node i
  //  \param pair_cost: pair_cost[a](i,j) is cost of selecting values (i,j) for nodes at end of arc a.
  //  \param x: On exit, x[i] gives choice for node i
  // NOTE: If arc a connects nodes v1,v2, the associated pair_cost is ordered
  // with the node with the lowest index being the first parameter.  Thus if
  // v1 has value i1, v2 has value i2, then the cost of this choice is
  // (v1<v2?pair_cost(i1,i2):pair_cost(i2,i1))
  // Returns the minimum cost
  virtual double solve(const vcl_vector<vnl_vector<double> >& node_cost,
                 const vcl_vector<vnl_matrix<double> >& pair_cost,
                 vcl_vector<unsigned>& x);

  //: Find values for each node with minimise the total cost
  //  As solve(node_cost,pair_cost,x), but allows inclusion of
  //  costs for triplets (ie when v0 depends on v1 and v2).
  //  tri_cost(i,j,k) gives cost of node min(v0,v1,v2) being
  //  value i, node mid(v0,v1,v2) being value j and
  //  node max(v0,v1,v2) being value k.
  double solve(const vcl_vector<vnl_vector<double> >& node_cost,
                 const vcl_vector<vnl_matrix<double> >& pair_cost,
                 const vcl_vector<vil_image_view<double> >& tri_cost,
                 vcl_vector<unsigned>& x);

  //: Compute optimal values for x[i] given that root node is root_value
  //  Assumes that solve() has been already called.
  void backtrace(unsigned root_value,vcl_vector<unsigned>& x);

  //: root_cost()[i] is cost of selecting value i for the root node
  const vnl_vector<double>& root_cost() const { return nc_[root()]; }

  //: Initialise from a text stream
  virtual bool set_from_stream(vcl_istream &is);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mmn_solver* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // mmn_dp_solver_h_
