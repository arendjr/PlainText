//
//  AppDelegate.h
//  MUD
//
//  Created by Arend van Beelen on 6/7/12.
//  Copyright __MyCompanyName__ 2012. All rights reserved.
//

#import <UIKit/UIKit.h>

#ifdef CORDOVA_FRAMEWORK
    #import <Cordova/CDVViewController.h>
#else
    #import "CDVViewController.h"
#endif


@interface AppDelegate : NSObject <UIApplicationDelegate> {

}

@property (nonatomic, strong) IBOutlet UIWindow *window;
@property (nonatomic, strong) IBOutlet CDVViewController *viewController;

@end

