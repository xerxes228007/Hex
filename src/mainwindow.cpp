#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include "field.h"

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

void MainWindow::mousePressEvent(QMouseEvent *event){

    for(int i = 0; i<map.size(); ++i){
        for(int j = 0; j<map[i].size(); ++j){
            if(map[i][j].isInside(mapFromGlobal(QCursor::pos()).x(), mapFromGlobal(QCursor::pos()).y())){
                map[i][j].setColor(Qt::green);
                if(map[i][j].isHereUnit()&&firstCkick.x()==-1 && firstCkick.y()==-1){
                    firstCkick.setX(i);
                    firstCkick.setY(j);

                }
                if(!map[i][j].isHereUnit()&&firstCkick.x()>=0 && firstCkick.y()>=0){
                    moveUnit(firstCkick.x(), firstCkick.y(), i, j);
                    firstCkick.setX(-1);
                    firstCkick.setY(-1);
                    map[i][j].setColor(Qt::black);
                }
            }else{
                map[i][j].setColor(Qt::black);
            }
        }
    }

    QMainWindow::repaint();
}

void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this);

    QBrush brush;
    QPen pen;

    pen.setWidth(2);

    for(int i = 0; i<map.size(); ++i){
        for(int j = 0 ; j<map[i].size(); ++j){

            brush.setTextureImage(map[i][j].getTexture());
            painter.setBrush(brush);
            pen.setColor(map[i][j].getColor());
            painter.setPen(pen);
            painter.drawPolygon(map[i][j].getHexagon());
            if(map[i][j].isHereUnit()){
                painter.drawPolygon(map[i][j].getUnit().getPolygon());
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
}

void MainWindow::moveUnit(int i1, int j1, int i2,int j2){
    map[i2][j2].addUnit(map[i1][j1].getUnit().getType());
    map[i1][j1].removeUnit();
}
