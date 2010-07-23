//
//  MainViewController.h
//  ShralpTide
//
//  Created by Michael Parlee on 7/23/08.
//  Copyright Michael Parlee 2009. All rights reserved.
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

#import "SDTide.h"
#import "RootViewController.h"

@interface MainViewController : UIViewController 
{
	SDTide *sdTide;
	IBOutlet UILabel *locationLabel;
	IBOutlet UILabel *presentHeight;
	IBOutlet UIImageView *tideStateImage;
	IBOutlet UILabel *date;
	IBOutlet UILabel *time1;
	IBOutlet UILabel *time2;
	IBOutlet UILabel *time3;
	IBOutlet UILabel *time4;
	IBOutlet UILabel *height1;
	IBOutlet UILabel *height2;
	IBOutlet UILabel *height3;
	IBOutlet UILabel *height4;
	IBOutlet UILabel *state1;
	IBOutlet UILabel *state2;
	IBOutlet UILabel *state3;
	IBOutlet UILabel *state4;
	IBOutlet UIImageView *bullet1;
	IBOutlet UIImageView *bullet2;
	IBOutlet UIImageView *bullet3;
	IBOutlet UIImageView *bullet4;

	IBOutlet UILabel *correctionLabel;
	
	IBOutlet UIView *currentTideView;
	
	NSArray *table;
	int pageNumber;
    RootViewController *rootViewController;
}

-(void)refresh;
-(void)setSdTide:(SDTide*)newTide;
-(void)clearTable;
-(void)updatePresentTideInfo;

- (id)initWithPageNumber:(int)page;
-(IBAction)chooseTideStation:(id)sender;
-(IBAction)chooseNearbyTideStation:(id)sender;
-(IBAction)followHyperlink:(id)sender;

@property (readonly) SDTide *sdTide;
@property (nonatomic,retain) UIView *currentTideView;
@property (nonatomic,retain) RootViewController *rootViewController;
@end
