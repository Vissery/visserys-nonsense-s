/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "engines/wintermute/base/base_game.h"
#include "engines/wintermute/base/gfx/opengl/base_render_opengl3d_shader.h"
#include "engines/wintermute/base/gfx/opengl/base_surface_opengl3d.h"
#include "engines/wintermute/base/gfx/opengl/camera3d.h"
#include "engines/wintermute/base/gfx/opengl/mesh3ds_opengl_shader.h"
#include "engines/wintermute/base/gfx/opengl/meshx_opengl_shader.h"
#include "graphics/opengl/system_headers.h"
#include "math/glmath.h"

namespace Wintermute {
BaseRenderer3D *makeOpenGL3DShaderRenderer(BaseGame *inGame) {
	return new BaseRenderOpenGL3DShader(inGame);
}

#include "common/pack-start.h"

struct SpriteVertexShader {
	float x;
	float y;
	float u;
	float v;
	float r;
	float g;
	float b;
	float a;
} PACKED_STRUCT;

#include "common/pack-end.h"

BaseRenderOpenGL3DShader::BaseRenderOpenGL3DShader(BaseGame *inGame)
    : BaseRenderer3D(inGame), _spriteBatchMode(false) {
}

BaseRenderOpenGL3DShader::~BaseRenderOpenGL3DShader() {
	glDeleteBuffers(1, &_spriteVBO);
}

bool BaseRenderOpenGL3DShader::setAmbientLightColor(uint32 color) {
	_ambientLightColor = color;
	_overrideAmbientLightColor = true;
	setAmbientLight();
	return true;
}

bool BaseRenderOpenGL3DShader::setDefaultAmbientLightColor() {
	_ambientLightColor = 0x00000000;
	_overrideAmbientLightColor = false;
	setAmbientLight();
	return true;
}

void BaseRenderOpenGL3DShader::setAmbientLight() {
	byte a = RGBCOLGetA(_ambientLightColor);
	byte r = RGBCOLGetR(_ambientLightColor);
	byte g = RGBCOLGetG(_ambientLightColor);
	byte b = RGBCOLGetB(_ambientLightColor);

	if (!_overrideAmbientLightColor) {
		uint32 color = _gameRef->getAmbientLightColor();

		a = RGBCOLGetA(color);
		r = RGBCOLGetR(color);
		g = RGBCOLGetG(color);
		b = RGBCOLGetB(color);
	}

	Math::Vector4d value;
	value.x() = r / 255.0f;
	value.y() = g / 255.0f;
	value.z() = b / 255.0f;
	value.w() = a / 255.0f;

	_modelXShader->use();
	_modelXShader->setUniform("ambientLight", value);
}

int BaseRenderOpenGL3DShader::maximumLightsCount() {
	return 8;
}

void BaseRenderOpenGL3DShader::enableLight(int index) {
	_modelXShader->use();
	Common::String uniform = Common::String::format("lights[%i].enabled", index);
	_modelXShader->setUniform1f(uniform.c_str(), 1.0f);
}

void BaseRenderOpenGL3DShader::disableLight(int index) {
	_modelXShader->use();
	Common::String uniform = Common::String::format("lights[%i].enabled", index);
	_modelXShader->setUniform1f(uniform.c_str(), -1.0f);
}

void BaseRenderOpenGL3DShader::setLightParameters(int index, const Math::Vector3d &position, const Math::Vector3d &direction, const Math::Vector4d &diffuse, bool spotlight) {
	Math::Vector4d position4d;
	position4d.x() = position.x();
	position4d.y() = position.y();
	position4d.z() = position.z();
	position4d.w() = 1.0f;

	Math::Vector4d direction4d;
	direction4d.x() = direction.x();
	direction4d.y() = direction.y();
	direction4d.z() = direction.z();
	direction4d.w() = 0.0f;

	if (spotlight) {
		direction4d.w() = -1.0f;
	}

	_modelXShader->use();

	Common::String uniform = Common::String::format("lights[%i]._position", index);
	_modelXShader->setUniform(uniform.c_str(), position4d);

	uniform = Common::String::format("lights[%i]._direction", index);
	_modelXShader->setUniform(uniform.c_str(), direction4d);

	uniform = Common::String::format("lights[%i]._color", index);
	_modelXShader->setUniform(uniform.c_str(), diffuse);
}

void BaseRenderOpenGL3DShader::setSpriteBlendMode(Graphics::TSpriteBlendMode blendMode) {
	switch (blendMode) {
	case Graphics::BLEND_NORMAL:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;

	case Graphics::BLEND_ADDITIVE:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;

	case Graphics::BLEND_SUBTRACTIVE:
		// wme3d takes the color value here
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
		break;

	default:
		error("BaseRenderOpenGL3DShader::setSpriteBlendMode unsupported blend mode %i", blendMode);
	}
}

bool BaseRenderOpenGL3DShader::enableShadows() {
	warning("BaseRenderOpenGL3DShader::enableShadows not implemented yet");
	return true;
}

bool BaseRenderOpenGL3DShader::disableShadows() {
	warning("BaseRenderOpenGL3DShader::disableDhadows not implemented yet");
	return true;
}

bool BaseRenderOpenGL3DShader::stencilSupported() {
	// assume that we have a stencil buffer
	return true;
}

BaseImage *BaseRenderOpenGL3DShader::takeScreenshot() {
	warning("BaseRenderOpenGL3DShader::takeScreenshot not yet implemented");
	return nullptr;
}

bool BaseRenderOpenGL3DShader::saveScreenShot(const Common::String &filename, int sizeX, int sizeY) {
	warning("BaseRenderOpenGL3DShader::saveScreenshot not yet implemented");
	return true;
}

bool BaseRenderOpenGL3DShader::setViewport(int left, int top, int right, int bottom) {
	_viewportRect.setRect(left, top, right, bottom);
	glViewport(left, top, right - left, bottom - top);
	return true;
}

bool BaseRenderOpenGL3DShader::setViewport(Rect32 *rect) {
	return setViewport(rect->left, rect->top, rect->right, rect->bottom);
}

Rect32 BaseRenderOpenGL3DShader::getViewPort() {
	return _viewportRect;
}

void BaseRenderOpenGL3DShader::setWindowed(bool windowed) {
	warning("BaseRenderOpenGL3DShader::setWindowed not yet implemented");
}

Graphics::PixelFormat BaseRenderOpenGL3DShader::getPixelFormat() const {
	return OpenGL::Texture::getRGBAPixelFormat();
}

void BaseRenderOpenGL3DShader::fade(uint16 alpha) {
	fadeToColor(0, 0, 0, (byte)(255 - alpha));
}

void BaseRenderOpenGL3DShader::fadeToColor(byte r, byte g, byte b, byte a) {
	setProjection2D();

	Math::Vector4d color;
	color.x() = r / 255.0f;
	color.y() = g / 255.0f;
	color.z() = b / 255.0f;
	color.w() = a / 255.0f;

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, _fadeVBO);

