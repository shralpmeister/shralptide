//
//  ChartView.h
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

#import "ChartViewDatasource.h"
#import "SDTideInterval.h"
#import "SDTide.h"
#import "SDTideEvent.h"
#import "CursorView.h"

@interface ChartView : UIView {
	@private
		id datasource;
		NSMutableDictionary *times;
	@public
		IBOutlet UIView *cursorView;
		IBOutlet UINavigationBar *navBarView;
		IBOutlet UIActivityIndicatorView *activityIndicator;
}

-(int)currentTimeInMinutes:(SDTide*)tide;
-(void)animateCursorViewToCurrentTime;

@property (readwrite, assign) id datasource;
@property (nonatomic, retain) UIView *cursorView;
@property (nonatomic, retain) UINavigationBar *navBarView;
@property (nonatomic, retain) UIActivityIndicatorView *activityIndicator;
@property (nonatomic, retain) NSMutableDictionary *times;
@end
