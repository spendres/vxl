//:
// \file
// \brief  executable to sort the bvxm scene regions based on their overlap with geo_cover urban region
//
// \author Yi Dong
// \date Nov 13, 2013
// \verbatim
//  Modification
//   <None yet>
// \endverbatim

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vgl/vgl_intersection.h>
#include <bkml/bkml_parser.h>
#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_geo_index2_sptr.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <vil/vil_save.h>
#include <vcl_ios.h>


void error(vcl_string log_file, vcl_string msg)
{
  vcl_cerr << msg;  volm_io::write_post_processing_log(log_file, msg);
}

int main(int argc, char** argv)
{
  // input
  vul_arg<vcl_string> geo_folder("-geo", "folder where geo_cover tif images stores", "");
  vul_arg<vcl_string> scene_root("-scene-root", "root directory where all scene xml and lvcs files are stored", "");
  vul_arg<vcl_string> out_filename("-out-file", "name of the txt file which the sorted results will be written", "urban_sort_scene.txt");
  vul_arg<float> voxel_size("-vox", "size of voxel in meters", 1.0f);
  vul_arg<float> world_size("-size", "the size of the world in meters", 500.0f);
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, the scenes that cover this polygon will be created", "");
  vul_arg_parse(argc, argv);

  // check input
  if (geo_folder().compare("") == 0 || in_poly().compare("") == 0 || scene_root().compare("") == 0) {
    vcl_cerr << " ERROR: input is missing!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_stringstream log_file;
  vcl_stringstream log;
  
  log_file << scene_root() << "/log_sort_scene_urban.xml";
  
  
  // create geo_index to fetch the size of leaf used in scene
  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  // find the bbox of ROI from its polygon
  vgl_box_2d<double> bbox_roi;
  for (unsigned i = 0; i < poly[0].size(); i++) {
    bbox_roi.add(poly[0][i]);
  }
  double meter_to_deg = 1.0/(30.0*3600.0);  // 1 arcsecond is roughly 30 meter
  double min_size = world_size()*meter_to_deg;
  // create a geo index and use the leaves as scenes, use template param as volm_loc_hyp_sptr but it won't actually be used
  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(bbox_roi, min_size, poly);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  vcl_cout << "the scene has " << leaves.size() << " leaves and depth is " << volm_geo_index2::depth(root) << vcl_endl;

  // load geo_cover image
  vcl_vector<volm_img_info> geo_info;
  volm_io_tools::load_geocover_imgs(geo_folder(), geo_info);
  vcl_cout << geo_info.size() << " geo cover images are loaded" << vcl_endl;

  // loop over all scene lvcs file
  vcl_multimap<double, vcl_string> scene_order;  // sorted list of scene xmls

  vcl_string file_glob = scene_root() + "/*.lvcs";
  unsigned cnt=0;
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn) {
    vcl_string lvcs_file = fn();
    vcl_string scene_file = vul_file::strip_extension(lvcs_file) + ".xml";
    vcl_string scene_name = vul_file::strip_directory(scene_file);
    vcl_string name = vul_file::strip_extension(vul_file::strip_directory(lvcs_file));
    name = name.substr(name.find_first_of('_')+1, name.size());
    vcl_stringstream str(name);
    unsigned scene_id;
    str >> scene_id;

    if (!vul_file::exists(lvcs_file) || !vul_file::exists(scene_file)) {
      log << "ERROR: can not find scene " << lvcs_file << '\n';  error(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    // load the lvcs
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs;
    vcl_ifstream ifs(lvcs_file.c_str());
    if (!ifs.good()) {
      log << "ERROR: load lvcs: " << lvcs_file << " failed\n";  error(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    lvcs->read(ifs);
    ifs.close();

    // obtain the bounding box from the leaf
    vgl_box_2d<double> bbox_geo = leaves[scene_id]->extent_;
    double lon_min, lat_min, lon_max, lat_max;
    lon_min = bbox_geo.min_x();  lat_min = bbox_geo.min_y();
    lon_max = bbox_geo.max_x();  lat_max = bbox_geo.max_y();

    double lvcs_ori_lon, lvcs_ori_lat, lvcs_ori_elev;
    lvcs->get_origin(lvcs_ori_lat, lvcs_ori_lon, lvcs_ori_elev);

    if ( (lvcs_ori_lat-lat_min)*(lvcs_ori_lat-lat_min) > 1e-5 || (lvcs_ori_lon-lon_min)*(lvcs_ori_lon-lon_min) > 1e-5) {
      log << " ERROR: mismatch for leaf extent " << bbox_geo << " with lvcs origin (" << lvcs_ori_lon << ", " << lvcs_ori_lat << ") for scene " << scene_id << '\n';
      error(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    // locate the geo_cover image which intersects with current leaf
    unsigned geo_cover_id;
    for (unsigned g_idx = 0; g_idx < geo_info.size(); g_idx++) {
      if (!vgl_intersection(geo_info[g_idx].bbox, bbox_geo).is_empty()) {
        geo_cover_id = g_idx;
        break;
      }
    }
    volm_img_info geo_cover = geo_info[geo_cover_id];
    vil_image_view<vxl_byte>* geo_img = dynamic_cast<vil_image_view<vxl_byte> * >(geo_cover.img_r.ptr());

    // create the image associated with current scene
    double box_lx, box_ly, box_lz;
    lvcs->global_to_local(lon_max, lat_max, 0, vpgl_lvcs::wgs84, box_lx, box_ly, box_lz);
    
    unsigned ni = (unsigned)vcl_ceil(box_lx);
    unsigned nj = (unsigned)vcl_ceil(box_ly);
#if 0
    vil_image_view<vxl_byte> out_img(ni, nj, 1);
    out_img.fill(0);
#endif

    // count the urban pixels
    unsigned urban_pixels = 0;
    for (int i = 0; i < ni; i++) {
      for (int j = 0; j < nj; j++) {
        // transfer coords to get geo cover pixel
        double lon, lat, gz;
        float local_x = (float)(i+0+0.5);
        float local_y = (float)(box_ly-j+0.5);
        lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
        double u, v;
        geo_cover.cam->global_to_img(lon, lat, gz, u, v);
        unsigned uu = (unsigned)vcl_floor(u+0.5);
        unsigned vv = (unsigned)vcl_floor(v+0.5);
        if (uu > 0 && vv > 0 && uu < geo_cover.ni && vv < geo_cover.nj)
          if ((*geo_img)(uu,vv) == volm_osm_category_io::GEO_URBAN) {
            //out_img(i,j) = 255;
            urban_pixels++;
          }
      }
    }

    // calcualte ration of the urban region
    double urban_ratio = (double)urban_pixels/(double)(ni*nj);
    scene_order.insert(vcl_pair<double, vcl_string>(urban_ratio, scene_name));
    vcl_cout << " for scene " << scene_id << " img size is " << ni << " x " << nj
             << ", urban pixels are " << urban_pixels << " and the urban ration is: " << urban_ratio << vcl_endl;
#if 0
    vcl_cout << " for scene " << scene_id << " img size is " << ni << " x " << nj
             << ", urban pixels are " << urban_pixels << " and the urban ration is: " << urban_ratio << vcl_endl;
    // output an image for debug purpose
    vcl_stringstream out_img_name;
    out_img_name << scene_root() << "/urban_img_" << scene_id << ".tif";
    vil_save(out_img, out_img_name.str().c_str());
#endif
    cnt++;
    if (cnt % 1000 == 0) {
      vcl_cout << cnt << '.';
    }
  } // end of loop over all lvcs files

  // output to a text file
  vcl_string out_txt = scene_root() + '/' + out_filename();

  vcl_ofstream ofs(out_txt.c_str());
  ofs << "urban_ratio \t\t scene_xml\n";

  vcl_multimap<double, vcl_string>::iterator mit = scene_order.end();
  --mit;
  for (; mit != scene_order.begin(); --mit) {\
    ofs.precision(7);  ofs.width(14);
    ofs << vcl_setiosflags(vcl_ios_left) << mit->first << ' ' << mit->second << '\n';
  }
  ofs.close();

  return volm_io::SUCCESS;
}