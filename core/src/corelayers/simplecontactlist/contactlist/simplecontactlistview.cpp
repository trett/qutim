#include "simplecontactlistview.h"
#include "simplecontactlistitem.h"
#include "abstractcontactmodel.h"
#include <qutim/messagesession.h>
#include <QtGui/QContextMenuEvent>
#include <QHeaderView>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/event.h>
#include <QPainter>
#include <QDebug>
#include <QLabel>
#include <QApplication>
#include <qutim/systemintegration.h>
#include <qutim/servicemanager.h>
#include <QTimer>

namespace Core
{
namespace SimpleContactList
{

TreeView::TreeView(AbstractContactModel *model, QWidget *parent) : QTreeView(parent)
{
	connect(this, SIGNAL(activated(QModelIndex)),
			this, SLOT(onClick(QModelIndex)));

	setAlternatingRowColors(true);
	setRootIsDecorated(false);
	setIndentation(0);
	setEditTriggers(QAbstractItemView::EditKeyPressed);
	setHeaderHidden(true);

#ifndef QUTIM_MOBILE_UI
	setDragEnabled(true);
	setAcceptDrops(true);
	setDropIndicatorShown(true);
#endif

	QTimer::singleShot(0, this, SLOT(initScrolling()));

	connect(this, SIGNAL(collapsed(QModelIndex)), SLOT(onCollapsed(QModelIndex)));
	connect(this, SIGNAL(expanded(QModelIndex)), SLOT(onExpanded(QModelIndex)));

	setModel(model);
}

void TreeView::setModel(AbstractContactModel *model)
{
	storeClosedTags();
	Config group = Config().group("contactList").group(model->metaObject()->className());
	m_closedIndexes = group.value("closedTags", QStringList()).toSet();
	connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
			SLOT(onRowsInserted(QModelIndex,int,int)));
	QTreeView::setModel(model);
	checkTag(QModelIndex(), model);
}

void TreeView::initScrolling()
{
	if(QObject *scroller = ServiceManager::getByName("Scroller"))
		QMetaObject::invokeMethod(scroller, "enableScrolling", Q_ARG(QObject*, viewport()));	
}

void TreeView::onClick(const QModelIndex &index)
{
	ContactItemType type = getItemType(index);
	if (type == ContactType) {
		Buddy *buddy = index.data(BuddyRole).value<Buddy*>();
		if (ChatSession *session = ChatLayer::get(buddy, true))
			session->activate();
	}
}

void TreeView::contextMenuEvent(QContextMenuEvent *event)
{
	QModelIndex index = indexAt(event->pos());
	ContactItemType type = getItemType(index);
	if (type == ContactType) {
		Buddy *buddy = index.data(BuddyRole).value<Buddy*>();
		qDebug("%s", qPrintable(buddy->id()));
		buddy->menu(true)->popup(event->globalPos());
	}
}

void TreeView::startDrag(Qt::DropActions supportedActions)
{
	QModelIndex index = selectedIndexes().value(0);
	if (!index.isValid())
		return;

	QMimeData *data = model()->mimeData(QModelIndexList() << index);
	if (!data)
		return;
	QRect rect;
	QPixmap pixmap;
	QPoint point;
	{
		QAbstractItemDelegate *delegate = itemDelegate(index);
		QStyleOptionViewItemV4 option = viewOptions();
		option.locale = this->locale();
		option.locale.setNumberOptions(QLocale::OmitGroupSeparator);
		option.widget = this;
		option.state |= QStyle::State_Selected;
		option.rect = visualRect(index);
		point = option.rect.topLeft();
		option.rect.moveTo(0, 0);
		option.rect.setSize(delegate->sizeHint(option, index));
		rect = option.rect;
		pixmap = QPixmap(rect.size());
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		delegate->paint(&painter, option, index);
	}
	QDrag *drag = new QDrag(this);
	drag->setPixmap(pixmap);
	drag->setMimeData(data);
	point = QCursor::pos() - viewport()->mapToGlobal(point);
	drag->setHotSpot(point);
	//			drag->setHotSpot(QCursor::pos() - rect.topLeft());
	Qt::DropAction setDefaultDropAction = QAbstractItemView::defaultDropAction();
	Qt::DropAction defaultDropAction = Qt::IgnoreAction;
	if (setDefaultDropAction != Qt::IgnoreAction && (supportedActions & setDefaultDropAction))
		defaultDropAction = setDefaultDropAction;
	else if (supportedActions & Qt::CopyAction && dragDropMode() != QAbstractItemView::InternalMove)
		defaultDropAction = Qt::CopyAction;
	if (drag->exec(supportedActions, defaultDropAction) == Qt::IgnoreAction
			&& getItemType(index) == ContactType) {
		if (QWidget *widget = QApplication::topLevelAt(QCursor::pos())) {
			if (widget->window() == this->window())
				return;
		}
		Event ev("contact-list-drop",
				 QCursor::pos() - point,
				 index.data(BuddyRole));
		ev.send();
	}
	//			qDebug() << "DropAction" << drag->exec(supportedActions, defaultDropAction);
	//			if (drag->exec(supportedActions, defaultDropAction) == Qt::MoveAction)
	//				d->clearOrRemove();
	//			{}
}

void TreeView::onCollapsed(const QModelIndex &index)
{
	QString name = index.data(TagName).toString();
	if (!name.isEmpty()) {
		m_closedIndexes.insert(name);
		storeClosedTags();
	}
}

void TreeView::onExpanded(const QModelIndex &index)
{
	QString name = index.data(TagName).toString();
	if (!name.isEmpty()) {
		m_closedIndexes.remove(name);
		storeClosedTags();
	}
}

void TreeView::onRowsInserted(const QModelIndex &parent, int first, int last)
{
	for (; first <= last; ++first) {
		QModelIndex index = model()->index(first, 0, parent);
		if (!m_closedIndexes.contains(index.data(TagName).toString()))
			setExpanded(index, true);
	}
}

void TreeView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	QTreeView::dataChanged(topLeft,bottomRight);
}

TreeView::~TreeView()
{
}

void TreeView::storeClosedTags()
{
	if (!model())
		return;
	Config group = Config().group("contactList").group(model()->metaObject()->className());
	group.setValue("closedTags", QStringList(m_closedIndexes.toList()));
}

void TreeView::checkTag(const QModelIndex &parent, QAbstractItemModel *model)
{
	for (int i = 0, c = model->rowCount(parent); i != c; ++i) {
		QModelIndex index = model->index(i, 0, parent);
		checkTag(index, model);
		if (!m_closedIndexes.contains(index.data(TagName).toString()))
			setExpanded(index, true);
	}
}

}
}