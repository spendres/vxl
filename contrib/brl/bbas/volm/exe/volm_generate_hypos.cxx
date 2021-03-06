//:
// \file
// \brief executable to generate hypothesis locations using DEMs or LIDAR
// \author Ozge C. Ozcanli
// \date Nov 15, 2012

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_loc_hyp.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_utm.h>
#include <vpgl/vpgl_lvcs.h>
#include <bkml/bkml_parser.h>
#include <vul/vul_timer.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_category_io.h>


inline float next_mult_2(float val)
{
  return (float)(2.0f*vcl_ceil(val*0.5f));
}

#if 0  // region 1
// generate gt hypos
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<unsigned> inc_i("-inc_i", "increments in pixels in the DEM image", 1);
  vul_arg<unsigned> inc_j("-inc_j", "increments in pixels in the DEM image", 1);
  vul_arg<vcl_string> out_pre("-out_pre", "output file prefix", "");
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  if (out_pre().compare("") == 0 || in_poly().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << "will use increments " << inc_i() << " pixels along i and " << inc_j() << " pixels along j in the DEM image!\n";
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(38.074326, -117.654869, 1200.0, vpgl_lvcs::utm, 0.0, 0.0, vpgl_lvcs::DEG, vpgl_lvcs::METERS, 0.0, 0.0, 0.0);


  //vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());

  double x[] = {-118.21f, -117.102f, -117.102f, -118.21f, -118.21f};
  double y[] = {38.553f, 38.553f, 37.623f, 37.623f, 38.553f};
  vgl_polygon<double> poly(x, y, 5);

  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs();

  volm_loc_hyp hyp;

  // add region 1 gt positions
  hyp.add(37.6763153076, -117.978889465, 1891.40698242);
  hyp.add(37.6763076782, -117.978881836, 1891.40698242);
  hyp.add(37.9629554749, -117.964019775, 1670.87597656);
  hyp.add(37.9623947144, -117.962753296, 1643.76293945);
  hyp.add(38.5223121643, -118.147850037, 1382.84594727);
  hyp.add(38.5202674866, -118.147697449, 1378.42004395);
  hyp.add(38.0597877502, -117.936508179, 1376.12097168);
  hyp.add(38.0598144531, -117.936523438, 1376.12097168);
  hyp.add(37.7081604004, -117.416557312, 1860.39599609);
  hyp.add(37.7084999084, -117.416664124, 1863.23205566);
  hyp.add(37.7082748413, -117.416511536, 1860.99194336);

  //vcl_string data_folder = "I:/";
  vcl_string data_folder = "/home/ozge/mnt/fileserver/projects/FINDER/";
  vcl_string dem_file = data_folder + "P-1A/Finder_Program_Only/ReferenceData/Desert/NAMDTM/NAM_DEM.tif";
  vil_image_resource_sptr dem_res = vil_load_image_resource(dem_file.c_str());
  vil_image_view<float> dem = dem_res->get_view();
  vpgl_geo_camera* geocam = 0;
  vpgl_geo_camera::init_geo_camera(dem_res, lvcs, geocam);

  hyp.add(poly, dem, geocam, inc_i(), inc_j());

  vcl_cout << "after addition hyp has: " << hyp.size() << " elements\n";

  vcl_stringstream str;
  str << out_pre() << '_' << inc_i() << '_' << inc_j() << ".bin";
  hyp.write_hypotheses(str.str());

  return volm_io::SUCCESS;
}
#endif

#if 0
// region 2 gt only
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<vcl_string> out_pre("-out_pre", "output file prefix", "");
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  if (out_pre().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs();

  volm_loc_hyp hyp;

  // add region 2 gt positions
  // note to self, the order here is not necessarily the order given by get_next() method of hyp
  // increase elev by 1 meter
  hyp.add(33.912397,-78.281430,next_mult_2(1.60));  // p1a_res12_trashcan-GROUNDTRUTH
  hyp.add(33.912068,-78.282300,next_mult_2(1.60)); // p1a_res13_shakypier-GROUNDTRUTH
  hyp.add(34.370855,-77.627093,next_mult_2(1.60)); // p1a_res14_boat-GROUNDTRUTH.kml
  hyp.add(34.368807,-77.624855,next_mult_2(3.61)); // p1a_res15_walkway-GROUNDTRUTH.kml
  hyp.add(34.370836,-77.627113,next_mult_2(1.60)); // p1a_res16_whitehouse-GROUNDTRUTH
  hyp.add(32.650674,-79.948996,next_mult_2(3.00)); // p1a_res17_beachgrass-GROUNDTRUTH.kml
  hyp.add(32.648747,-79.952434,next_mult_2(1.40)); // p1a_res18_rocks-GROUNDTRUTH.kml
  hyp.add(36.116348,-75.741750,next_mult_2(1.60)); // p1a_res19_channelfront-GROUNDTRUTH.kml
  hyp.add(36.126512,-75.743331,next_mult_2(1.60)); // p1a_res20_drivingbyboats-GROUNDTRUTH.kml
  hyp.add(32.375492,-80.436054,next_mult_2(1.60)); // p1a_res21_360spin-GROUNDTRUTH.kml
  hyp.add(32.375502,-80.437492,next_mult_2(40.30)); // p1a_res22_lookingsouth-GROUNDTRUTH.kml

  vcl_stringstream str;
  str << out_pre() << ".bin";
  hyp.write_hypotheses(str.str());

  return volm_io::SUCCESS;
}
#endif

