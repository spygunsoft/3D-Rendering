/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include "TriangleVertex.h"
#ifndef GZ_WORLD2SCREEN
#define GZ_WORLD2SCREEN

int ShadowTestValue = 50000;
GzMatrix gz_World2Screen;
GzMatrix gz_Screen2World;

GzMatrix gz_ShadowWorld2Screen; //for shadow map
GzMatrix gz_ShadowScreen2World;

GzMatrix	gz_ShadowModel2Screen =
{
	1.0,	0.0,	0.0,	0.0,
	0.0,	1.0,	0.0,	0.0,
	0.0,	0.0,	1.0,	0.0,
	0.0,	0.0,	0.0,	1.0
};
GzMatrix	gz_Model2WorldNormal =
{
	1.0,	0.0,	0.0,	0.0,
	0.0,	1.0,	0.0,	0.0,
	0.0,	0.0,	1.0,	0.0,
	0.0,	0.0,	0.0,	1.0
};
#endif
GzMatrix * GzGetPeakMatrix(GzRender *render);
double getA(GzMatrix arcs, int n)
{
	if (n == 1)
	{
		return arcs[0][0];
	}
	double ans = 0;
	GzMatrix temp;
	int i, j, k;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n - 1; j++)
		{
			for (k = 0; k < n - 1; k++)
			{
				temp[j][k] = arcs[j + 1][(k >= i) ? k + 1 : k];

			}
		}
		double t = getA(temp, n - 1);
		if (i % 2 == 0)
		{
			ans += arcs[0][i] * t;
		}
		else
		{
			ans -= arcs[0][i] * t;
		}
	}
	return ans;
}

void  getAStart(GzMatrix arcs, int n, GzMatrix ans)
{
	if (n == 1)
	{
		ans[0][0] = 1;
		return;
	}
	int i, j, k, t;
	GzMatrix temp;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			for (k = 0; k < n - 1; k++)
			{
				for (t = 0; t < n - 1; t++)
				{
					temp[k][t] = arcs[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
				}
			}


			ans[j][i] = getA(temp, n - 1);
			if ((i + j) % 2 == 1)
			{
				ans[j][i] = -ans[j][i];
			}
		}
	}
}
bool GetMatrixInverse(GzMatrix src, int n, GzMatrix des)
{
	double flag = getA(src, n);
	GzMatrix t;
	if (flag == 0)
	{
		return false;
	}
	else
	{
		getAStart(src, n, t);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				des[i][j] = t[i][j] / flag;
			}

		}
	}
	return true;
}



void UnitayAndNoTranMatrix(GzMatrix matrix) {
	//NoTranslate
	matrix[0][3] = 0;
	matrix[1][3] = 0;
	matrix[2][3] = 0;
	float k = sqrt(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			matrix[i][j] = matrix[i][j] / k;
		}
	}
}
void inverseCoord(GzCoord &coord) {
	coord[0] = -coord[0];
	coord[1] = -coord[1];
	coord[2] = -coord[2];
}
void NormalizeCoord(GzCoord &coord) {
	float sum = sqrt(coord[0] * coord[0] + coord[1] * coord[1] + coord[2] * coord[2]);
	coord[0] = coord[0] /sum;
	coord[1] = coord[1] / sum;
	coord[2] = coord[2] / sum;
}
float vectorDistance(GzCoord a, GzCoord b) {
	float result;
	result =(a[0] - b[0])*(a[0] - b[0]) + (a[1] - b[1])*(a[1] - b[1]) + (a[2] - b[2])*(a[2] - b[2]);
	result = sqrt(result);
	return result;
}
float DotProduct(GzCoord a, GzCoord b) {
	float result;
	result = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	return result;
}
void ClearMatrix(GzMatrix &matrix) {
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matrix[i][j] = 0;
		}
	}
}
int FloatToInt(float value) {
	return int(value + 0.5);
}
short	ctoi(float color)		/* convert float color to GzIntensity short */
{
	return(short)((int)(color * ((1 << 12) - 1)));
}
void VectorMulMatrixLeft(float *vector,GzMatrix matrix) {
	float x = vector[0] * matrix[0][0] + vector[1] * matrix[0][1] + vector[2] * matrix[0][2] + matrix[0][3];
	float y = vector[0] * matrix[1][0] + vector[1] * matrix[1][1] + vector[2] * matrix[1][2] + matrix[1][3];
	float z = vector[0] * matrix[2][0] + vector[1] * matrix[2][1] + vector[2] * matrix[2][2] + matrix[2][3];
	float w= vector[0] * matrix[3][0] + vector[1] * matrix[3][1] + vector[2] * matrix[3][2] + matrix[3][3];
	vector[0] = x / w;
	vector[1] = y / w;
	vector[2] = z / w;
}
void VectorMulMatrixRight(float *vector, GzMatrix matrix) {
	float x = vector[0] * matrix[0][0] + vector[1] * matrix[1][0] + vector[2] * matrix[2][0] + matrix[3][0];
	float y = vector[0] * matrix[0][1] + vector[1] * matrix[1][1] + vector[2] * matrix[2][1] + matrix[3][1];
	float z = vector[0] * matrix[0][2] + vector[1] * matrix[1][2] + vector[2] * matrix[2][2] + matrix[3][2];
	float w = vector[0] * matrix[0][3] + vector[1] * matrix[1][3] + vector[2] * matrix[2][3] + matrix[3][3];
	vector[0] = x / w;
	vector[1] = y / w;
	vector[2] = z / w;
}
void transfromUV(GzRender *render, TriangleVertex *vertex) {

	float tanNumbe = render->camera.FOV / 2;
	tanNumbe = tanNumbe*3.1415926 / 180;
	float d = 1 / tanNumbe;

	float VzP =  vertex->Vertex[2] / (MAX_DEPTH -  vertex->Vertex[2]);

	vertex->uv[0] = vertex->uv[0] / (VzP + 1);
	vertex->uv[1] = vertex->uv[1] / (VzP + 1);
}
void SortVertex(GzCoord	*vertexList, GzCoord *normalList, GzTextureIndex *uvList, TriangleVertex *maxVertext, TriangleVertex *midleVertex, TriangleVertex *minVertex) {  //get the Max middle min Y
	int maxY = vertexList[0][1], maxIndex = 0;

	for (int i = 0; i < 3; i++)
	{
		if (vertexList[i][1] > maxY) {
			maxIndex = i;
			maxY = vertexList[i][1];
		}
	}

	maxVertext->Vertex = vertexList[maxIndex];
	maxVertext->Normal= normalList[maxIndex];
	maxVertext->uv[0] = uvList[maxIndex][0];
	maxVertext->uv[1] = uvList[maxIndex][1];
	if (vertexList[(maxIndex + 1) % 3][1] > vertexList[(maxIndex + 2) % 3][1]) {
		midleVertex->Vertex = vertexList[(maxIndex + 1) % 3];
		minVertex->Vertex = vertexList[(maxIndex + 2) % 3];

		midleVertex->Normal = normalList[(maxIndex + 1) % 3];
		minVertex->Normal = normalList[(maxIndex + 2) % 3];

		midleVertex->uv[0] = uvList[(maxIndex + 1) % 3][0];
		minVertex->uv[0] = uvList[(maxIndex + 2) % 3][0];
		midleVertex->uv[1] = uvList[(maxIndex + 1) % 3][1];
		minVertex->uv[1] = uvList[(maxIndex + 2) % 3][1];
	}
	else
	{
		minVertex->Vertex = vertexList[(maxIndex + 1) % 3];
		midleVertex->Vertex = vertexList[(maxIndex + 2) % 3];

		minVertex->Normal = normalList[(maxIndex + 1) % 3];
		midleVertex->Normal = normalList[(maxIndex + 2) % 3];

		minVertex->uv[0] = uvList[(maxIndex + 1) % 3][0];
		midleVertex->uv[0] = uvList[(maxIndex + 2) % 3][0];
		minVertex->uv[1] = uvList[(maxIndex + 1) % 3][1];
		midleVertex->uv[1] = uvList[(maxIndex + 2) % 3][1];
	}
}

