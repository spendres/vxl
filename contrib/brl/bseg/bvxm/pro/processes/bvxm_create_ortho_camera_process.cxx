// This is brl/bseg/bvxm/pro/processes/bvxm_create_ortho_camera_process.cxx
#include "bvxm_create_ortho_camera_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_edge_ray_processor.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/vil_image_view.h>

bool bvxm_create_ortho_camera_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_ortho_camera_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";  // return the ortho camera of the scene, as it may be needed for other processes
  return pro.set_output_types(output_types_);
}

// generates an ortho camera from the scene bounding box, GSD of the image is 1 meter
bool bvxm_create_ortho_camera_process(bprb_func_process& pro)
{
  using namespace bvxm_create_ortho_camera_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //voxel_world
  bvxm_voxel_world_sptr world =  pro.get_input<bvxm_voxel_world_sptr>(0);
  if (!world) {
    vcl_cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  // generate vpgl_geo_camera for the scene
  bvxm_world_params_sptr params = world->get_params();
  vgl_box_3d<double> box = params->world_box_local();
  vgl_point_3d<float> corner = params->corner();
  vgl_point_3d<float> upper_left(corner.x(), (float)(corner.y() + box.height()), corner.z());
  vgl_point_3d<float> lower_right((float)(corner.x()+box.width()), corner.y(), corner.z());
  vpgl_lvcs_sptr lvcs = params->lvcs();
  double lat, lon, elev;
  lvcs->get_origin(lat, lon, elev);
  vcl_cout << " lvcs origin: " << lat << " " << lon << " " << elev << vcl_endl;

  // determine the upper left corner to use a vpgl_geo_cam, WARNING: assumes that the world is compass-alinged
  double upper_left_lon, upper_left_lat, upper_left_elev;
  lvcs->local_to_global(upper_left.x(), upper_left.y(), upper_left.z(), vpgl_lvcs::wgs84, upper_left_lon, upper_left_lat, upper_left_elev);
  vcl_cout << "upper left corner in the image is: " << upper_left_lon << " lat: " << upper_left_lat << vcl_endl;

  double lower_right_lon, lower_right_lat, lower_right_elev;
  lvcs->local_to_global(lower_right.x(), lower_right.y(), lower_right.z(), vpgl_lvcs::wgs84, lower_right_lon, lower_right_lat, lower_right_elev);
  vcl_cout << "lower right corner in the image is: " << lower_right_lon << " lat: " << lower_right_lat << vcl_endl;
  
  int ni = box.width();
  int nj = box.height();
  vnl_matrix<double> trans_matrix(4,4,0.0);
  //trans_matrix[0][0] = (lower_right_lon-lon)/ni; trans_matrix[1][1] = -(upper_left_lat-lat)/nj;
  // lvcs origin is not necessarily one of the corners of the scene
  trans_matrix[0][0] = (lower_right_lon-upper_left_lon)/ni; trans_matrix[1][1] = -(upper_left_lat-lower_right_lat)/nj;
  trans_matrix[0][3] = upper_left_lon; trans_matrix[1][3] = upper_left_lat;
  vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, lvcs); 
  cam->set_scale_format(true);
  vpgl_camera_double_sptr camera = new vpgl_geo_camera(*cam);  

  pro.set_output_val<vpgl_camera_double_sptr>(0, camera);
  return true;
}