# if 0 // adding region 2 using LIDAR, go over each lidar tile and for each pixel add a hypo
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> in_folder("-in", "input folder to read files as .tif", "");
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<unsigned> inc_i("-inc_i", "increments in pixels in the DEM image", 1);
  vul_arg<unsigned> inc_j("-inc_j", "increments in pixels in the DEM image", 1);
  vul_arg<vcl_string> out_pre("-out_pre", "output file prefix", "");
  vul_arg<bool> add_gt("-addgt", "add known gt locations?", false); // if no -addgt argument then the value is false, if there is one then the value is true
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  if (in_folder().compare("") == 0 || out_pre().compare("") == 0 || in_poly().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << "will use increments " << inc_i() << " pixels along i and " << inc_j() << " pixels along j in the DEM image!\n";

  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs();

  volm_loc_hyp hyp;

  vcl_string file_glob = in_folder() + "/*.tif";
  for (vul_file_iterator fn=file_glob; fn; ++fn) {
    vcl_string tiff_fname = fn();
    vcl_cout << tiff_fname << vcl_endl;

    vil_image_view_base_sptr img_sptr = vil_load(tiff_fname.c_str());
    vil_image_view<float> img(img_sptr);
    unsigned ni = img.ni(); unsigned nj = img.nj();
    vcl_cout << " tile size: "<< ni << " x " << nj << vcl_endl;

    // determine the translation matrix from the image file name and construct a geo camera
    vcl_string name = vul_file::strip_directory(tiff_fname);
    name = name.substr(name.find_first_of('_')+1, name.size());
    vcl_cout << "will determine transformation matrix from the file name: " << name << vcl_endl;
    vcl_string n = name.substr(name.find_first_of('N')+1, name.find_first_of('W'));
    float lon, lat, scale;
    vcl_stringstream str(n); str >> lat;
    n = name.substr(name.find_first_of('W')+1, name.find_first_of('_'));
    vcl_stringstream str2(n); str2 >> lon;
    n = name.substr(name.find_first_of('x')+1, name.find_last_of('.'));
    vcl_stringstream str3(n); str3 >> scale;
    vcl_cout << " lat: " << lat << " lon: " << lon << " WARNING: using same scale for both ni and nj: scale:" << scale << vcl_endl;

    // determine the upper left corner to use a vpgl_geo_cam, subtract from lat
    vcl_cout << "upper left corner in the image is: " << lat+scale << " N " << lon << " W\n"
             << "lower right corner in the image is: " << lat << " N " << lon-scale << " W" << vcl_endl;
    vnl_matrix<double> trans_matrix(4,4,0.0);
    trans_matrix[0][0] = -scale/ni; trans_matrix[1][1] = -scale/nj;
    trans_matrix[0][3] = lon; trans_matrix[1][3] = lat+scale;
    vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, dummy_lvcs);
    cam->set_scale_format(true);
    double lon2, lat2;
    cam->img_to_global(ni, nj, lon2, lat2);
    vpgl_utm utm; double x, y; int zone; utm.transform(lat2, -lon2, x, y, zone);
    vcl_cout << "lower right corner in the image given by geocam is: " << lat2 << " N " << lon2 << " W " << " zone: " << zone << vcl_endl;

    hyp.add(poly, img, cam, inc_i(), inc_j(), true, 'N', 'W');
    delete cam;
    vcl_cout << "after addition hyp has: " << hyp.size() << " elements\n";
  }
  if (add_gt()) {
    //: add any gt positions if any
    hyp.add(32.759063, -79.857689, 1.60);  // p1a_test1_06-GROUNDTRUTH
    hyp.add(32.775959, -79.813014, 4.41);  // p1a_test1_28-GROUNDTRUTH
    hyp.add(33.911997, -78.282153, 1.60);  // p1a_test1_08-GROUNDTRUTH
    hyp.add(33.912397, -78.281430, 1.60);  // p1a_test1_46-GROUNDTRUTH
    hyp.add(30.720336, -81.550366, 1.60);  // p1a_test1_18-GROUNDTRUTH
    hyp.add(32.648980, -79.951930, 1.60);  // p1a_test1_38-GROUNDTRUTH
    hyp.add(33.365799, -79.268871, 1.60);  // p1a_test1_34-GROUNDTRUTH

    vcl_cout << "after addition of some gt points hyp has: " << hyp.size() << " elements\n";
  }
  vcl_stringstream str;
  str << out_pre() << '_' << inc_i() << '_' << inc_j() << ".bin";
  hyp.write_hypotheses(str.str());

  return volm_io::SUCCESS;
}

