#include "grouptreewidget.h"

GroupTreeWidget::GroupTreeWidget(bool expanded,QWidget*)
{

    this->expanded=false;

    /* Creamos el TreeWidget */
    QGridLayout* mainLayout=new QGridLayout();
    QGridLayout* frameLayout=new QGridLayout();
    mainLayout->setMargin(0);
    frameLayout->setMargin(0);
    mainLayout->setSpacing(0);
    frameLayout->setSpacing(0);

    frame=new QFrame();
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setLayout(frameLayout);
    mainLayout->addWidget(frame);

    treewidget=new CustomTreeWidget();
    treewidget->setFocusPolicy(Qt::NoFocus);
    treewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    treewidget->setRootIsDecorated(false);
    treewidget->setHeaderHidden(true);
    treewidget->setColumnCount(2);
    treewidget->setIndentation(0);
    treewidget->setAnimated(true);
    treewidget->setIconSize(QSize(40,40));
    treewidget->header()->setStretchLastSection(false);
    treewidget->header()->setResizeMode(0,QHeaderView::Stretch);
    treewidget->header()->setResizeMode(1,QHeaderView::Fixed);
    treewidget->setColumnWidth(1,200);
    treewidget->setFrameShape(QFrame::NoFrame);
    if(!expanded) treewidget->hide();
    frameLayout->addWidget(treewidget);

    this->setLayout(mainLayout);
}

GroupTreeWidget::~GroupTreeWidget()
{

}

void GroupTreeWidget::addItem(Connection *connection)
{
    qDebug()<<"Añadiendo item, id="+QString::number(connection->getID());

    /* Creamos un nuevo item y seteamos el icono */
    QTreeWidgetItem* item=new QTreeWidgetItem();
    item->setIcon(0,QIcon(":/res/img/blue-iconset/user.png"));

    /* Guardamos un ID único para el item usando el UserRole */
    item->setData(0,Qt::UserRole,QVariant(connection->getID()));

    /* Añadimos el item al TreeView */
    treewidget->addTopLevelItem(item);

    /* Creamos y añadimos al item los labels de información */
    ItemInfo* info=new ItemInfo(connection,this);
    treewidget->setItemWidget(treewidget->topLevelItem(treewidget->topLevelItemCount()-1),0,info);

    /* Creamos el GraphicsView y lo añadimos al item */
    QGraphicsView* gvSpeed=new QGraphicsView();
    gvSpeed->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
    gvSpeed->setFocusPolicy(Qt::NoFocus);
    gvSpeed->setStyleSheet("border: 1px solid grey");
    gvSpeed->setFixedHeight(50);
    gvSpeed->setFixedWidth(200);
    treewidget->setItemWidget(treewidget->topLevelItem(treewidget->topLevelItemCount()-1),1,gvSpeed);


    /* Añadimos líneas aleatorias al GraphicsView como muestra */
    QGraphicsScene* scene=new QGraphicsScene();
    scene->setSceneRect(gvSpeed->rect());

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
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        int lastX=0,lastY=0;

        for(int i=0;i<20;i++)
        {
            int newHeight=qrand()%48;
            scene->addLine(lastX,lastY,lastX+10,newHeight,pen);
            lastX+=10;
            lastY=newHeight;
        }
    }

    QGraphicsTextItem* textItem = new QGraphicsTextItem();
    textItem->setPos(60,0);
    textItem->setHtml("<font size=\"14\"><b>DEMO</b></font>");
    scene->addItem(textItem);

    gvSpeed->setScene(scene);
    gvSpeed->fitInView(scene->sceneRect());
}
