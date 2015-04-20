#import "MainController.h"
#import "MyOpenGLView.h"
#import "MyApplication.h"

@implementation MainController

@synthesize openGLView = _glView;

/*- (IBAction) goFullScreen:(id)sender
{
	isInFullScreenMode = YES;
	
	
	// Pause the non-fullscreen view
	[self.openGLView stopAnimation];
	
	NSRect mainDisplayRect, viewRect;
	mainDisplayRect = [[NSScreen mainScreen] frame];
	// Mac OS X 10.6 and later offer a simplified mechanism to create full-screen contexts
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
	
	
	
	// Create a screen-sized window on the display you want to take over
	// Note, mainDisplayRect has a non-zero origin if the key window is on a secondary display
	
	fullScreenWindow = [[NSWindow alloc] initWithContentRect:mainDisplayRect styleMask:NSBorderlessWindowMask 
													 backing:NSBackingStoreBuffered defer:YES];
	
	// Set the window level to be above the menu bar
	[fullScreenWindow setLevel:NSMainMenuWindowLevel+1];
	
	// Perform any other window configuration you desire
	[fullScreenWindow setOpaque:YES];
	[fullScreenWindow setHidesOnDeactivate:YES];
	
	// Create a view with a double-buffered OpenGL context and attach it to the window
	// By specifying the non-fullscreen context as the shareContext, we automatically inherit the OpenGL objects (textures, etc) it has defined
	viewRect = NSMakeRect(0.0, 0.0, mainDisplayRect.size.width, mainDisplayRect.size.height);
	fullScreenView = [[MyOpenGLView alloc] initWithFrame:viewRect shareContext:[self.openGLView openGLContext]];
	[fullScreenWindow setContentView:fullScreenView];
	
	// Show the window
	[fullScreenWindow makeKeyAndOrderFront:self];
	
	// Set the scene with the full-screen viewport and viewing transformation
	//[scene setViewportRect:viewRect];
	
	// Assign the view's MainController to self
	[fullScreenView setMainController:self];
	
	if (!isAnimating) {
		// Mark the view as needing drawing to initalize its contents
		[fullScreenView setNeedsDisplay:YES];
	}
	else {
		// Start playing the animation
		[fullScreenView startAnimation];
	}
	InitDeviceScreenInfoEx(mainDisplayRect.size.width, mainDisplayRect.size.height, ORIENTATION_LANDSCAPE_LEFT);

#else
	// Mac OS X 10.5 and eariler require additional work to capture the display and set up a special context
	// This demo uses CGL for full-screen rendering on pre-10.6 systems. You may also use NSOpenGL to achieve this.
	
	CGLPixelFormatObj pixelFormatObj;
	GLint numPixelFormats;
	
	// Capture the main display
	CGDisplayCapture(kCGDirectMainDisplay);
	
	// Set up an array of attributes
	CGLPixelFormatAttribute attribs[] = {
		
		// The full-screen attribute
		kCGLPFAFullScreen,
		
		// The display mask associated with the captured display
		// We may be on a multi-display system (and each screen may be driven by a different renderer), so we need to specify which screen we want to take over. For this demo, we'll specify the main screen.
		kCGLPFADisplayMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
		
		// Attributes common to full-screen and non-fullscreen
		kCGLPFAAccelerated,
		kCGLPFANoRecovery,
		kCGLPFADoubleBuffer,
		kCGLPFAColorSize, 24,
		kCGLPFADepthSize, 16,
        0
    };
	
	// Create the full-screen context with the attributes listed above
	// By specifying the non-fullscreen context as the shareContext, we automatically inherit the OpenGL objects (textures, etc) it has defined
	CGLChoosePixelFormat(attribs, &pixelFormatObj, &numPixelFormats);
	CGLCreateContext(pixelFormatObj, (_CGLContextObject*)[[self.openGLView openGLContext] CGLContextObj], &fullScreenContextObj);
	CGLDestroyPixelFormat(pixelFormatObj);
	
	if (!fullScreenContextObj) {
        NSLog(@"Failed to create full-screen context");
		CGReleaseAllDisplays();
		[self goWindow];
        return;
    }
	
	// Set the current context to the one to use for full-screen drawing
	CGLSetCurrentContext(fullScreenContextObj);
	
	// Attach a full-screen drawable object to the current context
	CGLSetFullScreen(fullScreenContextObj);
	
    // Lock us to the display's refresh rate
    GLint newSwapInterval = 1;
    CGLSetParameter(fullScreenContextObj, kCGLCPSwapInterval, &newSwapInterval);
	
	// Tell the scene the dimensions of the area it's going to render to, so it can set up an appropriate viewport and viewing transformation
//    [scene setViewportRect:NSMakeRect(0, 0, CGDisplayPixelsWide(kCGDirectMainDisplay), CGDisplayPixelsHigh(kCGDirectMainDisplay))];
	
	// Perform the application's main loop until exiting full-screen
	// The shift here is from a model in which we passively receive events handed to us by the AppKit (in window mode)
	// to one in which we are actively driving event processing (in full-screen mode)
	InitDeviceScreenInfoEx(mainDisplayRect.size.width, mainDisplayRect.size.height, ORIENTATION_LANDSCAPE_LEFT);

	while (isInFullScreenMode)
	{
	
		
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		
		// Check for and process input events
        NSEvent *event;
        while (event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES])
		{
            switch ([event type])
			{
                case NSLeftMouseDown:
                    [self mouseDown:event];
                    break;
					
                case NSLeftMouseUp:
                    [self mouseUp:event];
                    break;
					
                case NSLeftMouseDragged:
                    [self mouseDragged:event];
                    break;
					
                case NSKeyDown:
                    [self keyDown:event];
                    break;
					
                default:
                    break;
            }
        }
		
		// Update our animation
        CFAbsoluteTime currentTime = CFAbsoluteTimeGetCurrent();

        renderTime = currentTime;
		
		// Delegate to the scene object for rendering
		CGLFlushDrawable(fullScreenContextObj);
		
		[pool release];
	}
	
#endif
	 
}*/