#endif

#include <volm/volm_geo_index.h>

// find the elevation of the point and add it to the geo index if found
void add_hypo(volm_geo_index_node_sptr hyp_root, vcl_vector<volm_img_info> const& infos, vgl_point_2d<double> const& pt, double inc_in_sec_radius, bool search)
{
  // find the elevation of the hypotheses
  float z = 0;
  bool found_it = false;
  for (unsigned mm = 0; mm < infos.size(); mm++) {
    if (infos[mm].contains(pt)) {
      double u,v;
      infos[mm].cam->global_to_img(pt.x(), pt.y(), 0, u, v); 
      int uu = (int)vcl_floor(u+0.5);
      int vv = (int)vcl_floor(v+0.5);
      if (infos[mm].valid_pixel(uu,vv)) {
        vil_image_view<vxl_int_16> img(infos[mm].img_r);
        z = img(uu,vv);
        found_it = true;
        break;
      }
    }
  }
  if (found_it) { // add the hypo
    if (search) {
      if (!volm_geo_index::exists(hyp_root, pt.y(), pt.x(), inc_in_sec_radius))
        volm_geo_index::add_hypothesis(hyp_root, pt.x(), pt.y(), z);
    } else
      volm_geo_index::add_hypothesis(hyp_root, pt.x(), pt.y(), z);

  }

}

int find_land_type(vcl_vector<volm_img_info>& geo_infos, vgl_point_2d<double>& pt) 
{
  int type = -1;
  for (unsigned mm = 0; mm < geo_infos.size(); mm++) {
     if (geo_infos[mm].contains(pt)) {
      double u,v;
      geo_infos[mm].cam->global_to_img(pt.x(), pt.y(), 0, u, v); 
      int uu = (int)vcl_floor(u+0.5);
      int vv = (int)vcl_floor(v+0.5);
      if (geo_infos[mm].valid_pixel(uu,vv)) {
        vil_image_view<vxl_byte> img(geo_infos[mm].img_r);
        type = img(uu,vv);
        break;
      }
    }
  }
  return type;
}

