/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/text.cpp
 *  A text object.
 */

#include "events/requests.h"

#include "graphics/graphics.h"
#include "graphics/font.h"

#include "graphics/aurora/text.h"

namespace Graphics {

namespace Aurora {

Text::Text(const FontHandle &font, const Common::UString &str, float align) :
	_font(font), _x(0.0), _y(0.0), _hasColor(false), _align(align), _list(0) {

	set(str);

	_distance = -FLT_MAX;
}

Text::Text(const FontHandle &font, const Common::UString &str,
		float r, float g, float b, float a, float align) :
	_font(font), _x(0.0), _y(0.0), _hasColor(true), _r(r), _g(g), _b(b), _a(a),
	_align(align), _ready(false), _list(0) {

	set(str);

	_distance = -FLT_MAX;
}

Text::~Text() {
	if (_list != 0)
		GfxMan.abandon(_list, 1);
}

void Text::set(const Common::UString &str) {
	_ready = false;
	if (str.empty())
		return;

	_str = str;

	const Font &font = _font.getFont();

	_height = font.getHeight(_str);
	_width  = font.getWidth (_str);

	RequestMan.dispatchAndForget(RequestMan.rebuild(*this));
}

void Text::setColor(float r, float g, float b, float a) {
	_hasColor = true;
	_r = r;
	_g = g;
	_b = b;
	_a = a;

	RequestMan.dispatchAndForget(RequestMan.rebuild(*this));
}

void Text::unsetColor() {
	_hasColor = false;

	RequestMan.dispatchAndForget(RequestMan.rebuild(*this));
}

void Text::setPosition(float x, float y, float z) {
	_x = roundf(x);
	_y = roundf(y);

	_distance = z;
}

void Text::show() {
	Renderable::addToQueue();
}

void Text::hide() {
	Renderable::removeFromQueue();
}

bool Text::isVisible() {
	return Renderable::isInQueue();
}

float Text::getWidth() const {
	return _width;
}

float Text::getHeight() const {
	return _height;
}

void Text::newFrame() {
}

void Text::render() {
	if (!_ready)
		return;

	glTranslatef(_x, _y, 0.0);

	if (_list > 0)
		glCallList(_list);
}

void Text::doRebuild() {
	const Font &font = _font.getFont();

	_list = glGenLists(1);

	glNewList(_list, GL_COMPILE);
	if (_hasColor)
		font.draw(_str, _r, _g, _b, _a, _align);
	else
		font.draw(_str, _align);
	glEndList();

	_ready = true;
}

void Text::doDestroy() {
	if (_list == 0)
		return;

	glDeleteLists(_list, 0);

	_list = 0;
}

} // End of namespace Aurora

} // End of namespace Graphics