- (void) goWindow
{
	isInFullScreenMode = NO;
	
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
	
	// Release the screen-sized window and view
	[fullScreenWindow release];
	[fullScreenView release];
	
#else
	
	// Set the current context to NULL
	CGLSetCurrentContext(NULL);
	// Clear the drawable object
	CGLClearDrawable(fullScreenContextObj);
	// Destroy the rendering context
	CGLDestroyContext(fullScreenContextObj);
	// Release the displays
	CGReleaseAllDisplays();
	
#endif
	
	// Switch to the non-fullscreen context
	[[self.openGLView openGLContext] makeCurrentContext];
	
	if (!isAnimating) {
		// Mark the view as needing drawing
		// The animation has advanced while we were in full-screen mode, so its current contents are stale
		[self.openGLView setNeedsDisplay:YES];
	}
	else {
		// Continue playing the animation
		[self.openGLView startAnimation];
	}
}

- (void) awakeFromNib
{
	// Allocate the scene object
	
	// Assign the view's MainController to self
	[self.openGLView setMainController:self];
	
	// Activate the display link now
	[self.openGLView startAnimation];
	isAnimating = YES;
	
}

- (void) dealloc
{

	[super dealloc];
}


- (CFAbsoluteTime) renderTime
{
	return renderTime;
}

- (void) setRenderTime:(CFAbsoluteTime)time
{
	renderTime = time;
}

- (void) startAnimation
{
	if (!isAnimating)
	{
		if (!isInFullScreenMode)
			[self.openGLView startAnimation];
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
		else
			[fullScreenView startAnimation];
#endif
		isAnimating = YES;
	}
}

- (void) stopAnimation
{
	if (isAnimating)
	{
		if (!isInFullScreenMode)
			[self.openGLView stopAnimation];
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
		else
			[fullScreenView stopAnimation];
#endif
		isAnimating = NO;
	}
}

- (void) toggleAnimation
{
	if (isAnimating)
		[self stopAnimation];
	else
		[self startAnimation];
}

