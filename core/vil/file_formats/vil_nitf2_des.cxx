// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_des.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_typed_field_formatter.h"

vil_nitf2_des::vil_nitf2_des( vil_nitf2_classification::file_version version, int data_width )
  : m_field_sequence( *create_field_definitions( version, data_width ) )
{ }

bool vil_nitf2_des::read(vil_stream* stream)
{
  return m_field_sequence.read(*stream);
}

vil_nitf2_field_definitions* vil_nitf2_des::create_field_definitions( vil_nitf2_classification::file_version ver, int data_width )
{
  vil_nitf2_field_definitions* field_definitions = new vil_nitf2_field_definitions();
  add_shared_field_defs_1(field_definitions);
  vil_nitf2_classification::add_field_defs(field_definitions, ver, "I", "Image");
  add_shared_field_defs_2(field_definitions, data_width);
  return field_definitions;
}

void vil_nitf2_des::add_shared_field_defs_1( vil_nitf2_field_definitions* defs )
{
  (*defs)
    .field( "DE", "Data Extension Subheader", NITF_ENUM( 2, vil_nitf2_enum_values().value( "DE" ) ), false, 0, 0 )
    .field( "DESID", "Unique DES Type Identifier", NITF_ENUM( 25, vil_nitf2_enum_values().value( "TRE_OVERFLOW" ) ), 
            false, 0, 0 )
    .field( "DESVER", "Version of the Data Definition", NITF_INT( 2, false ), false, 0, 0 );
}

void vil_nitf2_des::add_shared_field_defs_2( vil_nitf2_field_definitions* defs, int data_width )
{
  vil_nitf2_enum_values overflow_enum;
  overflow_enum.value( "UDHD", "User Defined Header Data" )
               .value( "UDID" "User Defined Image Data" )
               .value( "XHD", "Extended Header Data" )
               .value( "IXSHD", "Image Extended Subheader Data" )
               .value( "SXSHD", "Graphic Extended Subheader Data" )
               .value( "TXSHD", "Text Extended Subheader Data" );
  (*defs)
    .field( "DESOFLW", "Overflowed Header Type", NITF_ENUM( 6, overflow_enum ), 
            false, 0, new vil_nitf2_field_value_one_of<vcl_string>( "DESID", "TRE_OVERFLOW" ) )
    .field( "DESITEM", "Data Item Overflowed", NITF_INT( 3, false ), 
            false, 0, new vil_nitf2_field_value_one_of<vcl_string>( "DESID", "TRE_OVERFLOW" ))
    .field( "DESSHL", "Length of DES-Defined Subheader Fields", NITF_INT( 4, false ), false, 0, 0 )
    .field( "DESDATA", "DES-Defined Data Field", NITF_TRES(), false, 
            new vil_nitf2_max_field_value_plus_offset_and_threshold( "DESSHL", data_width, 0, -1 ), 
            new vil_nitf2_field_value_one_of<vcl_string>( "DESID", "TRE_OVERFLOW" ) )
    .end();
}

vil_nitf2_field::field_tree* vil_nitf2_des::get_tree( int i ) const
{
  vil_nitf2_field::field_tree* t = new vil_nitf2_field::field_tree;
  vcl_stringstream name_stream;
  name_stream << "Data Extension Segment";
  if( i > 0 ) name_stream << " #" << i;
  t->columns.push_back( name_stream.str() );
  m_field_sequence.get_tree( t );
  return t;
}

vil_nitf2_des::~vil_nitf2_des()
{ }
