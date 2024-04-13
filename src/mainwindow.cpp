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
}
