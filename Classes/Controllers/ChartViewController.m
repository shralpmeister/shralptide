//
//  ChartViewController.m
//  ShralpTide
//
//  Created by Michael Parlee on 9/22/08.
//  Copyright 2009 Michael Parlee. All rights reserved.
/*
   This file is part of ShralpTide.

   ShralpTide is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ShralpTide is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ShralpTide.  If not, see <http://www.gnu.org/licenses/>.
*/

#import "ChartViewController.h"

@implementation ChartViewController

@synthesize sdTide;

// Override initWithNibName:bundle: to load the view using a nib file then perform additional customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil tide:(SDTide *)aTide {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
		self.sdTide = aTide;
    }
    return self;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


-(void)showCurrentTime {
	NSLog(@"ChartView showCurrentTime called.");
	[(ChartView*)self.view animateCursorViewToCurrentTime];
}

//-(void)drawChart {
//	NSLog(@"Drawing chart...");
//	ChartView *chartView = (ChartView*)self.view;
//	[chartView.activityIndicator stopAnimating];
//	[chartView.activityIndicator removeFromSuperview];
//	chartView.displayChart = YES;
//	[chartView setNeedsDisplay];
//}

#pragma mark ChartViewDatasource
-(SDTide *)tideDataToChart {
	return self.sdTide;
}

- (void)dealloc {
    [super dealloc];
	[sdTide release];
}


@end
