// tcc -run gentouch.c -lm
#include <stdio.h>
#include <math.h>

float lerp( float a, float b, float t )
{
	return t * b + ( 1.0f - t ) * a;
}

int main()
{
	FILE * fv = fopen( "temp.txt", "w" );
	float x1 = -16.9;
	float y1 = -3.6;
	float x2 = 18.2;
	float y2 = 7.6;
	float x3 = 10;
	float y3 = 10.2;
	float x4 = -11.2;
	float y4 = 1.5;

	const float ufmax = .75;
	int toothmax = 6;
	int side = 0;

	// force
	float f1 = 32; // as 2 views 1
	float f2 = -9;
	
	double dx1 = y4-y1;
	double dy1 = (x4-x1);
	double dx2 = y3-y2;
	double dy2 = (x3-x2);
	
	// Tricky, corner 2 and 3 are offset by half a tooth.
	x2 += dx2 * 1.0/toothmax;
	x3 += dx2 * 1.0/toothmax;
	y2 += dy2 * 1.0/toothmax;
	y3 += dy2 * 1.0/toothmax;

	dx1 /= sqrt( dx1*dx1+dy1*dy1 );
	dy1 /= sqrt( dx1*dx1+dy1*dy1 );
	dx2 /= sqrt( dx2*dx2+dy2*dy2 );
	dy2 /= sqrt( dx2*dx2+dy2*dy2 );
	
	printf( "%f %f\n", dx1, dy1 );
	printf( "%f %f\n", dx2, dy2 );


	float f, uf;
	int tooth;
	for( side = 0; side < 2; side++ )
	{
		int gotfirst = 0;
		float fx, fy;
		fprintf( fv, "(fp_poly (pts\n" );
		for( tooth = 0; tooth < toothmax; tooth++ )
		{
			for( uf = 0; uf <= ufmax + .001; uf+= 0.01 )
			{
				if( tooth & 1 )
					f = ufmax - uf;
				else
					f = uf;
				float toothfrof = ((float)tooth/(float)toothmax) + (uf/ufmax)/toothmax + side / (float)toothmax;
				
				float sx1 = lerp( x1, x4, toothfrof );
				float sy1 = lerp( y1, y4, toothfrof );
				float sx2 = lerp( x2, x3, toothfrof );
				float sy2 = lerp( y2, y3, toothfrof );
				float dxt1 = dx1;
				float dyt1 = dy1;
				float dxt2 = dx2;
				float dyt2 = dy2;
				float fb = f1;
				float fa = f2;
				if( side )
				{
					float t;
					t = sx2; sx2 = sx1; sx1 = t;
					t = sy2; sy2 = sy1; sy1 = t;
					t = dxt1; dxt1 = dxt2; dxt2 = t;
					t = dyt1; dyt1 = dyt2; dyt2 = t;
					t = fa; fa = fb; fb = t;
				}

				float dxc1 = sx1 + dxt1 * f * fa;
				float dyc1 = sy1 + dyt1 * f * fa;
				float dxc2 = sx2 + dxt2 * (1.0-f) * fb;
				float dyc2 = sy2 + dyt2 * (1.0-f) * fb;

				float x = lerp( dxc1, dxc2, f );
				float y = lerp( dyc1, dyc2, f );
				fprintf( fv, "\t(xy %f %f)\n", x, y );
				if( !gotfirst )
				{
					fx = x;
					fy = y;
					gotfirst = 1;
				}
			}
		}
		fprintf( fv, "\t(xy %f %f)\n", fx, fy );
		fprintf( fv, ") (layer \"F.Cu\") (width 0.01) (fill solid) (tstamp 3556f104-9c12-4e9f-917e-ad59bf2e4e3f))" );
		fprintf( fv, "\n\n\n\n\n" );
	}
}
/*
  (fp_poly (pts
      (xy 10.760963 -1.844308)
      (xy 10.763866 -1.842771)
      (xy 10.767053 -1.839673)
      (xy 10.770878 -1.834336)
      (xy 10.754954 -1.844932)
      (xy 10.757993 -1.844965)
      (xy 10.760963 -1.844308)
*/
    ) (layer "F.Cu") (width 0.01) (fill solid) (tstamp 3556f104-9c12-4e9f-917e-ad59bf2e4e3a))
	*/
