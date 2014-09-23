#include "aux_viewer.h"
#include "ui_aux_viewer.h"

Aux_Viewer::Aux_Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Aux_Viewer)
{
    ui->setupUi(this);
}

Aux_Viewer::~Aux_Viewer()
{
    this->close();
    delete ui;
}

void Aux_Viewer::Display_Image(QLabel *view, uchar *data, int width, int height)
{
    QImage qi(data, width, height, QImage::Format_RGB888);

    QPixmap qp = QPixmap::fromImage(qi).scaled(QSize(view->width(),view->height()), Qt::IgnoreAspectRatio);

    /*
    QPainter qpaint(&qp);
    qpaint.setBrush(Qt::SolidPattern);
    QPen qpen(QColor(255,0,0,100));
    qpen.setWidth(1);
    qpaint.setPen(qpen);

    qpaint.drawLine(0, View->height()/2, View->width(), View->height()/2);
    */



    view->setPixmap(qp);
}

void Aux_Viewer::on_Front_Aux0_Minimum_Width_valueChanged(double arg1)
{
    emit Front_Aux_Settings_Changed();
}

void Aux_Viewer::on_Front_Aux0_Maximum_Width_valueChanged(double arg1)
{
    emit Front_Aux_Settings_Changed();
}

void Aux_Viewer::on_Front_Aux1_Minimum_Width_valueChanged(double arg1)
{
    emit Front_Aux_Settings_Changed();
}

void Aux_Viewer::on_Front_Aux1_Maximum_Width_valueChanged(double arg1)
{
    emit Front_Aux_Settings_Changed();
}

void Aux_Viewer::on_Front_Aux1_Minimum_Height_valueChanged(double arg1)
{
    emit Front_Aux_Settings_Changed();
}

void Aux_Viewer::on_Front_Aux1_Maximum_Height_valueChanged(double arg1)
{
    emit Front_Aux_Settings_Changed();
}

void Aux_Viewer::on_Back_Aux0_Minimum_Width_valueChanged(double arg1)
{

}

void Aux_Viewer::on_Back_Aux0_Maximum_Width_valueChanged(double arg1)
{

}

void Aux_Viewer::on_Back_Aux1_Minimum_Width_valueChanged(double arg1)
{

}

void Aux_Viewer::on_Back_Aux1_Maximum_Width_valueChanged(double arg1)
{

}

void Aux_Viewer::on_Back_Aux1_Minimum_Height_valueChanged(double arg1)
{

}

void Aux_Viewer::on_Back_Aux1_Maximum_Height_valueChanged(double arg1)
{

}
