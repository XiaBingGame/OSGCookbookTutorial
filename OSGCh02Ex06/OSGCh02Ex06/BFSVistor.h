#pragma once

#include <osg/NodeVisitor>
#include <deque>

class BFSVistor : public osg::NodeVisitor
{
public:
	BFSVistor();

	virtual void reset() { _pendingNodes.clear(); }
	virtual void apply(osg::Node& node) {
		traverseBFS(node);
	}

protected:
	virtual ~BFSVistor();
	void traverseBFS(osg::Node& node);

	std::deque<osg::Node*> _pendingNodes;
};

