//
//  MyViewController.h
//  Created by Seth Robinson on 3/6/09.
//  For license info, check the license.txt file that should have come with this.
//


#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@class EAGLView;

@interface MyViewController : UIViewController
{
    EAGLView*       _glView;
    UIWindow*       _window;

}

@property (nonatomic, retain) IBOutlet EAGLView* glView;
@property (nonatomic, retain) IBOutlet UIWindow* window;

@end
