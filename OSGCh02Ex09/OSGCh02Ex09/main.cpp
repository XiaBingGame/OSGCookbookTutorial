#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>

#include <osgUtil/Optimizer>

#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>

#include <osgSim/OverlayNode>

#include <osgText/Font>
#include <osgText/Text>

#include <osgViewer/Viewer>
#include <iostream>

#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <osg/Camera>

class CgDrawCallback : public osg::Camera::DrawCallback
{
public:
	void addProfile(CGprofile profile) {
		_profiles.push_back(profile);
	}
	void addCompiledProgram(CGprogram prog) {
		_programs.push_back(prog);
	}
protected:
	std::vector<CGprofile> _profiles;
	std::vector<CGprogram> _programs;
};

class CgStartDrawCallback : public CgDrawCallback
{
public:
	CgStartDrawCallback() : _initialized(false) {}
	virtual void operator()(osg::RenderInfo&
		renderInfo) const;
protected:
	mutable bool _initialized;
};
class CgEndDrawCallback : public CgDrawCallback
{
public:
	virtual void operator()(osg::RenderInfo&
		renderInfo) const;
};

void CgStartDrawCallback::operator()(osg::RenderInfo&
	renderInfo) const
{
	if (!_initialized)
	{
		// Load all Cg shader programs
		for (unsigned int i = 0; i < _programs.size(); ++i)
			cgGLLoadProgram(_programs[i]);
		_initialized = true;
	}
	// Bind the programs to current graphics context
	for (unsigned int i = 0; i < _programs.size(); ++i)
		cgGLBindProgram(_programs[i]);
	// Enable Cg profiles to work under specified devices
	for (unsigned int i = 0; i < _profiles.size(); ++i)
		cgGLEnableProfile(_profiles[i]);
}
void CgEndDrawCallback::operator()(osg::RenderInfo&
	renderInfo) const
{
	// Disable profiles after the drawing
	for (unsigned int i = 0; i < _profiles.size(); ++i)
		cgGLDisableProfile(_profiles[i]);
}

static const char* cgProgramCode = {
	"struct app_input {\n"
	"float4 vertex : POSITION;\n"
	"float4 normal : NORMAL;\n"
	"};\n"
	"struct vertex_to_fragment {\n"
	"float4 position : POSITION;\n"
	"float3 normal3  : TEXCOORD0;\n"
	"};\n"
	"vertex_to_fragment vertex_main(app_input input)\n"
	"{\n"
	"vertex_to_fragment output;\n"
	"output.position = mul(glstate.matrix.mvp, input.vertex); \n"
	"output.normal3 = input.normal.xyz;\n"
	"return output;\n"
	"}\n"
	"float4 fragment_main(vertex_to_fragment input) : COLOR\n"
	"{\n"
	"float4 output = float4(input.normal3.x, input.normal3.y, input.normal3.z, 1.0); \n"
	"return output;\n"
	"}\n"
};

CGcontext g_context;
void error_callback()
{
	CGerror lastError = static_cast<CGerror>(cgGetError());
	OSG_WARN << "Cg error: " << cgGetErrorString(lastError)
		<< std::endl;
	if (lastError == CG_COMPILER_ERROR)
		OSG_WARN << std::string(cgGetLastListing(g_context))
		<< std::endl;
}

int main(int argc, char** argv)
{
	osg::ArgumentParser arguments(&argc, argv);
	osg::ref_ptr<osg::Node> root = osgDB::readNodeFiles(
		arguments);
	if (!root) root = osgDB::readNodeFile("cow.osg");
	osg::ref_ptr<CgStartDrawCallback> preCB =
		new CgStartDrawCallback;
	osg::ref_ptr<CgEndDrawCallback> postCB =
		new CgEndDrawCallback;

	osgViewer::Viewer viewer;
	viewer.getCamera()->setPreDrawCallback(preCB.get());
	viewer.getCamera()->setPostDrawCallback(postCB.get());
	viewer.setSceneData(root.get());
	viewer.setUpViewInWindow(100, 100, 800, 600);
	
	CGprofile vertProfile, fragProfile;
	CGprogram vertProg = 0, fragProg = 0;
	osg::GraphicsContext* gc =
		viewer.getCamera()->getGraphicsContext();
	if (gc)
	{
		gc->realize();
		gc->makeCurrent();
		g_context = cgCreateContext();
		cgSetErrorCallback(error_callback);
		vertProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
		vertProg = cgCreateProgram(
			g_context, CG_SOURCE, cgProgramCode, vertProfile,
			"vertex_main", NULL);
		fragProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
		fragProg = cgCreateProgram(
			g_context, CG_SOURCE, cgProgramCode, fragProfile,
			"fragment_main", NULL);
		gc->releaseContext();

		preCB->addProfile(vertProfile);
		preCB->addProfile(fragProfile);
		preCB->addCompiledProgram(vertProg);
		preCB->addCompiledProgram(fragProg);
		postCB->addProfile(vertProfile);
		postCB->addProfile(fragProfile);
		postCB->addCompiledProgram(vertProg);
		postCB->addCompiledProgram(fragProg);
		viewer.run();
	}

	if (gc)
	{
		cgDestroyProgram(vertProg);
		cgDestroyProgram(fragProg);
		cgDestroyContext(g_context);
	}
	
	return 0;
}