/*
This Tutorial shows how to load a Quake 3 map into the
engine, create a SceneNode for optimizing the speed of
rendering and how to create a user controlled camera.

Lets start like the HelloWorld example: We include
the irrlicht header files.
*/
#include <irrlicht.h>
#include "ISkyBoxFaderSceneNode.h"

using namespace irr;

#pragma comment(lib, "Irrlicht.lib")


/*
As already written in the HelloWorld example, in the Irrlicht
Engine, everything can be found in the namespace 'irr'. 
To get rid of the irr:: in front of the name of every class,
we tell the compiler that we use that namespace from now on, 
and we will not have to write that 'irr::'.
There are 5 other sub namespaces 'core', 'scene', 'video',
'io' and 'gui'. Unlike in the HelloWorld example,
we do not a 'using namespace' for these 5 other namespaces
because in this way you will see what can be found in which
namespace. But if you like, you can also include the namespaces
like in the previous example. Code just like you want to.
*/
using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


/*
Again, to be able to use the Irrlicht.DLL file, we need to link with the 
Irrlicht.lib. We could set this option in the project settings, but
to make it easy, we use a pragma comment lib:
*/
#pragma comment(lib, "Irrlicht.lib")

/*
Ok, lets start. Again, we use the main() method as start, not the
WinMain(), because its shorter to write.
*/

void map2d();
void map3d();

int main()
{

	map3d();
//	map2d();
	return 0;
}

