//
//  CurrentTidesVoiceOver.m
//  ShralpTide
//
//  Created by Michael Parlee on 9/19/09.
//  Copyright 2009 Michael Parlee. All rights reserved.
//

#import "CurrentTideView.h"


@implementation CurrentTideView

- (BOOL)isAccessibilityElement
{
	return YES;
}

- (NSString *)accessibilityLabel
{
	NSString *locationText = location.text;
	NSString *heightText = height.text;
	NSString *directionText = directionImgView.accessibilityLabel;
	
	if (nil == directionImgView.image) {
		return [NSString stringWithFormat:@"%@, %@", locationText, date.text];
	} else {
		return [NSString stringWithFormat:@"%@, %@, tide level is currently %@ and %@", locationText, date.text, heightText, directionText];
	}
}

- (UIAccessibilityTraits)accessibilityTraits
{
	return UIAccessibilityTraitStaticText;
}

- (NSString *)accessibilityHint
{
	return @"Current tide conditions for selected location.";
}

- (BOOL)canBecomeFirstResponder
{
	return YES;
}

- (void)dealloc
{
	[location release];
	[date release];
	[height release];
	[date release];
	[super dealloc];
}

@end
