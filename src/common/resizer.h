#ifndef CAPTURER_RESIZER_H
#define CAPTURER_RESIZER_H

#include <probe/graphics.h>
#include <QPoint>
#include <QRect>
#include <QWidget>
#include <type_traits>

template<typename T> struct qpoint_trait
{
    using type = QPoint;
};

template<typename T>
requires std::floating_point<T>
struct qpoint_trait<T>
{
    using type = QPointF;
};

///
template<typename T> struct qrect_trait
{
    using type = QRect;
};

template<typename T>
requires std::floating_point<T>
struct qrect_trait<T>
{
    using type = QRectF;
};

///
template<typename T> struct qsize_trait
{
    using type = QSize;
};

template<typename T>
requires std::floating_point<T>
struct qsize_trait<T>
{
    using type = QSizeF;
};

///
template<typename T> struct qmargins_trait
{
    using type = QMargins;
};

template<typename T>
requires std::floating_point<T>
struct qmargins_trait<T>
{
    using type = QMarginsF;
};

// clang-format off
enum class ResizerLocation {
    DEFAULT         = 0x00000000,

    BORDER          = 0x00000001 | 0x00000002 | 0x00000004 | 0x00000008,

    X1_BORDER       = 0x00000001,  L_BORDER    = 0x10000001,
    X2_BORDER       = 0x00000002,  R_BORDER    = 0x10000002,
    Y1_BORDER       = 0x00000004,  T_BORDER    = 0x10000003,
    Y2_BORDER       = 0x00000008,  B_BORDER    = 0x10000004,

    ANCHOR          = 0x00000010 | 0x00000020 | 0x00000040 | 0x00000080 | 0x00000100 | 0x00000200 | 0x00000400 | 0x00000800,

    X1_ANCHOR       = 0x00000010,  L_ANCHOR    = 0x10000010,
    X2_ANCHOR       = 0x00000020,  R_ANCHOR    = 0x10000020,
    Y1_ANCHOR       = 0x00000040,  T_ANCHOR    = 0x10000030,
    Y2_ANCHOR       = 0x00000080,  B_ANCHOR    = 0x10000040,
    
    X1Y1_ANCHOR     = 0x00000100,  TL_ANCHOR   = 0x10000100,
    X1Y2_ANCHOR     = 0x00000200,  BL_ANCHOR   = 0x10000200,
    X2Y1_ANCHOR     = 0x00000400,  TR_ANCHOR   = 0x10000300,
    X2Y2_ANCHOR     = 0x00000800,  BR_ANCHOR   = 0x10000400,

    ROTATE_ANCHOR   = 0x00001000,
    EMPTY_INSIDE    = 0x00100000, // inside: such as a QGraphicsRectItem without brush
    OUTSIDE         = 0x00200000, // outside
    EDITING_INSIDE  = 0x00400000, // inside: QGraphicsTextItem with focus
    FILLED_INSIDE   = 0x00800000, // inside: QGraphicsTextItem with focus, QGraphicsRectItem with brush

    ADJUST_AREA = BORDER | ANCHOR | ROTATE_ANCHOR | FILLED_INSIDE, // move, resize ...

   ENABLE_BITMASK_OPERATORS(),
};
// clang-format on

///
template<typename T>
requires((std::integral<T> || std::floating_point<T>) && !std::same_as<T, bool>)
class _Resizer
{
public:
    using qpoint_t   = qpoint_trait<T>::type;
    using qrect_t    = qrect_trait<T>::type;
    using qsize_t    = qsize_trait<T>::type;
    using qmargins_t = qmargins_trait<T>::type;

public:
    _Resizer()
        : _Resizer(0, 0, 0, 0, 5)
    {}

    _Resizer(T x1, T y1, T x2, T y2, T border_w = 5, T anchor_w = 7)
        : x1_(x1), y1_(y1), x2_(x2), y2_(y2), border_w_(border_w), anchor_w_(anchor_w)
    {}

    _Resizer(const qpoint_t& p1, const qpoint_t& p2, T border_width = 5, T anchor_w = 7)
        : _Resizer(p1.x(), p1.y(), p2.x(), p2.y(), border_width, anchor_w)
    {}

    _Resizer(const qpoint_t& p1, const qsize_t& s, T border_width = 5, T anchor_w = 7)
        : _Resizer(p1.x(), p1.y(), p1.x() + s.width() - 1, p1.y() + s.height() - 1, border_width, anchor_w)
    {}