float getInterpolateZ(int x, int y, float* maxVertext, float* midleVertex, float* minVertex) { //Z actually can be R G B and some other stuff
	float *vectorA, *vectorB;

	vectorA = (float *)malloc(sizeof(float) * 3);
	vectorB = (float *)malloc(sizeof(float) * 3);

	vectorA[0] = maxVertext[0] - midleVertex[0];
	vectorA[1] = maxVertext[1] - midleVertex[1];
	vectorA[2] = maxVertext[2] - midleVertex[2];

	vectorB[0] = maxVertext[0] - minVertex[0];
	vectorB[1] = maxVertext[1] - minVertex[1];
	vectorB[2] = maxVertext[2] - minVertex[2];

	float A = vectorA[1] * vectorB[2] - vectorA[2] * vectorB[1];
	float B = vectorA[2] * vectorB[0] - vectorA[0] * vectorB[2];
	float C = vectorA[0] * vectorB[1] - vectorA[1] * vectorB[0];
	float D = -A*maxVertext[0] - B*maxVertext[1] - C*maxVertext[2];

	float newZ = (D*(-1) - A*x - B*y) / C;
	free(vectorA);
	free(vectorB);
	return newZ;
}
bool CheckZBuff(GzDepth *zValue, int x, int y, float* maxVertext, float* midleVertex, float* minVertex) { //test zbuffer
	
	float newZ = getInterpolateZ(x,y,maxVertext,midleVertex,minVertex);// (D*(-1) - A*x - B*y) / C;
	if ((int)newZ < *zValue) {
		*zValue = newZ;
	}
	else
	{
		return false;
	}
	return true;
}
void getTextureColor(GzRender	*render,float x,float y,TriangleVertex maxVertext, TriangleVertex midleVertex, TriangleVertex minVertex,GzColor &textureColor) {
	float *interA, *interB, *interC;
	interA = (float *)malloc(sizeof(float) * 3);
	interB = (float *)malloc(sizeof(float) * 3);
	interC = (float *)malloc(sizeof(float) * 3);

	for (int i=0;i<3;i++)
	{
		interA[i] = maxVertext.Vertex[i];
		interB[i] = midleVertex.Vertex[i];
		interC[i] = minVertex.Vertex[i];
	}
	
	float vertexZ = getInterpolateZ(x, y, interA, interB, interC);
	float Vz = vertexZ / (MAX_DEPTH - vertexZ);

	//interpolate U V non perspective
	interA[2] = maxVertext.uv[0];
	interB[2] = midleVertex.uv[0];
	interC[2] = minVertex.uv[0];
	float interU = getInterpolateZ(x, y, interA, interB, interC);
	interU = interU*(Vz + 1);

	interA[2] = maxVertext.uv[1];
	interB[2] = midleVertex.uv[1];
	interC[2] = minVertex.uv[1];
	float interY = getInterpolateZ(x, y, interA, interB, interC);
	interY = interY*(Vz + 1);

	render->tex_fun(interU, interY, textureColor);
}
#pragma region lighting code
void calculateDiffuseColor(GzRender	*render,GzCoord normal,GzColor &color,float *Kd) {
	GzCoord eyePoint;
	eyePoint[0] = 0;
	eyePoint[1] = 0;
	eyePoint[2] = -1;
	for (int i=0;i<3;i++)
	{
		color[i] = 0;
		for (int j = 0; j < 1; j++)
		{
			GzLight light = render->lights[j];
			NormalizeCoord(light.direction);
			float NL = DotProduct(normal, light.direction);
			float EN = DotProduct(eyePoint, normal);
			if (EN < 0 && NL>0 || EN > 0 && NL < 0)
			{
				continue;
			}
			if (NL < 0&&EN<0) {
				NL = -NL;
			}	
			color[i]+= NL*light.color[i];
			if (color[i]<0)
			{
				int shit = 0;
					shit++;
			}
		}
		color[i] *= Kd[i];
	}
}
void calculateAmbientColor(GzRender*render, GzColor &color,float *ka) {
	color[0] = render->ambientlight.color[0] * ka[0];
	color[1] = render->ambientlight.color[1] * ka[1];
	color[2] = render->ambientlight.color[2] * ka[2];
}
void calculateSpecularColor(GzRender*render, GzCoord normal, GzColor &color,float *Ks) {
	GzCoord eyePoint;
	eyePoint[0] = -0.5;
	eyePoint[1] = 0;
	eyePoint[2] = -0.5;
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	for (int i = 0; i < 3; i++)
	{
		color[i] = 0;
		for (int j = 0; j < 1; j++)
		{
			GzLight light = render->lights[j];
			NormalizeCoord(light.direction);
			float NL = DotProduct(normal, light.direction);		
			if (DotProduct(eyePoint, normal) < 0&&NL>0) {
				return;
			}

			NL = NL < 0 ? -NL : NL;
			GzCoord reflect;
			reflect[0] = 2 * NL*normal[0] - light.direction[0];
			reflect[1] = 2 * NL*normal[1] - light.direction[1];
			reflect[2] = 2 * NL*normal[2] - light.direction[2];
			NormalizeCoord(reflect);
			color[i] += pow(DotProduct(reflect, eyePoint), 1)*light.color[i];
		}
		color[i] *= Ks[i];
	}
}
void setDiffuseSpecularColor(GzRender	*render, TriangleVertex &vertex) {
	float *Kd = render->Kd;
	float *Ks = render->Ks;
	GzCoord normalCoord;
	normalCoord[0] = vertex.Normal[0];
	normalCoord[1] = vertex.Normal[1];
	normalCoord[2] = vertex.Normal[2];
	NormalizeCoord(normalCoord);
	GzColor diffuseColor,specuColor,ambientColor,textureColor;

	//float Vz = vertex.Vertex[2] / (MAX_DEPTH - vertex.Vertex[2]);
	//float interU = vertex.uv[0]*(Vz + 1);
	//float interV = vertex.uv[1] * (Vz + 1);
	//render->tex_fun(interU, interV, textureColor);
	GzColor defaultColor;
	defaultColor[0] = 1;
	defaultColor[1] = 1;
	defaultColor[2] = 1;
	calculateDiffuseColor(render, normalCoord, diffuseColor, defaultColor);
	calculateSpecularColor(render, normalCoord, specuColor, defaultColor);
	calculateAmbientColor(render, ambientColor, defaultColor);
	for (int i=0;i<3;i++)
	{
		vertex.diffuseColor[i] = diffuseColor[i];
		vertex.specularColor[i] = specuColor[i];
		vertex.ambientColor[i] = ambientColor[i];
	}
}
void PhongShading(GzRender *render, int x, int y, TriangleVertex maxVertext, TriangleVertex midleVertex, TriangleVertex minVertex, GzColor &returnColor) {
	float *interA, *interB, *interC;
	GzCoord newNormal;


	//eyePoint[0] = 0;
	//eyePoint[1] = 0;
	//eyePoint[2] =-1;

	interA = (float *)malloc(sizeof(float) * 3);
	interB = (float *)malloc(sizeof(float) * 3);
	interC = (float *)malloc(sizeof(float) * 3);

	//interpolate normalX
	interA[0] = maxVertext.Vertex[0];
	interA[1] = maxVertext.Vertex[1];
	interA[2] = maxVertext.Normal[0];

	interB[0] = midleVertex.Vertex[0];
	interB[1] = midleVertex.Vertex[1];
	interB[2] = midleVertex.Normal[0];

	interC[0] = minVertex.Vertex[0];
	interC[1] = minVertex.Vertex[1];
	interC[2] = minVertex.Normal[0];

	float normalX = getInterpolateZ(x, y, interA, interB, interC);

	interA[2] = maxVertext.Normal[1];
	interB[2] = midleVertex.Normal[1];
	interC[2] = minVertex.Normal[1];
	float normalY = getInterpolateZ(x, y, interA, interB, interC);

	interA[2] = maxVertext.Normal[2];
	interB[2] = midleVertex.Normal[2];
	interC[2] = minVertex.Normal[2];
	float normalZ = getInterpolateZ(x, y, interA, interB, interC);

	newNormal[0] = normalX;
	newNormal[1] = normalY;
	newNormal[2] = normalZ;

	GzColor ambientColor, diffuseColor, specuColor;
	GzColor textureColor;
		
	calculateAmbientColor(render, ambientColor, render->Ka);
	calculateDiffuseColor(render, newNormal, diffuseColor, render->Kd);
	calculateSpecularColor(render, newNormal, specuColor,render->Ks);



   for (int i = 0; i < 3; i++)
	{
			returnColor[i] =  ambientColor[i] + diffuseColor[i] + specuColor[i];
			returnColor[i] = returnColor[i] > 1 ? 1 : returnColor[i];
			returnColor[i] = returnColor[i] < 0 ? 0 : returnColor[i];
	}


	
	free(interA);
	free(interB);
	free(interC);
}
void GouraudShading(GzRender *render,int x,int y,TriangleVertex maxVertext, TriangleVertex midleVertex, TriangleVertex minVertex,GzColor &returnColor) { //interpolate color
#pragma region interpolate stuff
	float *vectorA, *vectorB, *vectorC;

	vectorA = (float *)malloc(sizeof(float) * 3);
	vectorB = (float *)malloc(sizeof(float) * 3);
	vectorC = (float *)malloc(sizeof(float) * 3);

	//interpolate red
	vectorA[0] = maxVertext.Vertex[0];
	vectorA[1] = maxVertext.Vertex[1];
	vectorA[2] = maxVertext.ambientColor[0]+maxVertext.diffuseColor[0]+maxVertext.specularColor[0];

	vectorB[0] = midleVertex.Vertex[0];
	vectorB[1] = midleVertex.Vertex[1];
	vectorB[2] = midleVertex.ambientColor[0] + midleVertex.diffuseColor[0] + midleVertex.specularColor[0];

	vectorC[0] = minVertex.Vertex[0];
	vectorC[1] = minVertex.Vertex[1];
	vectorC[2] = minVertex.ambientColor[0] + minVertex.diffuseColor[0] + minVertex.specularColor[0];

	float red = getInterpolateZ(x, y, vectorA, vectorB, vectorC);

	//interpolate green
	vectorA[2] = maxVertext.ambientColor[1] + maxVertext.diffuseColor[1] + maxVertext.specularColor[1];
	vectorB[2] = midleVertex.ambientColor[1] + midleVertex.diffuseColor[1] + midleVertex.specularColor[1];;
	vectorC[2] = minVertex.ambientColor[1] + minVertex.diffuseColor[1] + minVertex.specularColor[1];;
	float green = getInterpolateZ(x, y, vectorA, vectorB, vectorC);

	//interpolate blue
	vectorA[2] = maxVertext.ambientColor[2] + maxVertext.diffuseColor[2] + maxVertext.specularColor[2];
	vectorB[2] = midleVertex.ambientColor[2] + midleVertex.diffuseColor[2] + midleVertex.specularColor[2];
	vectorC[2] = minVertex.ambientColor[2] + minVertex.diffuseColor[2] + minVertex.specularColor[2];
	float blue = getInterpolateZ(x, y, vectorA, vectorB, vectorC);

	GzColor textureColor;
	getTextureColor(render, x, y, maxVertext, midleVertex, minVertex, textureColor);
	red *=textureColor[0];
	green*= textureColor[1];
	blue *= textureColor[2];
#pragma endregion	
	//set return value
	red = red > 1 ? 1 : red;
	red = red < 0 ? 0 : red;
	green = green > 1 ? 1 : green;
	green = green < 0 ? 0 : green;
	blue = blue > 1 ? 1 : blue;
	blue = blue < 0 ? 0 : blue;
	returnColor[0] = red;
	returnColor[1] = green;
	returnColor[2] = blue;
}

