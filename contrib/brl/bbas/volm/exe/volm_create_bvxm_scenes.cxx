//:
// \file
// \brief  executable to take a polygonal ROI and cover it with bvxm scenes with different lvcs's 
//
// \author Ozge C. Ozcanli 
// \date August 22, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
#include <volm/volm_tile.h>
#include <vcl_iostream.h>
#include <volm/volm_osm_objects.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_geo_index2_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vcl_where_root_dir.h>
#include <vpgl/vpgl_lvcs.h>
#include <bkml/bkml_parser.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <bvxm/bvxm_world_params.h>
#include <bkml/bkml_write.h>


int main(int argc, char** argv)
{
  // input
  vul_arg<vcl_string> in_folder("-in", "input folder to read DEM files as .tif", "");
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, the scenes that cover this polygon will be created", "");
  vul_arg<vcl_string> out_folder("-out", "folder to write xml files","");   
  vul_arg<vcl_string> world_root("-world_dir", "the world folder where bvxm vox binary will be stored","");
  vul_arg<float> voxel_size("-vox", "size of voxel in meters", 1.0f);
  vul_arg<float> world_size("-size", "the size of the world in meters", 500.0f);
  vul_arg<float> height("-height", "the amount to be added on top of the terrain height to create the scene in meters", 0.0f);

  vul_arg_parse(argc, argv);

  // check input
  if (in_folder().compare("") == 0 || in_poly().compare("") == 0 || out_folder().compare("") == 0 || world_root().compare("") == 0) {
    vcl_cerr << " ERROR: input is missing!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  
  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  // find the bbox of ROI from its polygon
  vgl_box_2d<double> bbox;
  for (unsigned i = 0; i < poly[0].size(); i++) {
    bbox.add(poly[0][i]);
  }
  vcl_cout << "bbox of ROI: " << bbox << vcl_endl;

  double meter_to_deg = 1.0/(30.0*3600.0);  // 1 arcsecond is roughly 30 meter
  double min_size = world_size()*meter_to_deg;
  vcl_cout << "the scene size is " << world_size() << " meters which is " << min_size << " degrees!\n";

  // create a geo index and use the leaves as scenes, use template param as volm_loc_hyp_sptr but it won't actually be used
  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(bbox, min_size, poly);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  unsigned tree_depth = volm_geo_index2::depth(root);
  vcl_cout << "the scene has " << leaves.size() << " leaves and depth is " << tree_depth << vcl_endl;
  //volm_geo_index2::write_to_kml(root, tree_depth, out_folder() + "scenes.kml");
  vcl_string temp_name = out_folder() + "scenes.kml";
  vcl_ofstream ofs(temp_name.c_str());
  bkml_write::open_document(ofs);
  for (unsigned i = 0; i < leaves.size(); i++) {
    vcl_stringstream str; str << "scene" << i;
    volm_geo_index2::write_to_kml_node(ofs, leaves[i], 0, 0, str.str());
  }
  bkml_write::close_document(ofs);
  ofs.close();
  
  vcl_vector<volm_img_info> infos;
  volm_io_tools::load_aster_dem_imgs(in_folder(), infos);
  vcl_cout << " loaded " << infos.size() << " DEM tiles!\n";

  double largest_dif = 0;
  for (unsigned i = 0; i < leaves.size(); i++) {
    vcl_stringstream name; name << out_folder() << "scene_" << i ;
    //vcl_cout << name.str() << vcl_endl;
    //vul_file::make_directory(dir);
    vgl_point_2d<double> lower_left = leaves[i]->extent_.min_point();
    vgl_point_2d<double> upper_right = leaves[i]->extent_.max_point();

    double min = 10000.0, max = -10000.0;
    if (!volm_io_tools::find_min_max_height(lower_left, upper_right, infos, min, max)) {
      vcl_cerr << " problems in the leaf: " << lower_left << " " << upper_right << " - cannot find height!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    double dif = max-min;
    if (dif > largest_dif) largest_dif = dif;
    //vcl_cout << "min: " << min << " " << max << "\n";

    //construct lvcs
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lower_left.y(), lower_left.x(), min, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    vcl_string lvcs_name = name.str() + ".lvcs";
    vcl_ofstream ofs(lvcs_name.c_str());
    if (!ofs) {
      vcl_cerr << "Cannot open file: " << lvcs_name << "!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    lvcs->write(ofs);
    ofs.close();

    vgl_point_3d<float> corner(0,0,0);
    unsigned dim_xy = (unsigned)vcl_ceil(world_size()/voxel_size());
    unsigned dim_z = (unsigned)vcl_ceil(dif+height());
    vgl_vector_3d<unsigned int> num_voxels(dim_xy, dim_xy, dim_z);
    bvxm_world_params params;
    //params.set_params(name.str(), corner, num_voxels, voxel_size(), lvcs); 
    //params.set_params(out_folder().substr(0, out_folder().size()-1), corner, num_voxels, voxel_size(), lvcs);  // for now set model dir as out_folder
    // set bvxm_scene world parameters
    vcl_stringstream world_dir;
    world_dir << world_root() << "/scene_" << i;
    if (!vul_file::is_directory(world_dir.str())) {
      vul_file::make_directory(world_dir.str());
    }
    params.set_params(world_dir.str(), corner, num_voxels, voxel_size(), lvcs);   // the world dir is now different from the out_folder where scene.xml and scene.lvcs stores
    vcl_string xml_name = name.str() + ".xml";
    params.write_xml(xml_name, lvcs_name);
  }

  vcl_cout << "largest height difference in the whole ROI is: " << largest_dif << '\n';
  
  return volm_io::SUCCESS;
}
