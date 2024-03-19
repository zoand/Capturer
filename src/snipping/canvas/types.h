#ifndef CAPTURER_CANVAS_TYPES_H
#define CAPTURER_CANVAS_TYPES_H

namespace canvas
{
    enum graphics_t
    {
        none      = 0x0000,
        rectangle = 0x0001,
        ellipse   = 0x0002,
        arrow     = 0x0004,
        line      = 0x0008,
        curve     = 0x0010,
        pixmap    = 0x0020,
        counter   = 0x0040,
        text      = 0x0080,
        mosaic    = 0x0100,
        eraser    = 0x0200,
    };

    enum class adjusting_t
    {
        none     = 0x00,
        moving   = 0x01,
        resizing = 0x02,
        rotating = 0x04,
        editing  = 0x08, // text
        creating = 0x10,
    };

    inline bool has_color(const graphics_t graph)
    {
        switch (graph) {
        case rectangle:
        case ellipse:
        case arrow:
        case line:
        case curve:
        case counter:
        case text:      return true;
        default:        return false;
        }
    }

    inline bool has_width(const graphics_t graph)
    {
        switch (graph) {
        case rectangle:
        case ellipse:
        case line:
        case curve:
        case mosaic:
        case eraser:    return true;
        default:        return false;
        }
    }
} // namespace canvas

#endif //! CAPTURER_CANVAS_TYPES_H