void ToonShading(GzRender *render, int x, int y, TriangleVertex maxVertext, TriangleVertex midleVertex, TriangleVertex minVertex, GzColor &returnColor) { //interpolate color
	float *interA, *interB, *interC;
	GzCoord newNormal;
	GzCoord eyePoint;
	GzCamera disCamera = render->camera;
	eyePoint[0] = disCamera.lookat[0] - disCamera.position[0];
	eyePoint[1] = disCamera.lookat[1] - disCamera.position[1];
	eyePoint[2] = disCamera.lookat[2] - disCamera.position[2];

	//eyePoint[0] = disCamera.position[0] - disCamera.lookat[0];
	//eyePoint[1] = disCamera.position[1] - disCamera.lookat[1];
	//eyePoint[2] = disCamera.position[2] - disCamera.lookat[2];


	interA = (float *)malloc(sizeof(float) * 3);
	interB = (float *)malloc(sizeof(float) * 3);
	interC = (float *)malloc(sizeof(float) * 3);

	//interpolate normalX
	interA[0] = maxVertext.Vertex[0];
	interA[1] = maxVertext.Vertex[1];
	interA[2] = maxVertext.Normal[0];

	interB[0] = midleVertex.Vertex[0];
	interB[1] = midleVertex.Vertex[1];
	interB[2] = midleVertex.Normal[0];

	interC[0] = minVertex.Vertex[0];
	interC[1] = minVertex.Vertex[1];
	interC[2] = minVertex.Normal[0];

	float normalX = getInterpolateZ(x, y, interA, interB, interC);

	interA[2] = maxVertext.Normal[1];
	interB[2] = midleVertex.Normal[1];
	interC[2] = minVertex.Normal[1];
	float normalY = getInterpolateZ(x, y, interA, interB, interC);

	interA[2] = maxVertext.Normal[2];
	interB[2] = midleVertex.Normal[2];
	interC[2] = minVertex.Normal[2];
	float normalZ = getInterpolateZ(x, y, interA, interB, interC);

	newNormal[0] = normalX;
	newNormal[1] = normalY;
	newNormal[2] = normalZ;

	for (int i = 0; i < 3; i++)
	{
		interA[i] = maxVertext.Vertex[i];
		interB[i] = midleVertex.Vertex[i];
		interC[i] = minVertex.Vertex[i];
	}

	float vertexZ = getInterpolateZ(x, y, interA, interB, interC);
	GzCoord eyePoint2;
	eyePoint2[0] =  disCamera.position[0];
	eyePoint2[1] =  disCamera.position[1];
	eyePoint2[2] =  disCamera.position[2];

	GzColor ambientColor, diffuseColor, specuColor;
	//VectorMulMatrix(eyePoint2, gz_World2Screen);
	eyePoint[0] = -0.245143279;
	eyePoint[1] = 1;
	eyePoint[2] = 1;
	
	//VectorMulMatrix(eyePoint, gz_World2Screen);

	eyePoint2[0] = 7.43676648e-11;
	eyePoint2[1] = 2.62290941e-05;
	eyePoint2[2] = 1;

	NormalizeCoord(eyePoint2);
	NormalizeCoord(eyePoint);
	float edge = DotProduct(eyePoint2, newNormal);
	float edge2 = DotProduct(render->lights[0].direction, newNormal);

	float toondifsplit[5] = { 0, 0.5, 0.7, 0.8,1 };

	//Toon diffuse
	float diff;
	float diffPar = DotProduct(newNormal, render->lights[0].direction);

	diffPar = diffPar*0.5 + 0.5;
	for (int i=1;i<=4;i++)
	{
		if (diffPar < toondifsplit[i])
		{
			//lerp
			if ((toondifsplit[i] - diffPar) < 0.06)
			{
				float length = toondifsplit[i] - toondifsplit[i-1];
				float part = (toondifsplit[i] - diffPar) / length;
				diff = part*toondifsplit[i-1] + (1 - part)*toondifsplit[i];
			}
			else {
				diff = toondifsplit[i-1];
			}
			break;;
		}
	}

	//Toon Specular
	GzLight light = render->lights[0];
	NormalizeCoord(light.direction);
	float NL = DotProduct(newNormal, light.direction);
	if (DotProduct(eyePoint, newNormal) < 0 && NL > 0) {
		specuColor[0] = 0;
		specuColor[1] = 0;
		specuColor[2] = 0;
	}
	NL = NL < 0 ? -NL : NL;
	GzCoord reflect;
	reflect[0] = 2 * NL*newNormal[0] - light.direction[0];
	reflect[1] = 2 * NL*newNormal[1] - light.direction[1];
	reflect[2] = 2 * NL*newNormal[2] - light.direction[2];
	NormalizeCoord(reflect);
	float specPara = pow(DotProduct(reflect, eyePoint), 24);
	if (specPara > 0.75)
	{
		if ((specPara-0.75)<0.05)
		{
			float length = 0.7;
			float part = 0.7-(specPara-0.7) / length;
			specuColor[0] = 1* (1-part);
			specuColor[1] = 1* (1 - part);
			specuColor[2] = 1* (1 - part);
		}
		else {
			specuColor[0] = 1;
			specuColor[1] = 1;
			specuColor[2] = 1;
		}	
	}
	else {
		specuColor[0] = 0;
		specuColor[1] = 0;
		specuColor[2] = 0;
	}

	for (int i=0;i<3;i++)
	{
		diffuseColor[i] = render->lights[0].color[i] * diff;
	}

	returnColor[0] = diffuseColor[0] +specuColor[0];
	returnColor[1] = diffuseColor[1] +specuColor[1];
	returnColor[2] = diffuseColor[2] +specuColor[2];

	if (edge >-0.36&&edge <0.15)
	{
		for (int i = 0; i < 3; i++)
		{
			returnColor[i] = 0;
			returnColor[i] = 0;
			returnColor[i] = 0;
		}
	}
	//shadow test
	float* shadowBuf = render->display->Shadowfbuf;
	float poissonDiskX[4];
	poissonDiskX[0] = -0.5420162;
	poissonDiskX[1] = 0.54558609;
	poissonDiskX[2] = -0.094184101;
	poissonDiskX[3] = 0.34495938;

	float poissonDiskY[4];
	poissonDiskY[0] = -0.39906216;
	poissonDiskY[1] = 0.36890725;
	poissonDiskY[2] = -0.52938870;
	poissonDiskY[3] = 0.29387760;

	GzCoord worldSpace;
	worldSpace[0] = x;
	worldSpace[1] = y;
	worldSpace[2] = getInterpolateZ(x, y, maxVertext.Vertex, midleVertex.Vertex, minVertex.Vertex);

	VectorMulMatrixLeft(worldSpace, gz_Screen2World);

	VectorMulMatrixLeft(worldSpace, gz_ShadowWorld2Screen);

	worldSpace[2] = worldSpace[2] / ShadowTestValue;

	bool isOutOfScreen = false;
	worldSpace[0] = (int)worldSpace[0];
	worldSpace[1] = (int)worldSpace[1];
	if (worldSpace[0] >= render->display->xres || worldSpace[1] >= render->display->yres || worldSpace[0] < 0 || worldSpace[1] < 0) //clamp
		isOutOfScreen = true;
	float bias = 8.5;
	float value = 1;
	GzCoord unitCoord;
	unitCoord[0] = 1;
	unitCoord[1] = 1;
	unitCoord[2] = 1;
	float tanValue = 0.005*tan(acos(DotProduct(newNormal, unitCoord)));
	for (int shadowOffset=0; shadowOffset<4; shadowOffset++)
	{
		int x = worldSpace[0] + poissonDiskX[shadowOffset];
		int y = worldSpace[1] + poissonDiskY[shadowOffset];
		int shadowIndex = (x + (y * render->display->xres));
		if (!isOutOfScreen&&(worldSpace[2] - shadowBuf[shadowIndex]) > bias)
		{
			value -= 0.15;//0.075*(worldSpace[0]/x)+ 0.075*(worldSpace[1] / y);
		}
		
	}
	//do not make shadow for the teapot
	if (!isOutOfScreen&&(diffPar<0.7||render->display->TrinagleCount>=0))
	{	
		returnColor[0] = returnColor[0] * value;
		returnColor[1] = returnColor[1] * value;
		returnColor[2] = returnColor[2] * value;
	}


	returnColor[0] = returnColor[0] > 1 ? 1 : returnColor[0];
	returnColor[0] = returnColor[0] < 0 ? 0 : returnColor[0];
	returnColor[1] = returnColor[1]  > 1 ? 1 : returnColor[1];
	returnColor[1] = returnColor[1]  < 0 ? 0 : returnColor[1];
	returnColor[2] = returnColor[2]  > 1 ? 1 : returnColor[2];
	returnColor[2] = returnColor[2]  < 0 ? 0 : returnColor[2];

	for (int i = 0; i < render->displaycount; i++)
	{
		if (render->multidisplay[i]->style.renderstyle == GZ_MSIMPLETOON)
		{
			if (returnColor[0]!=0)
			{
				int shit = 0;
				shit++;
			}
			GzPutDisplay(render->multidisplay[i], (int)floor(x), (int)floor(y), ctoi(returnColor[0]), ctoi(returnColor[1]), ctoi(returnColor[2]), 4095, (int)floor(vertexZ));
		}
	}

}
#pragma endregion