    explicit _Resizer(const qrect_t& rect, T border_width = 5, T anchor_w = 7)
        : _Resizer(rect.topLeft(), rect.bottomRight(), border_width, anchor_w)
    {}

    _Resizer(const _Resizer& r)
    {
        x1_ = r.x1_;
        x2_ = r.x2_;
        y1_ = r.y1_;
        y2_ = r.y2_;

        range_ = r.range_;

        rotate_f_ = r.rotate_f_;

        border_w_ = r.border_w_;
        anchor_w_ = r.anchor_w_;
    }

    _Resizer& operator=(const _Resizer& r)
    {
        x1_ = r.x1_;
        x2_ = r.x2_;
        y1_ = r.y1_;
        y2_ = r.y2_;

        range_ = r.range_;

        rotate_f_ = r.rotate_f_;

        border_w_ = r.border_w_;
        anchor_w_ = r.anchor_w_;

        return *this;
    }

    bool operator==(const _Resizer& rhs)
    {
        return x1_ == rhs.x1_ && y1_ == rhs.y1_ && x2_ == rhs.x2_ && y2_ == rhs.y2_;
    }

    // TODO: constrain the box coordinates by range
    void range(const qrect_t& r) { range_ = r; }

    void range(const probe::geometry_t& r)
    {
        range_ = qrect_t{
            static_cast<T>(r.x),
            static_cast<T>(r.y),
            static_cast<T>(r.width),
            static_cast<T>(r.height),
        };
    }

    qrect_t range() const { return range_; }

    void enableRotate(bool val) { rotate_f_ = val; }

    [[nodiscard]] bool rotationEnabled() const { return rotate_f_; }

    void coords(T x1, T y1, T x2, T y2)
    {
        x1_ = clamp_x(x1);
        y1_ = clamp_y(y1);
        x2_ = clamp_x(x2);
        y2_ = clamp_y(y2);
    }

    void coords(const qpoint_t& p1, const qpoint_t& p2) { coords(p1.x(), p1.y(), p2.x(), p2.y()); }

    void coords(const qrect_t& rect) { coords(rect.left(), rect.top(), rect.right(), rect.bottom()); }

    void coords(const probe::geometry_t& rect)
    {
        coords(rect.left(), rect.top(), rect.right(), rect.bottom());
    }

    // clang-format off
    T clamp_x(T v)           { return std::clamp(v, range_.left(), range_.right()); }
    T clamp_y(T v)           { return std::clamp(v, range_.top(), range_.bottom()); }
    
    T x1()     const         { return x1_; }
    T x2()     const         { return x2_; }
    T y1()     const         { return y1_; }
    T y2()     const         { return y2_; }

    void x1(T v)             { x1_ = clamp_x(v); }
    void x2(T v)             { x2_ = clamp_x(v); }
    void y1(T v)             { y1_ = clamp_y(v); }
    void y2(T v)             { y2_ = clamp_y(v); }

    void point1(T x1, T y1)  { x1_ = clamp_x(x1), y1_ = clamp_y(y1); }
    void point2(T x2, T y2)  { x2_ = clamp_x(x2), y2_ = clamp_y(y2); }

    void point1(const qpoint_t& p)  { x1_ = clamp_x(p.x()), y1_ = clamp_y(p.y()); }
    void point2(const qpoint_t& p)  { x2_ = clamp_x(p.x()), y2_ = clamp_y(p.y()); }

    T left()   const         { return x1_ < x2_ ? x1_ : x2_; }
    T right()  const         { return x1_ > x2_ ? x1_ : x2_; }
    T top()    const         { return y1_ < y2_ ? y1_ : y2_; }
    T bottom() const         { return y1_ > y2_ ? y1_ : y2_; }

    void left(int v)         { x1_ < x2_ ? (x1_ = clamp_x(v)) : (x2_ = clamp_x(v)); }
    void right(int v)        { x1_ > x2_ ? (x1_ = clamp_x(v)) : (x2_ = clamp_x(v)); }
    void top(int v)          { y1_ < y2_ ? (y1_ = clamp_y(v)) : (y2_ = clamp_y(v)); }
    void bottom(int v)       { y1_ > y2_ ? (y1_ = clamp_y(v)) : (y2_ = clamp_y(v)); }
    // clang-format on

    void adjust(T dx1, T dy1, T dx2, T dy2)
    {
        x1_ = clamp_x(x1_ + dx1);
        x2_ = clamp_x(x2_ + dx2);
        y1_ = clamp_y(y1_ + dy1);
        y2_ = clamp_y(y2_ + dy2);
    }

