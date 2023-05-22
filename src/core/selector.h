#ifndef SELECTOR_H
#define SELECTOR_H

#include "hunter.h"
#include "json.h"
#include "probe/graphics.h"
#include "resizer.h"
#include "utils.h"

#include <QLabel>
#include <QPainter>
#include <QWidget>

// clang-format off
#define LOCKED()            do{ status_ = SelectorStatus::LOCKED;   emit locked();              } while(0)
#define CAPTURED()          do{ status_ = SelectorStatus::CAPTURED; emit captured(); update();  } while(0)
// clang-format on

class Selector : public QWidget
{
    Q_OBJECT

public:
    enum class SelectorStatus
    {
        INITIAL,
        NORMAL,
        START_SELECTING,
        SELECTING,
        CAPTURED,
        MOVING,
        RESIZING,
        LOCKED,
    };

    enum class scope_t
    {
        desktop, // virtual screen
        display,
    };

public:
    explicit Selector(QWidget *parent = nullptr);
    ~Selector() override = default;

public slots:
    virtual void start();
    void setBorderColor(const QColor&);
    void setBorderWidth(int);
    void setBorderStyle(Qt::PenStyle s);
    void setMaskColor(const QColor&);
    void setUseDetectWindow(bool);

    void showRegion()
    {
        info_->hide();
        mask_hidden_ = true;
        repaint();
    }

    void resetSelected()
    {
        box_.coords(use_detect_ ? probe::graphics::virtual_screen_geometry() : probe::geometry_t{});
    }

    // hiding
    void hide()
    {
        info_->hide();
        resetSelected();
        repaint();
        QWidget::hide();
    }

    virtual void exit();

    void updateTheme(json& setting);

    // minimum size of selected area
    void setMinValidSize(const QSize& size) { min_size_ = size; }

    bool isValid() { return box_.width() >= min_size_.width() && box_.height() >= min_size_.height(); }

signals:
    void captured();
    void moved();
    void resized();
    void locked();

protected:
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void paintEvent(QPaintEvent *) override;

    void update_info_label();

    // selected area
    [[nodiscard]] inline QRect selected(bool relative = false) const
    {
        return relative ? box_.rect().translated(box_.range().topLeft()) : box_.rect();
    }

    QPainter painter_;
    SelectorStatus status_             = SelectorStatus::INITIAL;
    Resizer::PointPosition cursor_pos_ = Resizer::OUTSIDE;

    // move
    QPoint mbegin_{ 0, 0 }, mend_{ 0, 0 };

    // selecting
    QPoint sbegin_{ 0, 0 };

    // selected area @{
    void select(const hunter::prey_t&);
    void select(const probe::graphics::display_t&);
    void select(const QRect& rect);

    void translate(int32_t dx, int32_t dy);
    void adjust(int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2);
    void margins(int32_t dt, int32_t dr, int32_t db, int32_t dl);

    Resizer box_;                       // TODO: do not use this variable directly

    scope_t scope_{ scope_t::desktop }; // selection scope
    hunter::prey_t prey_{};             // capture object
    // @}

    bool prevent_transparent_ = false;

    // windows detection flags
    probe::graphics::window_filter_t windows_detection_flags_{ probe::graphics::window_filter_t::visible };

private:
    void registerShortcuts();

    QLabel *info_{ nullptr };

    QPen pen_{ Qt::cyan, 1, Qt::DashDotLine, Qt::SquareCap, Qt::MiterJoin };
    QColor mask_color_{ 0, 0, 0, 100 };
    bool use_detect_{ true };
    bool mask_hidden_{ false };

    QSize min_size_{ 2, 2 };
};

std::string to_string(Selector::scope_t);

#endif // !SELECTOR_H
