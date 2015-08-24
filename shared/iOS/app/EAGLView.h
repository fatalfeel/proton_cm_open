/*
 EAGLView.h
 *  Created by Seth Robinson on 3/6/09.
 *  For license info, check the license.txt file that should have come with this.
 *
 */


#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>


/*
 This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
 The view content is basically an EAGL surface you render your OpenGL scene into.
 Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
 */
enum eIosActions
{
    ACTION_DOWN,
    ACTION_UP,
    ACTION_MOVE,
    ACTION_CANCEL,
    ACTION_OUTSIDE,
};

class IosMessageCache
{
public:
    IosMessageCache()
    {}
    ~IosMessageCache()
    {}
    
    eIosActions     type;
    float			x,y;
    int				finger;
};

@interface EAGLView : UIView
{
@private
	/* The pixel dimensions of the backbuffer */
	GLint backingWidth;
	GLint backingHeight;
	
	EAGLContext *context;
	
	/* OpenGL names for the renderbuffer and framebuffers used to render to this view */
	GLuint viewRenderbuffer, viewFramebuffer;
	
	/* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
	GLuint depthRenderbuffer;
    
    GLuint m_StencilBuffer;
	
	NSTimer *animationTimer;
	NSTimeInterval animationInterval, animationIntervalSave;
}

@property NSTimeInterval animationInterval;
@property NSTimeInterval animationIntervalSave;

- (void) MouseKeyProcess:(int)method : (IosMessageCache*) amsg : (unsigned int*) qsize;
- (void) CheckTouchCommand;
- (void) drawView;
- (void) startAnimation;
- (void) stopAnimation;
- (void) destroyFramebuffer;
- (BOOL) createFramebuffer;
//- (void)onKill;

@end
