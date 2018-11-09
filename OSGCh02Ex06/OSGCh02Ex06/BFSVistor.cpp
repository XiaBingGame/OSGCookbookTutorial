#include "BFSVistor.h"
#include <osg/group>


BFSVistor::BFSVistor()
{
	// setVisitorType(TRAVERSE_ALL_CHILDREN);
	setTraversalMode(TRAVERSE_ALL_CHILDREN);
}


BFSVistor::~BFSVistor()
{
}

void BFSVistor::traverseBFS(osg::Node& node)
{
	osg::Group* group = node.asGroup();
	if (!group)
		return;

	for (int i = 0; i < group->getNumChildren(); ++i)
	{
		_pendingNodes.push_back(group->getChild(i));
	}

	while (_pendingNodes.size() > 0)
	{
		osg::Node* node = _pendingNodes.front();
		_pendingNodes.pop_front();
		node->accept(*this);
	}
}