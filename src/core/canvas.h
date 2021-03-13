#ifndef CANVAS_H
#define CANVAS_H

#include <cstdint>
#include <QPixmap>
#include "utils.h"
#include "imageeditmenu.h"
#include "command.h"
#include "circlecursor.h"

#define HIDE_AND_COPY_CMD(CMD)                           \
        st(                                              \
            auto pre_cmd = CMD;                          \
            pre_cmd->visible(false);                     \
            CMD = make_shared<PaintCommand>(*pre_cmd);   \
            CMD->previous(pre_cmd);                      \
            CMD->visible(true);                          \
        )

class Canvas : public QObject
{
    Q_OBJECT

public:
    enum EditStatus : uint32_t {
        NONE            = 0x00000000,
        READY           = 0x00010000,
        GRAPH_CREATING  = 0x00100000,
        GRAPH_MOVING    = 0x00200000,
        GRAPH_RESIZING  = 0x00400000,
        GRAPH_ROTATING  = 0x00800000,

        READY_MASK      = 0x000f0000,
        OPERATION_MASK  = 0xfff00000,
        GRAPH_MASK      = 0x0000ffff
    };
public:
    Canvas(QWidget*parent = nullptr);
    ImageEditMenu* menu_ = nullptr;

    QCursor getCursorShape();
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
    void wheelEvent(QWheelEvent*);

    void updateCanvas();
    void drawModifying(QPainter*);

    QPixmap canvas() { return canvas_; }

    void offset(const QPoint& offset) { offset_ = offset; }

    bool editing();

    void clear();
    void reset();
signals:
    void focusOnGraph(Graph);
    void closed();
    void changed();

public slots:
    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }

    void changeGraph(Graph);
    void canvas(const QPixmap& canvas);
    void copy();
    void paste();
    void remove();

    void undo();
    void redo();

    void modified(PaintType type) {
        if (type == PaintType::UNMODIFIED) {
            modified_ = PaintType::UNMODIFIED;
        }
        else if(type > modified_){
            modified_ = type;
        }

        changed();
    }

private:
    bool eventFilter(QObject* object, QEvent* event);
    void focusOn(shared_ptr<PaintCommand>);


    void updateHoverPos(const QPoint&);
    QImage mosaic(const QImage& );

    Resizer::PointPosition hover_position_ = Resizer::OUTSIDE;
    QPoint move_begin_{ 0, 0 };
    QPoint resize_begin_{ 0, 0 };

    CommandStack commands_;
    CommandStack redo_stack_;

    shared_ptr<PaintCommand> hover_cmd_ = nullptr;    // hover
    shared_ptr<PaintCommand> focus_cmd_ = nullptr;    // focus
    shared_ptr<PaintCommand> copied_cmd_ = nullptr;   // copied

    PaintType modified_ = PaintType::UNMODIFIED;

    CircleCursor circle_cursor_{ 20 };
    QPixmap canvas_;
    QPixmap backup_;
    uint32_t edit_status_ = EditStatus::NONE;

    QPoint offset_{ 0, 0 };

    bool enabled_ = false;
};

#endif // CANVAS_H
