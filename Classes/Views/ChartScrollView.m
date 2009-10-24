//
//  ChartScrollView.m
//  ShralpTide
//
//  Created by Michael Parlee on 8/5/09.
//  Copyright 2009 IntelliDOT Corporation. All rights reserved.
//

#import "ChartScrollView.h"

@implementation ChartScrollView

- (BOOL)touchesShouldBegin:(NSSet *)touches withEvent:(UIEvent *)event inContentView:(UIView *)view
{
	// we'll send touch events through to the content view
	return YES;
}

- (BOOL)touchesShouldCancelInContentView:(UIView *)view {
	// we won't cancel touches in content view
	return NO;
}

@end
