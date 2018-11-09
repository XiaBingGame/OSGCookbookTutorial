#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>

#include <osgUtil/Optimizer>
#include <osgUtil/PrintVisitor>

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
#include "BFSVistor.h"

class BFSPrintVisitor : public BFSVistor
{
public:
	virtual void apply(osg::Node& node) {
		std::cout << node.libraryName() << "::" << node.className() << std::endl;

		traverseBFS(node);
	}
};

int main(int argc, char** argv)
{
	osg::ArgumentParser arguments(&argc, argv);
	osg::ref_ptr<osg::Node> root = osgDB::readNodeFiles(arguments);

	if (!root)
		root = osgDB::readNodeFile("osgcool.osgt");

	std::cout << "DFS Visitor traversal: " << std::endl;
	osgUtil::PrintVisitor pv(std::cout);
	root->accept(pv);
	std::cout << std::endl;


	std::cout << "BFS Visitor traversal: " << std::endl;
	BFSPrintVisitor bpv;
	root->accept(bpv);

	system("pause");
	return 0;
}