void map3d(){

	/*
	Like in the HelloWorld example, we create an IrrlichtDevice with
	createDevice(). The difference now is that we use the DirectX8 driver
	because the Software device would be too slow to draw a huge Quake 3 map.
	If you do not have DirectX8 installed or working on your pc, you could
	try some other drivers by replacing EDT_DIRECTX8 with EDT_OPENGL or
	EDT_DIRECTX9.
	*/
 
	IrrlichtDevice *device =
		createDevice(video::EDT_DIRECTX8, core::dimension2d<s32>(640, 480));

	/*
	Get a pointer to the video driver and the SceneManager so that
	we do not always have to write device->getVideoDriver() and
	device->getSceneManager().
	*/
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	/*
	To display the Quake 3 map, we first need to load it. Quake 3 maps
	are packed into .pk3 files wich are nothing other than .zip files.
	So we add the .pk3 file to our FileSystem. After it was added,
	we are able to read from the files in that archive as they would
	directly be stored on disk.
	*/
	device->getFileSystem()->addZipFileArchive("../../media/demo_map.pk3");

	

	/* 
	Now we can load the mesh by calling getMesh(). We get a pointer returned
	to a IAnimatedMesh. As you know, Quake 3 maps are not really animated,
	they are only a huge chunk of static geometry with some materials
	attached. Hence the IAnimated mesh consists of only one frame,
	so we get the "first frame" of the "animation", which is our quake level
	and create an OctTree scene node with it, using addOctTreeSceneNode().
	The OctTree optimizes the scene a little bit, trying to draw only geometry
	which is currently visible. An alternative to the OctTree would be a 
	AnimatedMeshSceneNode, which would draw always the complete geometry of 
	the mesh, without optimization. Try it out: Write addAnimatedMeshSceneNode
	instead of addOctTreeSceneNode and compare the primitives drawed by the
	video driver. (There is a getPrimitiveCountDrawed() method in the 
	IVideoDriver class). Note that this optimization with the Octree is only
	useful when drawing huge meshes consiting of lots of geometry.
	*/
	scene::IAnimatedMesh* mesh = smgr->getMesh("demo_map.bsp");
	scene::ISceneNode* node = 0;
	
	if (mesh)
		node = smgr->addOctTreeSceneNode(mesh->getMesh(0));
	
	smgr->addLightSceneNode(0, core::vector3df(10,10,10), 
		video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 100000.0f);

	/*
	Because the level was modelled not around the origin (0,0,0), we translate
	the whole level a little bit.
	*/
	if (node) {
		node->setMaterialFlag(video::EMF_LIGHTING, false);
		node->setPosition(core::vector3df(-1300,-144,-1249));
	}

	scene::ISceneNode* skyboxNode = smgr->addSkyBoxSceneNode(
		driver->getTexture("..\\..\\media\\skybox\\citysunset_up.bmp"),
		driver->getTexture("..\\..\\media\\skybox\\citysunset_dn.bmp"),
		driver->getTexture("..\\..\\media\\skybox\\citysunset_lf.bmp"),
		driver->getTexture("..\\..\\media\\skybox\\citysunset_rt.bmp"),
		driver->getTexture("..\\..\\media\\skybox\\citysunset_ft.bmp"),
		driver->getTexture("..\\..\\media\\skybox\\citysunset_bk.bmp"));

	scene::ISkyBoxFaderSceneNode* skyboxFader = smgr->addSkyBoxFaderSceneNode();
	skyboxFader->setColor(video::SColor(255,255,255,255));
	skyboxFader->fadeOut((u32)0);

	/*
	Now we only need a Camera to look at the Quake 3 map.
	And we want to create a user controlled camera. There are some
	different cameras available in the Irrlicht engine. For example the 
	Maya Camera which can be controlled compareable to the camera in Maya:
	Rotate with left mouse button pressed, Zoom with both buttons pressed,
	translate with right mouse button pressed. This could be created with
	addCameraSceneNodeMaya(). But for this example, we want to create a 
	camera which behaves like the ones in first person shooter games (FPS).
	*/
	core::vector3df startPos(0,0,-10000), endPos(-1000,0,0);
	u32 startAnimTime = 2000;
	scene::ICameraSceneNode* cameraNode = smgr->addCameraSceneNode(0, startPos, core::vector3df(-1*startPos.X,-1*startPos.Y,-1*startPos.Z));
	scene::ICameraSceneNode* fpsCamera = 0;
	scene::ISceneNodeAnimator* anim = smgr->createFlyStraightAnimator(startPos, endPos, startAnimTime, false);
	cameraNode->addAnimator(anim);
	anim->drop();
	s32 startTime = device->getTimer()->getTime();
	/*
	

	The mouse cursor needs not to be visible, so we make it invisible.
	*/

	device->getCursorControl()->setVisible(false);

	/*
	We have done everything, so lets draw it. We also write the current
	frames per second and the drawn primitives to the caption of the
	window.
	*/
	int lastFPS = -1;

	int skyboxAlpha = 0;

	while(device->run())
	{

		if(!fpsCamera && device->getTimer()->getTime() - startTime > startAnimTime)
		{
			fpsCamera = smgr->addCameraSceneNodeFPS();
			fpsCamera->setPosition(cameraNode->getPosition());
			fpsCamera->setTarget(cameraNode->getTarget());
			cameraNode->remove();
			skyboxFader->fadeIn((u32)1200);
		}

		
		driver->beginScene(true, true, video::SColor(0,100,100,100));
		smgr->drawAll();
		driver->endScene();

		int fps = driver->getFPS();

		if (lastFPS != fps)
		{
			wchar_t tmp[1024];
			swprintf(tmp, 1024, L"Quake 3 Map Example - Irrlicht Engine (fps:%d) Triangles:%d", 
				fps, driver->getPrimitiveCountDrawn());

			device->setWindowCaption(tmp);
			lastFPS = fps;
		}
	}

	/*
	In the end, delete the Irrlicht device.
	*/
	device->drop();
	
}


