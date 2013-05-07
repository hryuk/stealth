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
    treewidget->setColumnCount(1);
    treewidget->setIndentation(0);
    treewidget->setAnimated(true);
    treewidget->header()->setStretchLastSection(true);
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

    /* Guardamos un ID único para el item usando el UserRole */
    item->setData(0,Qt::UserRole,QVariant(connection->getID()));

    /* Añadimos el item al TreeView */
    treewidget->addTopLevelItem(item);

    /* Creamos y añadimos al item los labels de información */
    ItemInfo* info=new ItemInfo(connection,this);
    treewidget->setItemWidget(item,0,info);
}
