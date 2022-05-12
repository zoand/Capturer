#include "imageeditmenu.h"
#include <QPushButton>
#include <QPixmap>
#include <QStyle>
#include <QHBoxLayout>
#include <QMoveEvent>

#define CONNECT_BTN_MENU(X, BTN, MENU)      connect(BTN, &IconButton::toggled, [=](bool checked) {      \
                                                if(checked) {                                           \
                                                    emit graphChanged(X);                               \
                                                    MENU->show();                                       \
                                                    MENU->move(pos().x(), pos().y() +                   \
                                                        (height() + 3) * (sub_menu_show_pos_ ? -1 : 1));\
                                                }                                                       \
                                                else {                                                  \
                                                    MENU->hide();                                       \
                                                }                                                       \
                                            });                                                         \
                                            connect(this, &EditMenu::moved, [this]() {                  \
                                                if(MENU->isVisible())                                   \
                                                    MENU->move(pos().x(), pos().y() +                   \
                                                        (height() + 3) * (sub_menu_show_pos_ ? -1 : 1));\
                                            })

ImageEditMenu::ImageEditMenu(QWidget* parent, uint32_t groups)
    : EditMenu(parent)
{
    group_ = new ButtonGroup(this);
    connect(group_, &ButtonGroup::uncheckedAll, [this]() { graphChanged(Graph::NONE); });

    const auto icon_size = QSize(HEIGHT, HEIGHT);

    if (groups & GRAPH_GROUP) {
        auto rectangle_btn = new IconButton(QPixmap(":/icon/res/rectangle"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, true, this);
        rectangle_btn->setCheckable(true);
        rectangle_btn->setToolTip(tr("Rectangle"));
        rectangle_menu_ = new GraphMenu(this);
        connect(rectangle_menu_, &EditMenu::changed, [this]() { emit styleChanged(Graph::RECTANGLE); });
        CONNECT_BTN_MENU(Graph::RECTANGLE, rectangle_btn, rectangle_menu_);
        group_->addButton(rectangle_btn);
        addButton(rectangle_btn);
        buttons_[Graph::RECTANGLE] = rectangle_btn;

        auto circle_btn = new IconButton(QPixmap(":/icon/res/circle"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, true, this);
        circle_btn->setCheckable(true);
        circle_btn->setToolTip(tr("Ellipse"));
        circle_menu_ = new GraphMenu(this);
        connect(circle_menu_, &EditMenu::changed, [this]() { emit styleChanged(Graph::CIRCLE); });
        CONNECT_BTN_MENU(Graph::CIRCLE, circle_btn, circle_menu_);
        group_->addButton(circle_btn);
        addButton(circle_btn);
        buttons_[Graph::CIRCLE] = circle_btn;

        auto arrow_btn = new IconButton(QPixmap(":/icon/res/arrow"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, true, this);
        arrow_btn->setCheckable(true);
        arrow_btn->setToolTip(tr("Arrow"));
        arrow_menu_ = new ArrowEditMenu(this);
        connect(arrow_menu_, &EditMenu::changed, [this]() { emit styleChanged(Graph::ARROW); });
        CONNECT_BTN_MENU(Graph::ARROW, arrow_btn, arrow_menu_);
        group_->addButton(arrow_btn);
        addButton(arrow_btn);
        buttons_[Graph::ARROW] = arrow_btn;

        auto line_btn = new IconButton(QPixmap(":/icon/res/line"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, true, this);
        line_btn->setCheckable(true);
        line_btn->setToolTip(tr("Line"));
        line_menu_ = new LineEditMenu(this);
        connect(line_menu_, &EditMenu::changed, [this]() { emit styleChanged(Graph::LINE); });
        CONNECT_BTN_MENU(Graph::LINE, line_btn, line_menu_);
        group_->addButton(line_btn);
        addButton(line_btn);
        buttons_[Graph::LINE] = line_btn;

        auto pen_btn = new IconButton(QPixmap(":/icon/res/feather"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, true, this);
        pen_btn->setCheckable(true);
        pen_btn->setToolTip(tr("Pencil"));
        curves_menu_ = new LineEditMenu(this);
        connect(curves_menu_, &EditMenu::changed, [this]() { emit styleChanged(Graph::CURVES); });
        CONNECT_BTN_MENU(Graph::CURVES, pen_btn, curves_menu_);
        group_->addButton(pen_btn);
        addButton(pen_btn);
        buttons_[Graph::CURVES] = pen_btn;

        auto text_btn = new IconButton(QPixmap(":/icon/res/text"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, true, this);
        text_btn->setCheckable(true);
        text_btn->setToolTip(tr("Text"));
        text_menu_ = new FontMenu(this);
        connect(text_menu_, &EditMenu::changed, [this]() { emit styleChanged(Graph::TEXT); });
        CONNECT_BTN_MENU(Graph::TEXT, text_btn, text_menu_);
        group_->addButton(text_btn);
        addButton(text_btn);
        buttons_[Graph::TEXT] = text_btn;

        auto mosaic_btn = new IconButton(QPixmap(":/icon/res/mosaic"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, true, this);
        mosaic_btn->setCheckable(true);
        mosaic_btn->setToolTip(tr("Mosaic"));
        mosaic_menu_ = new EraseMenu(this);
        connect(mosaic_menu_, &EditMenu::changed, [this]() { emit styleChanged(Graph::MOSAIC); });
        CONNECT_BTN_MENU(Graph::MOSAIC, mosaic_btn, mosaic_menu_);
        group_->addButton(mosaic_btn);
        addButton(mosaic_btn);
        buttons_[Graph::MOSAIC] = mosaic_btn;

        auto eraser_btn = new IconButton(QPixmap(":/icon/res/eraser"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, true, this);
        eraser_btn->setCheckable(true);
        eraser_btn->setToolTip(tr("Eraser"));
        erase_menu_ = new EraseMenu(this);
        connect(erase_menu_, &EditMenu::changed, [this]() { emit styleChanged(Graph::ERASER); });
        CONNECT_BTN_MENU(Graph::ERASER, eraser_btn, erase_menu_);
        group_->addButton(eraser_btn);
        addButton(eraser_btn);
        buttons_[Graph::ERASER] = eraser_btn;
    }

    /////////////////////////////////////////////////////////////////////////////////
    if (groups & REDO_UNDO_GROUP) {
        addSeparator();

        undo_btn_ = new IconButton(QPixmap(":/icon/res/undo"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, false, this);
        undo_btn_->setDisabled(true);
        connect(undo_btn_, &IconButton::clicked, this, &ImageEditMenu::undo);
        addButton(undo_btn_);

        redo_btn_ = new IconButton(QPixmap(":/icon/res/redo"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, false, this);
        redo_btn_->setDisabled(true);
        connect(redo_btn_, &IconButton::clicked, this, &ImageEditMenu::redo);
        addButton(redo_btn_);
    }

    /////////////////////////////////////////////////////////////////////////////////
    if (groups & SAVE_GROUP) {
        addSeparator();

        auto fix_btn = new IconButton(QPixmap(":/icon/res/pin"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, false, this);
        connect(fix_btn, &IconButton::clicked, [this]() { group_->uncheckAll(); fix(); hide(); });
        addButton(fix_btn);

        auto save_btn = new IconButton(QPixmap(":/icon/res/save"), { HEIGHT, HEIGHT }, { ICON_W, ICON_W }, false, this);
        connect(save_btn, &IconButton::clicked, this, &ImageEditMenu::save);
        connect(save_btn, &IconButton::clicked, [=]() { group_->uncheckAll(); });
        addButton(save_btn);
    }

    /////////////////////////////////////////////////////////////////////////////////
    if (groups & EXIT_GROUP) {
        addSeparator();

        auto close_btn = new QPushButton(QIcon(":/icon/res/wrong"), QString(), this);
        close_btn->setObjectName("close_btn");
        close_btn->setIconSize({ ICON_W, ICON_W });
        connect(close_btn, &QPushButton::clicked, [this]() { group_->uncheckAll(); exit(); hide(); });
        addWidget(close_btn);

        auto ok_btn = new QPushButton(QIcon(":/icon/res/right"), QString(), this);
        ok_btn->setObjectName("ok_btn");
        ok_btn->setIconSize({ ICON_W, ICON_W });
        connect(ok_btn, &QPushButton::clicked, [this]() { group_->uncheckAll(); ok(); hide(); });
        addWidget(ok_btn);
    }
}

QPen ImageEditMenu::pen(Graph graph)
{
    switch (graph) {
    case Graph::RECTANGLE: return rectangle_menu_->pen();
    case Graph::CIRCLE:    return circle_menu_->pen();
    case Graph::LINE:      return line_menu_->pen();
    case Graph::CURVES:    return curves_menu_->pen();
    case Graph::MOSAIC:    return mosaic_menu_->pen();
    case Graph::ARROW:     return arrow_menu_->pen();
    case Graph::TEXT:      return text_menu_->pen();
    case Graph::ERASER:    return erase_menu_->pen();
    default:        break;
    }
    return QPen();
}

void ImageEditMenu::pen(Graph graph, QPen pen)
{
    if (pen.width() < 1) pen.setWidth(1);

    switch (graph) {
    case Graph::RECTANGLE: rectangle_menu_->pen(pen);    break;
    case Graph::CIRCLE:    circle_menu_->pen(pen);       break;
    case Graph::LINE:      line_menu_->pen(pen);         break;
    case Graph::CURVES:    curves_menu_->pen(pen);       break;
    case Graph::MOSAIC:    mosaic_menu_->pen(pen);       break;
    case Graph::ARROW:     arrow_menu_->pen(pen);        break;
    case Graph::TEXT:      text_menu_->pen(pen);         break;
    case Graph::ERASER:    erase_menu_->pen(pen);        break;
    default:        break;
    }
    emit styleChanged(graph);
}

void ImageEditMenu::style(Graph graph, QPen pen, bool fill)
{
    if (pen.width() < 1) pen.setWidth(1);

    switch (graph) {
    case Graph::RECTANGLE: rectangle_menu_->style(pen, fill);   break;
    case Graph::CIRCLE:    circle_menu_->style(pen, fill);      break;
    case Graph::LINE:      line_menu_->pen(pen);                break;
    case Graph::CURVES:    curves_menu_->pen(pen);              break;
    case Graph::MOSAIC:    mosaic_menu_->pen(pen);              break;
    case Graph::ARROW:     arrow_menu_->pen(pen);               break;
    case Graph::TEXT:      text_menu_->pen(pen);                break;
    case Graph::ERASER:    erase_menu_->pen(pen);               break;
    default:        break;
    }
    emit styleChanged(graph);
}

bool ImageEditMenu::fill(Graph graph)
{
    switch (graph) {
    case Graph::RECTANGLE: return rectangle_menu_->fill();
    case Graph::CIRCLE:    return circle_menu_->fill();
    case Graph::ARROW:     return true;

    case Graph::LINE:
    case Graph::CURVES:
    case Graph::MOSAIC:
    case Graph::TEXT:
    case Graph::ERASER:
    default:               return false;
    }
}

void ImageEditMenu::fill(Graph graph, bool fill)
{
    switch (graph) {
    case Graph::RECTANGLE: rectangle_menu_->fill(fill); break;
    case Graph::CIRCLE:    circle_menu_->fill(fill);    break;
    default:        break;
    }

    emit styleChanged(graph);
}

QFont ImageEditMenu::font(Graph graph)
{
    switch (graph) {
    case Graph::TEXT:       return text_menu_->font();
    default:                return QFont();
    }
}

void ImageEditMenu::font(const QFont& font)
{
    if (text_menu_)
        text_menu_->font(font);
}

void ImageEditMenu::reset()
{
    group_->uncheckAll();
}