void PrintTriangle(GzRender	*render, TriangleVertex maxVertext, TriangleVertex midleVertex, TriangleVertex minVertex,bool isShadowMap) {
	float maxK = (maxVertext.Vertex[1] - minVertex.Vertex[1]) / (maxVertext.Vertex[0] - minVertex.Vertex[0]); // y/x=k
	float maxB = maxVertext.Vertex[1] - maxVertext.Vertex[0] * maxK;

	float upperK = (maxVertext.Vertex[1] - midleVertex.Vertex[1]) / (maxVertext.Vertex[0] - midleVertex.Vertex[0]);
	float upperB = maxVertext.Vertex[1] - maxVertext.Vertex[0] * upperK;

	float downK = (midleVertex.Vertex[1] - minVertex.Vertex[1]) / (midleVertex.Vertex[0] - minVertex.Vertex[0]);
	float downB = midleVertex.Vertex[1] - midleVertex.Vertex[0] * downK;

	if (maxVertext.Vertex[2] < 0 || minVertex.Vertex[2] < 0 || midleVertex.Vertex[2] < 0)
		return;
	GzPixel	*fbuf = render->display->fbuf;
	float   *shadowBuf = render->display->Shadowfbuf;
	//calculate diffuse color for three point
	setDiffuseSpecularColor(render, maxVertext);
	setDiffuseSpecularColor(render, midleVertex);
	setDiffuseSpecularColor(render, minVertex);

	for (int y = maxVertext.Vertex[1]; y > minVertex.Vertex[1]; y--) { //scan line
		int nextX = (y - maxB) / maxK;

		int nextX2;
		if (y > midleVertex.Vertex[1])
			nextX2 = (y - upperB) / upperK;
		else
			nextX2 = (y - downB) / downK;

		if (nextX2 < nextX) {  //swap two X
							   //nextX -= 1;
			int temp = nextX2;
			nextX2 = nextX;
			nextX = temp;
		}

		//WTF? Why+1 and <= ,actually I do not get it......but it work out
		for (int x = nextX + 1; x <= nextX2; x++) {
			if (x >= render->display->xres || y >= render->display->yres || x< 0 || y   < 0) //clamp
				continue;
			int index = (x + (y *render->display->xres));
			if (CheckZBuff(&fbuf[index].z, x,y, maxVertext.Vertex, midleVertex.Vertex, minVertex.Vertex)) {
				if (isShadowMap)
				{
					GzCoord worldSpace;
					worldSpace[0] = x;
					worldSpace[1] = y;
					worldSpace[2] =  getInterpolateZ(x, y, maxVertext.Vertex, midleVertex.Vertex, minVertex.Vertex);

					//VectorMulMatrixLeft(worldSpace, gz_ShadowScreen2World);

					float dis=vectorDistance(worldSpace, render->ShadowCamera.position);

					if (shadowBuf[index]>worldSpace[2] / ShadowTestValue)
					{
						shadowBuf[index] = worldSpace[2] / ShadowTestValue;
						fbuf[index].red = worldSpace[2] / ShadowTestValue;
						fbuf[index].green = worldSpace[2] / ShadowTestValue;
						fbuf[index].blue = worldSpace[2] / ShadowTestValue;

					/*	fbuf[index].red = dis* 50;
						fbuf[index].green = dis * 50;
						fbuf[index].blue = dis * 50;*/
					}
				
					//shadowBuf[index]
				}
				else if (render->interp_mode== GZ_NORMALS) //phong shading
				{
					GzColor color;
					
					float z=0;
					PhongShading(render, x, y, maxVertext, midleVertex, minVertex, color);		
					
					fbuf[index].red = ctoi(color[0]);
					fbuf[index].green = ctoi(color[1]);
					fbuf[index].blue = ctoi(color[2]);

				}
				else if (render->interp_mode == GZ_COLOR) { /* Gouraud shading */
					GzColor color;
					GouraudShading(render, x, y, maxVertext, midleVertex, minVertex,color);
			
					fbuf[index].red = ctoi(color[0]);
					fbuf[index].green = ctoi(color[1]);
					fbuf[index].blue = ctoi(color[2]);
								
				}else if (render->interp_mode == GZ_TOON)
				{
					GzColor color;
					ToonShading(render, x, y, maxVertext, midleVertex, minVertex, color);

				   fbuf[index].red = ctoi(color[0]);
				   fbuf[index].green = ctoi(color[1]);
				   fbuf[index].blue = ctoi(color[2]);
				
				}
				else {
					fbuf[index].red = ctoi(render->flatcolor[0]);
					fbuf[index].green = ctoi(render->flatcolor[1]);
					fbuf[index].blue = ctoi(render->flatcolor[2]);	
				}
				//if (!isShadowMap)
				{
					fbuf[index].red = fbuf[index].red >> 4;
					fbuf[index].green = fbuf[index].green >> 4;
					fbuf[index].blue = fbuf[index].blue >> 4;
				}		
			}
		}
	}
}

