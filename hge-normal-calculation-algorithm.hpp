#ifndef HGENORMALCALCULATIONALGORITHM_HPP
#define HGENORMALCALCULATIONALGORITHM_HPP
#include <QtCore/QThread>
class QMutex;
class NormalCalculationAlgorithm : public QThread
{
	Q_OBJECT
public:
	explicit NormalCalculationAlgorithm(
			QObject *parent,
			unsigned char *pixels,
			int width,
			int height,
			int pixelBytes,
			int threadIndex,
			int totalThreadsNumber,
			unsigned char *normals);
signals:
public slots:
private:
	unsigned char *pixels;
	int width;
	int height;
	int pixelBytes;
	int threadIndex;
	int totalThreadsNumber;
	int modW;
	int modH;
	unsigned char *normals;
	void run() Q_DECL_OVERRIDE;
	float calcLoc(int h, int w);
};
#endif
