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

signals:

    void Front_Aux_Settings_Changed();
    void Back_Aux_Settings_Changed();

private slots:
    void on_Front_Aux0_Minimum_Width_valueChanged(double arg1);
    void on_Front_Aux0_Maximum_Width_valueChanged(double arg1);
    void on_Front_Aux1_Minimum_Width_valueChanged(double arg1);
    void on_Front_Aux1_Maximum_Width_valueChanged(double arg1);
    void on_Front_Aux1_Minimum_Height_valueChanged(double arg1);
    void on_Front_Aux1_Maximum_Height_valueChanged(double arg1);
    void on_Back_Aux0_Minimum_Width_valueChanged(double arg1);
    void on_Back_Aux0_Maximum_Width_valueChanged(double arg1);
    void on_Back_Aux1_Minimum_Width_valueChanged(double arg1);
    void on_Back_Aux1_Maximum_Width_valueChanged(double arg1);
    void on_Back_Aux1_Minimum_Height_valueChanged(double arg1);
    void on_Back_Aux1_Maximum_Height_valueChanged(double arg1);
};

#endif // AUX_VIEWER_H