// read the tiles of the region, create a geo index and write the hyps
int main(int argc,  char** argv)
{
  vul_timer t;
  t.mark();

  vul_arg<vcl_string> in_folder("-in", "input folder to read files as .tif", "");
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<float> inc("-inc", "increments in arcseconds, e.g. 0.1 for ~3m increments", 0.1f);
  vul_arg<unsigned> nh("-nh", "number of hyps in each direction in one leaf tile, e.g. 100 so that each tile has 100x100 hyps", 100);
  vul_arg<vcl_string> out_pre("-out_pre", "output file folder with file separator at the end", "");
  vul_arg<vcl_string> add_gt("-addgt", "add known gt locations? pass the name of txt file containing gt locations", ""); // if no -addgt argument then the value is false, if there is one then the value is true
  vul_arg<bool> only_gt("-onlygt", "add only known gt locations", false);
  vul_arg<unsigned> tile_id("-tile", "id of the tile", 0);
  vul_arg<unsigned> utm_zone("-zone", "utm zone to fill", 17);
  vul_arg<bool> read("-read", "if passed only read the index in the out_pre() folder and report some statistics", false);
  vul_arg<bool> use_osm_roads("-osm_roads", "if passed, use the osm binary file to read the roads and generate location hypotheses along them", false);
  vul_arg<vcl_string> osm_bin_file("-osm_bin", "the binary file that has all the objects", "");
  vul_arg<vcl_string> osm_tree_folder("-osm_tree", "the geoindex tree folder that has the tree structure with the ids of osm objects at its leaves", "");
  vul_arg<int> world_id("-world", "the id of the world", -1);
  vul_arg<bool> p1b("-p1b", "use the p1b tiles", false);
  vul_arg<vcl_string> land("-land", "the input folder of the land type .tif files", "");

  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  double arcsec_to_sec = 1.0f/3600.0f;
  double inc_in_sec = inc()*arcsec_to_sec;
  double inc_in_sec_radius_ratio = 3.0/4.0;
  double inc_in_sec_rad = inc_in_sec*inc_in_sec_radius_ratio; // radius to search for existence of before adding a new one

  if (p1b()) {

    if (out_pre().compare("") == 0 || in_poly().compare("") == 0) {
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    vcl_cout << "will use increments " << inc() << " arcseconds along north and east directions!\n";

    vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
    vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

    vcl_vector<volm_tile> tiles = volm_tile::generate_p1b_wr_tiles(world_id());
    if (!tiles.size()) {  
      vcl_cerr << "Unknown world id: " << world_id() << vcl_endl;
      return volm_io::EXE_ARGUMENT_ERROR;
    }
     
    vcl_cout << " number of tiles: " << tiles.size() << vcl_endl;
    unsigned i = tile_id();
    if (i >= tiles.size()) {
      vcl_cerr << "Unknown tile id: " << i << vcl_endl;
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    double meter_to_sec = volm_io_tools::meter_to_seconds(tiles[i].lat_, tiles[i].lon_);

    float size = 0.1;  // in seconds, set a fixed size for the leaves
    volm_geo_index_node_sptr hyp_root = volm_geo_index::construct_tree(tiles[i], (float)size, poly);
    // write the geo index and the hyps
    vcl_stringstream file_name; file_name << out_pre() << "geo_index_tile_" << i << ".txt";
    volm_geo_index::write(hyp_root, file_name.str(), (float)size);
    unsigned depth = volm_geo_index::depth(hyp_root);
    vcl_stringstream file_name2; file_name2 << out_pre() << "geo_index_tile_" << i << ".kml";
    volm_geo_index::write_to_kml(hyp_root, 0, file_name2.str());
    vcl_stringstream file_name3; file_name3 << out_pre() << "geo_index_tile_" << i << "_depth_" << depth << ".kml";
    volm_geo_index::write_to_kml(hyp_root, depth, file_name3.str());

    if (only_gt() && add_gt().compare("") != 0) {  // only add the ground truth points in the file as they are given (i.e. with the elevs in the file)
      if (out_pre().compare("") == 0 || world_id() < 0) {
        vul_arg_display_usage_and_exit();
        return volm_io::EXE_ARGUMENT_ERROR;
      }

      vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_pair<vcl_string, int>, vcl_string> > > samples;
      int cnt = volm_io::read_gt_file(add_gt(), samples);
      vcl_cout << " adding " << cnt <<" gt locs!\n";
    
      for (int j = 0; j < cnt; ++j) {
        vcl_cout << samples[j].second.first.first << " adding.. " << samples[j].first.y() << ", " << samples[j].first.x() << ' ';
        bool added = volm_geo_index::add_hypothesis(hyp_root, samples[j].first.x(), samples[j].first.y(), samples[j].first.z());
        if (added) vcl_cout << " success!\n";
        else       vcl_cout << " not found in tree of tile: " << tile_id() << "!\n";
      }

      unsigned r_cnt = volm_geo_index::hypo_size(hyp_root) ;
      vcl_cout << " after addition of gt locs, hyp_root " << i << " has total " << r_cnt << " hypotheses in its leaves!\n";

      // write the hypos
      vcl_stringstream file_name4; file_name4 << out_pre() << "geo_index_tile_" << i;
      vcl_cout << "writing hyps to: " << file_name4.str() << vcl_endl;
      volm_geo_index::write_hyps(hyp_root, file_name4.str());
    
      return volm_io::SUCCESS;
    }

    if (p1b() && use_osm_roads()) {  // if an osm binary file is specified
      if (in_folder().compare("") == 0 || out_pre().compare("") == 0 || osm_bin_file().compare("") == 0 || osm_tree_folder().compare("") == 0 || world_id() < 0) {
        vul_arg_display_usage_and_exit();
        return volm_io::EXE_ARGUMENT_ERROR;
      }

      volm_osm_objects osm_objs; 
      double min_size;
      vcl_stringstream file_name_pre;
      file_name_pre << osm_tree_folder() << "geo_index2_wr" << world_id() << "_tile_" << tile_id();
      volm_geo_index2_node_sptr root = volm_io_tools::read_osm_data_and_tree(file_name_pre.str(), osm_bin_file(), osm_objs, min_size);
      if (!root) {
        vcl_cerr << "Errors in reading " << file_name_pre.str() << " and/or " << osm_bin_file() << vcl_endl;
        return volm_io::EXE_ARGUMENT_ERROR;
      }
      vcl_stringstream kml_roads;
      kml_roads << out_pre() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_roads.kml";
      osm_objs.write_lines_to_kml(kml_roads.str());

      // load the DEM tiles
      vcl_vector<volm_img_info> infos;
      volm_io_tools::load_aster_dem_imgs(in_folder(), infos);

      if (land().compare("") == 0) {  // if not using land types

        // now go over each road in the osm file and insert into the tree of loc hyps
        vcl_vector<volm_osm_object_line_sptr>& roads = osm_objs.loc_lines();
        for (unsigned k = 0; k < roads.size(); k++) {
          volm_osm_object_line_sptr r = roads[k];
          vcl_string name = r->prop().name_;
          vcl_vector<vgl_point_2d<double> > points = r->line();
          for (unsigned kk = 1; kk < points.size(); kk++) {
            if (poly.contains(points[kk-1].x(), points[kk-1].y())) 
              add_hypo(hyp_root, infos, points[kk-1], inc_in_sec_rad, true);
            // now interpolate along a straight line, assume locally planar
            double dif_dy = points[kk].y() - points[kk-1].y();
            double dif_dx = points[kk].x() - points[kk-1].x();
            double ds = vcl_sqrt(dif_dy*dif_dy + dif_dx*dif_dx);
            double inc_dy = inc_in_sec*(dif_dy/ds);
            double inc_dx = inc_in_sec*(dif_dx/ds);
            int cnt = 0;
            while (ds > inc_in_sec) {
              ds = ds-inc_in_sec;
              cnt++;
              double x = points[kk-1].x()+inc_dx*cnt;
              double y = points[kk-1].y()+inc_dy*cnt;
              if (poly.contains(x, y)) 
                add_hypo(hyp_root, infos, vgl_point_2d<double>(x, y), inc_in_sec_rad, false);
            }

          }
        }
      } else {
        vcl_cout << "Generating hyps along the OSM roads and with increments depending on the Geocover land type!\n";
        vcl_vector<volm_img_info> geo_infos;
        volm_io_tools::load_geocover_imgs(land(), geo_infos);

        // now go over each road in the osm file and insert into the tree of loc hyps
        vcl_vector<volm_osm_object_line_sptr>& roads = osm_objs.loc_lines();
        vcl_cout << "will process " << roads.size() << " roads...\n"; vcl_cout.flush();
#if 0
        double y = 12.587963; double x = 76.822019;
        vcl_cout << "testing a location's type: x: " << x << " y: " << y << " (first roads first x: " << roads[0]->line()[0].x() << ")\n";
        int type_prev = find_land_type(geo_infos, vgl_point_2d<double>(x,y));
        vcl_cout << "\t\t type is: " << type_prev << "\n";

        y = 12.575318; x = 76.835210;
        vcl_cout << "testing a location's type: x: " << x << " y: " << y << " (first roads first x: " << roads[0]->line()[0].x() << ")\n";
        type_prev = find_land_type(geo_infos, vgl_point_2d<double>(x,y));
        vcl_cout << "\t\t type is: " << type_prev << "\n";
#endif

        for (unsigned k = 0; k < roads.size(); k++) {
          if (k%1000 == 0) vcl_cout << k << "."; vcl_cout.flush();
          volm_osm_object_line_sptr r = roads[k];
          vcl_string name = r->prop().name_;
          vcl_vector<vgl_point_2d<double> > points = r->line();
          if (!points.size())
            continue;
          int type_prev = find_land_type(geo_infos, points[0]);
          if (type_prev < 0)
            inc_in_sec = 4 * meter_to_sec;  // if unknown type, use the smallest possible interval to be safe
          else 
            inc_in_sec = volm_osm_category_io::geo_land_hyp_increments[type_prev] * meter_to_sec;
          inc_in_sec_rad = inc_in_sec*inc_in_sec_radius_ratio;
          double remainder = 0.0;
          if (poly.contains(points[0].x(), points[0].y())) 
            add_hypo(hyp_root, infos, points[0], inc_in_sec_rad, true);
          for (unsigned kk = 1; kk < points.size(); kk++) {  
            double prev_x = points[kk-1].x();
            double prev_y = points[kk-1].y();
            int type = find_land_type(geo_infos, points[kk]);
            if (type > 0 && type != type_prev) {
              double inc_now = volm_osm_category_io::geo_land_hyp_increments[type] * meter_to_sec;
              inc_in_sec = inc_in_sec < inc_now ? inc_in_sec : inc_now;  // pick the smaller of the increments if different types
              inc_in_sec_rad = inc_in_sec*inc_in_sec_radius_ratio;
            }

            // interpolate along a straight line, assume locally planar
            double dif_dy = points[kk].y() - points[kk-1].y();
            double dif_dx = points[kk].x() - points[kk-1].x();
            double ds = vcl_sqrt(dif_dy*dif_dy + dif_dx*dif_dx);

            if (inc_in_sec > ds) {
              remainder += ds;
              type_prev = type;
              continue;
            }

            double cos = dif_dx/ds;
            double sin = dif_dy/ds;

            double inc_dy = inc_in_sec*sin;
            double inc_dx = inc_in_sec*cos;
            
            // get rid of remainder first
            if (remainder < inc_in_sec) {
              double rem = inc_in_sec-remainder;
              double inc_dy_rem = rem*sin;
              double inc_dx_rem = rem*cos;
              double x = prev_x+inc_dx_rem;
              double y = prev_y+inc_dy_rem;
              if (poly.contains(x, y)) 
                add_hypo(hyp_root, infos, vgl_point_2d<double>(x, y), inc_in_sec_rad, true);
              prev_x = x; 
              prev_y = y;
              ds -= rem;
            } 

            while (ds > inc_in_sec) {
              ds -= inc_in_sec;
              double x = prev_x+inc_dx;
              double y = prev_y+inc_dy;
              if (poly.contains(x, y)) 
                add_hypo(hyp_root, infos, vgl_point_2d<double>(x, y), inc_in_sec_rad, false);
              prev_x = x; 
              prev_y = y;
            }
            type_prev = type;
            remainder = ds;
          }
        }

      }
    
      unsigned r_cnt = volm_geo_index::hypo_size(hyp_root) ;
      vcl_cout << "\n root " << i << " has total " << r_cnt << " hypotheses in its leaves!\n";

      // write the hypos
      vcl_stringstream file_name4; file_name4 << out_pre() << "geo_index_tile_" << i;
      vcl_cout << "writing hyps to: " << file_name4.str() << vcl_endl;
      volm_geo_index::write_hyps(hyp_root, file_name4.str());

      vcl_vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(hyp_root, leaves);
      for (unsigned jj = 0; jj < leaves.size(); jj++) {
        vcl_string out_file = leaves[jj]->get_hyp_name(file_name4.str()) + ".kml";
        leaves[jj]->hyps_->write_to_kml(out_file, inc_in_sec_rad);
      }

      return volm_io::SUCCESS;
    }
  }
  // for P1A and P1B regions reading (world_id 6 and 7 becomes desert and coast)

  // change to wr1 tiles for desert
  vcl_vector<volm_tile> tiles;
  if (world_id() == 1)      tiles = volm_tile::generate_p1b_wr1_tiles();
  else if (world_id() == 2) tiles = volm_tile::generate_p1b_wr2_tiles();
  else if (world_id() == 3) tiles = volm_tile::generate_p1b_wr3_tiles();
  else if (world_id() == 4) tiles = volm_tile::generate_p1b_wr4_tiles();
  else if (world_id() == 5) tiles = volm_tile::generate_p1b_wr5_tiles();
  else if (world_id() == 6) tiles = volm_tile::generate_p1_wr1_tiles();
  else if (world_id() == 7) tiles = volm_tile::generate_p1_wr2_tiles();
  vcl_cout << " number of tiles: " << tiles.size() << vcl_endl;
  unsigned i = tile_id();
  if (i >= tiles.size()) {
    vcl_cerr << "tile id: " << i << " is greater than number of tiles: " << tiles.size() << "!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  if (read()) {
    if (in_folder().compare("") == 0) {
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    float min_s;
    vcl_stringstream file_name; file_name << out_pre() << "geo_index_tile_" << i;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name.str() + ".txt", min_s);
    vcl_vector<volm_geo_index_node_sptr> leaves2;
    volm_geo_index::get_leaves(root, leaves2);
    vcl_cout << "\t number of leaves: " << leaves2.size() << vcl_endl;
    volm_geo_index::read_hyps(root, file_name.str());
    vcl_cout << " read hyps!\n";
    vcl_vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(root, leaves);
    vcl_cout << "Geo index for tile: " << i << " stored in: " << file_name.str() << '\n'
             << "\t number of leaves with hyps: " << leaves.size() << vcl_endl;
    unsigned size = volm_geo_index::hypo_size(root);
    vcl_cout << "\t total number of hypos: " << size << vcl_endl;
    for (unsigned l = 0; l < leaves.size(); l++) {
      vcl_cout << " leaf_id = " << l << " leaf bbox = " << leaves[l]->extent_ << vcl_endl;
    }
    return volm_io::SUCCESS;
  }


  if (in_folder().compare("") == 0 || out_pre().compare("") == 0 || in_poly().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << "will use increments " << inc() << " arcseconds along north and east directions!\n";

  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  // determine depth of the geo index depending on inc, if we want to have 100x100 = 10K hyps in each leaf
  double size = nh()*inc_in_sec; // inc() is given in arcseconds, convert it to seconds;
  vcl_cout << " each leaf has size: " << size << " seconds in geographic coords..\n"
           << " increments in seconds: " << inc_in_sec << '\n'
           << " increments in meters: " << (inc_in_sec*21/0.000202) << '\n'
           << " only putting hyps in UTM zone: " << utm_zone() << '\n';


  volm_geo_index_node_sptr root = volm_geo_index::construct_tree(tiles[i], (float)size, poly);

  // write the geo index and the hyps
  vcl_stringstream file_name; file_name << out_pre() << "geo_index_tile_" << i << ".txt";
  volm_geo_index::write(root, file_name.str(), (float)size);
  unsigned depth = volm_geo_index::depth(root);
  vcl_stringstream file_name2; file_name2 << out_pre() << "geo_index_tile_" << i << ".kml";
  volm_geo_index::write_to_kml(root, 0, file_name2.str());
  vcl_stringstream file_name3; file_name3 << out_pre() << "geo_index_tile_" << i << "_depth_" << depth << ".kml";
  volm_geo_index::write_to_kml(root, depth, file_name3.str());

  // prune the out of zone leaves
  if (!volm_geo_index::prune_by_zone(root, utm_zone())) {
    vcl_cout << " root " << i << " is not in zone: " << utm_zone() << "! no hypotheses in its leaves!\n";
    return 0;
  }
  if (!only_gt()) {
  vcl_string file_glob = in_folder() + "/*.tif";
  unsigned cnt = 0;
  for (vul_file_iterator fn=file_glob; fn; ++fn, ++cnt) {
    vcl_string tiff_fname = fn();

    vil_image_view_base_sptr img_sptr = vil_load(tiff_fname.c_str());
    vil_image_view<float> img(img_sptr);
    unsigned ni = img.ni(); unsigned nj = img.nj();
    volm_tile t(tiff_fname, ni, nj);
    //t.write_kml(out_pre() + t.get_string() + ".kml", 0);

    // write the geo index and the hyps
    vcl_vector<volm_geo_index_node_sptr> leaves;
    vgl_box_2d<double> leaf_box = t.bbox_double();
    volm_geo_index::get_leaves(root, leaves, leaf_box);
    if (!leaves.size())
      continue;
    float size_leaf = leaves[0]->extent_.width();
#if 0
    volm_index_node_sptr dummy_root = new volm_index_node(t.bbox());
    dummy_root->children_ = leaves;
    vcl_stringstream file_name; file_name << out_pre() << "geo_index_tile_" << i << "_intersection_" << cnt << ".kml";
    volm_index::write_to_kml(dummy_root, 1, file_name.str());
#endif
    // generate the hyps and find heights from LIDAR
    for (unsigned j = 0; j < leaves.size(); ++j) {
      if (!leaves[j]->hyps_)
        leaves[j]->hyps_ = new volm_loc_hyp();
      float lower_left_lon = leaves[j]->extent_.min_point().x();
      float lower_left_lat = leaves[j]->extent_.min_point().y();
      unsigned nhi = (unsigned)vcl_ceil(size_leaf/inc_in_sec);
      for (unsigned hi = 0; hi < nhi; ++hi) {
        double lon = lower_left_lon + hi*inc_in_sec;
        for (unsigned hj = 0; hj < nhi; ++hj) {
          double lat = lower_left_lat + hj*inc_in_sec;
          vpgl_utm u; int zone;  double x, y;
          u.transform(lat, lon, x, y, zone);
          if (zone != (int)utm_zone())
            continue;
          unsigned ii, jj;
          bool contains = t.global_to_img(lon, lat, ii, jj);
          if (contains) {
            float z = img(ii, jj);
            unsigned id;
            if (z > 0  && !(leaves[j]->hyps_->exist(lat, lon, inc_in_sec_rad, id)))
              leaves[j]->hyps_->add(lat, lon, z);
          }
        }
      }
    }

    //if (cnt > 0)
    //  break;
  }
  }
  unsigned r_cnt = volm_geo_index::hypo_size(root) ;
  vcl_cout << " root " << i << " has total " << r_cnt << " hypotheses in its leaves!\n";
 
  if (add_gt().compare("") != 0) {  // user passed the path to a text file with the gt locations

    // load the images
    vcl_string file_glob = in_folder() + "/*.tif";
    vcl_vector<vcl_pair<vil_image_view_base_sptr,  volm_tile > > tiles;
    for (vul_file_iterator fn=file_glob; fn; ++fn) {
      vcl_string tiff_fname = fn();

      vil_image_view_base_sptr img_sptr = vil_load(tiff_fname.c_str());
      unsigned ni = img_sptr->ni(); unsigned nj = img_sptr->nj();
      volm_tile t(tiff_fname, ni, nj);
      tiles.push_back(vcl_pair<vil_image_view_base_sptr, volm_tile>(img_sptr, t));
    }


#if 0    //: add any gt positions if any
    if (volm_geo_index::add_hypothesis(root, -79.857689, 32.759063, 1.60))
      vcl_cout << " added p1a_test1_06-GROUNDTRUTH\n";

    if (volm_geo_index::add_hypothesis(root, -79.813014, 32.775959, 4.41))
      vcl_cout << " added p1a_test1_28-GROUNDTRUTH\n";
    if (volm_geo_index::add_hypothesis(root, -78.282153, 33.911997, 1.60))
      vcl_cout << " added p1a_test1_08-GROUNDTRUTH\n";
    if (volm_geo_index::add_hypothesis(root, -78.281430, 33.912397, 1.60))
      vcl_cout << " added p1a_test1_46-GROUNDTRUTH\n";
    if (volm_geo_index::add_hypothesis(root, -81.550366, 30.720336, 1.60))
      vcl_cout << " added p1a_test1_18-GROUNDTRUTH\n";
    if (volm_geo_index::add_hypothesis(root, -79.951930, 32.648980, 1.60))
      vcl_cout << " added p1a_test1_38-GROUNDTRUTH\n";
    if (volm_geo_index::add_hypothesis(root, -79.268871, 33.365799, 1.60))
      vcl_cout << " added p1a_test1_34-GROUNDTRUTH\n";
#endif

    vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_pair<vcl_string, int>, vcl_string> > > samples;
    int cnt = volm_io::read_gt_file(add_gt(), samples);
    vcl_cout << " adding " << cnt <<" gt locs!\n";
    for (int j = 0; j < cnt; ++j) {
      vpgl_utm u; int zone;  double x, y;
      u.transform(samples[j].first.y(), samples[j].first.x(), x, y, zone);
      if (zone != (int)utm_zone()) {
        vcl_cout << samples[j].second.first.first << " is in zone: " << zone <<" not in " << utm_zone() << " skipping!\n";
        continue;
      }
      vcl_cout << samples[j].second.first.first << " adding.. " << samples[j].first.y() << ", " << samples[j].first.x() << ' ';

      // find which box contains it
      bool added = false;
      for (unsigned kk = 0; kk < tiles.size(); kk++) {
         unsigned ii, jj;
         bool contains = tiles[kk].second.global_to_img(samples[j].first.x(), samples[j].first.y(), ii, jj);
         if (contains) {
          vil_image_view<float> img(tiles[kk].first);
          float z = img(ii, jj);
          // check the neighborhood
          for (int ii2 = ii - 1; ii2 <= ii+1; ii2++)
            for (int jj2 = jj - 1; jj2 <= jj+1; jj2++) {
              if (ii2 >= 0 && jj2 >= 0 && ii2 < img.ni() && jj2 < img.nj()) {
                if (z < img(ii2, jj2)) z = img(ii2, jj2);
              }
            }
          if (z > 0.0f) {
            vcl_cout << " corrected height from: " << samples[j].first.z() << " to: " << z+1.6 << '\n';
            added = volm_geo_index::add_hypothesis(root, samples[j].first.x(), samples[j].first.y(), z+1.6);
          } else {
            vcl_cout << " height from LIDAR is: " << z << " writing original height: " << samples[j].first.z() << '\n';
            added = volm_geo_index::add_hypothesis(root, samples[j].first.x(), samples[j].first.y(), samples[j].first.z());
          }
          break;
         }
      }
      //bool added = volm_geo_index::add_hypothesis(root, samples[j].first.x(), samples[j].first.y(), samples[j].first.z());
      if (added) vcl_cout << " success!\n";
      else       vcl_cout <<" not found in tree of tile: " << tile_id() << "!\n";
    }

    unsigned r_cnt = volm_geo_index::hypo_size(root) ;
    vcl_cout << " after addition of gt locs, root " << i << " has total " << r_cnt << " hypotheses in its leaves!\n";
  }

  // write the hypos
  vcl_stringstream file_name4; file_name4 << out_pre() << "geo_index_tile_" << i;
  vcl_cout << "writing hyps to: " << file_name4.str() << vcl_endl;
  volm_geo_index::write_hyps(root, file_name4.str());
//#if DEBUG
  vcl_vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);
  vcl_stringstream file_name5; file_name5 << out_pre() << "geo_index_tile_" << i << "_hyps.kml";
  leaves[0]->hyps_->write_to_kml(file_name5.str(), inc_in_sec);
//#endif

  vcl_cout << "total time: " << t.all()/1000 << " seconds = " << t.all()/(1000*60) << " mins.\n";
  return volm_io::SUCCESS;
}

