#include "iteminfo.h"
#include "ui_iteminfo.h"

ItemInfo::ItemInfo(Connection *connection, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::iteminfo)
{
    ui->setupUi(this);

    /* Seteamos la IP, las otras labels de info son de muestra */
    if(connection)
    {
        ui->label_4->setText(connection->peerAddress().toString());
    }

    ui->gvSpeed->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
    ui->gvSpeed->setFocusPolicy(Qt::NoFocus);
    ui->gvSpeed->setStyleSheet("border: 1px solid grey");

    /* Añadimos líneas aleatorias al GraphicsView como muestra */
    QGraphicsScene* scene=new QGraphicsScene();
    scene->setSceneRect(ui->gvSpeed->rect());

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    for(int i=0;i<3;i++)
    {
        QColor color;

        switch(i)
        {
            case 0:
                color=Qt::darkGreen;
            break;
            case 1:
                color=Qt::darkRed;
            break;
            case 2:
                color=Qt::darkBlue;
            break;
        }

        QPen pen;
        pen.setColor(color);
        pen.setWidth(1.2);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        int lastX=0,lastY=0;

        for(int i=0;i<20;i++)
        {
            int newHeight=qrand()%38;
            scene->addLine(lastX,lastY,lastX+5,newHeight,pen);
            lastX+=5;
            lastY=newHeight;
        }
    }

    QGraphicsTextItem* textItem = new QGraphicsTextItem();
    textItem->setPos(10,0);
    textItem->setHtml("<font size=\"10\"><b>DEMO</b></font>");
    scene->addItem(textItem);

    ui->gvSpeed->setScene(scene);
    ui->gvSpeed->fitInView(scene->sceneRect());
}

ItemInfo::~ItemInfo()
{
    delete ui;
}