	_fadeShader->use();
	_fadeShader->setUniform("color", color);
	_fadeShader->setUniform("projMatrix", _projectionMatrix2d);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	setup2D(true);
}

bool BaseRenderOpenGL3DShader::drawLine(int x1, int y1, int x2, int y2, uint32 color) {
	warning("BaseRenderOpenGL3DShader::drawLine not yet implemented");
	return true;
}

bool BaseRenderOpenGL3DShader::drawRect(int x1, int y1, int x2, int y2, uint32 color, int width) {
	warning("BaseRenderOpenGL3DShader::drawRect not yet implemented");
	return true;
}

bool BaseRenderOpenGL3DShader::setProjection() {
	// is the viewport already set here?
	float viewportWidth = _viewportRect.right - _viewportRect.left;
	float viewportHeight = _viewportRect.bottom - _viewportRect.top;

	float verticalViewAngle = _fov;
	float aspectRatio = float(viewportWidth) / float(viewportHeight);
	// same defaults as wme
	float nearPlane = 90.0f;
	float farPlane = 10000.0f;

	float top = nearPlane * tanf(verticalViewAngle * 0.5f);

	_projectionMatrix3d = Math::makeFrustumMatrix(-top * aspectRatio, top * aspectRatio, -top, top, nearPlane, farPlane);
	return true;
}

