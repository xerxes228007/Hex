#include "gamewindow.h"
#include "./ui_gamewindow.h"
#include <QPainter>
#include "field.h"
#include <QWheelEvent>
#include "texture.h"
#include <QPixmap>
#include <QPainterPath>

#define SCALE_FACTOR 1.1
#define SIZE_MAP_X 60
#define SIZE_MAP_Y 30

GameWindow::GameWindow(Client* cli, QVector<QVector2D> cord, QVector<QVector<Biome::BiomeName>> BMap, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
    , client(cli)
{
    for (int i = 0; i < BMap.size(); ++i)
    {
        int offset = (i%2==0 ? 20 : 0);
        QVector<Field> row;
        for (int j = 0; j < BMap[i].size(); ++j) {
            row.push_back(Field(offset+30+j*40, 30 + i*35, 20, BMap[i][j]));
        }
        map.push_back(row);
    }

    for(int i = 0; i<map.size(); ++i){
        QVector<QPolygonF> row;
        QVector<QPolygonF> urow;
        QVector<QString> nrow;
        for(int j = 0; j<map[i].size(); ++j){
            row.push_back(map[i][j].getHexagon());
            if(map[i][j].isHereUnit()){
                urow.push_back(map[i][j].getUnit().getPolygon());
                nrow.push_back(map[i][j].getUnit().getName());
            }
            else
            {
                urow.push_back(QPolygonF());
                nrow.push_back("");
            }

        }
        mapPolygons.push_back(row);
        unitPolygons.push_back(urow);
        namesOfOther.push_back(nrow);
    }
    qDebug()<<client->getID();
    for (int i = 0; i < cord.size(); i+=7) {
        map[cord[i].x()][cord[i].y()].addStructure(Structure::Type::CASTLE);
        map[cord[i].x()][cord[i].y()].setDefaultColor(PLAYER_COLORS[i/7]);
        map[cord[i].x()][cord[i].y()].setColor(PLAYER_COLORS[i/7]);
        if(i/7 == client->getID())
        {
            map[cord[i].x()][cord[i].y()].setVisible();
            refreshAvailiableFields(QVector2D(cord[i].x(), cord[i].y()));
            foreach (auto f, availiableFields) {
                map[f.x()][f.y()].setVisible();
            }
            availiableFields.clear();
        }
        map[cord[i+1].x()][cord[i+1].y()].addStructure(Structure::Type::GOLD_MINE);
        map[cord[i+2].x()][cord[i+2].y()].addStructure(Structure::Type::GOLD_MINE);
        map[cord[i+3].x()][cord[i+3].y()].addStructure(Structure::Type::GOLD_MINE);
        map[cord[i+4].x()][cord[i+4].y()].addStructure(Structure::Type::SHOOTING_RANGE);
        map[cord[i+5].x()][cord[i+5].y()].addStructure(Structure::Type::SHOOTING_RANGE);
        map[cord[i+6].x()][cord[i+6].y()].addStructure(Structure::Type::HORSE_STABLE);
    }

    socket = client->getSocket();
    connect(socket, &QTcpSocket::readyRead, this, &GameWindow::ReadSlot);
    ui->setupUi(this);
    if(client->getID()==0)
    {
        isMyTurn = true;
        ui->step_label->setText("Ваш ход");
    }
    else
    {
        isMyTurn = false;
        ui->step_label->setText("Ход соперника...");
        turnButtons(false);
    }

    GameWindow::repaint();
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::wheelEvent(QWheelEvent *event)
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

void GameWindow::mouseMoveEvent(QMouseEvent* event){
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
            if(mapPolygons.last().last()[0].x()<1720 && dx>0) dx = 0;
            if(mapPolygons[0][0][0].y()>100 && dy<0) dy = 0;
            if(mapPolygons.last().last()[0].y()<980 && dy>0) dy = 0;
            moveMap(dx, dy);
            lastMoveCordinate.setX(mapFromGlobal(event->pos()).x());
            lastMoveCordinate.setY(mapFromGlobal(event->pos()).y());
        }
    }
}

void GameWindow::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() & Qt::MiddleButton)
    {
        lastMoveCordinate=NO_CLICK;
        isMovingMap = false;
    }
}