void map2d(){
	IrrlichtDevice *device = createDevice(video::EDT_DIRECTX8,
		core::dimension2d<s32>(512, 384));

	device->setWindowCaption(L"Irrlicht Engine - 2D Graphics Demo");

	video::IVideoDriver* driver = device->getVideoDriver();

/*
	All 2d graphics in this example are put together into one texture,
	2ddemo.bmp. Because we want to draw colorkey based sprites, we need 
	to load this texture and tell the engine, which
	part of it should be transparent based on a colorkey. In this example,
	we don't tell it the color directly, we just say "Hey Irrlicht Engine, 
	you'll find the color I want at position (0,0) on the texture.".
	Instead, it would be also possible to call 
	driver->makeColorKeyTexture(images, video::SColor(0,0,0,0)), to make
	e.g. all black pixels transparent. Please note, that makeColorKeyTexture
	just creates an alpha channel based on the color. 
*/
	video::ITexture* images = driver->getTexture("../../media/2ddemo.bmp");
	driver->makeColorKeyTexture(images, core::position2d<s32>(0,0));

	
/*
	To be able to draw some text with two different fonts, we load them.
	Ok, we load just one, as first font we just use the default font which is
	built into the engine.
	Also, we define two rectangles, which specify the position of the
	images of the red imps (little flying creatures) in the texture.
*/
	gui::IGUIFont* font = device->getGUIEnvironment()->getBuiltInFont();
	gui::IGUIFont* font2 = device->getGUIEnvironment()->getFont("../../media/fonthaettenschweiler.bmp");

	core::rect<s32> imp1(349,15,385,78);
	core::rect<s32> imp2(387,15,423,78);


/*
	Everything is prepared, now we can draw everything in the draw loop,
	between the begin scene and end scene calls. In this example, we 
	are just doing 2d graphics, but it would be no problem to mix them
	with 3d graphics. Just try it out, and draw some 3d vertices or set
	up a scene with the scene manager and draw it.
*/
	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			u32 time = device->getTimer()->getTime();

			driver->beginScene(true, true, video::SColor(0,122,65,171));

			/*
			First, we draw 3 sprites, using the alpha channel we created with
			makeColorKeyTexture. The last parameter specifiys that the drawing
			method should use thiw alpha channel. The parameter before the last
			one specifies a color, with wich the sprite should be colored.
			(255,255,255,255) is full white, so the sprite will look like the 
			original. The third sprite is drawed colored based on the time.
			*/
	
			// draw fire & dragons background world
			driver->draw2DImage(images, core::position2d<s32>(50,50),
				core::rect<s32>(0,0,342,224), 0, 
				video::SColor(255,255,255,255), true);

			// draw flying imp 
			driver->draw2DImage(images, core::position2d<s32>(164,125),
				(time/500 % 2) ? imp1 : imp2, 0, 
				video::SColor(255,255,255,255), true);

			// draw second flying imp with colorcylce
			driver->draw2DImage(images, core::position2d<s32>(270,105),
				(time/500 % 2) ? imp1 : imp2, 0, 
				video::SColor(255,(time) % 255,255,255), true);

			/*
			Drawing text is really simple. The code should be self explanatory.
			*/

			// draw some text
			if (font)
			font->draw(L"This demo shows that Irrlicht is also capable of drawing 2D graphics.", 
				core::rect<s32>(130,10,300,50),
				video::SColor(255,255,255,255));

			// draw some other text
			if (font2)
			font2->draw(L"Also mixing with 3d graphics is possible.", 
				core::rect<s32>(130,20,300,60),
				video::SColor(255,time % 255,time % 255,255));

			/*
			At last, we draw the Irrlicht Engine logo (without using a color or
			an alpha channel) and a transparent 2d Rectangle at the position of 
			the mouse cursor.
			*/

			// draw logo
			driver->draw2DImage(images, core::position2d<s32>(10,10),
				core::rect<s32>(354,87,442,118));

			// draw transparent rect under cursor
			core::position2d<s32> m = device->getCursorControl()->getPosition();
			driver->draw2DRectangle(video::SColor(100,255,255,255),
				core::rect<s32>(m.X-20, m.Y-20, m.X+20, m.Y+20));

			driver->endScene();
		}
	}

	/*
	That's all, it was not really difficult, I hope.
	*/

	device->drop();

	

}