int GzRotXMat(float degree, GzMatrix mat)
{
	 degree = degree*3.1415 / 180;
	mat[1][1] = cos(degree);
	mat[1][2] =-sin(degree);
	mat[2][1] = sin(degree);
	mat[2][2] = cos(degree);
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value

	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
	degree = degree*3.1415 / 180;
	mat[0][0] = cos(degree);
	mat[0][2] = sin(degree);
	mat[2][0] = -sin(degree);
	mat[2][2] = cos(degree);
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value

	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
	degree = degree*3.1415 / 180;
	mat[0][0] = cos(degree);
	mat[0][1] = -sin(degree);
	mat[1][0] = sin(degree);
	mat[1][1] = cos(degree);
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value

	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	mat[0][3] = translate[0];
	mat[1][3] = translate[1];
	mat[2][3] = translate[2];
	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];
// Create scaling matrix
// Pass back the matrix using mat value

	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzDisplay	*display)
{
	*render = (GzRender*)malloc(sizeof(GzRender));
	(*render)->display = display;
	(*render)->matlevel = 0;
	(*render)->XnormMatlevel = 0;
	GzCamera	camera; //default camera
	camera.lookat[0] = 0;
	camera.lookat[1] = 0;
	camera.lookat[2] = 0;
	camera.position[0] = -10;
	camera.position[1] = 5;
	camera.position[2] = -10;
	camera.worldup[0] = 0;
	camera.worldup[1] = 1;
	camera.worldup[2] = 0;
	camera.FOV = 35;
	(*render)->camera = camera;
	/*
	- malloc a renderer struct
	- span interpolator needs pointer to display for pixel writes
	*/

	return GZ_SUCCESS;

}


