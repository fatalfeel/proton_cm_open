
#import "MyOpenGLView.h"
#import "MainController.h"
#import "MyApplication.h"

//by stone
#import "App.h"
#import "Irrlicht/IrrlichtManager.h"

@implementation MyOpenGLView

// This is the renderer output callback function
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef      displayLink,
                               const CVTimeStamp*    now,
                               const CVTimeStamp*    outputTime,
                               CVOptionFlags         flagsIn,
                               CVOptionFlags*        flagsOut,
                               void*                 displayLinkContext)
{
    CVReturn result = [(MyOpenGLView*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (id) initWithFrame:(NSRect)frameRect
{
    self = [self initWithFrame:frameRect shareContext:nil];

	return self;
}

- (id) initWithFrame:(NSRect)frameRect shareContext:(NSOpenGLContext*)context
{
    GLint swapInt;
    
    //[[NSApplication sharedApplication] setDelegate:[NSApplication sharedApplication]];
	NSOpenGLPixelFormatAttribute attribs[] =
    {
		kCGLPFAAccelerated,
		kCGLPFANoRecovery,
        NSOpenGLPFAStencilSize, 8,
		kCGLPFADoubleBuffer,
		kCGLPFAColorSize, 24,
		kCGLPFADepthSize, 16,
		0
    };
    
    m_bViewSetting = false;
	
    pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
	
    if (!pixelFormat)
		NSLog(@"No OpenGL pixel format");
		
	
	// NSOpenGLView does not handle context sharing, so we draw to a custom NSView instead
	openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:context];
		
	
	if (self = [super initWithFrame:frameRect])
	{
		[[self openGLContext] makeCurrentContext];
		
		// Synchronize buffer swaps with vertical refresh rate
		swapInt = 1;
		[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; 
		
		//by stone, too early to call drawView
        //[self setupDisplayLink];
		
		// Look for changes in view size
		// Note, -reshape will not be called automatically on size changes because NSView does not export it to override 
		[[NSNotificationCenter defaultCenter] addObserver:self 
												 selector:@selector(reshape) 
													 name:NSViewGlobalFrameDidChangeNotification
												   object:self];

		//path names to load stuff anyway.
		CFBundleRef mainBundle      = CFBundleGetMainBundle();
		CFURLRef    resourcesURL    = CFBundleCopyResourcesDirectoryURL(mainBundle);
		char        path[PATH_MAX];
		if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
		{
			LogMsg("Error getting bundle path");
		}
		
        CFRelease(resourcesURL);
		chdir(path);
		//LogMsg( path);
	}
    
	return self;
}

- (void) dealloc
{
	m_bQuitASAP = true;
    
    // Stop and release the display link
	CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
	
	// Destroy the context
	[openGLContext release];
    openGLContext = NULL;
	[pixelFormat release];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:NSViewGlobalFrameDidChangeNotification 
													object:self];
	[super dealloc];
}	

- (void) setupDisplayLink
{
	// Create a display link capable of being used with all active displays
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	
	// Set the renderer output callback function
	CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
	
	// Set the display link for the current renderer
	CGLContextObj       cglContext =
    (_CGLContextObject*)[[self openGLContext] CGLContextObj];
	
    CGLPixelFormatObj   cglPixelFormat =
    (_CGLPixelFormatObject*)[[self pixelFormat] CGLPixelFormatObj];
	
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
}

- (void) lockFocus
{
	[super lockFocus];
	
    /*if ([[self openGLContext] view] != self)
    {
		[[self openGLContext] setView:self];
    }*/
    
    //by stone, clear define
    if ([[self openGLContext] view] != self->maincontroller.openGLView)
    {
        //this will call reshape
        [[self openGLContext] setView:self->maincontroller.openGLView];
        
        m_bViewSetting = true;
    }
}

- (CVReturn) getFrameForTime:(const CVTimeStamp*)outputTime
{
	// There is no autorelease pool when this method is called because it will be called from a background thread
	// It's important to create one or you will leak objects
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	if (openGLContext)
		[self drawView];

	[pool release];
    
    return kCVReturnSuccess;
}

- (void) drawRect:(NSRect)dirtyRect
{
	// Ignore if the display link is still running
	if (!CVDisplayLinkIsRunning(displayLink) &&  openGLContext)
		[self drawView];
}

- (void) drawView
{
    CGLLockContext( (_CGLContextObject*) [[self openGLContext] CGLContextObj]);

   	OSMessage osm;
    
	// Make sure we draw to the right context
	[[self openGLContext] makeCurrentContext];
    
    while (!BaseApp::GetBaseApp()->GetOSMessages()->empty())
	{
		osm = BaseApp::GetBaseApp()->GetOSMessages()->front();
		BaseApp::GetBaseApp()->GetOSMessages()->pop_front();
		
		//LogMsg("Got OS message %d, %s", m.m_type, m.m_string.c_str());
		[self onOSMessage: &osm];
	}
    
    if( m_bViewSetting ) //by stone, after openglview setting done
    {
        if (BaseApp::GetBaseApp()->IsInitted() && !BaseApp::GetBaseApp()->IsInBackground() && ![self inLiveResize])
        {
            BaseApp::GetBaseApp()->Update();
        }
        
        if (BaseApp::GetBaseApp()->IsInitted() && !m_bQuitASAP)
        {
            BaseApp::GetBaseApp()->Draw();
        }
    }
	
	[[self openGLContext] flushBuffer];
	CGLUnlockContext( (_CGLContextObject*) [[self openGLContext] CGLContextObj]);
}

- (void) reshape
{
	// This method will be called on the main thread when resizing
    // Add a mutex around to avoid the threads accessing the context simultaneously
	CGLLockContext( (_CGLContextObject*)[[self openGLContext] CGLContextObj]);
	
	core::dimension2d<u32>  size;
    NSRect                  bounds = [self bounds];
    
    size.Width  = bounds.size.width;
    size.Height = bounds.size.height;
	
	//glViewport(0, 0, bounds.size.width, bounds.size.height);
    IrrlichtManager::GetIrrlichtManager()->SetReSize(size);
	
	if (![self inLiveResize])
	{
		LogMsg("Reshaping: %.2f %.2f",  bounds.size.width, bounds.size.height);
		InitDeviceScreenInfoEx(bounds.size.width, bounds.size.height, ORIENTATION_LANDSCAPE_LEFT);
	}
	
	[[self openGLContext] update];
		
	CGLUnlockContext( (_CGLContextObject*)[[self openGLContext] CGLContextObj] );
}

- (void) setMainController:(MainController*)theController;
{
	maincontroller = theController;
    
    [self setupDisplayLink]; //by stone
    
    //if setting view here, value will no change
    //[[self openGLContext] setView:self->maincontroller.openGLView];
}

- (void)viewDidEndLiveResize
{
	[maincontroller OnResizeFinished];
}

- (NSOpenGLContext*) openGLContext
{
	return openGLContext;
}

- (NSOpenGLPixelFormat*) pixelFormat
{
	return pixelFormat;
}

- (void)onOSMessage:(OSMessage *)pMsg
{
	
	switch (pMsg->m_type)
	{
			
		case OSMessage::MESSAGE_OPEN_TEXT_BOX:
			break;
			
		case OSMessage::MESSAGE_CHECK_CONNECTION:
			//pretend we did it
			MessageManager::GetMessageManager()->SendGUI(MESSAGE_TYPE_OS_CONNECTION_CHECKED, (float)RT_kCFStreamEventOpenCompleted, 0.0f);	
			break;
			
		case OSMessage::MESSAGE_CLOSE_TEXT_BOX:
			break;
		case OSMessage::MESSAGE_SET_FPS_LIMIT:
			//glView.animationIntervalSave = 1.0/pMsg->m_x;
			break;
		case OSMessage::MESSAGE_SET_ACCELEROMETER_UPDATE_HZ:
			break;
		case OSMessage::MESSAGE_FINISH_APP:
		case OSMessage::MESSAGE_SUSPEND_TO_HOME_SCREEN:
		
            m_bQuitASAP = YES;
			[NSApp terminate:nil];
			break;
		case OSMessage::MESSAGE_SET_VIDEO_MODE:
		{
			NSWindow *window = [NSApp mainWindow];
			NSSize frameSize;
			frameSize.width = pMsg->m_x;
			frameSize.height = pMsg->m_y;
			[window setContentSize:frameSize];
			[window center];
			 }	
			break;
			
		default:
			LogMsg("Error, unknown message type: %d", pMsg->m_type);
	}
}

- (BOOL) acceptsFirstResponder
{
    // We want this view to be able to receive key events
    return YES;
}

- (void)mouseDown:(NSEvent *)theEvent
{

#ifdef _IRR_COMPILE_WITH_GUI_
    irr::SEvent ev;
    NSPoint     touchPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    
    ev.MouseInput.X = touchPoint.x;
    ev.MouseInput.Y = GetPrimaryGLY() - touchPoint.y;

    // event as mouse.
    ev.EventType            = irr::EET_MOUSE_INPUT_EVENT;
    ev.MouseInput.Event     = irr::EMIE_LMOUSE_PRESSED_DOWN;

    ev.MouseInput.ButtonStates = 0;
    IrrlichtManager::GetIrrlichtManager()->GetDevice()->postEventFromUser(ev);
#endif


	NSPoint pt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	pt.y = GetPrimaryGLY()-pt.y; //flip it to upper left hand coords
	
	ConvertCoordinatesIfRequired(pt.x, pt.y);
	MessageManager::GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CLICK_START,pt.x, pt.y);			

	//LogMsg("Got mouse down: %.2f, %0.2f", pt.x, pt.y);
	// [controller mouseDown:theEvent];
}

- (void)mouseUp:(NSEvent *)theEvent
{

#ifdef _IRR_COMPILE_WITH_GUI_
    irr::SEvent ev;
    NSPoint     touchPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    
    ev.MouseInput.X = touchPoint.x;
    ev.MouseInput.Y = GetPrimaryGLY() - touchPoint.y;
    
    // event as mouse.
    ev.EventType            = irr::EET_MOUSE_INPUT_EVENT;
    ev.MouseInput.Event     = irr::EMIE_LMOUSE_LEFT_UP;
    
    ev.MouseInput.ButtonStates = 0;
    IrrlichtManager::GetIrrlichtManager()->GetDevice()->postEventFromUser(ev);
#endif

    
    // Delegate to the controller object for handling mouse events
	NSPoint pt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	pt.y = GetPrimaryGLY()-pt.y; //flip it to upper left hand coords
	
	ConvertCoordinatesIfRequired(pt.x, pt.y);
	MessageManager::GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CLICK_END,pt.x, pt.y);			
	// [controller mouseDown:theEvent];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    // Delegate to the controller object for handling mouse events
	NSPoint pt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	pt.y = GetPrimaryGLY()-pt.y; //flip it to upper left hand coords
	
	ConvertCoordinatesIfRequired(pt.x, pt.y);
	MessageManager::GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CLICK_MOVE,pt.x, pt.y);			
	// [controller mouseDown:theEvent];
}

