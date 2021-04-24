#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osgGA/TrackballManipulator>

int main(int argc, char** argv)
{
    // Root node of the scene
    osg::ref_ptr<osg::Group> root = new osg::Group;

    // Create the viewer
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);
    viewer.realize();

    // Add axesNode under root
    osg::ref_ptr<osg::Node> axesNode = osgDB::readNodeFile("test.stl");
    if (!axesNode)
    {
        printf("Origin node not loaded, model not found\n");
        return 1;
    }
    root->addChild(axesNode);

    // Attach a manipulator (it's usually done for us when we use viewer.run())
    osg::ref_ptr<osgGA::TrackballManipulator> tm = new osgGA::TrackballManipulator;
    viewer.setCameraManipulator(tm);

    return viewer.run();
}
