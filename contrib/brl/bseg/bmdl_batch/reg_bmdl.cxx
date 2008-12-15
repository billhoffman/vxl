#include "reg_bmdl.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
// processes
#include <vil_pro/vil_load_image_view_process.h>
#include <vil_pro/vil_save_image_view_process.h>
#include <vil_pro/vil_rgbi_to_grey_process.h>
#include <vpgl_pro/vpgl_load_proj_camera_process.h>
#include <vpgl_pro/vpgl_load_perspective_camera_process.h>
#include <vpgl_pro/vpgl_save_perspective_camera_process.h>
#include <vpgl_pro/vpgl_load_rational_camera_process.h>
#include <vpgl_pro/vpgl_save_rational_camera_process.h>
#include <vpgl_pro/vpgl_load_rational_camera_nitf_process.h>

// datatypes
#include <vcl_string.h>
#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera.h>

// bmdl files
#include <bmdl/pro/bmdl_classify_process.h>
#include <bmdl/pro/bmdl_trace_boundaries_process.h>
#include <bmdl/pro/bmdl_generate_mesh_process.h>
#include <bmdl/pro/bmdl_lidar_roi_process.h>
#include <bmdl/pro/bmdl_modeling_process.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  REG_PROCESS(vil_load_image_view_process, bprb_batch_process_manager);
  REG_PROCESS(vil_save_image_view_process, bprb_batch_process_manager);
  REG_PROCESS(vil_rgbi_to_grey_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_rational_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_save_rational_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_rational_camera_nitf_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_proj_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_perspective_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_save_perspective_camera_process, bprb_batch_process_manager);

  REG_PROCESS(bmdl_classify_process, bprb_batch_process_manager);
  REG_PROCESS(bmdl_trace_boundaries_process, bprb_batch_process_manager);
  REG_PROCESS(bmdl_generate_mesh_process, bprb_batch_process_manager);
  REG_PROCESS(bmdl_lidar_roi_process, bprb_batch_process_manager);
  REG_PROCESS(bmdl_modeling_process, bprb_batch_process_manager);
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  REGISTER_DATATYPE(bool);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(int);
  REGISTER_DATATYPE(unsigned);
  REGISTER_DATATYPE(long);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(double);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  typedef vbl_smart_ptr<vpgl_camera<double> > vpgl_camera_double_sptr;
  REGISTER_DATATYPE( vpgl_camera_double_sptr );

  Py_INCREF(Py_None);
  return Py_None;
}

PyMODINIT_FUNC
initbmdl_batch(void)
{
  PyMethodDef reg_pro;
  reg_pro.ml_name = "register_processes";
  reg_pro.ml_meth = register_processes;
  reg_pro.ml_doc = "register_processes() create instances of each defined process";
  reg_pro.ml_flags = METH_VARARGS;
  

  PyMethodDef reg_data;
  reg_data.ml_name = "register_datatypes";
  reg_data.ml_meth = register_datatypes;
  reg_data.ml_doc = "register_datatypes() insert tables in the database for each type";
  reg_data.ml_flags = METH_VARARGS;
  
  bmdl_batch_methods[0]=reg_pro;
  bmdl_batch_methods[1]=reg_data;
  
  for (unsigned i=0; i<METHOD_NUM; i++) {
    bmdl_batch_methods[i+2]=batch_methods[i];
  }

  Py_InitModule("bmdl_batch", bmdl_batch_methods);
}

