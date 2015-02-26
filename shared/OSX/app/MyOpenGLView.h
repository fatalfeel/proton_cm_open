
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <QuartzCore/CVOpenGLBuffer.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/CGLContext.h>
#import <OpenGL/CGLTypes.h>
#import <AppKit/NSOpenGL.h>
#import <AppKit/NSOpenGLView.h>
#import <AppKit/NSOpenGLLayer.h>


@class MainController;

@interface MyOpenGLView : NSView<NSApplicationDelegate>
{
@public

    MainController*         maincontroller;
    NSOpenGLContext*        openGLContext;
	NSOpenGLPixelFormat*    pixelFormat;
	CVDisplayLinkRef        displayLink;

	BOOL    isAnimating;
    BOOL    m_bQuitASAP;
    BOOL    m_bViewSetting;
}

- (id) initWithFrame:(NSRect)frameRect;
- (id) initWithFrame:(NSRect)frameRect shareContext:(NSOpenGLContext*)context;

- (NSOpenGLContext*) openGLContext;

- (void) setMainController:(MainController*)theController;

- (void) drawView;

- (void) startAnimation;
- (void) stopAnimation;
- (void) reshape;
@end
