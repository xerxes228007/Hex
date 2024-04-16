#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include "field.h"
#include <QWheelEvent>

#define SCALE_FACTOR 1.1

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    generateMap();
    MainWindow::repaint();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();

    if (delta > 0 && mas < 3)
    {
        mas *= SCALE_FACTOR;
        scale = SCALE_FACTOR;
        scaleCoordinates.push_back(QVector2D(mapFromGlobal(QCursor::pos()).x(), mapFromGlobal(QCursor::pos()).y()));
        fieldsToPolygons();
        otherToPolygons();
        QWidget::repaint();

    }
    else if (delta < 0 && mas > 1)
    {
        mas /= SCALE_FACTOR;
        scale = 1/SCALE_FACTOR;
        fieldsToPolygons();
        scaleCoordinates.pop_back();
        otherToPolygons();

        QWidget::repaint();
    }


}

void MainWindow::mousePressEvent(QMouseEvent *event){

    wasClickedOnTheMap = false;
    for(int i = 0; i<map.size(); ++i){
        for(int j = 0; j<map[i].size(); ++j){
            map[i][j].setColor(Qt::lightGray);
            if(mapPolygons[i][j].containsPoint(QPointF(mapFromGlobal(QCursor::pos()).x(), mapFromGlobal(QCursor::pos()).y()),Qt::FillRule::OddEvenFill)){
                map[i][j].setColor(Qt::green);
                wasClickedOnTheMap = true;
                if(map[i][j].isHereUnit()&&firstClick == NO_CLICK){
                    firstClick.setX(i);
                    firstClick.setY(j);
                    refreshAvailiableFields(QVector2D(i, j));

                }
                else if((!map[i][j].isHereUnit())
                    &&(firstClick != NO_CLICK)
                    &&availiableFields.contains(QVector2D(i,j)))
                {
                    moveUnit(firstClick.x(), firstClick.y(), i, j);
                    firstClick = NO_CLICK;
                    map[i][j].setColor(Qt::lightGray);
                    availiableFields.clear();
                }
                else if(firstClick != NO_CLICK)
                {
                    firstClick=NO_CLICK;
                    availiableFields.clear();
                    map[i][j].setColor(Qt::lightGray);
                }
            }
        }
    }

    if(!wasClickedOnTheMap)
    {
        firstClick = NO_CLICK;
        availiableFields.clear();
    }

    for (const auto i:availiableFields){
        map[i.x()][i.y()].setColor(Qt::black);
    }

    QMainWindow::repaint();
}

QPolygonF MainWindow::scalePolygon(QPolygonF polygon){

    if(!scaleCoordinates.isEmpty())
    {
        for(int i = 0; i<polygon.size(); ++i){
            polygon[i].setX(polygon[i].x()-scaleCoordinates.last().x());
            polygon[i].setY(polygon[i].y()-scaleCoordinates.last().y());
        }

        for(int i = 0; i<polygon.size(); ++i){
            polygon[i].setX(polygon[i].x()*scale);
            polygon[i].setY(polygon[i].y()*scale);
        }
        for(int i = 0; i<polygon.size(); ++i){
            polygon[i].setX(polygon[i].x()+scaleCoordinates.last().x());
            polygon[i].setY(polygon[i].y()+scaleCoordinates.last().y());
        }
    }
    return polygon;
}

QPolygonF MainWindow::scaleOtherPolygon(QPolygonF polygon){
    for(int j = 0; j<scaleCoordinates.size(); ++j)
    {
        for(int i = 0; i<polygon.size(); ++i){
            polygon[i].setX(polygon[i].x()-scaleCoordinates[j].x());
            polygon[i].setY(polygon[i].y()-scaleCoordinates[j].y());
        }

        for(int i = 0; i<polygon.size(); ++i){
            polygon[i].setX(polygon[i].x()*SCALE_FACTOR);
            polygon[i].setY(polygon[i].y()*SCALE_FACTOR);
        }
        for(int i = 0; i<polygon.size(); ++i){
            polygon[i].setX(polygon[i].x()+scaleCoordinates[j].x());
            polygon[i].setY(polygon[i].y()+scaleCoordinates[j].y());
        }
    }

    return polygon;
}


void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this);

    QBrush brush;
    QPen pen;

    pen.setWidth(2);
    for(int i = 0; i<map.size(); ++i){
        for(int j = 0 ; j<map[i].size(); ++j){

            brush.setTextureImage(map[i][j].getTexture());
            pen.setColor(map[i][j].getColor());
            painter.setPen(pen);
            QTransform textureTransform;
            textureTransform.scale(mas, mas);
            brush.setTransform(textureTransform);
            painter.setBrush(brush);
            painter.drawPolygon(mapPolygons[i][j]);

        }
    }

    for(int i = 0; i<otherPolygons.size(); ++i){
        painter.drawPolygon(otherPolygons[i]);
    }


}

void MainWindow::fieldsToPolygons(){
    for(int i = 0; i<mapPolygons.size(); ++i){
        for(int j = 0; j<mapPolygons[i].size(); ++j){
            mapPolygons[i][j] = scalePolygon(mapPolygons[i][j]);
        }
    }
};

void MainWindow::otherToPolygons(){
    otherPolygons.clear();
    for(int i = 0; i<map.size(); ++i){
        for(int j = 0; j<map[i].size(); ++j){
            if(map[i][j].isHereStructere()){
                // ODO: otherPolygons.push_back(scalePolygon(map[i][j].getStructure().getPolygon()));
            }
            if(map[i][j].isHereUnit()){
                otherPolygons.push_back(scaleOtherPolygon(map[i][j].getUnit().getPolygon()));
            }
        }
    }
}

void MainWindow::generateMap(){
    for(int i = 0; i < 20; i++){
        int offset = (i%2==0 ? 20 : 0);
        QVector<Field> row;
        for(int j = 0; j < 40; ++j){
            row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::DESERT));
        }
        map.push_back(row);
    }
    map[10][20].addUnit(Unit::Type::INFANTRY);

    for(int i = 0; i<map.size(); ++i){
        QVector<QPolygonF> row;
        for(int j = 0; j<map[i].size(); ++j){
            row.push_back(map[i][j].getHexagon());
        }
        mapPolygons.push_back(row);
    }

    otherToPolygons();
}

void MainWindow::refreshAvailiableFields(QVector2D field){
    availiableFields.clear();
    if(static_cast<int>(field.x()) % 2==0){
        availiableFields.push_back(QVector2D(field.x()-1, field.y()));
        availiableFields.push_back(QVector2D(field.x()-1, field.y()+1));
        availiableFields.push_back(QVector2D(field.x(), field.y()-1));
        availiableFields.push_back(QVector2D(field.x(), field.y()+1));
        availiableFields.push_back(QVector2D(field.x()+1, field.y()));
        availiableFields.push_back(QVector2D(field.x()+1, field.y()+1));
    }
    else{
        availiableFields.push_back(QVector2D(field.x()-1, field.y()-1));
        availiableFields.push_back(QVector2D(field.x()-1, field.y()));
        availiableFields.push_back(QVector2D(field.x(), field.y()-1));
        availiableFields.push_back(QVector2D(field.x(), field.y()+1));
        availiableFields.push_back(QVector2D(field.x()+1, field.y()-1));
        availiableFields.push_back(QVector2D(field.x()+1, field.y()));
    }
}

void MainWindow::moveUnit(int i1, int j1, int i2,int j2){
    map[i2][j2].addUnit(map[i1][j1].getUnit().getType());
    map[i1][j1].removeUnit();
    otherToPolygons();
}

