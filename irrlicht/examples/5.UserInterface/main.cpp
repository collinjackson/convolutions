/*
This tutorial shows how to use the built in User Interface of
the Irrlicht Engine. It will give a brief overview and show
how to create and use windows, buttons, scroll bars, static 
texts and list boxes.

As always, we include the header files, and use the irrlicht
namespaces. We also store a pointer to the Irrlicht device,
a counter variable for changing the creation position of a window,
and a pointer to a listbox.
*/
#include <irrlicht.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#pragma comment(lib, "Irrlicht.lib")

IrrlichtDevice *device = 0;
s32 cnt = 0;
IGUIListBox* listbox = 0;


/*
The Event Receiver is not only capable of getting keyboard and
mouse input events, but also events of the graphical user interface 
(gui). There are events for almost everything: Button click, 
Listbox selection change, events that say that a element was hovered
and so on. To be able to react to some of these events, we create 
an event receiver. 
We only react to gui events, and if it's such an event, we get the
id of the caller (the gui element which caused the event) and get 
the pointer to the gui environment.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(SEvent event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUIEnvironment* env = device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{

			/*
			If a scrollbar changed its scroll position, and it is 'our'
			scrollbar (the one with id 104), then we change the 
			transparency of all gui elements. This is a very easy task:
			There is a skin object, in which all color settings are stored.
			We simply go through all colors stored in the skin and change
			their alpha value.
			*/
			case EGET_SCROLL_BAR_CHANGED:
				if (id == 104)
				{
					s32 pos = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
					
					for (s32 i=0; i<EGDC_COUNT ; ++i)
					{
						SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
						col.setAlpha(pos);
						env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
					}
					
				}
				break;

			/*
			If a button was clicked, it could be one of 'our'
			three buttons. If it is the first, we shut down the engine.
			If it is the second, we create a little window with some 
			text on it. We also add a string to the list box to log
			what happened. And if it is the third button, we create
			a file open dialog, and add also this as string to the list box.
			That's all for the event receiver.
			*/
			case EGET_BUTTON_CLICKED:

				if (id == 101)
				{
					device->closeDevice();
					return true;
				}

				if (id == 102)
				{
					listbox->addItem(L"Window created");
					cnt += 30;
					if (cnt > 200) 
						cnt = 0;

					IGUIWindow* window = env->addWindow(
						rect<s32>(100 + cnt, 100 + cnt, 300 + cnt, 200 + cnt), 
						false, // modal?
						L"Test window");

					env->addStaticText(L"Please close me",  
						rect<s32>(35,35,140,50),
						true, // border?
						false, // wordwrap?
						window);

					return true;
				}

				if (id == 103)
				{
					listbox->addItem(L"File open");
					env->addFileOpenDialog(L"Please choose a file.");
					return true;
				}

				break;
			}
		}

		return false;
	}
};


/*
Ok, now for the more interesting part. First, create the 
Irrlicht device, don't forget to use the event receiver and
store pointers to the driver and to the gui environment.
*/
int main()
{
	MyEventReceiver receiver;

	device = createDevice(video::EDT_DIRECTX8,
		core::dimension2d<s32>(512, 384), 16, false, false, &receiver);

	device->setWindowCaption(L"Irrlicht Engine - User Inferface Demo");

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	/*
	We add three buttons. The first closes the engine. The second
	creates a window and the third opens a file open dialog. The third
	parameter is the id of the button, with which we can easily identify
	the button in the event receiver.
	*/	

	env->addButton(rect<s32>(10,210,100,240), 0, 101, L"Quit");
	env->addButton(rect<s32>(10,250,100,290), 0, 102, L"New Window");
	env->addButton(rect<s32>(10,300,100,340), 0, 103, L"File Open");

	/*
	Now, we add a static text and a scrollbar, which modifies the
	transparency of all gui elements. We set the maximum value of
	the scrollbar to 255, because that's the maximal value for 
	a color value.
	Then we create an other static text and a list box.
	*/

	env->addStaticText(L"Transparent Control:", rect<s32>(150,20,350,40), true);
	IGUIScrollBar* scrollbar = env->addScrollBar(true, rect<s32>(150, 45, 350, 60), 0, 104);
	scrollbar->setMax(255);

	env->addStaticText(L"Logging ListBox:", rect<s32>(50,80,250,100), true);
	listbox = env->addListBox(rect<s32>(50, 110, 250, 180));

	/*
	To make the font a little bit nicer, we load an external font
	and set it as new font in the skin. An at last, we create a 
	nice Irrlicht Engine logo in the top left corner.
	*/

	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("../../media/fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);


	IGUIImage* img = env->addImage(rect<int>(10,10,98,41));
	img->setImage(driver->getTexture("../../media/irrlichtlogoaligned.jpg"));

	/*
	That's all, we only have to draw everything.
	*/

	while(device->run() && driver)
	if (device->isWindowActive())
	{
		driver->beginScene(true, true, SColor(0,122,65,171));

		env->drawAll();
	
		driver->endScene();
	}

	device->drop();

	return 0;
}
