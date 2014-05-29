#ifndef HGEMAINWINDOW_HPP
#define HGEMAINWINDOW_HPP
#include <QWidget>
namespace Ui
{
	class MainWindow;
}
class QImage;
class MainWindow : public QWidget
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
private slots:
	void onLoadClicked();
	void onOpenClicked();
	void onSaveClicked();
	void onGenerateClicked();
private:
	Ui::MainWindow *ui;
	bool hasNotLoadedImage;
};
#endif
