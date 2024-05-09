#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include "field.h"
#include <QWheelEvent>
#include "texture.h"
#include <QPixmap>
#include <QPainterPath>
#include <QRandomGenerator>

#define SCALE_FACTOR 1.1
#define SIZE_MAP_X 60
#define SIZE_MAP_Y 30

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
        otherToPolygons();
        scaleCoordinates.pop_back();

        QWidget::repaint();
    }


}

void MainWindow::mouseMoveEvent(QMouseEvent* event){
    if(isMovingMap){
        if(lastMoveCordinate==NO_CLICK){
            lastMoveCordinate.setX(mapFromGlobal(event->pos()).x());
            lastMoveCordinate.setY(mapFromGlobal(event->pos()).y());
        }
        else
        {
            int dx = lastMoveCordinate.x()-mapFromGlobal(event->pos()).x();
            int dy = lastMoveCordinate.y()-mapFromGlobal(event->pos()).y();
            if(mapPolygons[0][0][0].x()>100 && dx<0)dx = 0;
            if(mapPolygons.last().last()[0].x()<1820 && dx>0) dx = 0;
            if(mapPolygons[0][0][0].y()>100 && dy<0) dy = 0;
            if(mapPolygons.last().last()[0].y()<980 && dy>0) dy = 0;
            moveMap(dx, dy);
            lastMoveCordinate.setX(mapFromGlobal(event->pos()).x());
            lastMoveCordinate.setY(mapFromGlobal(event->pos()).y());
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() & Qt::MiddleButton)
    {
        lastMoveCordinate=NO_CLICK;
        isMovingMap = false;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event){

    if(event->button() &Qt::LeftButton){
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
    }
    if(event->button() & Qt::MiddleButton)
    {
        isMovingMap = true;
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

void MainWindow::moveMap(int dx, int dy){

    for(int i = 0; i<mapPolygons.size(); ++i){

        for(int j = 0; j<mapPolygons[i].size(); ++j){
            for(int k = 0; k < mapPolygons[i][j].size(); k++){
                mapPolygons[i][j][k].setX(mapPolygons[i][j][k].x()-dx);
                mapPolygons[i][j][k].setY(mapPolygons[i][j][k].y()-dy);
            }
        }
    }

    for(int i = 0; i<otherPolygons.size(); ++i){
        for(int j = 0; j < otherPolygons[i].size(); j++){
            otherPolygons[i][j].setX(otherPolygons[i][j].x()-dx);
            otherPolygons[i][j].setY(otherPolygons[i][j].y()-dy);
        }
    }

    for (int i = 0; i < scaleCoordinates.size(); ++i) {
        scaleCoordinates[i].setX(scaleCoordinates[i].x()-dx);
        scaleCoordinates[i].setY(scaleCoordinates[i].y()-dy);
    }

    QWidget::repaint();
}


void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this);

    QBrush brush;
    QPen pen;

    pen.setWidth(4);
    for(int i = 0; i<map.size(); ++i){
        for(int j = 0 ; j<map[i].size(); ++j){

            QPixmap pixmap = QPixmap::fromImage(*Texture::getTexture(map[i][j].getName().append(map[i][j].getStructure().genName())));
            pen.setColor(map[i][j].getColor());
            painter.setPen(pen);
            QPainterPath path;
            path.addPolygon(mapPolygons[i][j]);
            painter.setClipPath(path);
            painter.drawPixmap(mapPolygons[i][j].boundingRect().toRect() , pixmap);
            painter.drawPolygon(mapPolygons[i][j]);

        }
    }

    for(int i = 0; i<otherPolygons.size(); ++i){
        QPixmap pixmap = QPixmap::fromImage(*Texture::getTexture(namesOfOther[i]));
        QPainterPath path;
        path.addPolygon(otherPolygons[i]);
        painter.setClipPath(path);
        painter.drawPixmap(otherPolygons[i].boundingRect().toRect() , pixmap);
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

QPolygonF MainWindow::getUnitInMap(int i, int j){
    QPolygonF polygon = map[i][j].getUnit().getPolygon();
    for(int j = 0; j< scaleCoordinates.size(); ++j){
        for(int k = 0; k<polygon.size(); ++k){
            polygon[k].setX(polygon[k].x()-scaleCoordinates[j].x());
            polygon[k].setY(polygon[k].y()-scaleCoordinates.last().y());
        }

        for(int k = 0; k<polygon.size(); ++k){
            polygon[k].setX(polygon[k].x()*SCALE_FACTOR);
            polygon[k].setY(polygon[k].y()*SCALE_FACTOR);
        }
        for(int k = 0; k<polygon.size(); ++k){
            polygon[k].setX(polygon[k].x()+scaleCoordinates.last().x());
            polygon[k].setY(polygon[k].y()+scaleCoordinates.last().y());
        }
    }

    for(int k = 1; i<polygon.size(); ++i){
        polygon[k].setX(polygon[k].x()+ mapPolygons[i][j][0].x() - polygon[0].x());
        polygon[k].setY(polygon[k].y()+ mapPolygons[i][j][0].y() - polygon[0].y());
    }
    polygon[0].setX(mapPolygons[i][j][0].x());
    polygon[0].setY(mapPolygons[i][j][0].y());

    return polygon;
}

void MainWindow::otherToPolygons(){

    for(int i = 0; i<otherPolygons.size(); ++i){
        otherPolygons[i] = scalePolygon(otherPolygons[i]);
    }
}

double MainWindow::lerp(double t, double a, double b)
{
    return a + t * (b - a);
}

double MainWindow::grad(int hash, double x, double y)
{
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double MainWindow::perlin(double x, double y)
{



    int X = static_cast<int>(x) & 255;
    int Y = static_cast<int>(y) & 255;

    double xf = x - static_cast<int>(x);
    double yf = y - static_cast<int>(y);

    double u = xf * xf * xf * (xf * (xf * 6 - 15) + 10);
    double v = yf * yf * yf * (yf * (yf * 6 - 15) + 10);

    int A = permutation[X] + Y;
    int AA = permutation[A];
    int AB = permutation[A + 1];
    int B = permutation[X + 1] + Y;
    int BA = permutation[B];
    int BB = permutation[B + 1];

    return lerp(v, lerp(u, grad(AA, xf, yf),
                        grad(BA, xf - 1, yf)),
                lerp(u, grad(AB, xf, yf - 1),
                     grad(BB, xf - 1, yf - 1)));
}

void MainWindow::generateMap(){

    for(int i =0; i<256; ++i){
        int a = QRandomGenerator::global()->bounded(0, 256);
        while (std::find(permutation.begin(),permutation.end(), a) != permutation.end()) {
            a = QRandomGenerator::global()->bounded(1, 256);
        }
        permutation.push_back(a);
    }

    //std::random_shuffle(permutation.begin(), permutation.end());
    //std::random_device rd;
    //std::mt19937 rng(rd());
    int a = 0;
    for(int i = 0; i < SIZE_MAP_Y; i++){
        int offset = (i%2==0 ? 20 : 0);
        QVector<Field> row;
        for(int j = 0; j < SIZE_MAP_X; ++j){
            int p = static_cast<int>(perlin(j / 10.0, i / 10.0) * 10);
            a++;
            if(p < -3)
            {
                row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::RIVER));
            }
            else if(p<-2)
            {
                row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::DESERT));
            }
            else if(p<1)
            {
                row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::PLAIN));
            }
            else if(p<4)
            {
                row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::FOREST));
            }
            else
            {
                row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::MOUNTAINS));
            }
        }
        map.push_back(row);
    }
    qDebug()<<a;

    permutation.clear();

    map[10][20].addUnit(Unit::Type::INFANTRY);

    for(int i = 0; i<map.size(); ++i){
        QVector<QPolygonF> row;
        for(int j = 0; j<map[i].size(); ++j){
            row.push_back(map[i][j].getHexagon());
            if(map[i][j].isHereStructere()){
                //     // TODO: otherPolygons.push_back(scalePolygon(map[i][j].getStructure().getPolygon()));
            }
            if(map[i][j].isHereUnit()){
                otherPolygons.push_back(map[i][j].getUnit().getPolygon());
                namesOfOther.push_back(map[i][j].getUnit().getName());

            }
        }
        mapPolygons.push_back(row);
    }


}

void MainWindow::refreshUnits(){
    otherPolygons.clear();
    namesOfOther.clear();

    // TODO: for structions
    for(int i = 0; i <map.size(); ++i){

        for(int j = 0; j<map[i].size(); ++j){

            if(map[i][j].isHereStructere()){
                //otherPolygons.push_back(map[i][j].getStructure().getPolygon());
            }
            if(map[i][j].isHereUnit()){

                otherPolygons.push_back(getUnitInMap(i, j));
                namesOfOther.push_back(map[i][j].getUnit().getName());
            }

        }
    }




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

    for(int i = availiableFields.size()-1; i>=0; --i){

        if(availiableFields[i].x()>SIZE_MAP_Y-1 || availiableFields[i].x()<0||availiableFields[i].y()>SIZE_MAP_X-1||availiableFields[i].y()<0) availiableFields.remove(i);

    }
}

void MainWindow::moveUnit(int i1, int j1, int i2,int j2){
    map[i2][j2].addUnit(map[i1][j1].getUnit().getType());
    map[i1][j1].removeUnit();
    refreshUnits();
}

