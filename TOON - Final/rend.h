#include "disp.h" /* include your own disp.h file (e.g. hw1)*/

/* Camera defaults */
#define	DEFAULT_FOV		35.0
#define	DEFAULT_IM_Z	(-10.0)  /* world coords for image plane origin */
#define	DEFAULT_IM_Y	(5.0)    /* default look-at point = 0,0,0 */
#define	DEFAULT_IM_X	(-10.0)

#define	DEFAULT_AMBIENT	{0.1, 0.1, 0.1}
#define	DEFAULT_DIFFUSE	{0.7, 0.6, 0.5}
#define	DEFAULT_SPECULAR	{0.2, 0.3, 0.4}
#define	DEFAULT_SPEC		32

#define	MATLEVELS	100		/* how many matrix pushes allowed */
#define	MAX_LIGHTS	10		/* how many lights allowed */
#define MAX_DEPTH 2147483646
#define MAX_FLOAT 214748364

#ifndef GZRENDER
#define GZRENDER
typedef struct {			/* define a renderer */
  GzDisplay		*display;
  GzCamera		camera;
  GzCamera		ShadowCamera;
  short		    matlevel;	        /* top of stack - current xform */
  short         XnormMatlevel;      /* top of stack - current Xnorms */
  GzMatrix		Ximage[MATLEVELS];	/* stack of xforms (Xsm) */
  GzMatrix		Xnorm[MATLEVELS];	/* xforms for norms (Xim) */
  GzMatrix		Xsp;		        /* NDC to screen (pers-to-screen) */
  GzColor		flatcolor;          /* color state for flat shaded triangles */
  int			interp_mode;

  int			numlights;
  GzLight		lights[MAX_LIGHTS];
  GzLight		ambientlight;
  GzColor		Ka, Kd, Ks;
  float		    spec;		/* specular power */
  GzTexture		tex_fun;    /* tex_fun(float u, float v, GzColor color) */
  //HW6 stuff
  float	Xoffset;
  float	Yoffset;
  char	*name;

  //HW7
  int		style;
  GzDisplay		*multidisplay[10];
  int displaycount=0;
  int maindisplay;//共享信息给别的display
  int           TriangleCount;
}  GzRender;
#endif

// Function declaration
// HW2
int GzNewRender(GzRender **render, GzDisplay *display);
int GzFreeRender(GzRender *render);
int GzBeginRender(GzRender	*render);
int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList);
int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList,bool isShadowMap);

// HW3
int GzPutShadowCamera(GzRender *render, GzCamera *camera);
int GzPutCamera(GzRender *render, GzCamera *camera);
int GzPushMatrix(GzRender *render, GzMatrix	matrix, bool pushNormal);
int GzCreateShadowModel2Screen(GzMatrix matrix);
int GzPopMatrix(GzRender *render);
//HW4
int GzPushNomrMatrix(GzRender *render, GzMatrix	matrix);
// HW5
int GzFreeTexture();

//HW7
void GzAfterRendering(GzRender	*render);
//void GzCombineRendering(GzRender *render);

// Object Translation
int GzRotXMat(float degree, GzMatrix mat);
int GzRotYMat(float degree, GzMatrix mat);
int GzRotZMat(float degree, GzMatrix mat);
int GzTrxMat(GzCoord translate, GzMatrix mat);
int GzScaleMat(GzCoord scale, GzMatrix mat);