bool BaseRenderOpenGL3DShader::setProjection2D() {
	float viewportWidth = _viewportRect.right - _viewportRect.left;
	float viewportHeight = _viewportRect.bottom - _viewportRect.top;

	float nearPlane = -1.0f;
	float farPlane = 100.0f;

	_projectionMatrix2d.setToIdentity();

	_projectionMatrix2d(0, 0) = 2.0f / viewportWidth;
	_projectionMatrix2d(1, 1) = 2.0f / viewportHeight;
	_projectionMatrix2d(2, 2) = 2.0f / (farPlane - nearPlane);

	_projectionMatrix2d(3, 0) = -1.0f;
	_projectionMatrix2d(3, 1) = -1.0f;
	_projectionMatrix2d(3, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
	return true;
}

void BaseRenderOpenGL3DShader::resetModelViewTransform() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void BaseRenderOpenGL3DShader::pushWorldTransform(const Math::Matrix4 &transform) {
	Math::Matrix4 tmp = transform;
	tmp.transpose();
	Math::Matrix4 newTop = _transformStack.back() * tmp;
	_transformStack.push_back(newTop);

	newTop.transpose();

	Math::Matrix4 newInvertedTranspose = newTop * _lastViewMatrix;
	newInvertedTranspose.inverse();
	newInvertedTranspose.transpose();

	_modelXShader->use();
	_modelXShader->setUniform("modelMatrix", newTop);
	_modelXShader->setUniform("normalMatrix", newInvertedTranspose);
}

void BaseRenderOpenGL3DShader::popWorldTransform() {
	_transformStack.pop_back();

	Math::Matrix4 currentTransform = _transformStack.back();
	currentTransform.transpose();

	Math::Matrix4 currentInvertedTranspose = currentTransform * _lastViewMatrix;
	currentInvertedTranspose.inverse();
	currentTransform.transpose();

	_modelXShader->use();
	_modelXShader->setUniform("modelMatrix", currentTransform);
	_modelXShader->setUniform("normalMatrix", currentInvertedTranspose);
}

bool BaseRenderOpenGL3DShader::windowedBlt() {
	warning("BaseRenderOpenGL3DShader::windowedBlt not yet implemented");
	return true;
}

bool BaseRenderOpenGL3DShader::fill(byte r, byte g, byte b, Common::Rect *rect) {
	glClearColor(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	return true;
}

void Wintermute::BaseRenderOpenGL3DShader::onWindowChange() {
	warning("BaseRenderOpenGL3DShader::onWindowChange not yet implemented");
}

bool BaseRenderOpenGL3DShader::initRenderer(int width, int height, bool windowed) {
	glGenBuffers(1, &_spriteVBO);
	glBindBuffer(GL_ARRAY_BUFFER, _spriteVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(SpriteVertexShader), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	static const char *spriteAttributes[] = {"position", "texcoord", "color", nullptr};
	_spriteShader = OpenGL::Shader::fromFiles("sprite", spriteAttributes);

	_spriteShader->enableVertexAttribute("position", _spriteVBO, 2, GL_FLOAT, false, sizeof(SpriteVertexShader), 0);
	_spriteShader->enableVertexAttribute("texcoord", _spriteVBO, 2, GL_FLOAT, false, sizeof(SpriteVertexShader), 8);
	_spriteShader->enableVertexAttribute("color", _spriteVBO, 4, GL_FLOAT, false, sizeof(SpriteVertexShader), 16);

	_transformStack.push_back(Math::Matrix4());
	_transformStack.back().setToIdentity();

	static const char *modelXAttributes[] = {"position", "texcoord", "normal", nullptr};
	_modelXShader = OpenGL::Shader::fromFiles("modelx", modelXAttributes);

	setDefaultAmbientLightColor();

	for (int i = 0; i < maximumLightsCount(); ++i) {
		setLightParameters(i, Math::Vector3d(0, 0, 0), Math::Vector3d(0, 0, 0), Math::Vector4d(0, 0, 0, 0), false);
		disableLight(i);
	}

	_windowed = windowed;
	_width = width;
	_height = height;

	setViewport(0, 0, width, height);

	float fadeVertexCoords[8];

	fadeVertexCoords[0 * 2 + 0] = _viewportRect.left;
	fadeVertexCoords[0 * 2 + 1] = _viewportRect.bottom;
	fadeVertexCoords[1 * 2 + 0] = _viewportRect.left;
	fadeVertexCoords[1 * 2 + 1] = _viewportRect.top;
	fadeVertexCoords[2 * 2 + 0] = _viewportRect.right;
	fadeVertexCoords[2 * 2 + 1] = _viewportRect.bottom;
	fadeVertexCoords[3 * 2 + 0] = _viewportRect.right;
	fadeVertexCoords[3 * 2 + 1] = _viewportRect.top;

	glGenBuffers(1, &_fadeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, _fadeVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 8, fadeVertexCoords, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	static const char *fadeAttributes[] = { "position" };
	_fadeShader = OpenGL::Shader::fromFiles("fade", fadeAttributes);

	_fadeShader->enableVertexAttribute("position", _fadeVBO, 2, GL_FLOAT, false, 8, 0);

	_active = true;
	// setup a proper state
	setup2D(true);
	return true;
}

bool Wintermute::BaseRenderOpenGL3DShader::flip() {
	g_system->updateScreen();
	return true;
}

bool BaseRenderOpenGL3DShader::indicatorFlip() {
	warning("BaseRenderOpenGL3DShader::indicatorFlip not yet implemented");
	return true;
}

bool BaseRenderOpenGL3DShader::forcedFlip() {
	warning("BaseRenderOpenGL3DShader::forcedFlip not yet implemented");
	return true;
}

void BaseRenderOpenGL3DShader::initLoop() {
	deleteRectList();
	setup2D();
}

bool BaseRenderOpenGL3DShader::setup2D(bool force) {
	if (_state3D || force) {
		_state3D = false;

		// some states are still missing here

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_CLIP_PLANE0);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glEnable(GL_BLEND);
		glPolygonMode(GL_FRONT, GL_FILL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		setProjection2D();
	}

	return true;
}

bool BaseRenderOpenGL3DShader::setup3D(Camera3D *camera, bool force) {
	if (!_state3D || force) {
		_state3D = true;

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		setAmbientLight();

		if (camera) {
			_fov = camera->_fov;

			Math::Matrix4 viewMatrix;
			camera->getViewMatrix(&viewMatrix);
			Math::Matrix4 cameraTranslate;
			cameraTranslate.setPosition(-camera->_position);
			cameraTranslate.transpose();
			viewMatrix = cameraTranslate * viewMatrix;
			_lastViewMatrix = viewMatrix;
		}

		bool fogEnabled;
		uint32 fogColor;
		float fogStart;
		float fogEnd;

		_gameRef->getFogParams(&fogEnabled, &fogColor, &fogStart, &fogEnd);

		if (fogEnabled) {
			glEnable(GL_FOG);
			glFogi(GL_FOG_MODE, GL_LINEAR);
			glFogf(GL_FOG_START, fogStart);
			glFogf(GL_FOG_END, fogEnd);

			GLfloat color[4];
			color[0] = RGBCOLGetR(fogColor) / 255.0f;
			color[1] = RGBCOLGetG(fogColor) / 255.0f;
			color[2] = RGBCOLGetB(fogColor) / 255.0f;
			color[3] = RGBCOLGetA(fogColor) / 255.0f;

			glFogfv(GL_FOG_COLOR, color);
		} else {
			glDisable(GL_FOG);
		}

		setProjection();
	}

	_modelXShader->use();
	_modelXShader->setUniform("viewMatrix", _lastViewMatrix);
	_modelXShader->setUniform("projMatrix", _projectionMatrix3d);
	// this is 8 / 255, since 8 is the value used by wme (as a DWORD)
	_modelXShader->setUniform1f("alphaRef", 0.031f);

	return true;
}

bool BaseRenderOpenGL3DShader::setupLines() {
	warning("BaseRenderOpenGL3DShader::setupLines not yet implemented");
	return true;
}

void BaseRenderOpenGL3DShader::project(const Math::Matrix4 &worldMatrix, const Math::Vector3d &point, int &x, int &y) {
	Math::Vector3d windowCoords;
	Math::Matrix4 modelMatrix = worldMatrix * _lastViewMatrix;
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	Math::gluMathProject(point, modelMatrix.getData(), _projectionMatrix3d.getData(), viewport, windowCoords);
	x = windowCoords.x();
	// The Wintermute script code will expect a Direct3D viewport
	y = viewport[3] - windowCoords.y();
}

Math::Ray BaseRenderOpenGL3DShader::rayIntoScene(int x, int y) {
	Math::Vector3d direction((((2.0f * x) / _viewportRect.width()) - 1) / _projectionMatrix3d(0, 0),
							 -(((2.0f * y) / _viewportRect.height()) - 1) / _projectionMatrix3d(1, 1),
	                         -1.0f);

	Math::Matrix4 m = _lastViewMatrix;
	m.inverse();
	m.transpose();
	m.transform(&direction, false);

	Math::Vector3d origin = m.getPosition();
	return Math::Ray(origin, direction);
}

BaseSurface *Wintermute::BaseRenderOpenGL3DShader::createSurface() {
	return new BaseSurfaceOpenGL3D(_gameRef, this);
}

bool BaseRenderOpenGL3DShader::drawSprite(BaseSurfaceOpenGL3D &tex, const Wintermute::Rect32 &rect,
                                          float zoomX, float zoomY, const Wintermute::Vector2 &pos,
                                          uint32 color, bool alphaDisable, Graphics::TSpriteBlendMode blendMode,
                                          bool mirrorX, bool mirrorY) {
	Vector2 scale(zoomX / 100.0f, zoomY / 100.0f);
	return drawSpriteEx(tex, rect, pos, Vector2(0.0f, 0.0f), scale, 0.0f, color, alphaDisable, blendMode, mirrorX, mirrorY);
}

bool BaseRenderOpenGL3DShader::drawSpriteEx(BaseSurfaceOpenGL3D &tex, const Wintermute::Rect32 &rect,
                                            const Wintermute::Vector2 &pos, const Wintermute::Vector2 &rot, const Wintermute::Vector2 &scale,
                                            float angle, uint32 color, bool alphaDisable, Graphics::TSpriteBlendMode blendMode,
                                            bool mirrorX, bool mirrorY) {
	// original wme has a batch mode for sprites, we ignore this for the moment

	if (_forceAlphaColor != 0) {
		color = _forceAlphaColor;
	}

	float width = (rect.right - rect.left) * scale.x;
	float height = (rect.bottom - rect.top) * scale.y;

	glBindTexture(GL_TEXTURE_2D, tex.getTextureName());

	// for sprites we clamp to the edge, to avoid line fragments at the edges
	// this is not done by wme, though
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// might as well provide getters for those
	int texWidth;
	int texHeight;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

	float texLeft = (float)rect.left / (float)texWidth;
	float texTop = (float)rect.top / (float)texHeight;
	float texRight = (float)rect.right / (float)texWidth;
	float texBottom = (float)rect.bottom / (float)texHeight;

	float offset = _viewportRect.height() / 2.0f;
	float correctedYPos = (pos.y - offset) * -1.0f + offset;

	// to be implemented
	if (mirrorX) {
		warning("BaseRenderOpenGL3DShader::SpriteEx x mirroring is not yet implemented");
	}

	if (mirrorY) {
		warning("BaseRenderOpenGL3DShader::SpriteEx y mirroring is not yet implemented");
	}

	SpriteVertexShader vertices[4] = {};

	// texture coords
	vertices[0].u = texLeft;
	vertices[0].v = texTop;

	vertices[1].u = texLeft;
	vertices[1].v = texBottom;

	vertices[2].u = texRight;
	vertices[2].v = texTop;

	vertices[3].u = texRight;
	vertices[3].v = texBottom;

	// position coords
	vertices[0].x = pos.x - 0.5f;
	vertices[0].y = correctedYPos - 0.5f;

	vertices[1].x = pos.x - 0.5f;
	vertices[1].y = correctedYPos - height - 0.5f;

	vertices[2].x = pos.x + width - 0.5f;
	vertices[2].y = correctedYPos - 0.5f;

	vertices[3].x = pos.x + width - 0.5f;
	vertices[3].y = correctedYPos - height - 0.5;

	// not exactly sure about the color format, but this seems to work
	byte a = RGBCOLGetA(color);
	byte r = RGBCOLGetR(color);
	byte g = RGBCOLGetG(color);
	byte b = RGBCOLGetB(color);

	for (int i = 0; i < 4; ++i) {
		vertices[i].r = r / 255.0f;
		vertices[i].g = g / 255.0f;
		vertices[i].b = b / 255.0f;
		vertices[i].a = a / 255.0f;
	}

	// transform vertices here if necessary, add offset

	if (alphaDisable) {
		glDisable(GL_ALPHA_TEST);
	}

	_spriteShader->use();
	_spriteShader->setUniform("alphaTest", !alphaDisable);
	_spriteShader->setUniform("projMatrix", _projectionMatrix2d);

	glBindBuffer(GL_ARRAY_BUFFER, _spriteVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(SpriteVertexShader), vertices);

	setSpriteBlendMode(blendMode);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (alphaDisable) {
		glEnable(GL_ALPHA_TEST);
	}

	return true;
}

Mesh3DS *BaseRenderOpenGL3DShader::createMesh3DS() {
	return new Mesh3DSOpenGLShader();
}

MeshX *BaseRenderOpenGL3DShader::createMeshX() {
	return new MeshXOpenGLShader(_gameRef, _modelXShader);
}

} // namespace Wintermute
