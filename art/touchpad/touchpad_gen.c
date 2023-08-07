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

const float edge_override = .16;  // phase clocking of inside edge
const int sections = 5;
const float max_edge = 0.80;  // clearance on outside.
const float inside_standoff = 0.045; //Clearance between inside and outside.
const float inside_radius = 0.5;
const float core_radius = 0.4;
const float tooth_depth = 1.0;
const float pcbscale = 0.04;
const float epsilon = 0.02;
const float edge_width = 0.1; //in mm of width of edge line.
const float inside_epsilon = 0.002;
const int teeth = 3;
const int centerteeth = 10;
const float inside_phase = 1.1;

float CenterFunction( float theta )
{
	float tzone = centerteeth * theta / 3.14159;
	tzone += inside_phase;
	int tza = tzone;
	float tzp = tzone - tza;
	if( tzp < 0.5 )
	{
		return tzp*2.;
	}
	else
	{
		return 2.0-tzp*2.0;
	}
}

int main( int argc, char ** argv )
{
	if( argc < 2 )
	{
		fprintf( stderr, "Error: need file to output.\n" );
		return -9;
	}
	float w = 800;
	float h = 800;
	float cx = w/2;
	float cy = h/2;
	FILE * f = fopen( argv[1]git, "w" );
	FILE * part = fopen( "../../Swadge_Parts.pretty/Touch_Teeth_5.kicad_mod", "w" );
	fprintf( f, "<svg version=\"1.1\" width=\"%f\" height=\"%f\" xmlns=\"http://www.w3.org/2000/svg\">\n", w, h );
	fprintf( part, "(footprint \"Touch_Theeth_5\" (version 20211014) (generator pcbnew)\n\
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
  )\n", strdup( GetGUID() ), strdup( GetGUID() ) );
  

	float theta;
	float r;
	int section = 0;
	for( section = 0; section < sections; section++ )
	{
		float thetamin = (section) / (float)sections * 3.1415926 * 2.0;
		float thetamax = (section+max_edge) / (float)sections * 3.1415926 * 2.0;

		theta = (thetamin+thetamax)/2. + (0.7f*max_edge/sections)*6.283185f;
		r = 0.98;
		float x = cx + sin( theta - 1 ) * w * r/2;
		float y = cy + cos( theta - 1 ) * h * r/2;
		float pinx = x*pcbscale;
		float piny = y*pcbscale;
	
		fprintf( part, "(pad \"%d\" smd custom (at %f %f) (size 1 1) (layers \"F.Cu\")\n\
		(options (clearance outline) (anchor circle))\n\
		(primitives\n\
		(gr_poly (pts\n", section+1, pinx, piny );
	
		
		fprintf( f, "<polygon points=\"" );
		int toothedge;
		
		// Outside Edge
		r = 1;
		for( theta = thetamin; theta <= thetamax; theta += epsilon )
		{
			float x = cx + sin( theta ) * w/2.0*r;
			float y = cy + cos( theta ) * h/2.0*r;
			fprintf( f, "%f,%f ", x, y );
			fprintf( part, "(xy %f %f)\n", x*pcbscale-pinx, y*pcbscale-piny );
			printf( "A %f / %f\n", theta, r );
		}


		// Leading Teeth
		for( toothedge = 0; toothedge < teeth*2; toothedge++ )
		{
			if( (toothedge & 1) == 0 )
			{
				// Going Backwards
				for( theta = thetamax; theta > thetamax-tooth_depth; theta -= epsilon )
				{
					r = ((float)toothedge / (float)(teeth*2)) + (1./(float)(teeth*2)) * (thetamax-theta)/tooth_depth;
					r = (1.0 - (1.0-inside_radius)*r);
					float x = cx + sin( theta ) * w/2.0*r;
					float y = cy + cos( theta ) * h/2.0*r;
					fprintf( f, "%f,%f ", x, y );
					fprintf( part, "(xy %f %f)\n", x*pcbscale-pinx, y*pcbscale-piny );
					printf( "B %f / %f\n", theta, r );
				}
			}
			else
			{
				// Going forwards
				for( theta = thetamax-tooth_depth; theta < thetamax; theta += epsilon )
				{
					r = ((float)toothedge / (float)(teeth*2)) + (1./(float)(teeth*2)) * (tooth_depth-(thetamax-theta))/tooth_depth;
					r = (1.0 - (1.0-inside_radius)*r);
					float x = cx + sin( theta ) * w/2.0*r;
					float y = cy + cos( theta ) * h/2.0*r;
					fprintf( f, "%f,%f ", x, y );
					fprintf( part, "(xy %f %f)\n", x*pcbscale-pinx, y*pcbscale-piny );
					printf( "C %f / %f\n", theta, r );
				}
			}
		}

		// Inside Edge
		for( ; theta >= thetamin-edge_override; theta -= inside_epsilon )
		{
			float x = cx + sin( theta ) * w/2.0*r;
			float y = cy + cos( theta ) * h/2.0*r;
			r = core_radius + (inside_radius-core_radius)*CenterFunction(theta);
			fprintf( f, "%f,%f ", x, y );
			printf( "D %f / %f\n", theta, r );
			fprintf( part, "(xy %f %f)\n", x*pcbscale-pinx, y*pcbscale-piny );
		}

		//Trailing Teeth
		for( toothedge = 0; toothedge < teeth*2; toothedge++ )
		{
			if( (toothedge & 1) == 0 )
			{
				// Going Backwards
				if( toothedge != 0 ) theta = thetamin;
				int first = 1;
				for( ; theta > thetamin-tooth_depth; theta -= epsilon )
				{
					r = ((float)toothedge / (float)(teeth*2)) + (1./(float)(teeth*2)) * (thetamin-theta)/tooth_depth;
					if( toothedge == 0 && first )
					{
						r -= 0.05; // Champfered inside edge.
					}
					first = 0;
					r = (inside_radius + (1.0-inside_radius)*r);
					float x = cx + sin( theta ) * w/2.0*r;
					float y = cy + cos( theta ) * h/2.0*r;
					fprintf( f, "%f,%f ", x, y );
					printf( "B %f / %f\n", theta, r );
					fprintf( part, "(xy %f %f)\n", x*pcbscale-pinx, y*pcbscale-piny );
				}
			}
			else
			{
				// Going forwards
				for( theta = thetamin-tooth_depth; theta < thetamin; theta += epsilon )
				{
					r = ((float)toothedge / (float)(teeth*2)) + (1./(float)(teeth*2)) * (tooth_depth-(thetamin-theta))/tooth_depth;
					r = (inside_radius + (1.0-inside_radius)*r);
					float x = cx + sin( theta ) * w/2.0*r;
					float y = cy + cos( theta ) * h/2.0*r;
					fprintf( f, "%f,%f ", x, y );
					printf( "C %f / %f\n", theta, r );
					fprintf( part, "(xy %f %f)\n", x*pcbscale-pinx, y*pcbscale-piny );
				}
			}
		}
		
		theta = (thetamin+thetamax)/2. + (0.7f*max_edge/sections)*6.283185f;
		r = 0.98;
		x = cx + sin( theta ) * w * r/2;
		y = cy + cos( theta ) * h * r/2;
		fprintf( f, "\" style=\"fill:black\" />" );
		//fprintf( part, ") (layer \"F.Cu\") (width 0.01) (fill solid) (tstamp %s))\n", GetGUID() );
		//fprintf( part, "(pad \"%d\" smd circle (at %f %f) (size 1.524 1.524) (layers \"F.Cu\") (tstamp %s))\n", section+1, x*pcbscale, y*pcbscale, GetGUID() );
        fprintf( part, ") (width %f) (fill yes))) (tstamp %s))\n", edge_width, GetGUID() );

	}

//	fprintf( part,  "(fp_poly (pts\n" );
	float pinx = w/2*pcbscale;
	float piny = h/2*pcbscale;
	fprintf( part, "(pad \"%d\" smd custom (at %f %f) (size 1 1) (layers \"F.Cu\")\n\
		(options (clearance outline) (anchor circle))\n\
		(primitives\n\
		(gr_poly (pts\n", section+1, pinx, piny);


	fprintf( f, "<polygon points=\"" );
	for( theta = 0; theta < 6.283185; theta += inside_epsilon )
	{
		r = core_radius + (inside_radius-core_radius)*CenterFunction(theta) - inside_standoff;
		float x = cx + sin( theta ) * w/2.0*r;
		float y = cy + cos( theta ) * h/2.0*r;
		fprintf( f, "%f,%f ", x, y );
		printf( "B %f / %f\n", theta, r );
		fprintf( part, "(xy %f %f)\n", x*pcbscale-pinx, y*pcbscale-piny );
		
	}
	fprintf( f, "\" style=\"fill:black\" />" );
//	fprintf( part, ") (layer \"F.Cu\") (width 0.01) (fill solid) (tstamp %s))\n", GetGUID() );
//	fprintf( part, "(pad \"%d\" smd circle (at %f %f) (size 1.524 1.524) (layers \"F.Cu\") (tstamp %s))\n", sections+1, cx*pcbscale, cy*pcbscale, GetGUID() );
     fprintf( part, ") (width %f) (fill yes))) (tstamp %s))\n", edge_width, GetGUID() );

	fprintf( f, "</svg>" );
	fprintf( part, ")\n" );
	
	fclose( f );
	return 0;
}
