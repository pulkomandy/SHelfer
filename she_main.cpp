/* SHelfer Ver. 1.2
 *
 * Purpose:	Replicant Container
 *			Rather crash this than Tracker :-)
 *
 * License:	Freeware
 *
 * (C)2004 N3S (Andreas Herzig)
 * contact me at beos@herzig-net.de
 */

#include <be/AppKit.h>
#include <be/InterfaceKit.h>
#include <FindDirectory.h>
#include <Path.h>
#include <unistd.h>

const char *app_signature = "application/N3S.shelfer";


class MyWindow : public BWindow
{
private:
	BView	*myview;
	BShelf	*myshelf;

public:
	MyWindow(BRect frame, const char* Title) : 
		BWindow(frame, Title, B_TITLED_WINDOW, B_FRAME_EVENTS)
	{	
		BRect brect = Bounds();
		
		/* this window needs a view as background.
		 * I chose a BView because it does nothing special.
		 */
		myview = new BView(brect, "view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS);
		AddChild(myview);
		myview->SetViewColor(40,80,120,255);

		/* Now the important part: the BShelf
		 * AllowZombies accepts replicants whose application cannot be found.
		 * See the Interface Kit description in the BeBook for more details
		 */		
		myshelf = new BShelf(myview);
		myshelf->SetAllowsZombies(true);
		myshelf->SetDisplaysZombies(true);
		
		/* myshelf->Save(...) can be used store replicants so that they can rebuilt on
		 * next start.
		 * I actually never tested this.
		 */
		//myshelf->Save( /*Parameters?*/ );
	}
	
	bool QuitRequested()
	{
		int		ref;
		BPath	path;		
		BRect	frame = Frame();
		
		/* Save settings to /boot/home/config/settings/shelfer
		 * stores the position and size
		 *
		 * "Inspired" by the Be Clock Application
		 */
		if (find_directory (B_USER_SETTINGS_DIRECTORY, &path, true) == B_OK) {
			path.Append("shelfer");
			ref = creat(path.Path(), 0777);
			if (ref >= 0) {
				write(ref, (char *)&frame, sizeof(frame));
				close(ref);
			}
		}
		
		/* Tell the application that we're done here.
		 */ 
		be_app->PostMessage(B_QUIT_REQUESTED);
		return(TRUE);
	}
};

class MyApplication : public BApplication
{
private:
	MyWindow *theWindow;

public:
	
	// Creates the window and sets the title with the application name. 
	MyApplication(const char *signature) :
		BApplication(signature)
	{
		BRect	windowRect;
		BPath	path;
		int		ref;

		/* Load position and size:
		 */		
		if (find_directory (B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
			path.Append("shelfer");
			ref = open(path.Path(), O_RDONLY);
			if (ref >= 0) {
				read(ref, (char *)&windowRect, sizeof(windowRect));
				close(ref);
			}
		}		
		
		/* Create a new window at the stored position:
		 */
		theWindow = new MyWindow(windowRect,"Drop ya replicants 'ere!");
		theWindow->Show();
	}
	
	virtual void MessageReceived(BMessage *msg)
	{
		BApplication::MessageReceived(msg);
	}
};


int main()
{
	new MyApplication(app_signature);
	be_app->Run();

	delete be_app;
	return 0;
}

