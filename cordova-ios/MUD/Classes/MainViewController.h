//
//  MainViewController.h
//  MUD
//
//  Created by Arend van Beelen on 6/7/12.
//  Copyright __MyCompanyName__ 2012. All rights reserved.
//

#ifdef CORDOVA_FRAMEWORK
    #import <Cordova/CDVViewController.h>
#else
    #import "CDVViewController.h"
#endif

@interface MainViewController : CDVViewController

- (void) removeBar;

@end
