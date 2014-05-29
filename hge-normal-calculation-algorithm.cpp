#include "hge-normal-calculation-algorithm.hpp"
#define GLM_SWIZZLE
#define GLM_FORCE_SSE2
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
NormalCalculationAlgorithm::NormalCalculationAlgorithm(
		QObject *parent,
		unsigned char *pixels,
		int width,
		int height,
		int pixelBytes,
		int threadIndex,
		int totalThreadsNumber,
		unsigned char *normals) :
	QThread(parent),
	pixels(pixels),
	width(width),
	height(height),
	pixelBytes(pixelBytes),
	threadIndex(threadIndex),
	totalThreadsNumber(totalThreadsNumber),
	modW(width - 1),
	modH(height - 1),
	normals(normals)
{
}
void NormalCalculationAlgorithm::run() Q_DECL_OVERRIDE
{
	for(int h = ((threadIndex * height) / totalThreadsNumber), normalsIndex = h * width * 3,
		endH = (((threadIndex + 1) * height) / totalThreadsNumber); h < endH; h++)
	{
		for(int w = 0; w < width; w++)
		{
			float z = calcLoc(h, w);
			glm::vec3 N ( 0.0f, 1.0f, calcLoc(h - 1, w    ) - z);
			glm::vec3 NE( 1.0f, 1.0f, calcLoc(h - 1, w + 1) - z);
			glm::vec3 E ( 1.0f, 0.0f, calcLoc(h    , w + 1) - z);
			glm::vec3 SE( 1.0f,-1.0f, calcLoc(h + 1, w + 1) - z);
			glm::vec3 S ( 0.0f,-1.0f, calcLoc(h + 1, w    ) - z);
			glm::vec3 SW(-1.0f,-1.0f, calcLoc(h + 1, w - 1) - z);
			glm::vec3 W (-1.0f, 0.0f, calcLoc(h    , w - 1) - z);
			glm::vec3 NW(-1.0f, 1.0f, calcLoc(h - 1, w - 1) - z);
			//NORMALS
			glm::vec3 NxNW = glm::normalize(glm::cross(N , NW));
			glm::vec3 NExN = glm::normalize(glm::cross(NE, N ));
			glm::vec3 ExNE = glm::normalize(glm::cross(E , NE));
			glm::vec3 SExE = glm::normalize(glm::cross(SE, E ));
			glm::vec3 SxSE = glm::normalize(glm::cross(S , SE));
			glm::vec3 SWxS = glm::normalize(glm::cross(SW, S ));
			glm::vec3 WxSW = glm::normalize(glm::cross(W , SW));
			glm::vec3 NWxW = glm::normalize(glm::cross(NW, W ));
			//NORMAL
			glm::vec3 normal = glm::normalize(NxNW + NExN + ExNE + SExE + SxSE + SWxS + WxSW + NWxW);
			//
			normals[normalsIndex++] = (unsigned char)(normal.x * 255.0f);
			normals[normalsIndex++] = (unsigned char)(normal.y * 255.0f);
			normals[normalsIndex++] = (unsigned char)(normal.z * 255.0f);
		}
	}
}
float NormalCalculationAlgorithm::calcLoc(int hh, int ww)
{
	int h = hh;
	int w = ww;
	if(0 > h)
	{
		h += modH;
	}
	if(0 > w)
	{
		w += modW;
	}
	float result = 0.0f;
	for(int i = (((h & modH) * width) + (w & modW)) * pixelBytes, e = 0, shiftBits = 0;
		e < pixelBytes; e++, i++, shiftBits += 8)
	{
		int64_t tmpi = pixels[i];
		result += float(tmpi << shiftBits);
	}
	return result;
}
