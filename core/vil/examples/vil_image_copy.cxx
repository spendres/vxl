//:
// \file
// \brief Example of loading an image and saving it again (to test image loaders)
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <testlib/testlib_root_dir.h>
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_print.h>

int main(int argc, char** argv)
{
  if (argc!=3)
  {
    vcl_cout<<"vil2_image_copy  src_image dest_image\n";
    vcl_cout<<"Loads from file src_image, saves to file dest_image\n";
    return 0;
  }

  vil2_image_view<vxl_byte> src_im = vil2_load(argv[1]);
  if (src_im.size()==0)
  {
    vcl_cout<<"Unable to load source image from "<<argv[1]<<vcl_endl;
    return 1;
  }

  vcl_cout<<"Loaded image of size "<<src_im.ni()<<" x "<<src_im.nj()<<vcl_endl;

  if (!vil2_save(src_im, argv[2]))
  {
    vcl_cerr<<"Unable to save result image to "<<argv[2]<<vcl_endl;
    return 1;
  }

  vcl_cout<<"Saved image to "<<argv[2]<<vcl_endl;

  return 0;
}