- (NSSize) computeFrameSize: (NSSize) frameSize
{
	float aspect_r=(float)GetPrimaryGLX()/(float)GetPrimaryGLY(); // aspect ratio
	aspect_r = 1.333333; //TODO this shouldn't be hardcoded, but I need to as the Zoom button screws up the aspect ratio
	NSWindow *window = [NSApp mainWindow];
	float addX = 0;
	float addY = [window frame].size.height - [[window contentView] frame].size.height;
	
	//LogMsg("Shift down is %d", int([NSApp isShiftDown]));
	if ( 
		(GetForceAspectRatioWhenResizing() && ![NSApp isShiftDown])
		||
		(!GetForceAspectRatioWhenResizing() && [NSApp isShiftDown] )
		)
	{
		float oldHeight = frameSize.height;
		frameSize.height = ( (frameSize.width-addX) /aspect_r)+addY;
		
		LogMsg("Forcing aspect ratio %.2f offsetY: %.2f:  %.2f %.2f to %.2f %.2f", aspect_r, addY, frameSize.width, oldHeight, frameSize.width, frameSize.height);	
	} else
	{
		
		//let them change it to whatever they want
	}

	return frameSize;
}

- (NSSize)windowWillResize:(NSWindow*)sender
                    toSize:(NSSize)frameSize
{
	
	
	frameSize = [self computeFrameSize: frameSize];
	
	
	return frameSize;	
}

- (void) OnResizeFinished
{
		NSRect bounds = [self.openGLView bounds];
		LogMsg("Finished resizing");
		CGLLockContext( (_CGLContextObject*)[[self.openGLView openGLContext] CGLContextObj]);
		InitDeviceScreenInfoEx(bounds.size.width, bounds.size.height, ORIENTATION_LANDSCAPE_LEFT);
		CGLUnlockContext( (_CGLContextObject*) [[self.openGLView openGLContext] CGLContextObj]);
	
}

- (void) windowDidResize: (NSNotification *) aNotification
{
    NSLog (@"windowDidResize: called");
	
    //by stone, MyOpenGLView will auto reshape
    //[self.openGLView reshape];
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
	CGLLockContext( (_CGLContextObject*)[[self.openGLView openGLContext] CGLContextObj]);
	BaseApp::GetBaseApp()->OnEnterBackground();
	CGLUnlockContext( (_CGLContextObject*) [[self.openGLView openGLContext] CGLContextObj]);
	
}

- (BOOL)windowShouldZoom:(NSWindow *)window toFrame:(NSRect)newFrame
{
	
	LogMsg("Window should zoom");	
	return TRUE;
}

- (NSRect)windowWillUseStandardFrame:(NSWindow *)window defaultFrame:(NSRect)newFrame
{
	bool bZoomed = false; // [window isZoomed];
	LogMsg("Window will use standard frame: %.2f %.2f is zoomed is %d", newFrame.size.width, newFrame.size.height, int(bZoomed));	
	newFrame.size = [self computeFrameSize: newFrame.size];

	//this is wrong, temporary
	//newFrame.size.width = 1024;
	//newFrame.size.height = 768;
	
	return newFrame;
}


- (void) applicationDidResignActive: (NSNotification *) aNotification
{
	CGLLockContext( (_CGLContextObject*)[[self.openGLView openGLContext] CGLContextObj]);
	BaseApp::GetBaseApp()->OnEnterBackground();
	CGLUnlockContext( (_CGLContextObject*) [[self.openGLView openGLContext] CGLContextObj]);
	
}

- (void) applicationDidBecomeActive: (NSNotification *) aNotification
{
	CGLLockContext( (_CGLContextObject*)[[self.openGLView openGLContext] CGLContextObj]);
	BaseApp::GetBaseApp()->OnEnterForeground();
	CGLUnlockContext( (_CGLContextObject*) [[self.openGLView openGLContext] CGLContextObj]);

}
- (void)windowDidDeminiaturize:(NSNotification *)notification
{
	CGLLockContext( (_CGLContextObject*)[[self.openGLView openGLContext] CGLContextObj]);
	BaseApp::GetBaseApp()->OnEnterForeground();
	CGLUnlockContext( (_CGLContextObject*) [[self.openGLView openGLContext] CGLContextObj]);
	
}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	NSLog(@"Last window closed MainController");
	return YES;
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	LogMsg("App terminating");	
	
	//wait for render thread to complete
	
	CGLLockContext( (_CGLContextObject*)[[self.openGLView openGLContext] CGLContextObj]);
	
	if (BaseApp::GetBaseApp()->IsInitted())
	{
		BaseApp::GetBaseApp()->OnEnterBackground();
		//BaseApp::GetBaseApp()->Kill();
	}
    self.openGLView->m_bQuitASAP = true;
	
	CGLUnlockContext( (_CGLContextObject*) [[self.openGLView openGLContext] CGLContextObj]);
	
}
@end