    void margins(T dt, T dr, T db, T dl)
    {
        top(top() + dt);
        right(right() + dr);
        bottom(bottom() + db);
        left(left() + dl);
    }

    void resize(T w, T h)
    {
        right(left() + w);
        bottom(top() + h);
    }

    void resize(const qsize_t& size)
    {
        right(left() + size.width());
        bottom(top() + size.height());
    }

    void translate(T dx, T dy)
    {
        dx = std::clamp(dx, range_.left() - left(), range_.right() - right());
        dy = std::clamp(dy, range_.top() - top(), range_.bottom() - bottom());

        x1_ += dx;
        x2_ += dx;
        y1_ += dy;
        y2_ += dy;
    }

    void translate(const qpoint_t& d) { translate(d.x(), d.y()); }

    bool contains(const qpoint_t& p) const { return qrect_t(left(), top(), width(), height()).contains(p); }

    // clang-format off
    qrect_t Y1Anchor()          const { return { (x1_ + x2_) / 2 - anchor_w_ / 2, y1_ - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t X1Anchor()          const { return { x1_ - anchor_w_ / 2, (y1_ + y2_) / 2 - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t Y2Anchor()          const { return { (x1_ + x2_) / 2 - anchor_w_ / 2, y2_ - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t X2Anchor()          const { return { x2_ - anchor_w_ / 2, (y1_ + y2_) / 2 - anchor_w_ / 2, anchor_w_, anchor_w_ }; }

    qrect_t X1Y1Anchor()        const { return { x1_ - anchor_w_ / 2, y1_ - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t X1Y2Anchor()        const { return { x1_ - anchor_w_ / 2, y2_ - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t X2Y1Anchor()        const { return { x2_ - anchor_w_ / 2, y1_ - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t X2Y2Anchor()        const { return { x2_ - anchor_w_ / 2, y2_ - anchor_w_ / 2, anchor_w_, anchor_w_ }; }

    qrect_t topAnchor()         const { return { (x1_ + x2_) / 2 - anchor_w_ / 2, top() - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t leftAnchor()        const { return { left() - anchor_w_ / 2, (y1_ + y2_) / 2 - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t bottomAnchor()      const { return { (x1_ + x2_) / 2 - anchor_w_ / 2, bottom() - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t rightAnchor()       const { return { right() - anchor_w_ / 2, (y1_ + y2_) / 2 - anchor_w_ / 2, anchor_w_, anchor_w_ }; }

    qrect_t topLeftAnchor()     const { return { left() - anchor_w_ / 2, top() - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t topRightAnchor()    const { return { right() - anchor_w_ / 2, top() - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t bottomLeftAnchor()  const { return { left() - anchor_w_ / 2, bottom() - anchor_w_ / 2, anchor_w_, anchor_w_ }; }
    qrect_t bottomRightAnchor() const { return { right() - anchor_w_ / 2, bottom() - anchor_w_ / 2, anchor_w_, anchor_w_ }; }

    QVector<qrect_t> anchors() const
    {
        return QVector<qrect_t>{
            rightAnchor(),   topAnchor(),      bottomAnchor(),     leftAnchor(),
            topLeftAnchor(), topRightAnchor(), bottomLeftAnchor(), bottomRightAnchor(),
        };
    }

    QVector<qrect_t> cornerAnchors() const
    {
        return QVector<qrect_t>{ 
            topLeftAnchor(),       topRightAnchor(), 
            bottomLeftAnchor(),    bottomRightAnchor(),
        };
    }

    qrect_t rotateAnchor() const
    {
        qrect_t anchor{ 0, 0, 11, 11 };
        anchor.moveCenter({ (x1_ + x2_) / 2, y1_ + (vflipped() ? 20 : - 20) });
        return anchor;
    }

    // borders
    bool isX1Border(const qpoint_t& p)      const { return qrect_t(x1_ - border_w_ / 2, top(), border_w_, height()).contains(p); }
    bool isX2Border(const qpoint_t& p)      const { return qrect_t(x2_ - border_w_ / 2, top(), border_w_, height()).contains(p); }
    bool isY1Border(const qpoint_t& p)      const { return qrect_t(left(), y1_ - border_w_ / 2, width(), border_w_).contains(p); }
    bool isY2Border(const qpoint_t& p)      const { return qrect_t(left(), y2_ - border_w_ / 2, width(), border_w_).contains(p); }
    
    bool isBorder(const qpoint_t& p)        const { return isX1Border(p) || isX2Border(p) || isY1Border(p) || isY2Border(p); }
    
    bool isLeftBorder(const qpoint_t& p)    const { return qrect_t(left() - border_w_ / 2, top(), border_w_, height()).contains(p); }
    bool isRightBorder(const qpoint_t& p)   const { return qrect_t(right() - border_w_ / 2, top(), border_w_, height()).contains(p); }
    bool isTopBorder(const qpoint_t& p)     const { return qrect_t(left(), top() - border_w_ / 2, width(), border_w_).contains(p); }
    bool isBottomBorder(const qpoint_t& p)  const { return qrect_t(left(), bottom() - border_w_ / 2, width(), border_w_).contains(p); }

    // anchors
    bool isRotateAnchor(const qpoint_t& p)       const { return rotate_f_ && rotateAnchor().contains(p); }

    bool isY1Anchor(const qpoint_t& p)           const { return Y1Anchor().contains(p); }
    bool isY2Anchor(const qpoint_t& p)           const { return Y2Anchor().contains(p); }
    bool isX1Anchor(const qpoint_t& p)           const { return X1Anchor().contains(p); }
    bool isX2Anchor(const qpoint_t& p)           const { return X2Anchor().contains(p); }

    bool isX1Y1Anchor(const qpoint_t& p)         const { return X1Y1Anchor().contains(p); }
    bool isX2Y1Anchor(const qpoint_t& p)         const { return X2Y1Anchor().contains(p); }
    bool isX1Y2Anchor(const qpoint_t& p)         const { return X1Y2Anchor().contains(p); }
    bool isX2Y2Anchor(const qpoint_t& p)         const { return X2Y2Anchor().contains(p); }

    bool isTopAnchor(const qpoint_t& p)          const { return topAnchor().contains(p); }
    bool isBottomAnchor(const qpoint_t& p)       const { return bottomAnchor().contains(p); }
    bool isLeftAnchor(const qpoint_t& p)         const { return leftAnchor().contains(p); }
    bool isRightAnchor(const qpoint_t& p)        const { return rightAnchor().contains(p); }
    bool isTopLeftAnchor(const qpoint_t& p)      const { return topLeftAnchor().contains(p); }
    bool isTopRightAnchor(const qpoint_t& p)     const { return topRightAnchor().contains(p); }
    bool isBottomLeftAnchor(const qpoint_t& p)   const { return bottomLeftAnchor().contains(p); }
    bool isBottomRightAnchor(const qpoint_t& p)  const { return bottomRightAnchor().contains(p); }

    bool isAnchor(const qpoint_t& p) const
    {
        return isX1Anchor(p) || isX2Anchor(p) || isY2Anchor(p) || isY1Anchor(p) || isX1Y1Anchor(p) ||
               isX2Y1Anchor(p) || isX1Y2Anchor(p) || isX2Y2Anchor(p);
    }

    bool isCornerAnchor(const qpoint_t& p) const
    {
        return isX1Y1Anchor(p) || isX2Y1Anchor(p) || isX1Y2Anchor(p) || isX2Y2Anchor(p);
    }

    T width()                const { return std::abs(x1_ - x2_) + 1; } // like QRect class
    T height()               const { return std::abs(y1_ - y2_) + 1; }

    qsize_t size()           const { return { width(), height() }; }

    qpoint_t topLeft()       const { return { left(), top() }; }
    qpoint_t bottomRight()   const { return { right(), bottom() }; }
    qpoint_t topRight()      const { return { right(), top() }; }
    qpoint_t bottomLeft()    const { return { left(), bottom() }; }

    qpoint_t point1()        const { return { x1_, y1_ }; }
    qpoint_t point2()        const { return { x2_, y2_ }; }

    qrect_t rect()           const { return { qpoint_t{ left(), top() }, qpoint_t{ right(), bottom() } }; }
    
    qpoint_t center()        const { return rect().center(); }

    qrect_t boundingRect()   const
    {
        auto half = std::max(anchor_w_ / 2, border_w_ / 2);
        auto rect = qrect_t{ qpoint_t{ left(), top() }, qpoint_t{ right(), bottom() } }.adjusted(-half, -half, half, half);
        return rotate_f_ ? rect.united(rotateAnchor()) : rect;
    }

    ResizerLocation absolutePos(const qpoint_t& p, bool filled = false, bool border_anchors = true) const
    {
        using enum ResizerLocation;

        if (isX1Y1Anchor(p))        return X1Y1_ANCHOR;
        if (isX2Y1Anchor(p))        return X2Y1_ANCHOR;
        if (isX1Y2Anchor(p))        return X1Y2_ANCHOR;
        if (isX2Y2Anchor(p))        return X2Y2_ANCHOR;

        if (border_anchors) {
            if (isX1Anchor(p))      return X1_ANCHOR;
            if (isX2Anchor(p))      return X2_ANCHOR;
            if (isY2Anchor(p))      return Y2_ANCHOR;
            if (isY1Anchor(p))      return Y1_ANCHOR;
        }

        if (isY1Border(p))          return Y1_BORDER;
        if (isY2Border(p))          return Y2_BORDER;
        if (isX1Border(p))          return X1_BORDER;
        if (isX2Border(p))          return X2_BORDER;

        if (isRotateAnchor(p))      return ROTATE_ANCHOR;

        return contains(p) ? (filled ? FILLED_INSIDE : EMPTY_INSIDE) : OUTSIDE;
    }

    ResizerLocation relativePos(const qpoint_t& p, bool filled = false, bool border_anchors = true) const
    {
        using enum ResizerLocation;

        if (isTopLeftAnchor(p))     return TL_ANCHOR;
        if (isTopRightAnchor(p))    return TR_ANCHOR;
        if (isBottomRightAnchor(p)) return BR_ANCHOR;
        if (isBottomLeftAnchor(p))  return BL_ANCHOR;

        if (border_anchors) {
            if (isLeftAnchor(p))    return L_ANCHOR;
            if (isRightAnchor(p))   return R_ANCHOR;
            if (isTopAnchor(p))     return T_ANCHOR;
            if (isBottomAnchor(p))  return B_ANCHOR;
        }

        if (isLeftBorder(p))        return L_BORDER;
        if (isRightBorder(p))       return R_BORDER;
        if (isTopBorder(p))         return T_BORDER;
        if (isBottomBorder(p))      return B_BORDER;

        if (isRotateAnchor(p))      return ROTATE_ANCHOR;

        return contains(p) ? (filled ? FILLED_INSIDE : EMPTY_INSIDE) : OUTSIDE;
    }
    // clang-format on

    T borderWidth() const { return border_w_; }
    T anchorWidth() const { return anchor_w_; }

    void setBorderWidth(const T& value) { border_w_ = value; }
    void setAnchorWidth(const T& value) { anchor_w_ = value; }

    [[nodiscard]] bool hflipped() const { return x1_ > x2_; }
    [[nodiscard]] bool vflipped() const { return y1_ > y2_; }

    void flip(bool h, bool v)
    {
        if (h) std::swap(x1_, x2_);
        if (v) std::swap(y1_, y2_);
    }

protected:
    // point 1
    T x1_{};
    T y1_{};
    // point 2
    T x2_{};
    T y2_{};

    qrect_t range_{ qpoint_t{ std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest() },
                    qpoint_t{ std::numeric_limits<T>::max(), std::numeric_limits<T>::max() } };

    bool rotate_f_{};

    T border_w_{ 5 };
    T anchor_w_{ 7 };
};

using Resizer  = _Resizer<int>;
using ResizerF = _Resizer<qreal>;

inline QCursor getCursorByLocation(ResizerLocation pos, const QCursor& default_cursor = Qt::CrossCursor)
{
    switch (pos) {
    case ResizerLocation::EDITING_INSIDE: return Qt::IBeamCursor;

    case ResizerLocation::X1_ANCHOR:
    case ResizerLocation::X2_ANCHOR:      return Qt::SizeHorCursor;
    case ResizerLocation::Y1_ANCHOR:
    case ResizerLocation::Y2_ANCHOR:      return Qt::SizeVerCursor;
    case ResizerLocation::X1Y1_ANCHOR:
    case ResizerLocation::X2Y2_ANCHOR:    return Qt::SizeFDiagCursor;
    case ResizerLocation::X1Y2_ANCHOR:
    case ResizerLocation::X2Y1_ANCHOR:    return Qt::SizeBDiagCursor;

    case ResizerLocation::BORDER:
    case ResizerLocation::FILLED_INSIDE:
    case ResizerLocation::X1_BORDER:
    case ResizerLocation::X2_BORDER:
    case ResizerLocation::Y1_BORDER:
    case ResizerLocation::Y2_BORDER:      return Qt::SizeAllCursor;

    case ResizerLocation::ROTATE_ANCHOR:
        return QCursor{
            QPixmap(":/icons/rotate").scaled(21, 21, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        };

    default: return default_cursor;
    }
}

#endif //! CAPTURER_RESIZER_H
