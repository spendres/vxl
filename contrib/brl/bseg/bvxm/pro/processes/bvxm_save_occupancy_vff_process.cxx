// This is brl/bseg/bvxm/pro/processes/bvxm_save_occupancy_vff_process.cxx

//:
// \file
// \brief Save the voxel world occupancy grid in binary format
// A process that saves the voxel world occupancy grid in a binary format
// readable by GE MicroView and other volume renderers
//
// \author Daniel Crispell
// \date May 01, 2008
// \verbatim
//  Modifications
//   Brandon Mayer - Jan 28, 2009 - converted process-class to function to conform with bvxm_process architecture.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

namespace bvxm_save_occupancy_vff_process_globals
{
  const unsigned n_inputs_ = 3;
}

bool bvxm_save_occupancy_vff_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_save_occupancy_vff_process_globals;
  // process takes 3 inputs:
  //input[0]: The voxel world
  //input[1]: The filename to write to
  //input[2]: Scale Index

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}

bool bvxm_save_occupancy_vff_process(bprb_func_process& pro)
{
  using namespace bvxm_save_occupancy_vff_process_globals;

  //check number of inputs
  if (pro.n_inputs() < n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string filename = pro.get_input<vcl_string>(i++);
  unsigned scale_idx = pro.get_input<unsigned>(i++);

  if ( !world ){
    vcl_cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  return world->save_occupancy_vff(filename,scale_idx);
}
