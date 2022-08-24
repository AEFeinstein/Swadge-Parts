#!/usr/bin/tcc -run

// Resize potions of a .kicad_pcb file.  
// copy-and-paste in graphic sections or graphic line sections.
// I.e.    ./resize.c < infile > outfile

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buff[262144];

int main( int argc, char **argv )
{
	float scale = 0;

	if( argc != 2 || ( scale = atof( argv[1] ) ) == 0 )
	{
		fprintf( stderr, "Command-line stream resize of kicad components and lines.\nUsage: ./resize.c [scale in multiplier, 0.1 ... 10000]\n" );
		return -5;
	}
	int lineno = 1;
	while( !feof( stdin ) )
	{
		char * s = fgets( buff, sizeof( buff ), stdin );
		if( !s ) break;
		if( ( strlen( s ) > 12 ) && strncmp( s, "        (xy ", 12 ) == 0 )
		{
			float x, y;
			if( sscanf( s+12, "%f %f", &x, &y ) != 2 )
			{
				fprintf( stderr, "Error reading xy on line %d\n", lineno );
			}
			else
			{
				printf( "        (xy %f %f)\n", x*scale, y*scale );
			}
		}
		else if( strlen( s ) > 18 && strncmp( s, "  (gr_line (start ", 18 ) == 0 )
		{
			float x1, y1, x2, y2;
			char buf1[1024];
			char buf2[8192];
			memset( buf2, 0, sizeof(buf2) );
			int r = sscanf( s+18, "%f %f %6c %f %f %8191c", &x1, &y1, buf1, &x2, &y2, buf2 );
			if( r != 6 )
			{
				fprintf( stderr, "Error reading xy on line %d (%d) %s\n", lineno, r, buf1 );
			}
			else
			{
				printf( "  (gr_line (start %f %f) (end %f %f%s", x1*scale, y1*scale, x2*scale, y2*scale, buf2 );
			}
		}



		else
		{
			printf( "%s", s );
		}
		lineno++;
	}
	return 0;
}
