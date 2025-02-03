#pragma once
#include "Inputs.h"

namespace SP
{
    // AppWindow Events

    struct WindowMaximizeEvent {};

    struct WindowIconifyEvent {};

    struct WindowRestoreEvent {};

    struct WindowCloseEvent {};

    struct WindowResizeEvent
    {
        SP_INLINE WindowResizeEvent(int32_t w, int32_t h) :
            Width(w), Height(h)
        {}
        int32_t Height = 0;
        int32_t Width = 0;
    };

    // Key Events

    struct KeyReleaseEvent
    {
        SP_INLINE KeyReleaseEvent(int32_t key) :
            Key(key)
        {}
        int32_t Key = -1;
    };

    struct KeyPressEvent
    {
        SP_INLINE KeyPressEvent(int32_t key) :
            Key(key)
        {}
        int32_t Key = -1;
    };

    struct KeyRepeatEvent
    {
        SP_INLINE KeyRepeatEvent(int32_t key) :
            Key(key)
        {}
        int32_t Key = -1;
    };

    // Mouse Events

    struct MouseReleaseEvent
    {
        SP_INLINE MouseReleaseEvent(int32_t b) :
            Button(b)
        {}
        int32_t Button = -1;
    };

    struct MouseDownEvent
    {
        SP_INLINE MouseDownEvent(int32_t b) :
            Button(b)
        {}
        int32_t Button = -1;
    };

    struct MouseDragEvent
    {
        SP_INLINE MouseDragEvent(double dx, double dy) :
            DeltaX(dx), DeltaY(dy)
        {}
        double DeltaX = 0.0, DeltaY = 0.0;
    };

    struct MouseMotionEvent
    {
        SP_INLINE MouseMotionEvent(double x, double y) :
            TargetX(x), TargetY(y)
        {}
        double TargetX = 0.0, TargetY = 0.0;
    };

    struct MouseWheelEvent
    {
        SP_INLINE MouseWheelEvent(double sx, double sy) :
            ScrollX(sx), ScrollY(sy)
        {}
        double ScrollX = 0.0, ScrollY = 0.0;
    };

}
