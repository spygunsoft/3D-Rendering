/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

GzColor	*image=NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

  //read the RGB accroding to U V
  
  /* bounds-test u,v to make sure nothing will overflow image array bounds */
  u = u < 0 ? 0 : u;
  u = u > 1 ? 1 : u;
  v = v < 0 ? 0 : v;
  v = v > 1 ? 1 : v;

  /* determine texture cell corner values and perform bilinear interpolation */
  float scaleU = u*(xs - 1);
  float scaleV = v*(ys - 1);
  float s = scaleU - int(scaleU);
  float t = scaleV - int(scaleV);
  int minX = int(scaleU);
  int minY = int(scaleV);
  int maxX = minX + 1;
  int maxY = minY + 1;

/* set color to interpolated GzColor value and return */
  for (int i = 0; i<3; i++)
  { 
	  color[i] = s*t*image[maxX+maxY*xs][i] + (1 - s)*t*image[minX+maxY*xs][i] + (1 - t)*s*image[maxX+minY*xs][i] + (1 - t)*(1 - s)*image[minX+minY*xs][i];
  }
  return GZ_SUCCESS;



}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	float scaleU = u*(255);
	float scaleV = v*(255);
	int tempX = int(scaleU);
	int tempY = int(scaleV);
	float s = scaleU - int(scaleU);
	float t = scaleV - int(scaleV);

	int indexX = tempX / 5;
	int indexY = tempY / 5;
	float colorValue = indexX / 16 + indexY / 16;

	if ((tempX&16 )==0&&(tempY & 16) == 0)
	{
		color[0] = colorValue*u;
		color[1] = colorValue*v;
		color[2] = colorValue*u*v;
	}
	else {
		color[0] = 0.5;
		color[1] = 0.5;
		color[2] = 0.5;
	}

	//int indexX = tempX / 10;
	//int indexY = tempY / 10;
	//float colorValue = indexX / 16 + indexY / 16;
	//color[0] = colorValue*u;
	//color[1] = colorValue*v;
	//color[2] = colorValue*u*v;

	//if (tempX %2==0&&tempY%2==0)
	//{
	//	color[0] = 0.5*u;
	//	color[1] = 0.5*v;
	//	color[2] = 0.5*u*v;
	//}
	//else if(tempX%2!=0&& tempY % 2 != 0) {
	//	color[0] = 0.5*u*v;
	//	color[1] = 0.5*u;
	//	color[2] = 0.5*v;
	//}
	//else {
	//	color[0] = 0.5*u;
	//	color[1] = 0.5*u*v;
	//	color[2] = 0.5*v;
	//}

	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

