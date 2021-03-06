#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <volm/volm_osm_parser.h>
#include <bkml/bkml_parser.h>
#include <bkml/bkml_write.h>

static void test_osm_parser()
{
#if 0
  vcl_string filename = "D:/work/vishal_point_cloud/starbucks.osm";
  vcl_vector<vgl_point_3d<double> > loc_pts;
  volm_osm_parser::parse_points_3d(loc_pts, filename);

  vcl_string kml_file = "D:/work/vishal_point_cloud/starbucks.kml";
  vcl_ofstream ofs_kml(kml_file.c_str());
  bkml_write::open_document(ofs_kml);
  vcl_string name = "";
  vcl_string description = "";
  for (unsigned i = 0; i < loc_pts.size(); i++) {
    bkml_write::write_location(ofs_kml, loc_pts[i].y(), loc_pts[i].x(), loc_pts[i].z(), name, description, 0.3, 255,0,0);
  }
  bkml_write::close_document(ofs_kml);
#endif
//// write out points of interests
//  for (unsigned i = 0; i < loc_pts.size(); i++) {
//    vcl_string name = "location";
//    for (unsigned j = 0; j < pt_keys[i].size(); j++)
//      if (pt_keys[i][j].first.compare("name:en") == 0)
//        name = pt_keys[i][j].second;
//    bkml_write::write_location(ofs_kml, loc_pts[i], name);
//  }

  vcl_string filename = "d:/work/vxl/bin_vs10/contrib/brl/bbas/volm/tests/Debug/test.osm";

  //test the method of parsing places
  vcl_vector<vgl_point_2d<double> > loc_pts;
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > pt_keys;
  volm_osm_parser::parse_points(loc_pts, pt_keys, filename);

  TEST("method of parsing places", loc_pts.size(), pt_keys.size());

  // test the method of parsing lines
  vcl_vector<vcl_vector<vgl_point_2d<double> > > lines;
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > line_keys;
  volm_osm_parser::parse_lines(lines, line_keys, filename);

  TEST("method of parsing lines", lines.size(), line_keys.size());

  // test the method of parsing polygons
  vcl_vector<vgl_polygon<double> > polygons;
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > poly_keys;
  volm_osm_parser::parse_polygons(polygons, poly_keys, filename);

  TEST("method of parsing polygons", polygons.size(), poly_keys.size());

  // write out
  vcl_string kml_file = "./test.kml";
  vcl_ofstream ofs_kml(kml_file.c_str());
  bkml_write::open_document(ofs_kml);

  // write out points of interests
  for (unsigned i = 0; i < loc_pts.size(); i++) {
    vcl_string name = "location";
    for (unsigned j = 0; j < pt_keys[i].size(); j++)
      if (pt_keys[i][j].first.compare("name:en") == 0)
        name = pt_keys[i][j].second;
    bkml_write::write_location(ofs_kml, loc_pts[i], name);
  }

  // write out paths
  for (unsigned i = 0; i < lines.size(); i++) {
    vcl_string name = "path";
    for (unsigned j = 0; j < line_keys[i].size(); j++)
      if (line_keys[i][j].first.compare("name:en") == 0)
        name = line_keys[i][j].second;
    bkml_write::write_path(ofs_kml, lines[i], name);
  }

  // write out polygons
  for (unsigned i = 0; i < polygons.size(); i++) {
    vcl_string name = "polygon";
    for (unsigned j = 0; j < poly_keys[i].size(); j++)
      if (poly_keys[i][j].first.compare("name:en") == 0)
        name = poly_keys[i][j].second;
    bkml_write::write_polygon(ofs_kml, polygons[i], name);
  }
  bkml_write::close_document(ofs_kml);
  ofs_kml.close();

}

TESTMAIN(test_osm_parser);