int GzFreeRender(GzRender *render)
{
	free(render);
	/*
	-free all renderer resources
	*/

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
	GzPixel	*fbuf = render->display->fbuf;  // get framebuff
	for (int i = 0; i < render->display->xres*render->display->yres; i++) { //set background color
		fbuf[i].alpha = 100;
		fbuf[i].blue = 100;
		fbuf[i].green = 100;
		fbuf[i].red = 100;
		fbuf[i].z = MAX_DEPTH;
	}
/*  
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 

	GzMatrix preCalMatrix;
	ClearMatrix(preCalMatrix);
	preCalMatrix[0][0] = render->display->xres / 2;
	preCalMatrix[0][3] = render->display->xres / 2;
	preCalMatrix[1][1] = -render->display->yres / 2;
	preCalMatrix[1][3] = render->display->yres / 2;
	preCalMatrix[2][2] = MAX_DEPTH;
	preCalMatrix[3][3] = 1;
	GzPushMatrix(render, preCalMatrix,false);//Matrix Xsp
	GzCreateShadowModel2Screen(preCalMatrix);//Matrxi for shadow map
///////////////////////////////////////////////////////////////////////
	ClearMatrix(preCalMatrix);
	preCalMatrix[0][0] = 1;
	preCalMatrix[1][1] = 1;
	float tanNumbe = render->camera.FOV / 2;
	tanNumbe = tanNumbe*3.1415926 / 180;
	preCalMatrix[2][2] =tan(tanNumbe);
	preCalMatrix[3][2] = tan(tanNumbe);
	preCalMatrix[3][3] = 1;
	GzPushMatrix(render, preCalMatrix,false);//Matrix Xpi

	preCalMatrix[0][0] = 1;
	preCalMatrix[1][1] = 1;
     tanNumbe = render->ShadowCamera.FOV / 2;
	tanNumbe = tanNumbe*3.1415926 / 180;
	preCalMatrix[2][2] = tan(tanNumbe);
	preCalMatrix[3][2] = tan(tanNumbe);
	preCalMatrix[3][3] = 1;
	GzCreateShadowModel2Screen(preCalMatrix);
///////////////////////////////////////////////////////////////////
	ClearMatrix(preCalMatrix);
	GzCamera disCamera = render->camera;
	GzCoord vectorZ;

	vectorZ[0] = disCamera.lookat[0] - disCamera.position[0]; //CL
	vectorZ[1] = disCamera.lookat[1] - disCamera.position[1];
	vectorZ[2] = disCamera.lookat[2] - disCamera.position[2];
	NormalizeCoord(vectorZ);

	GzCoord vectorY, vectorY2;
	float absUp = DotProduct(disCamera.worldup, vectorZ);  
	vectorY2[0] = vectorZ[0] * absUp;//UP'
	vectorY2[1] = vectorZ[1] * absUp;
	vectorY2[2] = vectorZ[2] * absUp;

	vectorY[0] = disCamera.worldup[0] - vectorY2[0];
	vectorY[1] = disCamera.worldup[1] - vectorY2[1];
	vectorY[2] = disCamera.worldup[2] - vectorY2[2];
	NormalizeCoord(vectorY);

	GzCoord vectorX;
	vectorX[0] = vectorY[1] * vectorZ[2] - vectorY[2] * vectorZ[1];
	vectorX[1] = vectorY[2] * vectorZ[0] - vectorY[0] * vectorZ[2];
	vectorX[2] = vectorY[0] * vectorZ[1] - vectorY[1] * vectorZ[0];


	float oneW, twoW, threeW;
	oneW = -DotProduct(vectorX, disCamera.position);
	twoW = -DotProduct(vectorY, disCamera.position);
	threeW = -DotProduct(vectorZ, disCamera.position);
	preCalMatrix[0][3] = oneW;
	preCalMatrix[1][3] = twoW;
	preCalMatrix[2][3] = threeW;
	preCalMatrix[3][3] = 1;
	for (int i=0;i<3;i++)
    {
		for (int j = 0; j < 3; j++)
		{
			if(i==0)
			 preCalMatrix[i][j] = vectorX[j];
			else if(i==1)
			 preCalMatrix[i][j] = vectorY[j];
			else
			 preCalMatrix[i][j] = vectorZ[j];
		}
    }
	GzPushMatrix(render, preCalMatrix,true);//Matrix Xiw

////////////////////////////////////////////////////////////////
	ClearMatrix(preCalMatrix);
   disCamera = render->ShadowCamera;

	vectorZ[0] = disCamera.lookat[0] - disCamera.position[0]; //CL
	vectorZ[1] = disCamera.lookat[1] - disCamera.position[1];
	vectorZ[2] = disCamera.lookat[2] - disCamera.position[2];
	NormalizeCoord(vectorZ);

	 absUp = DotProduct(disCamera.worldup, vectorZ);
	vectorY2[0] = vectorZ[0] * absUp;//UP'
	vectorY2[1] = vectorZ[1] * absUp;
	vectorY2[2] = vectorZ[2] * absUp;

	vectorY[0] = disCamera.worldup[0] - vectorY2[0];
	vectorY[1] = disCamera.worldup[1] - vectorY2[1];
	vectorY[2] = disCamera.worldup[2] - vectorY2[2];
	NormalizeCoord(vectorY);

	vectorX[0] = vectorY[1] * vectorZ[2] - vectorY[2] * vectorZ[1];
	vectorX[1] = vectorY[2] * vectorZ[0] - vectorY[0] * vectorZ[2];
	vectorX[2] = vectorY[0] * vectorZ[1] - vectorY[1] * vectorZ[0];

	oneW = -DotProduct(vectorX, disCamera.position);
	twoW = -DotProduct(vectorY, disCamera.position);
	threeW = -DotProduct(vectorZ, disCamera.position);
	preCalMatrix[0][3] = oneW;
	preCalMatrix[1][3] = twoW;
	preCalMatrix[2][3] = threeW;
	preCalMatrix[3][3] = 1;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == 0)
				preCalMatrix[i][j] = vectorX[j];
			else if (i == 1)
				preCalMatrix[i][j] = vectorY[j];
			else
				preCalMatrix[i][j] = vectorZ[j];
		}
	}
	GzCreateShadowModel2Screen(preCalMatrix);//shadow map matrix

	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
		{
		 GzMatrix* ma=GzGetPeakMatrix(render);
		 gz_World2Screen[i][j] = (*ma)[i][j];
		}
	}
	GetMatrixInverse(gz_World2Screen, 4, gz_Screen2World);


	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			gz_ShadowWorld2Screen[i][j] = gz_ShadowModel2Screen[i][j];
		}
	}
	GetMatrixInverse(gz_ShadowWorld2Screen, 4, gz_ShadowScreen2World);
//////////////////////////////////////////////////////////////////
	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
	render->camera = *camera;

/*
- overwrite renderer camera structure with new camera definition
*/
	return GZ_SUCCESS;	
}

