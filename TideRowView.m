//
//  TideRowView.m
//  ShralpTide
//
//  Created by Michael Parlee on 9/20/09.
//  Copyright 2009 Michael Parlee. All rights reserved.
//

#import "TideRowView.h"


@implementation TideRowView

- (BOOL)isAccessibilityElement
{
	return YES;
}

- (NSString *)accessibilityLabel
{
	NSString *timeText = time.text;
	NSString *heightText = height.text;
	NSString *stateText = state.text;
	
	if ([timeText isEqualToString:@""]) {
		return @"";
	} else {
		return [NSString stringWithFormat:@"%@ tide of %@ at %@", stateText,heightText,timeText];
	}
}

- (UIAccessibilityTraits)accessibilityTraits
{
	return UIAccessibilityTraitStaticText;
}

- (NSString *)accessibilityHint
{
	return @"Daily tide event time and height";
}

- (void)dealloc
{
	[time release];
	[height release];
	[state release];
	[super dealloc];
}

@end
