#include "grouptreewidget.h"

GroupTreeWidget::GroupTreeWidget(bool expanded,QWidget* parent)
{

    this->expanded=false;

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

    QPushButton* btnExpand=new QPushButton();
    btnExpand->setFlat(true);
    frameLayout->addWidget(btnExpand);
    connect(btnExpand,SIGNAL(clicked()),SLOT(on_btnExpand_clicked()));

    treewidget=new CustomTreeWidget();
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


    QHBoxLayout* nl=new QHBoxLayout();
    nl->setMargin(0);
    nl->setSpacing(4);
    iconLabel=new QLabel();
    iconLabel->setPixmap(QPixmap(":/res/img/bullet_arrow_left.png"));
    iconLabel->setScaledContents(true);
    iconLabel->setFixedSize(24,24);
    nl->addWidget(iconLabel);
    QLabel* groupLabel=new QLabel("Grupo 1");
    QFont font = groupLabel->font();
    font.setPointSize(10);
    font.setBold(true);
    groupLabel->setFont(font);
    nl->addWidget(groupLabel);
    nl->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Expanding));
    QLabel* numItemsLabel=new QLabel("0 equipos");
    nl->addWidget(numItemsLabel);
    nl->addSpacerItem(new QSpacerItem(4,1,QSizePolicy::Fixed,QSizePolicy::Fixed));
    btnExpand->setLayout(nl);

    this->setLayout(mainLayout);
    for(int i=0;i<3;i++)
    {
        QTreeWidgetItem* item=new QTreeWidgetItem();
        item->setIcon(0,QIcon(":/res/img/the-dark-knight-the-joker-02.png"));
        QGraphicsView* gvSpeed=new QGraphicsView();
        gvSpeed->setFixedHeight(50);
        gvSpeed->setFixedWidth(200);
        treewidget->addTopLevelItem(item);
        treewidget->setItemWidget(treewidget->topLevelItem(i),1,gvSpeed);


        QGraphicsScene* scene=new QGraphicsScene();
        scene->addText("To Do");
        gvSpeed->setScene(scene);

        ItemInfo* itemInfo=new ItemInfo();
        QTreeWidgetItem* itInfo=new QTreeWidgetItem();
        treewidget->topLevelItem(i)->addChild(itInfo);
        treewidget->setItemWidget(treewidget->topLevelItem(i)->child(0),0,itemInfo);
    }
}

GroupTreeWidget::~GroupTreeWidget()
{
}

void GroupTreeWidget::setExpanded(bool expanded)
{
    if(expanded)
    {
        emit expandedChanged(this);
        frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        treewidget->show();
        iconLabel->setPixmap(QPixmap(":/res/img/bullet_arrow_down.png"));
        this->expanded=true;
    }
    else
    {
        frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        treewidget->hide();

        iconLabel->setPixmap(QPixmap(":/res/img/bullet_arrow_left.png"));
        this->expanded=false;
    }
}

bool GroupTreeWidget::isExpanded()
{
    return this->expanded;
}

void GroupTreeWidget::on_btnExpand_clicked()
{
    if(!this->isExpanded()) setExpanded(true);
}

void GroupTreeWidget::on_treewidget_itemClicked(QTreeWidgetItem *item, int column)
{
    item->setExpanded(!item->isExpanded());
}