int GzPutShadowCamera(GzRender *render, GzCamera *camera)
{
	render->ShadowCamera = *camera;

	/*
	- overwrite renderer camera structure with new camera definition
	*/
	return GZ_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int GzPushMatrix(GzRender *render, GzMatrix	matrix,bool pushNormal)
{
	//stack is empty
	if (render->matlevel==0) {
		for (int i=0;i<4;i++)
		{
			for (int j = 0; j<4; j++)
			{
				render->Ximage[render->matlevel][i][j] = matrix[i][j];
			}
		}	
	}
	else
	{	
		int previousIndex = render->matlevel - 1;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				render->Ximage[render->matlevel][i][j] = 0; //clear
				for (int m = 0; m < 4; m++)
				{
					render->Ximage[render->matlevel][i][j] += render->Ximage[previousIndex][i][m] * matrix[m][j];
				}
				
			}
		}
	}
	render->matlevel++;

	if (pushNormal)
	{
		UnitayAndNoTranMatrix(matrix);
		GzPushNomrMatrix(render, matrix);
	}
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	return GZ_SUCCESS;
}
int GzCreateShadowModel2Screen(GzMatrix matrix) {
	GzMatrix temp;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			temp[i][j] = 0;//clear
			for (int m = 0; m < 4; m++)
			{

					temp[i][j] += gz_ShadowModel2Screen[i][m] * matrix[m][j];		
			}

		}
	}
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
		gz_ShadowModel2Screen[i][j] = temp[i][j];	
		}
	}
	return GZ_SUCCESS;

}
GzMatrix * GzGetPeakMatrix(GzRender *render)
{
	GzMatrix *pM = &render->Ximage[render->matlevel - 1];
	return pM;
}
int GzPopMatrix(GzRender *render)
{
	render->matlevel--;
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	return GZ_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////// Xsm matrix Stack¡ü
#pragma region Xim stuff use for normal
int GzPushNomrMatrix(GzRender *render, GzMatrix	matrix)
{
	//stack is empty
	if (render->XnormMatlevel == 0) {
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				render->Xnorm[render->XnormMatlevel][i][j] = matrix[i][j];
			}
		}
	}
	else
	{
		int previousIndex = render->XnormMatlevel - 1;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				render->Xnorm[render->XnormMatlevel][i][j] = 0; //clear
				for (int m = 0; m < 4; m++)
				{
					render->Xnorm[render->XnormMatlevel][i][j] += render->Xnorm[previousIndex][i][m] * matrix[m][j];
				}

			}
		}
	}
	render->XnormMatlevel++;
	/*
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/
	return GZ_SUCCESS;
}
GzMatrix * GzGetNormalPeakMatrix(GzRender *render)
{
	GzMatrix *pM = &render->Xnorm[render->XnormMatlevel - 1];
	return pM;
}
int GzPopNomalMatrix(GzRender *render)
{
	render->XnormMatlevel--;
	/*
	- pop a matrix off the Ximage stack
	- check for stack underflow
	*/
	return GZ_SUCCESS;
}
#pragma endregion


