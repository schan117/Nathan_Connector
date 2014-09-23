#ifndef AUX_VIEWER_H
#define AUX_VIEWER_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class Aux_Viewer;
}

class Aux_Viewer : public QWidget
{
    Q_OBJECT

public:
    explicit Aux_Viewer(QWidget *parent = 0);
    ~Aux_Viewer();

    void Display_Image(QLabel* view, uchar* data, int width, int height);

    Ui::Aux_Viewer *ui;
};

#endif // AUX_VIEWER_H
