/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.16.1 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/**
 * @file touchgfx/widgets/canvas/PainterGRAY4.hpp
 *
 * Declares the touchgfx::PainterGRAY4 class.
 */
#ifndef PAINTERGRAY4_HPP
#define PAINTERGRAY4_HPP

#include <stdint.h>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterGRAY4.hpp>

namespace touchgfx
{
/**
 * The PainterGRAY4 class allows a shape to be filled with a given color and alpha
 * value. This allows transparent, anti-aliased elements to be drawn.
 *
 * @see AbstractPainter
 */
class PainterGRAY4 : public AbstractPainterGRAY4
{
public:
    /**
     * Initializes a new instance of the PainterGRAY4 class.
     *
     * @param  color (Optional) the color, default is black.
     * @param  alpha (Optional) the alpha, default is 255 i.e. solid.
     */
    PainterGRAY4(colortype color = 0, uint8_t alpha = 255)
        : AbstractPainterGRAY4()
    {
        setColor(color);
        setAlpha(alpha);
    }

    /**
     * Sets color and alpha to use when drawing the CanvasWidget.
     *
     * @param  color The color.
     */
    void setColor(colortype color)
    {
        painterGray = color & 0x0F;
    }

    /**
     * Gets the current color.
     *
     * @return The color.
     */
    colortype getColor() const
    {
        return static_cast<colortype>(painterGray);
    }

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

protected:
    virtual bool renderNext(uint8_t& gray, uint8_t& alpha);

    uint8_t painterGray; ///< The gray color
};

} // namespace touchgfx

#endif // PAINTERGRAY4_HPP