////////////////////////////////////////////////////////////////////////////////////////////////// Xim matrix Stack
int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
	for (int i = 0; i < numAttributes; i++)
	{
		if (nameList[i] == GZ_RGB_COLOR)
		{
			//a = 
			//(*(*render).display).xres = 1;

			GzColor *tempcolor = (GzColor*)valueList[i];
			(*render).flatcolor[0] = (*tempcolor)[0];
			(*render).flatcolor[1] = (*tempcolor)[1];
			(*render).flatcolor[2] = (*tempcolor)[2];

			//= (GzColor*)valueList[i];
		}
		if (nameList[i] == GZ_DIRECTIONAL_LIGHT)
		{

			(*render).lights[i] = *(GzLight*)valueList[i];
			NormalizeCoord((*render).lights[i].direction);
			
		}
		if (nameList[i] == GZ_AMBIENT_LIGHT)
		{
			(*render).ambientlight = *(GzLight*)valueList[i];
		}
		if (nameList[i] == GZ_DIFFUSE_COEFFICIENT)
		{
			GzColor *tempcolor = (GzColor*)valueList[i];
			(*render).Kd[0] = (*tempcolor)[0];
			(*render).Kd[1] = (*tempcolor)[1];
			(*render).Kd[2] = (*tempcolor)[2];
		}
		if (nameList[i] == GZ_SPECULAR_COEFFICIENT)
		{
			GzColor *tempcolor = (GzColor*)valueList[i];
			(*render).Ks[0] = (*tempcolor)[0];
			(*render).Ks[1] = (*tempcolor)[1];
			(*render).Ks[2] = (*tempcolor)[2];
		}
		if (nameList[i] == GZ_AMBIENT_COEFFICIENT)
		{

			GzColor *tempcolor = (GzColor*)valueList[i];
			(*render).Ka[0] = (*tempcolor)[0];
			(*render).Ka[1] = (*tempcolor)[1];
			(*render).Ka[2] = (*tempcolor)[2];
		}
		if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT)
		{
			(*render).spec = *(float*)valueList[i]; //Should be float

		}
		if (nameList[i] == GZ_INTERPOLATE)
		{
			(*render).interp_mode = *(int*)valueList[i];
		}
		if (nameList[i] == GZ_TEXTURE_MAP)
		{
			(*render).tex_fun = (GzTexture)valueList[i];
		}
		if (nameList[i] == GZ_AASHIFTX)
		{
			(*render).Xoffset = *(float*)valueList[i];
		}
		if (nameList[i] == GZ_AASHIFTY)
		{
			(*render).Yoffset = *(float*)valueList[i];
		}
		if (nameList[i] == 9527)
		{
			(*render).name = (char*)valueList[i];
		}
		if (nameList[i] == GZ_MODE)
		{
			(*render).style = *(int*)valueList[i];
		}
		if (nameList[i] == GZ_NEWDISPLAY)
		{
			if ((*render).displaycount<0)
			{
				(*render).displaycount = 0;
			}
			GzNewDisplay(&(*render).multidisplay[(*render).displaycount], render->display->xres, render->display->yres);
			GzInitDisplay((*render).multidisplay[(*render).displaycount],true);
			(*render).multidisplay[(*render).displaycount]->style = *(GzDisplaystyle*)valueList[i];
			(*render).displaycount++;
		}
	}

	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, GzPointer	*valueList,bool isShadowMap)
/* numParts : how many names and values */
{
	if (render->display->TrinagleCount >= 2)
	{
		render->display->TrinagleCount = -1;
	}

	if (render->display->TrinagleCount >=0)
	{
		render->display->TrinagleCount++;
	}
	


	GzCoord	*vertexList = (GzCoord	*)malloc(3 * 3 * sizeof(float)); //
	GzCoord	*normalList = (GzCoord	*)malloc(3 * 3 * sizeof(float));
	GzTextureIndex *uvList = (GzTextureIndex	*)malloc(2 * 3 * sizeof(float));
	GzCoord	*tempVertex = (GzCoord*)valueList[0];
	GzCoord	*tempNormal = (GzCoord*)valueList[1];
	GzTextureIndex *tempUV = (GzTextureIndex*)valueList[2];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			vertexList[i][j] = tempVertex[i][j];
			normalList[i][j] = tempNormal[i][j];
			if (j != 2)
			{
				uvList[i][j] = tempUV[i][j];
			}
		}

	}
	if (isShadowMap)
	{
		VectorMulMatrixLeft(vertexList[0], gz_ShadowModel2Screen);
		VectorMulMatrixLeft(vertexList[1], gz_ShadowModel2Screen);
		VectorMulMatrixLeft(vertexList[2], gz_ShadowModel2Screen);
	}
	else {
		VectorMulMatrixLeft(vertexList[0], *GzGetPeakMatrix(render));
		VectorMulMatrixLeft(vertexList[1], *GzGetPeakMatrix(render));
		VectorMulMatrixLeft(vertexList[2], *GzGetPeakMatrix(render));
	}
	//vertext to screen space
	

	//normal to image spaces
	VectorMulMatrixLeft(normalList[0], *GzGetNormalPeakMatrix(render));
	VectorMulMatrixLeft(normalList[1], *GzGetNormalPeakMatrix(render));
	VectorMulMatrixLeft(normalList[2], *GzGetNormalPeakMatrix(render));

	TriangleVertex *maxVertex = new TriangleVertex();
	TriangleVertex *midleVertex = new TriangleVertex();
	TriangleVertex *minVertex = new TriangleVertex();

	//sort to do scane line
	SortVertex(vertexList, normalList, uvList, maxVertex, midleVertex, minVertex);  //sort from the top to the bottom

	transfromUV(render, maxVertex);
	transfromUV(render, midleVertex);
	transfromUV(render, minVertex);

	//scane line
	PrintTriangle(render, *maxVertex, *midleVertex, *minVertex, isShadowMap);
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts using matrix on top of stack 
- Clip - just discard any triangle with any vert(s) behind view plane 
       - optional: test for triangles with all three verts off-screen (trivial frustum cull)
- invoke triangle rasterizer  
*/ 
	return GZ_SUCCESS;
}


void GzAfterRendering(GzRender	*render)
/* numParts : how many names and values */
{
		float linesample[20] = { -0.707, -0.707 , 0, -1 , 0.707, -0.707, -1, 0, 0, 0, 1, 0, -0.707, 0.707, 0, 1, 0.707, 0.707 };
		//float scale = 50000000;
		float scale = 50000000;
		GzIntensity* temp = new GzIntensity();
		GzDepth* tempz = new GzDepth();
		for (int ix = 0; ix < render->display->xres; ix++)
			for (int iy = 0; iy < render->display->yres; iy++)
			{
				int max = 0;
				int min = MAXINT;
				for (int k = 0; k < 9; k++)
				{
					if (ix + linesample[k * 2] >= render->display->xres
						|| iy + linesample[k * 2 + 1] >= render->display->yres
						|| ix + linesample[k * 2] < 0 || iy + linesample[k * 2 + 1] < 0) continue;
					GzGetDisplay((*render).display, (int)floor(ix + linesample[k * 2]), (int)floor(iy + linesample[k * 2 + 1]), temp, temp, temp, temp, tempz);
					if (max < *tempz) max = *tempz;
					if (min > *tempz)min = *tempz;
				}
				float linecolor = (max - min) / scale;
				if (linecolor > 1) linecolor = 1;
				if (linecolor < 0) linecolor = 0;

				if (linecolor>0.1)
				{
					linecolor = 0;
					GzPutDisplay(render->display, (int)ix, (int)iy, ctoi(linecolor), ctoi(linecolor), ctoi(linecolor), 4095, *tempz);
				}
			
			}
	
}
/* NOT part of API - just for general assistance */