void GameWindow::mousePressEvent(QMouseEvent *event){

    if(event->button() &Qt::LeftButton){
        if(!isMyTurn) return;
        wasClickedOnTheMap = false;
        for(int i = 0; i<map.size(); ++i){
            for(int j = 0; j<map[i].size(); ++j){
                map[i][j].setColor(map[i][j].getDefColor());
                if(mapPolygons[i][j].containsPoint(QPointF(mapFromGlobal(QCursor::pos()).x(), mapFromGlobal(QCursor::pos()).y()),Qt::FillRule::OddEvenFill)){
                    map[i][j].setColor(Qt::green);
                    wasClickedOnTheMap = true;
                    if(map[i][j].isHereUnit()&&firstClick == NO_CLICK && map[i][j].getUnit().getID() == client->getID()&&!wasMoved){
                        firstClick.setX(i);
                        firstClick.setY(j);
                        refreshAvailiableFields(QVector2D(i, j));

                    }
                    else if((firstClick != NO_CLICK)
                            &&((!map[i][j].isHereUnit())
                            || (map[i][j].getUnit().getType() == map[firstClick.x()][firstClick.y()].getUnit().getType()
                            && map[i][j].getUnit().getID() == map[firstClick.x()][firstClick.y()].getUnit().getID()))
                            &&availiableFields.contains(QVector2D(i,j))
                            &&map[i][j].getBiome() != Biome::BiomeName::RIVER)
                    {
                        moveUnit(firstClick.x(), firstClick.y(), i, j);
                        wasMoved = true;
                        firstClick = NO_CLICK;
                        map[i][j].setColor(map[i][j].getDefColor());
                        availiableFields.clear();
                        refreshAvailiableFields(QVector2D(i, j));
                        foreach (auto f, availiableFields) {
                            map[f.x()][f.y()].setVisible();
                        }
                        availiableFields.clear();

                    }
                    else if(firstClick != NO_CLICK
                               && map[i][j].isHereUnit()
                               && map[i][j].getUnit().getID()!=map[firstClick.x()][firstClick.y()].getUnit().getID()
                               && availiableFields.contains(QVector2D(i,j)))
                    {
                        battleUnit(firstClick.x(), firstClick.y(), i, j);
                        wasMoved = true;
                        firstClick = NO_CLICK;
                        availiableFields.clear();
                    }
                    else if(firstClick != NO_CLICK)
                    {
                        firstClick=NO_CLICK;
                        availiableFields.clear();
                        map[i][j].setColor(map[i][j].getDefColor());
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



QPolygonF GameWindow::scalePolygon(QPolygonF polygon){

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

void GameWindow::moveMap(int dx, int dy){

    for(int i = 0; i<mapPolygons.size(); ++i){

        for(int j = 0; j<mapPolygons[i].size(); ++j){
            for(int k = 0; k < mapPolygons[i][j].size(); k++){
                mapPolygons[i][j][k].setX(mapPolygons[i][j][k].x()-dx);
                mapPolygons[i][j][k].setY(mapPolygons[i][j][k].y()-dy);
            }
        }
    }

    for(int i = 0; i<unitPolygons.size(); ++i){
        for(int j = 0; j < unitPolygons[i].size(); ++j){
            for (int k = 0; k < unitPolygons[i][j].size(); ++k) {
                unitPolygons[i][j][k].setX(unitPolygons[i][j][k].x()-dx);
                unitPolygons[i][j][k].setY(unitPolygons[i][j][k].y()-dy);
            }
        }
    }

    for (int i = 0; i < scaleCoordinates.size(); ++i) {
        scaleCoordinates[i].setX(scaleCoordinates[i].x()-dx);
        scaleCoordinates[i].setY(scaleCoordinates[i].y()-dy);
    }

    QWidget::repaint();
}


void GameWindow::paintEvent(QPaintEvent *event){


    QPainter painter(this);

    QBrush brush;
    QPen pen;

    pen.setWidth(4);

    if(isLose||isWin)
    {
        painter.setFont(QFont("Times", 96, QFont::Bold));
        if(isLose)painter.drawText(400, 400, "ПОРАЖЕНИЕ");
        if(isWin)painter.drawText(400, 400, "ПОБЕДА");
        ui->groupBox->setVisible(false);
        Data.clear();
        Data.push_back(Client::SenderCodes::LOSE);
        Data.push_back(client->getID());
        return;
    }

    for(int i = 0; i<map.size(); ++i){
        for(int j = 0 ; j<map[i].size(); ++j){



            QPixmap pixmap;
            if(map[i][j].isVisible())
            {
                pixmap = QPixmap::fromImage(*Texture::getTexture(map[i][j].getName().append(map[i][j].getStructure().genName())));
                pen.setColor(map[i][j].getColor());
            }
            else
            {
                pixmap = QPixmap::fromImage(*Texture::getTexture("smoke"));
                pen.setColor(Qt::black);
            }
            painter.setPen(pen);
            QPainterPath path;
            path.addPolygon(mapPolygons[i][j]);
            painter.setClipPath(path);
            painter.drawPixmap(mapPolygons[i][j].boundingRect().toRect() , pixmap);
            painter.drawPolygon(mapPolygons[i][j]);

            if(map[i][j].isVisible()&&map[i][j].isHereUnit())
            {

                QPixmap pixmap = QPixmap::fromImage(*Texture::getTexture(namesOfOther[i][j]));

                QPainterPath path;
                path.addPolygon(unitPolygons[i][j]);
                painter.setClipPath(path);
                painter.drawPixmap(unitPolygons[i][j].boundingRect().toRect() , pixmap);
                painter.drawPolygon(unitPolygons[i][j]);
            }

        }
    }

    QPainter text(this);
    text.setFont(QFont("Times", 14, QFont::Bold));

    for(int i = 0; i<map.size(); ++i){
        for(int j = 0 ; j<map[i].size(); ++j){
            if(map[i][j].isVisible()&&map[i][j].isHereUnit())
            {
                text.drawText(unitPolygons[i][j][0], QString::number(map[i][j].getUnitCount()));
            }
        }
    }
    text.setBrush(QBrush(Qt::lightGray));
    text.drawRect(1789, 0, 141, 800);
}

void GameWindow::fieldsToPolygons(){
    for(int i = 0; i<mapPolygons.size(); ++i){
        for(int j = 0; j<mapPolygons[i].size(); ++j){
            mapPolygons[i][j] = scalePolygon(mapPolygons[i][j]);
        }
    }
};

QPolygonF GameWindow::getUnitInMap(int i, int j){
    QPolygonF polygon = map[i][j].getUnit().getPolygon();
    for(int j = 0; j< scaleCoordinates.size(); ++j){
        for(int k = 0; k<polygon.size(); ++k){
            polygon[k].setX(polygon[k].x()*SCALE_FACTOR);
            polygon[k].setY(polygon[k].y()*SCALE_FACTOR);
        }
    }

    for(int k = 1; k<polygon.size(); ++k)
    {
        polygon[k].setX(mapPolygons[i][j][0].x() + polygon[k].x()  - polygon[0].x());
        polygon[k].setY(mapPolygons[i][j][0].y() + polygon[k].y()  - polygon[0].y());
    }
    polygon[0].setX(mapPolygons[i][j][0].x());
    polygon[0].setY(mapPolygons[i][j][0].y());

    return polygon;
}

void GameWindow::otherToPolygons(){

    for(int i = 0; i<unitPolygons.size(); ++i){
        for (int j = 0; j < unitPolygons[i].size(); ++j) {
            unitPolygons[i][j] = scalePolygon(unitPolygons[i][j]);
        }
    }
}

void GameWindow::refreshUnits(){
    unitPolygons.clear();
    namesOfOther.clear();
    for(int i = 0; i <map.size(); ++i){
        QVector<QPolygonF> row;
        QVector<QString> nrow;
        for(int j = 0; j<map[i].size(); ++j){

            if(map[i][j].isHereUnit()){
                row.push_back(getUnitInMap(i, j));
                nrow.push_back(map[i][j].getUnit().getName());
            }
            else
            {
                row.push_back(QPolygonF());
                nrow.push_back("");
            }
        }
        unitPolygons.push_back(row);
        namesOfOther.push_back(nrow);
    }
}

void GameWindow::refreshAvailiableFields(QVector2D field){
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

void GameWindow::moveUnit(int i1, int j1, int i2,int j2){
    Data.clear();
    Data.push_back(Client::SenderCodes::REQUEST_WALK);
    Data.push_back(client->getID());
    Data.push_back(i1);
    Data.push_back(j1);
    Data.push_back(i2);
    Data.push_back(j2);
    SendSlot();
}

void GameWindow::battleUnit(int i1, int j1, int i2,int j2)
{
    Data.clear();
    Data.push_back(Client::SenderCodes::BATTLE_REQUEST);
    Data.push_back(i1);
    Data.push_back(j1);
    Data.push_back(i2);
    Data.push_back(j2);
    Data.push_back(map[i1][j1].getUnitCount());
    Data.push_back(map[i2][j2].getUnitCount());
    SendSlot();
}

void GameWindow::ReadSlot()
{
    switch (Data.at(0)) {
    case Client::ServerCodes::WALK:
    {

        int id = Data.at(1);
        int i1 = Data.at(2);
        int j1 = Data.at(3);
        int i2 = Data.at(4);
        int j2 = Data.at(5);


        map[i2][j2].addUnit(map[i1][j1].getUnit().getType(), id);
        map[i2][j2].setDefaultColor(PLAYER_COLORS[id]);
        map[i2][j2].setColor(PLAYER_COLORS[id]);
        map[i2][j2].setUnitCount(map[i2][j2].getUnitCount()+map[i1][j1].getUnitCount());
        map[i1][j1].removeUnit();
        map[i1][j1].setUnitCount(0);
        refreshUnits();
        QWidget::repaint();
        break;
    }
    case Client::ServerCodes::NEXT_MOVE:
    {
        if(Data.at(1) == client->getID())
        {
            isMyTurn = true;
            wasMoved = false;
            ui->step_label->setText("Ваш ход");
            turnButtons(true);

            isLose = true;

            for(int i = 0; i<map.size(); ++i)
            {
                for(int j = 0; j<map[i].size(); ++j)
                {
                    if(map[i][j].getDefColor() == PLAYER_COLORS[client->getID()])
                    {
                        isLose = false;
                        myMoney += map[i][j].getStructure().getMoney();
                        if(!map[i][j].isHereStructere()) myMoney++;
                        if(map[i][j].getStructure().getType() == Structure::Type::CASTLE)
                        {
                            availiableUnitsT1 ++;
                            continue;
                        }
                        if(map[i][j].getStructure().getType() == Structure::Type::SHOOTING_RANGE)
                        {
                            availiableUnitsT2 ++;
                            continue;
                        }
                        if(map[i][j].getStructure().getType() == Structure::Type::HORSE_STABLE)
                        {
                            availiableUnitsT3 ++;
                            continue;
                        }
                    }
                }
            }
            updateLabels();
        }
        else
        {
            isMyTurn = false;
            turnButtons(false);
            ui->step_label->setText("Ход соперника...");
        }
        break;
    }
    case Client::ServerCodes::PLACE_UNIT_ACCEPT:
    {
        int i = Data.at(1);
        int j = Data.at(2);
        uint8_t type = Data.at(3);
        uint8_t id = Data.at(4);
        int count = Data.at(5);

        map[i][j].addUnit(Unit::Type(type), id);
        map[i][j].setUnitCount(count);
        refreshUnits();
        QWidget::repaint();
        break;
    }
    case Client::ServerCodes::WIN:
    {
        isWin=true;
        QWidget::repaint();
    }
    case Client::ServerCodes::BATTLE_RESULT:
    {
        int i1 = Data.at(1);
        int j1 = Data.at(2);
        int i2 = Data.at(3);
        int j2 = Data.at(4);
        int count = Data.at(5);
        bool b = Data.at(6);
        if(b)
        {

            map[i1][j1].removeUnit();
            map[i1][j1].setUnitCount(0);
            map[i2][j2].setUnitCount(count);
        }
        else
        {
            map[i2][j2].addUnit(map[i1][j1].getUnit().getType(), map[i1][j1].getUnit().getID());
            map[i2][j2].setUnitCount(count);
            map[i1][j1].removeUnit();
            map[i1][j1].setUnitCount(0);
            map[i2][j2].setDefaultColor(PLAYER_COLORS[map[i2][j2].getUnit().getID()]);
            map[i2][j2].setColor(PLAYER_COLORS[map[i2][j2].getUnit().getID()]);
            if(map[i2][j2].getUnit().getID() == client->getID())
            {
                refreshAvailiableFields(QVector2D(i2, j2));
                foreach (auto f, availiableFields) {
                    map[f.x()][f.y()].setVisible();
                }
                availiableFields.clear();
            }
        }
        refreshUnits();
        QWidget::repaint();
        break;
    }
    default:
        break;
    }
}

void GameWindow::SendSlot()
{
    socket->write(Data);
}

void GameWindow::on_endTurn_pushButtin_clicked()
{
    isWin = true;
    for(int i = 0; i<map.size(); ++i)
    {
        for(int j = 0; j<map[i].size(); ++j)
        {
            foreach (auto c, PLAYER_COLORS) {
                if(c!=PLAYER_COLORS[client->getID()]&&c==map[i][j].getDefColor()) isWin = false;
            }
        }
    }
    QWidget::repaint();
    isMyTurn = false;
    Data.clear();
    Data.push_back(Client::SenderCodes::END_WALK);
    Data.push_back(client->getID());
    SendSlot();
}

void GameWindow::turnButtons(bool status)
{
    ui->endTurn_pushButtin->setEnabled(status);
    ui->pushButton_2->setEnabled(status);
    ui->pushButton_3->setEnabled(status);
    ui->pushButton_4->setEnabled(status);
}

void GameWindow::on_pushButton_2_clicked()
{
    for(int i = 0; i<map.size(); ++i)
    {
        for(int j = 0; j<map[i].size(); ++j)
        {
            if(map[i][j].getStructure().getType() == Structure::Type::HORSE_STABLE
                && map[i][j].getDefColor() == PLAYER_COLORS[client->getID()]
                && myMoney>=15
                && availiableUnitsT3>=1
                && (map[i][j].getUnit().getType() == Unit::Type::CAVALRY
                    ||!map[i][j].isHereUnit()))
            {
                Data.clear();
                Data.push_back(Client::SenderCodes::PLACE_UNIT);
                Data.push_back(i);
                Data.push_back(j);
                Data.push_back(Unit::Type::CAVALRY);
                Data.push_back(client->getID());
                Data.push_back(map[i][j].getUnitCount()+1);
                SendSlot();

                myMoney-=15;
                availiableUnitsT3--;
                updateLabels();

            }
        }
    }
}


void GameWindow::on_pushButton_3_clicked()
{
    for(int i = 0; i<map.size(); ++i)
    {
        for(int j = 0; j<map[i].size(); ++j)
        {
            if(map[i][j].getStructure().getType() == Structure::Type::SHOOTING_RANGE
                && map[i][j].getDefColor() == PLAYER_COLORS[client->getID()]
                && myMoney>=10
                && availiableUnitsT2>=1
                && (map[i][j].getUnit().getType() == Unit::Type::ARCHERS
                    ||!map[i][j].isHereUnit()))
            {
                Data.clear();
                Data.push_back(Client::SenderCodes::PLACE_UNIT);
                Data.push_back(i);
                Data.push_back(j);
                Data.push_back(Unit::Type::ARCHERS);
                Data.push_back(client->getID());
                Data.push_back(map[i][j].getUnitCount()+1);
                SendSlot();

                myMoney-=10;
                availiableUnitsT2--;
                updateLabels();

            }
        }
    }
}


void GameWindow::on_pushButton_4_clicked()
{
    for(int i = 0; i<map.size(); ++i)
    {
        for(int j = 0; j<map[i].size(); ++j)
        {
            if(map[i][j].getStructure().getType() == Structure::Type::CASTLE
                && map[i][j].getDefColor() == PLAYER_COLORS[client->getID()]
                && myMoney>=5
                && availiableUnitsT1>=1
                && (map[i][j].getUnit().getType() == Unit::Type::INFANTRY
                ||!map[i][j].isHereUnit()))
            {
                Data.clear();
                Data.push_back(Client::SenderCodes::PLACE_UNIT);
                Data.push_back(i);
                Data.push_back(j);
                Data.push_back(Unit::Type::INFANTRY);
                Data.push_back(client->getID());
                Data.push_back(map[i][j].getUnitCount()+1);
                SendSlot();

                myMoney-=5;
                availiableUnitsT1--;
                updateLabels();

            }
        }
    }

}

void GameWindow::updateLabels()
{
    ui->myGoldLabel->setText(QString::number(myMoney));
    ui->UnitT1Label->setText(QString::number(availiableUnitsT1));
    ui->UnitT2Label->setText(QString::number(availiableUnitsT2));
    ui->UnitT3Label->setText(QString::number(availiableUnitsT3));
}

