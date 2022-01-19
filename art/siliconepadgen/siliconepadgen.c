#include <stdio.h>
#include <math.h> 
#include <time.h>
#include <stdlib.h>
#include <string.h>

const char * GetGUID()
{
	// Version 4 GUID https://stackoverflow.com/a/21980903/2926815
	// NOTE: Modification: srand() with call count for last numbers.
	static char GUID[40];
	static int callct;
	srand(clock());
	int t = 0;
	char *szTemp = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
	char *szHex = "0123456789ABCDEF-";
	int nLen = strlen (szTemp);

	for (t=0; t<nLen+1; t++)
	{
		int r = rand () % 16;
		char c = ' ';   

		switch (szTemp[t])
		{
			case 'x' : { c = szHex [r]; } break;
			case 'y' : { c = szHex [r & 0x03 | 0x08]; srand( callct++ ); } break;
			case '-' : { c = '-'; } break;
			case '4' : { c = '4'; } break;
		}

		GUID[t] = ( t < nLen ) ? c : 0x00;
	}
	GUID[t] = 0;

	return GUID;
}

const float foot_outside = 9.5/2; //radius
const float foot_inside = 6.5/2; //radius
const float pad_size = 4.0/2 + .3; //radius
const char * partname = "BTC007_Button_Silicone";

const int teeth = 6;
const float backoff_from_side = 0.4;
const float drawlinewidth = 0.21;

#define mask_courtyard ( pad_size + 0.3 )
#define silk_courtyard ( foot_outside + 0.3 )


int main()
{
	float w = 800;
	float h = 800;
	float cx = w/2;
	float cy = h/2;
	char filename[1024];
	sprintf( filename, "../../Swadge_Parts.pretty/%s.kicad_mod", partname );
	FILE * part = fopen( filename, "w" );
	fprintf( part, "(footprint \"%s\" (version 20211014) (generator pcbnew)\n\
  (layer \"F.Cu\")\n\
  (tedit 61CF7589)\n\
  (attr through_hole)\n\
  (fp_text reference \"Ref**\" (at 0 0) (layer \"F.SilkS\") hide\n\
    (effects (font (size 1.27 1.27) (thickness 0.15)))\n\
    (tstamp %s)\n\
  )\n\
  (fp_text value \"Val**\" (at 0 0) (layer \"F.SilkS\") hide\n\
    (effects (font (size 1.27 1.27) (thickness 0.15)))\n\
    (tstamp %s)\n\
  )\n", partname, strdup( GetGUID() ), strdup( GetGUID() ) );
  
	fprintf( part, "(fp_circle (center 0 0) (end %f 0) (layer \"F.SilkS\") (width 0.2) (fill none) (tstamp %s))\n", silk_courtyard, GetGUID() );
	fprintf( part, "(fp_circle (center 0 0) (end %f 0) (layer \"F.Fab\") (width 0.12) (fill none) (tstamp %s))\n", foot_outside, GetGUID() );
	fprintf( part, "(fp_circle (center 0 0) (end %f 0) (layer \"F.Fab\") (width 0.12) (fill none) (tstamp %s))\n", foot_inside, GetGUID() );
	fprintf( part, "(fp_circle (center 0 0) (end %f 0) (layer \"F.Fab\") (width 0.12) (fill none) (tstamp %s))\n", pad_size, GetGUID() );
	fprintf( part, "(fp_circle (center 0 0) (end %f 0) (layer \"F.Mask\") (width 0.12) (fill solid) (tstamp %s))\n", mask_courtyard, GetGUID() );
  
//	fprintf( part, "(pad \"1\" smd roundrect (at 0.0 %f) (size 2 2) (layers \"F.Cu\" \"F.Paste" \"F.Mask\") (roundrect_rratio 0.25) (tstamp %s))", mask_courtyard, GetGUID() );
//	fprintf( part, "(pad \"2\" smd roundrect (at 0.0 %f) (size 2 2) (layers \"F.Cu\" \"F.Paste" \"F.Mask\") (roundrect_rratio 0.25) (tstamp %s))", -mask_courtyard, GetGUID() );

	int i = 0;
	for( i = 0; i < 2; i++ )
	{
		float side = i?1:-1;
		float pinx = 0;
		float piny = pad_size * side;
		fprintf( part, "(pad \"%d\" smd custom (at %f %f) (size .2 .2) (layers \"F.Cu\")\n\
			(options (clearance outline) (anchor circle))\n\
			(primitives\n", i+1, pinx, piny );
			//(gr_poly (pts\n, i+1, pinx, piny );
			//) (width %f)
			//(gr_line (start 0 0) (end 10 10) (width 0.4) (fill yes))
		
		//Start by making the outside edge;
		float theta;
		float epsilon = 0.1;
		for( theta = -1.57; theta < 1.3; theta += epsilon )
		{
			float px = sin( theta ) * pad_size * side;
			float py = cos( theta ) * pad_size * side - pad_size * side;
			float px1 = sin( theta + epsilon ) * pad_size * side;
			float py1 = cos( theta + epsilon ) * pad_size * side - pad_size * side;
			fprintf( part, "(gr_line (start %f %f) (end %f %f) (width %f) (fill yes))", px, py, px1, py1, drawlinewidth );
		}
		
		int tooth;
		for( tooth = 1; tooth < teeth; tooth++ )
		{
			float toothedge = ((float)tooth)/(teeth/2-.25) - 1.0;
			float theta = asin( toothedge );
			printf( "%f / %d\n", theta, tooth );
			float px = sin( theta ) * pad_size * side;
			float py = cos( theta ) * pad_size * side - pad_size * side;
			float px1 = px;
			
			float reeps = 3.14159/2/teeth/2.0;
			if( toothedge < 0 ) reeps *= -1;
			float py1 = cos( theta + reeps ) * pad_size * -side - pad_size * side;
			if( tooth != 0 )
				py1 += backoff_from_side*side;
			if( tooth == teeth-1 )
				py1 = - pad_size * side; //Zero last snub
			fprintf( part, "(gr_line (start %f %f) (end %f %f) (width %f) (fill yes))", px, py, px1, py1, drawlinewidth );
		}
		
//		int j;
//		for( j = 0; j < 10; j++ )
//			fprintf( part, "(gr_line (start %f %f) (end %f %f) (width %f) (fill yes))\n", (float)j, 0, (float)j, 1.0f, drawlinewidth );

		fprintf( part, ") (tstamp %s))\n", GetGUID() );
	}
	

	fprintf( part, ")\n" );
	fclose( part );
	return 0;
}
