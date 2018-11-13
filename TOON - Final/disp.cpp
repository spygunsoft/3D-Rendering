/*   CS580 HW1 display functions to be completed   */

#include   "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"
#include "rend.h"


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- pass back pointer 
 */
	*framebuffer= (char*)calloc(height*width*3, sizeof(char));
	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay** display, int xRes, int yRes)
{
	*display= (GzDisplay*)calloc(1, sizeof(GzDisplay));
	(*display)->xres = xRes;
	(*display)->yres = yRes;
	(*display)->fbuf = (GzPixel*)calloc(xRes*yRes, sizeof(GzPixel));
	(*display)->Shadowfbuf = (float*)calloc(xRes*yRes, sizeof(float));
	
/* HW1.2 create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/
	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* HW1.3 clean up, free memory */
	free(display->fbuf);
	free(display->Shadowfbuf);
	free(display);
	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
	*xRes = (int)display->xres;
	*yRes = (int)display->yres;
/* HW1.4 pass back values for a display */
	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display,bool isInitShadow)
{
	GzPixel	*fbuf=	display->fbuf;
	display->TrinagleCount = 0;
	float	*Shadowfbuf = display->Shadowfbuf;
	for (int i = 0; i < display->xres*display->yres; i++) { //set background color
		fbuf[i].alpha = 50;
		fbuf[i].blue = 50;
		fbuf[i].green = 50;
		fbuf[i].red = 50;
		fbuf[i].z = MAX_DEPTH;
		if (isInitShadow)
		{
			Shadowfbuf[i] = MAX_DEPTH;
		}		
	}
/* HW1.5 set everything to some default values - start a new frame */
	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	GzPixel	*fbuf = display->fbuf;
	if (j < 0 || i < 0||j>=display->yres||i>=display->xres) return GZ_SUCCESS; //To ignore unvalid value
	int index = ARRAY(i, j);// j*display->xres + i;
	if (r > 4095) {
		r = 4095;
	}
	r = r >> 4;
	if (g > 4095) {
		g = 4095;
	}
	g = g >> 4;
	if (b > 4095) {
		b = 4095;
	}
	b = b >> 4;
	fbuf[index].alpha = a;
	fbuf[index].blue = b;
	fbuf[index].red = r;
	fbuf[index].green = g;
	fbuf[index].z = z;
/* HW1.6 write pixel values into the display */
	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	GzPixel	*fbuf = display->fbuf;
	int index = ARRAY(i, j);
	*r = fbuf[index].red;
	*g = fbuf[index].green;
	*b = fbuf[index].blue;
	*a = fbuf[index].alpha;
	*z = fbuf[index].z;

/* HW1.7 pass back a pixel value to the display */
	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{
	fprintf(outfile, "P6 ");
	fprintf(outfile, "%d ", display->xres);
	fprintf(outfile, "%d ", display->yres);
	fprintf(outfile, "255\r");
	GzPixel	*fbuf = display->fbuf;
	for (int i = 0; i < display->yres; i++) {
		for (int j = 0; j < display->xres; j++) {
			int index = ARRAY(j, i);
			fprintf(outfile,"%c", fbuf[index].red);
			fprintf(outfile, "%c", fbuf[index].green);
			fprintf(outfile, "%c", fbuf[index].blue);
		}
	}
/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\r" */
	return GZ_SUCCESS;
}
	
int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{
	GzPixel	*fbuf = display->fbuf;
	for (int i = 0; i < display->xres;i++) {
		for (int j = 0; j < display->yres; j++) {
			int index = ARRAY(i, j);
		
			GzIntensity bColor = fbuf[index].blue;
			GzIntensity gColor = fbuf[index].green;
			GzIntensity rColor = fbuf[index].red;
			//fbuf->red

			framebuffer[index*3] = bColor;
			framebuffer[index * 3 + 1] = gColor;
			framebuffer[index * 3 + 2] = rColor;
		}
		
	}
/* HW1.9 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	return GZ_SUCCESS;
}