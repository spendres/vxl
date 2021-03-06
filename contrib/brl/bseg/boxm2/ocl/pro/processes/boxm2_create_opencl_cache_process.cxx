// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_create_opencl_cache_process.cxx
//:
// \file
// \brief  A process for loading the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

namespace boxm2_create_opencl_cache_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}

bool boxm2_create_opencl_cache_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_opencl_cache_process_globals;

  //process takes 2 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_opencl_cache_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_opencl_cache_process(bprb_func_process& pro)
{
  using namespace boxm2_create_opencl_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= new boxm2_opencl_cache(scene, device);
  i=0;

  // store scene smart pointer
  pro.set_output_val<boxm2_opencl_cache_sptr>(i++, opencl_cache);
  return true;
}
