#include "Common.h"
#include <osgText/Font>
#include <osgText/Text>
#include <osg/PolygonMode>

namespace osgCookBook
{
	osg::ref_ptr<osgText::Font> g_font = osgText::readFontFile("fonts/arial.ttf");
	
	osg::Camera* createHUDCamera(double left, double right, double bottom, double top)
	{
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		// 参考帧为绝对坐标系
		camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		camera->setClearMask(GL_DEPTH_BUFFER_BIT);
		// 渲染至纹理设置 PRE_RENDER, HUD 则用 POST_RENDER
		camera->setRenderOrder(osg::Camera::POST_RENDER);
		// 该相机关联的窗口是否允许生成事件
		camera->setAllowEventFocus(false);
		camera->setProjectionMatrix(
			osg::Matrix::ortho2D(left, right, bottom, top));
		// 禁止光照
		camera->getOrCreateStateSet()->setMode(
			GL_LIGHTING, osg::StateAttribute::OFF);
	
		// 减少计数, 由其他部分控制该指针
		return camera.release();
	}
	
	osgText::Text* createText(const osg::Vec3& pos, const std::string& content, float size)
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setDataVariance(osg::Object::DYNAMIC);
		text->setFont(g_font.get());
		text->setCharacterSize(size);
		text->setAxisAlignment(osgText::TextBase::XY_PLANE);
		text->setPosition(pos);
		text->setText(content);
		return text.release();
	}

	osg::Camera* createRTTCamera(osg::Camera::BufferComponent buffer, osg::Texture* tex, bool isAbsolute)
	{
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		camera->setClearColor(osg::Vec4());
		camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
		camera->setRenderOrder(osg::Camera::PRE_RENDER);
		if (tex)
		{
			tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
			tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
			camera->setViewport(0, 0, tex->getTextureWidth(), tex->getTextureHeight());
			camera->attach(buffer, tex);
		}

		if (isAbsolute)
		{
			camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0));
			camera->setViewMatrix(osg::Matrix::identity());
			camera->addChild(createScreenQuad(1.0f, 1.0f));
		}
		return camera.release();
	}

	osg::Geode* createScreenQuad(float width, float height, float scale)
	{
		osg::Geometry* geom = osg::createTexturedQuadGeometry(
			osg::Vec3(), osg::Vec3(width, 0.0f, 0.0f), osg::Vec3(0.0f, height, 0.0f),
			0.0f, 0.0f, width*scale, height*scale);
		osg::ref_ptr<osg::Geode> quad = new osg::Geode;
		quad->addDrawable(geom);

		int values = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED;
		quad->getOrCreateStateSet()->setAttribute(
			new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL), values);
		quad->getOrCreateStateSet()->setMode(GL_LIGHTING, values);
		return quad.release();
	}
}