- (void)mouseMovedMessage:(NSEvent *)theEvent
{
    // Delegate to the controller object for handling mouse events
	NSPoint pt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	pt.y = GetPrimaryGLY()-pt.y; //flip it to upper left hand coords
	
	ConvertCoordinatesIfRequired(pt.x, pt.y);
	MessageManager::GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CLICK_MOVE_RAW,pt.x, pt.y);			
	// [controller mouseDown:theEvent];
}

- (void)keyDown:(NSEvent *)theEvent
{
	unichar c = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];

	MessageManager::GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR,  (float)ConvertOSXKeycodeToProtonVirtualKey(c), 0.0f);  //lParam holds a lot of random data about the press, look it up if
	
	if (theEvent.isARepeat == NO)
	{
		//if it's not a key repeat, also send here for the more arcade-like key up/down messages
		c =  [[[NSString stringWithCharacters:&c length:1] uppercaseString] characterAtIndex: 0]; //make it uppercase, same as Win

		MessageManager::GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR_RAW,   (float)ConvertOSXKeycodeToProtonVirtualKey(c), 1.0f);  
	}
}

- (void)keyUp:(NSEvent *)theEvent
{
	unichar c = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
	MessageManager::GetMessageManager()->SendGUI(MESSAGE_TYPE_GUI_CHAR_RAW,   (float)ConvertOSXKeycodeToProtonVirtualKey(c), 0.0f);  
}

- (void) startAnimation
{
	if (displayLink && !CVDisplayLinkIsRunning(displayLink))
		CVDisplayLinkStart(displayLink);
}

- (void) stopAnimation
{
	if (displayLink && CVDisplayLinkIsRunning(displayLink))
		CVDisplayLinkStop(displayLink);
}

@end
