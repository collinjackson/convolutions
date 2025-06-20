/*
This Tutorial shows how to do 2d graphics with the Irrlicht Engine.
It shows how to draw images, keycolor based sprites,
transparent rectangles and different fonts. You will may consider
this useful if you want to make a 2d game with the engine, or if 
you want to draw a cool interface or head up display for your 3d game.

As always, I include the header files, use the irr namespace,
and tell the linker to link with the .lib file. 
*/
#include <irrlicht.h>

using namespace irr;

#pragma comment(lib, "Irrlicht.lib")

/*
At first, we start up the engine, set a caption, and get a pointer
to the video driver.
*/
int main()
{
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

	return 0;
}
