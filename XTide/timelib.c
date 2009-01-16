/*  Timelib  Time services.
    Last modified 1997-08-15

    Copyright (C) 1997  David Flater.
    Also starring:  Geoff Kuenning; Rob Miracle; Dean Pentcheff;
    Eric Rosen.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "everythi.h"

/**************************************************************************/

/* Declarations for zoneinfo compatibility hack */

/* It's not worth detecting or supporting POSIX because all the platforms
   I know of that support full POSIX with floating day rules do so by
   means of adopting Olson's code, in which case we just use the zoneinfo
   extensions anyway.  POSIX without floating day rules is useless. */

typedef enum {NEWZONEINFO=0, OLDZONEINFO=1, HP=2, BRAINDEAD=3} tz_support;

/* The following are based on tzdata96i */

/* I note that tzdata96i is different yet again from even
   NEWZONEINFO, which is whatever came with Slackware 3.1. */

/* In many cases, these substitutions will either break DST adjustments
   or will be incorrect for historical dates.  It's not worth documenting
   every single quirk when the whole thing is an obscene hack. */

char *subs[][4] = {
{":Africa/Abidjan",             "WAT0", "WAT0", "WAT0"},
{":Africa/Accra",               "WAT0", "WAT0", "WAT0"},
{":Africa/Addis_Ababa",         "EAT-3", "EAT-3", "EAT-3"},
{":Africa/Algiers",             "MET-1", "MET-1", "MET-1"},
{":Africa/Asmera",              "EAT-3", "EAT-3", "EAT-3"},
{":Africa/Bamako",              "WAT0", "WAT0", "WAT0"},
{":Africa/Bangui",              "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Banjul",              "WAT0", "WAT0", "WAT0"},
{":Africa/Bissau",              "WAT0", "WAT0", "WAT0"},
{":Africa/Blantyre",            "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Brazzaville",         "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Bujumbura",           "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Cairo",               ":Egypt", "EET-2", "EET-2"},
{":Africa/Casablanca",          "WET0", "WET0", "WET0"},
{":Africa/Conakry",             "WAT0", "WAT0", "WAT0"},
{":Africa/Dakar",               "WAT0", "WAT0", "WAT0"},
{":Africa/Dar_es_Salaam",       "EAT-3", "EAT-3", "EAT-3"},
{":Africa/Djibouti",            "EAT-3", "EAT-3", "EAT-3"},
{":Africa/Douala",              "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Freetown",            "WAT0", "WAT0", "WAT0"},
{":Africa/Gaborone",            "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Harare",              "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Johannesburg",        "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Kampala",             "EAT-3", "EAT-3", "EAT-3"},
{":Africa/Khartoum",            "EET-2", "EET-2", "EET-2"},
{":Africa/Kigali",              "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Kinshasa",            "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Lagos",               "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Libreville",          "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Lome",                "WAT0", "WAT0", "WAT0"},
{":Africa/Luanda",              "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Lumumbashi",          "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Lusaka",              "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Malabo",              "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Maputo",              "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Maseru",              "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Mbabane",             "SAT-2", "SAT-2", "SAT-2"},
{":Africa/Mogadishu",           "EAT-3", "EAT-3", "EAT-3"},
{":Africa/Monrovia",            "WAT0", "WAT0", "WAT0"},
{":Africa/Nairobi",             "EAT-3", "EAT-3", "EAT-3"},
{":Africa/Ndjamena",            "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Niamey",              "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Nouakchott",          "WAT0", "WAT0", "WAT0"},
{":Africa/Ouagadougou",         "WAT0", "WAT0", "WAT0"},
{":Africa/Porto-Novo",          "CAT-1", "CAT-1", "CAT-1"},
{":Africa/Sao_Tome",            "WAT0", "WAT0", "WAT0"},
{":Africa/Timbuktu",            "WAT0", "WAT0", "WAT0"},
{":Africa/Tripoli",             ":Libya", "EET-2", "EET-2"},
{":Africa/Tunis",               "MET-1", "MET-1", "MET-1"},
{":Africa/Windhoek",            "SAT-2", "SAT-2", "SAT-2"},

{":America/Adak",               ":US/Aleutian", "HAST10HADT",  "AST10ADT"},
{":America/Anchorage",          ":US/Alaska", "AKST9AKDT", "YST9YDT"},
{":America/Anguilla",           "AST4", "AST4", "AST4"},
{":America/Antigua",            "AST4", "AST4", "AST4"},
{":America/Aruba",              "AST4", "AST4", "AST4"},
{":America/Asuncion",           "AST4", "AST4", "AST4"},
{":America/Atka",               ":US/Aleutian", "HAST10HADT",  "AST10ADT"},
{":America/Barbados",           "AST4", "AST4", "AST4"},
{":America/Belize",             "CST6", "CST6", "CST6"},
{":America/Bogota",             "EST5", "EST5", "EST5"},
{":America/Buenos_Aires",       "ARST3", "ARST3", "AST3"},
{":America/Caracas",            "AST4", "AST4", "AST4"},
{":America/Cayenne",            "EST3", "EST3", "EST3"},
{":America/Cayman",             "EST5", "EST5", "EST5"},
{":America/Chicago",            ":US/Central", "CST6CDT", "CST6CDT"},
{":America/Costa_Rica",         "CST6", "CST6", "CST6"},
{":America/Curacao",            "AST4", "AST4", "AST4"},
{":America/Denver",             ":US/Mountain", "MST7MDT", "MST7MDT"},
{":America/Detroit",            ":US/Eastern", "EST5EDT", "EST5EDT"},
{":America/Dominica",           "AST4", "AST4", "AST4"},
{":America/Edmonton",           ":Canada/Mountain", "MST7MDT", "MST7MDT"},
{":America/El_Salvador",        "CST6", "CST6", "CST6"},
{":America/Ensenada",           ":Mexico/BajaNorte", "PST8PDT", "PST8PDT"},
{":America/Godthab",            "WGT3", "WGT3", "WGT3"},
{":America/Grand_Turk",         ":US/Eastern", "EST5EDT", "EST5EDT"},
{":America/Grenada",            "AST4", "AST4", "AST4"},
{":America/Guadeloupe",         "AST4", "AST4", "AST4"},
{":America/Guatemala",          "CST6", "CST6", "CST6"},
{":America/Guayaquil",          "EST5", "EST5", "EST5"},
{":America/Guyana",             "EST3", "EST3", "EST3"},
{":America/Halifax",            ":Canada/Atlantic", "AST4ADT", "AST4ADT"},
{":America/Havana",             ":Cuba", "CST5", "CST5"},
{":America/Jamaica",            ":Jamaica", "EST5EDT", "EST5EDT"},
{":America/La_Paz",             "AST4", "AST4", "AST4"},
{":America/Lima",               "EST5", "EST5", "EST5"},
{":America/Los_Angeles",        ":US/Pacific", "PST8PDT", "PST8PDT"},
{":America/Managua",            "CST6", "CST6", "CST6"},
{":America/Manaus",             ":Brazil/West", "WST4", "WST4"},
{":America/Martinique",         "AST4", "AST4", "AST4"},
{":America/Mazatlan",           ":Mexico/BajaSur", "MST7MDT", "MST7MDT"},
{":America/Mexico_City",        ":Mexico/General", "CST6CDT", "CST6CDT"},
{":America/Miquelon",           "SPST3SPDT", "SPST3SPDT", "SST3SDT"},
{":America/Montevideo",         "EST3", "EST3", "EST3"},
{":America/Montreal",           ":Canada/Eastern", "EST5EDT", "EST5EDT"},
{":America/Montserrat",         "AST4", "AST4", "AST4"},
{":America/Nassau",             ":US/Eastern", "EST5EDT", "EST5EDT"},
{":America/New_York",           ":US/Eastern", "EST5EDT", "EST5EDT"},
{":America/Noronha",            ":Brazil/DeNoronha", "FST2", "FST2"},
{":America/Panama",             "EST5", "EST5", "EST5"},
{":America/Paramaribo",         "EST3", "EST3", "EST3"},
{":America/Port-au-Prince",     ":US/Eastern", "EST5EDT", "EST5EDT"},
{":America/Port_of_Spain",      "AST4", "AST4", "AST4"},
{":America/Porto_Acre",         ":Brazil/Acre", "AST5", "AST5"},
{":America/Puerto_Rico",        "AST4", "AST4", "AST4"},
{":America/Regina",             ":Canada/Saskatchewan", "CST6", "CST6"},
{":America/Santiago",           ":Chile/Continental", "CST4", "CST4"},
{":America/Santo_Domingo",      "AST4", "AST4", "AST4"},
{":America/Sao_Paulo",          ":Brazil/East", "EST3", "EST3"},
{":America/Scoresbysund",       "EGT1", "EGT1", "EGT1"},
{":America/St_Johns",      ":Canada/Newfoundland", "NST3:30NDT", "NST3:30NDT"},
{":America/St_Kitts",           "AST4", "AST4", "AST4"},
{":America/St_Lucia",           "AST4", "AST4", "AST4"},
{":America/St_Thomas",          "AST4", "AST4", "AST4"},
{":America/St_Vincent",         "AST4", "AST4", "AST4"},
{":America/Tegucigalpa",        "CST6", "CST6", "CST6"},
{":America/Thule",              "AST4", "AST4", "AST4"},
{":America/Tijuana",            ":Mexico/BajaNorte", "PST8PDT", "PST8PDT"},
{":America/Tortola",            "AST4", "AST4", "AST4"},
{":America/Vancouver",          ":Canada/Pacific", "PST8PDT", "PST8PDT"},
{":America/Whitehorse",         ":Canada/Yukon", "PST8PDT", "PST8PDT"},
{":America/Winnipeg",           ":Canada/Central", "CST6CDT", "CST6CDT"},

{":Asia/Aden",                  "AST-3", "AST-3", "AST-3"},
{":Asia/Aktau",                 "ASK-5", "ASK-5", "ASK-5"},
{":Asia/Alma-Ata",              "AASK-6", "AASK-6", "ASK-6"},
{":Asia/Amman",                 "EET-2", "EET-2", "EET-2"},
{":Asia/Anadyr",                "ASK-13", "ASK-13", "ASK-13"},
{":Asia/Ashkhabad",             "ASK-5", "ASK-5", "ASK-5"},
{":Asia/Baghdad",               "AST-3", "AST-3", "AST-3"},
{":Asia/Bahrain",               "AST-3", "AST-3", "AST-3"},
{":Asia/Baku",                  "BSK-3", "BSK-3", "BSK-3"},
{":Asia/Bangkok",               "ICT-7", "ICT-7", "ICT-7"},
{":Asia/Beirut",                "EET-2", "EET-2", "EET-2"},
{":Asia/Bishkek",               "BSK-5", "BSK-5", "BSK-5"},
{":Asia/Brunei",                "BNT-8", "BNT-8", "BNT-8"},
{":Asia/Calcutta",              "IST-5:30", "IST-5:30", "IST-5:30"},
{":Asia/Colombo",               "IST-5:30", "IST-5:30", "IST-5:30"},
{":Asia/Dacca",                 "BGT-6", "BGT-6", "BGT-6"},
{":Asia/Damascus",              "EET-2", "EET-2", "EET-2"},
{":Asia/Dubai",                 "GST-4", "GST-4", "GST-4"},
{":Asia/Dushanbe",              "DSK-6", "DSK-6", "DSK-6"},
{":Asia/Gaza",                  "IST-2", "IST-2", "IST-2"},
{":Asia/Hong_Kong",             ":Hongkong", "HKT-8", "HKT-8"},
{":Asia/Irkutsk",               "ISK-8", "ISK-8", "ISK-8"},
{":Asia/Istanbul",              ":Turkey", "EET-2EETDST", "EET-2"},
{":Asia/Jakarta",               "JVT-7", "JVT-7", "JVT-7"},
{":Asia/Jayapura",              "MLT-9", "MLT-9", "MLT-9"},
{":Asia/Jerusalem",             ":Israel", "IST-2", "IST-2"},
{":Asia/Kabul",                 "AFT-4:30", "AFT-4:30", "AFT-4:30"},
{":Asia/Kamchatka",             "PSK-12", "PSK-12", "PSK-12"},
{":Asia/Karachi",               "PKT-5", "PKT-5", "PKT-5"},
{":Asia/Katmandu",              "NPT-5:45", "NPT-5:45", "NPT-5:45"},
{":Asia/Kuala_Lumpur",          "SGT-8", "SGT-8", "SGT-8"},
{":Asia/Kuwait",                "AST-3", "AST-3", "AST-3"},
{":Asia/Macao",                 "CST-8", "CST-8", "CST-8"},
{":Asia/Magadan",               "MSK-11", "MSK-11", "MSK-11"},
{":Asia/Manila",                "PST-8", "PST-8", "PST-8"},
{":Asia/Muscat",                "GST-4", "GST-4", "GST-4"},
{":Asia/Nicosia",               "EET-2", "EET-2", "EET-2"},
{":Asia/Novosibirsk",           "NSK-6", "NSK-6", "NSK-6"},
{":Asia/Omsk",                  "OSK-6", "OSK-6", "OSK-6"},
{":Asia/Phnom_Penh",            "ICT-7", "ICT-7", "ICT-7"},
{":Asia/Pyongyang",             "KST-9", "KST-9", "KST-9"},
{":Asia/Qatar",                 "AST-3", "AST-3", "AST-3"},
{":Asia/Rangoon",               "BMT-6:30", "BMT-6:30", "BMT-6:30"},
{":Asia/Riyadh",                "AST-3", "AST-3", "AST-3"},
{":Asia/Saigon",                "ICT-7", "ICT-7", "ICT-7"},
{":Asia/Seoul",                 ":ROK", "KST-9", "KST-9"},
{":Asia/Shanghai",              ":PRC", "CST-8", "CST-8"},
{":Asia/Singapore",             ":Singapore", "SGT-8", "SGT-8"},
{":Asia/Taipei",                ":ROC", "CST-8", "CST-8"},
{":Asia/Tashkent",              "TSK-5", "TSK-5", "TSK-5"},
{":Asia/Tbilisi",               "TBSK-4", "TBSK-4", "TSK-4"},
{":Asia/Tehran",                ":Iran", "IST-3:30", "IST-3:30"},
{":Asia/Tel_Aviv",              ":Israel", "IST-2", "IST-2"},
{":Asia/Thimbu",                "BGT-6", "BGT-6", "BGT-6"},
{":Asia/Tokyo",                 ":Japan", "JST-9", "JST-9"},
{":Asia/Ujung_Pandang",         "BNT-8", "BNT-8", "BNT-8"},
{":Asia/Ulan_Bator",            "UST-8", "UST-8", "UST-8"},
{":Asia/Vientiane",             "ICT-7", "ICT-7", "ICT-7"},
{":Asia/Vladivostok",           "VSK-10", "VSK-10", "VSK-10"},
{":Asia/Yakutsk",               "YSK-9", "YSK-9", "YSK-9"},
{":Asia/Yekaterinburg",         "ESK-5", "ESK-5", "ESK-5"},
{":Asia/Yerevan",               "AMST-4", "AMST-4", "AST-4"},

{":Atlantic/Azores",            "ACT1", "ACT1", "ACT1"},
{":Atlantic/Bermuda",           "AST4ADT", "AST4ADT", "AST4ADT"},
{":Atlantic/Canary",            ":WET", "WET0WETDST", "WET0"},
{":Atlantic/Cape_Verde",        "AAT1", "AAT1", "AAT1"},
{":Atlantic/Faeroe",            ":WET", "WET0WETDST", "WET0"},
{":Atlantic/Jan_Mayen",         "EGT1", "EGT1", "EGT1"},
{":Atlantic/Madeira",           ":WET", "WET0WETDST", "WET0"},
{":Atlantic/Reykjavik",         ":Iceland", "GMT0", "GMT0"},
{":Atlantic/South_Georgia",     "FST2", "FST2", "FST2"},
{":Atlantic/St_Helena",         "GMT0", "GMT0", "GMT0"},
{":Atlantic/Stanley",           "AST4", "AST4", "AST4"},

{":Australia/Adelaide",         ":Australia/South", "CST-9:30CDT", "CST-9:30"},
{":Australia/Brisbane",         ":Australia/Queensland", "EST-10", "EST-10"},
{":Australia/Broken_Hill", ":Australia/Yancowinna", "CST-9:30CDT", "CST-9:30"},
{":Australia/Canberra",         ":Australia/ACT", "EST-10EDT", "EST-10"},
{":Australia/Darwin",           ":Australia/North", "CST-9:30CDT", "CST-9:30"},
{":Australia/Hobart",           ":Australia/Tasmania", "EST-10EDT", "EST-10"},
{":Australia/Lord_Howe",        ":Australia/LHI", "LST-10:30", "LST-10:30"},
{":Australia/Melbourne",        ":Australia/Victoria", "EST-10EDT", "EST-10"},
{":Australia/Perth",            ":Australia/West", "WST-8", "WST-8"},
{":Australia/Sydney",           ":Australia/NSW", "EST-10EDT", "EST-10"},

/* No, it's not a screwup; they really did invert the signs on the
   GMT-offset files from one version to the next in order to "agree"
   with POSIX. */

{":Etc/GMT",                    ":GMT", "GMT0", "GMT0"},
{":Etc/GMT+0",                  ":GMT-0", "GMT0", "GMT0"},
{":Etc/GMT+1",                  ":GMT-1", "LST1", "LST1"},
{":Etc/GMT+10",                 ":GMT-10", "LST10", "LST10"},
{":Etc/GMT+11",                 ":GMT-11", "LST11", "LST11"},
{":Etc/GMT+12",                 ":GMT-12", "LST12", "LST12"},
{":Etc/GMT+2",                  ":GMT-2", "LST2", "LST2"},
{":Etc/GMT+3",                  ":GMT-3", "LST3", "LST3"},
{":Etc/GMT+4",                  ":GMT-4", "LST4", "LST4"},
{":Etc/GMT+5",                  ":GMT-5", "LST5", "LST5"},
{":Etc/GMT+6",                  ":GMT-6", "LST6", "LST6"},
{":Etc/GMT+7",                  ":GMT-7", "LST7", "LST7"},
{":Etc/GMT+8",                  ":GMT-8", "LST8", "LST8"},
{":Etc/GMT+9",                  ":GMT-9", "LST9", "LST9"},
{":Etc/GMT-0",                  ":GMT+0", "GMT0", "GMT0"},
{":Etc/GMT-1",                  ":GMT+1", "LST-1", "LST-1"},
{":Etc/GMT-10",                 ":GMT+10", "LST-10", "LST-10"},
{":Etc/GMT-11",                 ":GMT+11", "LST-11", "LST-11"},
{":Etc/GMT-12",                 ":GMT+12", "LST-12", "LST-12"},
{":Etc/GMT-13",                 ":GMT+13", "LST-13", "LST-13"},
{":Etc/GMT-2",                  ":GMT+2", "LST-2", "LST-2"},
{":Etc/GMT-3",                  ":GMT+3", "LST-3", "LST-3"},
{":Etc/GMT-4",                  ":GMT+4", "LST-4", "LST-4"},
{":Etc/GMT-5",                  ":GMT+5", "LST-5", "LST-5"},
{":Etc/GMT-6",                  ":GMT+6", "LST-6", "LST-6"},
{":Etc/GMT-7",                  ":GMT+7", "LST-7", "LST-7"},
{":Etc/GMT-8",                  ":GMT+8", "LST-8", "LST-8"},
{":Etc/GMT-9",                  ":GMT+9", "LST-9", "LST-9"},
{":Etc/GMT0",                   ":GMT+0", "GMT0", "GMT0"},
{":Etc/Greenwich",              ":Greenwich", "GMT0", "GMT0"},
{":Etc/UCT",                    ":UCT", "GMT0", "GMT0"},
{":Etc/UTC",                    ":UTC", "GMT0", "GMT0"},
{":Etc/Universal",              ":Universal", "GMT0", "GMT0"},
{":Etc/Zulu",                   ":Zulu", "GMT0", "GMT0"},

/* Although the tztab file from an old HP that I have doesn't contain
   EET-2EETDST, I'm going to gamble that they have added it by now.
   It's an obvious extension. */

{":Europe/Amsterdam",           ":MET", "MET-1METDST", "MET-1"},
{":Europe/Andorra",             ":MET", "MET-1METDST", "MET-1"},
{":Europe/Athens",              ":EET", "EET-2EETDST", "EET-2"},
{":Europe/Belfast",             ":GB-Eire", "GMT0BST", "GMT0"},
{":Europe/Belgrade",            ":MET", "MET-1METDST", "MET-1"},
{":Europe/Berlin",              ":MET", "MET-1METDST", "MET-1"},
{":Europe/Bratislava",          ":MET", "MET-1METDST", "MET-1"},
{":Europe/Brussels",            ":MET", "MET-1METDST", "MET-1"},
{":Europe/Bucharest",           ":EET", "EET-2EETDST", "EET-2"},
{":Europe/Budapest",            ":MET", "MET-1METDST", "MET-1"},
{":Europe/Chisinau",            ":EET", "EET-2EETDST", "EET-2"},
{":Europe/Copenhagen",          ":MET", "MET-1METDST", "MET-1"},
{":Europe/Dublin",              ":GB-Eire", "GMT0BST", "GMT0"},
{":Europe/Gibraltar",           ":MET", "MET-1METDST", "MET-1"},
{":Europe/Helsinki",            ":EET", "EET-2EETDST", "EET-2"},
{":Europe/Istanbul",            ":Turkey", "EET-2EETDST", "EET-2"},
{":Europe/Kiev",                ":EET", "EET-2EETDST", "EET-2"},
{":Europe/Kuybyshev",           "KSK-4", "KSK-4", "KSK-4"},
{":Europe/Lisbon",              ":WET", "WET0WETDST", "WET0"},
{":Europe/Ljubljana",           ":MET", "MET-1METDST", "MET-1"},
{":Europe/London",              ":GB-Eire", "GMT0BST", "GMT0"},
{":Europe/Luxembourg",          ":MET", "MET-1METDST", "MET-1"},
{":Europe/Madrid",              ":MET", "MET-1METDST", "MET-1"},
{":Europe/Malta",               ":MET", "MET-1METDST", "MET-1"},
{":Europe/Minsk",               ":EET", "EET-2EETDST", "EET-2"},
{":Europe/Monaco",              ":MET", "MET-1METDST", "MET-1"},
{":Europe/Moscow",              ":W-SU", "MSK-3", "MSK-3"},
{":Europe/Oslo",                ":MET", "MET-1METDST", "MET-1"},
{":Europe/Paris",               ":MET", "MET-1METDST", "MET-1"},
{":Europe/Prague",              ":MET", "MET-1METDST", "MET-1"},
{":Europe/Riga",                ":EET", "EET-2EETDST", "EET-2"},
{":Europe/Rome",                ":MET", "MET-1METDST", "MET-1"},
{":Europe/San_Marino",          ":MET", "MET-1METDST", "MET-1"},
{":Europe/Sarajevo",            ":MET", "MET-1METDST", "MET-1"},
{":Europe/Simferopol",          ":W-SU", "MSK-3", "MSK-3"},
{":Europe/Skopje",              ":MET", "MET-1METDST", "MET-1"},
{":Europe/Sofia",               ":EET", "EET-2EETDST", "EET-2"},
{":Europe/Stockholm",           ":MET", "MET-1METDST", "MET-1"},
{":Europe/Tallinn",             ":EET", "EET-2EETDST", "EET-2"},
{":Europe/Tirane",              ":MET", "MET-1METDST", "MET-1"},
{":Europe/Vaduz",               ":MET", "MET-1METDST", "MET-1"},
{":Europe/Vatican",             ":MET", "MET-1METDST", "MET-1"},
{":Europe/Vienna",              ":MET", "MET-1METDST", "MET-1"},
{":Europe/Vilnius",             ":EET", "EET-2EETDST", "EET-2"},
{":Europe/Warsaw",              ":Poland", "MET-1METDST", "MET-1"},
{":Europe/Zagreb",              ":MET", "MET-1METDST", "MET-1"},
{":Europe/Zurich",              ":MET", "MET-1METDST", "MET-1"},

{":Indian/Antananarivo",        "EAT-3", "EAT-3", "EAT-3"},
{":Indian/Chagos",              "PKT-5", "PKT-5", "PKT-5"},
{":Indian/Christmas",           "JVT-7", "JVT-7", "JVT-7"},
{":Indian/Cocos",               "CCT-6:30", "CCT-6:30", "CCT-6:30"},
{":Indian/Comoro",              "EAT-3", "EAT-3", "EAT-3"},
{":Indian/Mahe",                "SMT-4", "SMT-4", "SMT-4"},
{":Indian/Maldives",            "PKT-5", "PKT-5", "PKT-5"},
{":Indian/Mauritius",           "SMT-4", "SMT-4", "SMT-4"},
{":Indian/Mayotte",             "EAT-3", "EAT-3", "EAT-3"},
{":Indian/Reunion",             "SMT-4", "SMT-4", "SMT-4"},

{":Pacific/Apia",               "SST11", "SST11", "SST11"},
{":Pacific/Auckland",           ":NZ", "NZST-12NZDT", "NST-12"},
{":Pacific/Chatham",            "CST-12:45", "CST-12:45", "CST-12:45"},
{":Pacific/Easter",             ":Chile/EasterIsland", "CST6", "CST6"},
{":Pacific/Efate",              "NCST-11", "NCST-11", "NST-11"},
{":Pacific/Enderbury",          "TGT-13", "TGT-13", "TGT-13"},
{":Pacific/Fakaofo",            "THT10", "THT10", "THT10"},
{":Pacific/Fiji",               "NZST-12", "NZST-12", "NST-12"},
{":Pacific/Funafuti",           "NZST-12", "NZST-12", "NST-12"},
{":Pacific/Galapagos",          "CST6", "CST6", "CST6"},
{":Pacific/Gambier",            "GBT9", "GBT9", "GBT9"},
{":Pacific/Guadalcanal",        "NCST-11", "NCST-11", "NST-11"},
{":Pacific/Guam",               "GST-10", "GST-10", "GST-10"},
{":Pacific/Honolulu",           ":US/Hawaii", "HST10", "HST10"},
{":Pacific/Kiritimati",         "KRT-14", "KRT-14", "KRT-14"},
{":Pacific/Kosrae",             "NZST-12", "NZST-12", "NST-12"},
{":Pacific/Kwajalein",          "NZST-12", "NZST-12", "NST-12"},
{":Pacific/Majuro",             "NZST-12", "NZST-12", "NST-12"},
{":Pacific/Marquesas",          "MQT9:30", "MQT9:30", "MQT9:30"},
{":Pacific/Midway",             "SST11", "SST11", "SST11"},
{":Pacific/Nauru",              "NZST-12", "NZST-12", "NST-12"}, 
{":Pacific/Niue",               "SST11", "SST11", "SST11"},
{":Pacific/Norfolk",            "NRFT-11:30", "NRFT-11:30", "NFT-11:30"},
{":Pacific/Noumea",             "NCST-11", "NCST-11", "NST-11"},
{":Pacific/Pago_Pago",          "SST11", "SST11", "SST11"},
{":Pacific/Palau",              "PLT-9", "PLT-9", "PLT-9"},
{":Pacific/Pitcairn",           "PIT8:30", "PIT8:30", "PIT8:30"},
{":Pacific/Ponape",             "NCST-11", "NCST-11", "NST-11"},
{":Pacific/Port_Moresby",       "EST-10", "EST-10", "EST-10"},
{":Pacific/Rarotonga",          "THT10", "THT10", "THT10"},
{":Pacific/Saipan",             "GST-10", "GST-10", "GST-10"},
{":Pacific/Samoa",              "SST11", "SST11", "SST11"},
{":Pacific/Tahiti",             "THT10", "THT10", "THT10"},
{":Pacific/Tarawa",             "NZST-12", "NZST-12", "NST-12"},
{":Pacific/Tongatapu",          "TGT-13", "TGT-13", "TGT-13"},
{":Pacific/Truk",               "GST-10", "GST-10", "GST-10"},
{":Pacific/Wake",               "NZST-12", "NZST-12", "NST-12"},
{":Pacific/Wallis",             "NZST-12", "NZST-12", "NST-12"},
{":Pacific/Yap",                "GST-10", "GST-10", "GST-10"},

/* Compatibility with outdated TZ specs */

{":US/Hawaii",                  ":US/Hawaii", "HST10",  "HST10"},
{":US/Aleutian",                ":US/Aleutian", "HAST10HADT",  "AST10ADT"},
{":US/Alaska",                  ":US/Alaska", "AKST9AKDT", "YST9YDT"},
{":US/Pacific",                 ":US/Pacific", "PST8PDT", "PST8PDT"},
{":US/Mountain",                ":US/Mountain", "MST7MDT", "MST7MDT"},
{":US/Central",                 ":US/Central", "CST6CDT", "CST6CDT"},
{":US/Eastern",                 ":US/Eastern", "EST5EDT", "EST5EDT"},

{":Canada/Pacific",             ":Canada/Pacific", "PST8PDT", "PST8PDT"},
{":Canada/Yukon",               ":Canada/Yukon", "PST8PDT", "PST8PDT"},
{":Canada/Mountain",            ":Canada/Mountain", "MST7MDT", "MST7MDT"},
{":Canada/Central",             ":Canada/Central", "CST6CDT", "CST6CDT"},
{":Canada/Saskatchewan",        ":Canada/Saskatchewan", "CST6", "CST6"},
{":Canada/East-Saskatchewan",   ":Canada/East-Saskatchewan", "CST6", "CST6"},
{":Canada/Eastern",             ":Canada/Eastern", "EST5EDT", "EST5EDT"},
{":Canada/Atlantic",            ":Canada/Atlantic", "AST4ADT", "AST4ADT"},
{":Canada/Newfoundland",   ":Canada/Newfoundland", "NST3:30NDT", "NST3:30NDT"},

{":Mexico/BajaNorte",           ":Mexico/BajaNorte", "PST8PDT", "PST8PDT"},
{":Mexico/BajaSur",             ":Mexico/BajaSur", "MST7MDT", "MST7MDT"},
{":Mexico/General",             ":Mexico/General", "CST6CDT", "CST6CDT"},

{":Brazil/West",                ":Brazil/West", "WST4", "WST4"},
{":Brazil/DeNoronha",           ":Brazil/DeNoronha", "FST2", "FST2"},
{":Brazil/East",                ":Brazil/East", "EST3", "EST3"},

{":Chile/Continental",          ":Chile/Continental", "CST4", "CST4"},
{":Chile/EasterIsland",         ":Chile/EasterIsland", "CST6", "CST6"},

{":Australia/ACT",              ":Australia/ACT", "EST-10EDT", "EST-10"},
{":Australia/LHI",              ":Australia/LHI", "LST-10:30", "LST-10:30"},
{":Australia/North",            ":Australia/North", "CST-9:30CDT", "CST-9:30"},
{":Australia/NSW",              ":Australia/NSW", "EST-10EDT", "EST-10"},
{":Australia/Queensland",       ":Australia/Queensland", "EST-10", "EST-10"},
{":Australia/South",            ":Australia/South", "CST-9:30CDT", "CST-9:30"},
{":Australia/Tasmania",         ":Australia/Tasmania", "EST-10EDT", "EST-10"},
{":Australia/Victoria",         ":Australia/Victoria", "EST-10EDT", "EST-10"},
{":Australia/West",             ":Australia/West", "WST-8", "WST-8"},
{":Australia/Yancowinna",  ":Australia/Yancowinna", "CST-9:30CDT", "CST-9:30"},

{":WET",                        ":WET", "WET0WETDST", "WET0"},
{":MET",                        ":MET", "MET-1METDST", "MET-1"},
{":EET",                        ":EET", "EET-2EETDST", "EET-2"},
{":GB-Eire",                    ":GB-Eire", "GMT0BST", "GMT0"},
{":PRC",                        ":PRC", "CST-8", "CST-8"},
{":ROC",                        ":ROC", "CST-8", "CST-8"},
{":ROK",                        ":ROK", "KST-9", "KST-9"},
{":W-SU",                       ":W-SU", "MSK-3", "MSK-3"},

{":Cuba",                       ":Cuba", "CST5", "CST5"},
{":Egypt",                      ":Egypt", "EET-2", "EET-2"},
{":Hongkong",                   ":Hongkong", "HKT-8", "HKT-8"},
{":Iceland",                    ":Iceland", "GMT0", "GMT0"},
{":Iran",                       ":Iran", "IST-3:30", "IST-3:30"},
{":Israel",                     ":Israel", "IST-2", "IST-2"},
{":Jamaica",                    ":Jamaica", "EST5EDT", "EST5EDT"},
{":Japan",                      ":Japan", "JST-9", "JST-9"},
{":Libya",                      ":Libya", "EET-2", "EET-2"},
{":Poland",                     ":Poland", "MET-1METDST", "MET-1"},
{":Singapore",                  ":Singapore", "SGT-8", "SGT-8"},
{":Turkey",                     ":Turkey", "EET-2EETDST", "EET-2"},

/* Terminator */
{NULL, NULL, NULL, NULL}};


/**************************************************************************/

/* Turn a time displacement of the form [-]HH:MM into the number of seconds. */
int
hhmm2seconds (char *hhmm)
{
  int h, m;
  char s;
  if (sscanf (hhmm, "%d:%d", &h, &m) != 2)
    barf (BADHHMM);
  if (sscanf (hhmm, "%c", &s) != 1)
    barf (BADHHMM);
  if (h < 0 || s == '-')
    m = -m;
  return h*HOURSECONDS + m*60;
}

/* The inverse. */
char *
seconds2hhmm (int seconds) {
  char sign;
  static char buf[80];
  if (seconds < 0) {
    sign = '-';
    seconds = -seconds;
  } else
    sign = '+';
  sprintf (buf,
    "%c%02d:%02d", sign, seconds/HOURSECONDS, (seconds%HOURSECONDS)/60);
  return buf;
}

/* This idiotic function is needed by the new tm2gmt. */
#define compare_int(a,b) (((int)(a))-((int)(b)))
static int
compare_tm (struct tm *a, struct tm *b) {
  int temp;
  /* printf ("A is %d:%d:%d:%d:%d:%d   B is %d:%d:%d:%d:%d:%d\n",
    a->tm_year+1900, a->tm_mon+1, a->tm_mday, a->tm_hour,
    a->tm_min, a->tm_sec,
    b->tm_year+1900, b->tm_mon+1, b->tm_mday, b->tm_hour,
    b->tm_min, b->tm_sec); */
  if ((temp = compare_int (a->tm_year, b->tm_year)))
    return temp;
  if ((temp = compare_int (a->tm_mon, b->tm_mon)))
    return temp;
  if ((temp = compare_int (a->tm_mday, b->tm_mday)))
    return temp;
  if ((temp = compare_int (a->tm_hour, b->tm_hour)))
    return temp;
  if ((temp = compare_int (a->tm_min, b->tm_min)))
    return temp;
  return compare_int (a->tm_sec, b->tm_sec);
}

/* Convert a struct tm in GMT back to a time_t.  isdst is ignored, since
   it never should have been needed by mktime in the first place.

   Note that switching the global time zone to GMT, using mktime, and
   switching back either screws up or core dumps on certain popular
   platforms.

   I continue to assert that the Posix time API should be taken out and
   shot.  The status quo sucks, and Posix just standardized it without
   fixing it.  As a result, we have to use idiotic kludges and workarounds
   like this one.
*/
time_t
tm2gmt (struct tm *ht)
{
  time_t guess, newguess, thebit;
  int loopcounter, compare;

  /*
      "A thing not worth doing at all is not worth doing well."

        -- Bruce W. Arden and Kenneth N. Astill, Numerical Algorithms:
           Origins and Applications, Addison-Wesley, 1970, Ch. 1.
  */

  guess = 0;
  loopcounter = sizeof(time_t) * 8;
  thebit = ((time_t)1) << (loopcounter-1);

  /* For simplicity, I'm going to insist that the time_t we want is
     positive.  If time_t is signed, skip the sign bit.
   */
  if (thebit < (time_t)0) {
    /* You can't just shift thebit right because it propagates the sign bit. */
    loopcounter--;
    thebit = ((time_t)1) << (loopcounter-1);
    assert (thebit > (time_t)0);
  }

  for (; loopcounter; loopcounter--) {
    newguess = guess | thebit;
    compare = compare_tm (gmtime(&newguess), ht);
    /* printf ("thebit=%lu  guess=%lu  newguess=%lu  compare=%d\n",
      thebit, guess, newguess, compare); */
    if (compare <= 0)
      guess = newguess;
    assert (thebit > (time_t)0);
    thebit >>= 1;
  }

  assert (!thebit);
  if (compare_tm (gmtime(&guess), ht)) {
    fprintf (stderr, "XTide:  tm2gmt failed to converge on the following \
input\n");
    fprintf (stderr, "%d:%d:%d:%d:%d:%d\n",
      ht->tm_year+1900, ht->tm_mon+1, ht->tm_mday, ht->tm_hour,
      ht->tm_min, ht->tm_sec);
    barf (BADTIMESTAMP);
  }

  return guess;
}

/* This is the old tm2gmt that served us well for many moons, until the
   time when an anonymous reviewer gave me hell about it.  Note that this
   function _is_ sensitive to the setting of isdst.  Choosing the right
   setting for isdst in various places was part of the magic.
*/
#if 0
time_t
tm2gmt (struct tm *ht)
{
  time_t temp;
#if defined(sun) && !defined(SVR4) && !defined(__svr4__)
  /* Old SunOS */
  temp = timegm (ht);
  /* {
    struct tm *sunos;
    temp = timelocal (ht);
    sunos = localtime (&temp);
    temp += sunos->tm_gmtoff;
  } */
#else
  if ((temp = mktime (ht)) == -1) {
    fprintf (stderr, "mktime failure in tm2gmt\n");
    fprintf (stderr, "%d:%d:%d:%d:%d:%d\n",
      ht->tm_year+1900, ht->tm_mon+1, ht->tm_mday, ht->tm_hour,
      ht->tm_min, ht->tm_sec);
    fprintf (stderr, "isdst = %d   tzfile = %s\n", ht->tm_isdst, tzfile);
    assert (0);
  }
  /* Determine the local time zone offset and correct for it */
  /* Taking timezone from gettimeofday didn't work (always zero) */
#if defined(ultrix) || defined(__bsdi__)
 {
  /* Ultrix compatibility courtesy of Rob Miracle */
  /* BSD/OS 2.1 patch by Eric Rosen */
  struct tm *tzinfo;
  tzinfo = localtime (&temp);
  temp += tzinfo->tm_gmtoff;
 }
#else

  /* The following is known to be bugged when DST is in effect because
     timezone is always standard time.

     Until Unix evolves a better time zone facility, fixing this bug
     requires a horrendous workaround such as those used in RCS
     (repeated trial and error with deltas calculated on the struct
     tms) or Emacs (set local time zone to GMT, do mktime, and change
     local time zone back again, with assorted kludges and workarounds
     for systems that screw up changing time zones).
  */

  localtime(&temp);
  temp -= timezone;
#endif
#endif
  return temp;
}
#endif

/* Calculate time_t of the epoch. */
void
set_epoch (int year, int num_epochs, int first_year)
{
  struct tm ht;
  if (year < first_year || year >= first_year + num_epochs) {
    fprintf (stderr, "Tidelib:  Don't have equilibrium arguments for %d\n",
      year);
    barf (MISSINGYEAR);
  }
  ht.tm_year = year - 1900;
  ht.tm_sec = ht.tm_min = ht.tm_hour = ht.tm_mon = 0;
  ht.tm_mday = 1;
  epoch = tm2gmt (&ht);
}

/* Attempt to load up the local time zone of the location.  Moof! */
void
change_time_zone (char *tz)
{
  static char env_string[MAXARGLEN+1];
  char junk[MAXARGLEN+1];
  tz_support tzlevel = BRAINDEAD;
  time_t testtime;

   /* BROKEN_ZONEINFO is now used only as a user override to force BRAINDEAD */
#ifndef BROKEN_ZONEINFO

  /* Probe to determine the tzlevel -- hopefully this won't hose anybody */

  testtime = time(NULL);

  /* This is needed on some Solaris and IRIX machines to avoid a false
     positive on NEWZONEINFO that happens when -gstart is used.  Don't
     know exactly what's up with that, but this fixes it. */
  strcpy (env_string, "TZ=GMT0");
  assert (putenv (env_string) == 0);
  tzset ();

  /* New zoneinfo? */
  strcpy (env_string, "TZ=:America/New_York");
  assert (putenv (env_string) == 0);
  tzset();
  strftime (junk, MAXARGLEN, "%Z", localtime (&testtime));
  if (junk[0] == 'E') {
    tzlevel = NEWZONEINFO;
  } else {

    /* Old zoneinfo? */
    strcpy (env_string, "TZ=:US/Eastern");
    assert (putenv (env_string) == 0);
    tzset();
    strftime (junk, MAXARGLEN, "%Z", localtime (&testtime));
    if (junk[0] == 'E') {
      tzlevel = OLDZONEINFO;
    } else {

      /* HP-UX? */
#ifdef __hpux
      tzlevel = HP;
#else
      tzlevel = BRAINDEAD;
#endif
    }
  }
#endif

  /* If our level of support is less than NEWZONEINFO, find the
     translation for the timezone string */
  if (tzlevel != NEWZONEINFO) {
    int index = 0;
    while (1) {
      if (subs[index][0] == NULL) {
        /* Not found.  Probably best not to issue a warning, since somebody
           might use something that will work. */
        break;
      }
      if (!strcmp (subs[index][0], tz)) {
        strcpy (tz, subs[index][tzlevel]);
        break;
      }
      index++;
    }
  }

  /* assert (setenv ("TZ", tz, 1) == 0); */
  /* According to the SYSV man page, I can't alter env_string ever again. */
  sprintf (env_string, "TZ=%s", tz);
  assert (putenv (env_string) == 0);
  tzset ();
}

/* Convert YYYY:MM:DD:HH:MM to time_t. */
time_t
parse_time_string (char *time_string)
{
  struct tm ht;
  time_t temp;
  if (sscanf (time_string, "%d:%d:%d:%d:%d",
    &(ht.tm_year), &(ht.tm_mon), &(ht.tm_mday), &(ht.tm_hour),
    &(ht.tm_min)) != 5)
    barf (BADTIMESTAMP);
  ht.tm_sec = 0;
  (ht.tm_mon)--;
  ht.tm_year -= 1900;
  if (uutc) {
    if ((temp = tm2gmt (&ht)) == -1)
      barf (BADTIMESTAMP);
  } else {
    ht.tm_isdst = -1;
    if ((temp = mktime (&ht)) == -1)
      barf (BADTIMESTAMP);
  }
  return temp;
}

/* Return the year for a time_t.  This function gets called a lot and might
   be a performance bottleneck if gmtime is slow. */
int
yearoftimet (time_t t)
{
  return ((gmtime (&t))->tm_year) + 1900;
}

/* Used by do_timestamp. */
static int
is_ascii (char a)
{
  if (a >= ' ' && a <= '~')
    return 1;
  return 0;
}

/* Generate a datestamp in the appropriate style. */
void
do_datestamp (char buf[20], struct tm *t)
{
  char fmt[12];
  fmt[0] = '\0';
  if (weekday)
    strcat (fmt, "%a ");
  if (skinny < 1)
    strcat (fmt, "%Y-");
  strcat (fmt, "%m-%d");
  strftime (buf, 20, fmt, t);
}

/* Generate a timestamp in the appropriate style. */
void
do_timestamp (char buf[20], struct tm *t)
{
  int a;
  if (skinny == 2)
    strftime (buf, 20, "%H%M", t);
  else if (tadjust) {
    if (noampm)
      strftime (buf, 20, "%H:%M", t);
    else
      strftime (buf, 20, "%I:%M %p", t);
  }
  else if (utc) {
    if (skinny < 1)
      strftime (buf, 20, "%H:%M UTC", t);
    else
      strftime (buf, 20, "%H:%M", t);
  }
  else {
    if (skinny < 1) {
      if (noampm)
        strftime (buf, 20, "%H:%M %Z", t);
      else
        strftime (buf, 20, "%I:%M %p %Z", t);
    } else {
      if (noampm)
        strftime (buf, 20, "%H:%M", t);
      else
        strftime (buf, 20, "%I:%M %p", t);
    }
  }
  if (skinny < 2 && buf[0] == '0') {
    if (text && !ppm && !gif && !banner)
      buf[0] = ' ';
    else
#if defined(sun) && !defined(SVR4) && !defined(__svr4__)
    /* Old SunOS */
    {
      for (a=0;a<19;a++)
        buf[a] = buf[a+1];
    }
#else
      memmove (buf, buf+1, 19);
#endif
  }

  /* Get rid of any blanks or binary garbage (national characters?) */
  buf[19] = '\0';
  for (a=18;a>=0;a--)
    if (!is_ascii (buf[a]))
      buf[a] = '\0';
  nojunk (buf);
}

/* Do both. */
char *
do_long_timestamp (struct tm *t)
{
  static char longbuf[40];
  char datebuf[20], timebuf[20];
  do_datestamp (datebuf, t);
  do_timestamp (timebuf, t);
  if (timebuf[0] == ' ')
    sprintf (longbuf, "%s %s", datebuf, timebuf+1);
  else
    sprintf (longbuf, "%s %s", datebuf, timebuf);
  return longbuf;
}

/* Convert a genuine time_t to a specially mangled struct tm. */
struct tm *
tmtime (time_t t)
{
  if (tadjust) {
    t += tadjust;
    return gmtime (&t);
  } else if (utc)
      return gmtime (&t);
    else
      return localtime (&t);
}

/* Find the previous hour-transition from a specified time.  If a DST
   change skips over the transition, then we skip it too. */
time_t
prev_hour (time_t t)
{
  struct tm ttm;
  time_t temp, temp2;

  /* Take care of UTC-based time zones. */
  if (utc || tadjust) {
    ttm = *(gmtime (&t));
    ttm.tm_sec = ttm.tm_min = 0;
    temp = tm2gmt (&ttm) - (tadjust % HOURSECONDS);
    if (temp <= t - HOURSECONDS)
      temp += HOURSECONDS;
    if (temp > t)
      temp -= HOURSECONDS;
    assert (temp > t - HOURSECONDS && temp <= t);
    return temp;
  }

  /* Deal with local time zones. */
  /* Can't use mktime, because it sucks!  It SUCKS!!! */

  /* The easy case will work unless we are hosed by DST. */
  ttm = *(localtime (&t));
  temp = t - (ttm.tm_min * 60 + ttm.tm_sec);
  assert (temp > t - HOURSECONDS && temp <= t);
  ttm = *(localtime (&temp));
  if (ttm.tm_sec == 0 && ttm.tm_min == 0)
    return temp;

  /* See if we went back too far. */
  temp2 = temp + HOURSECONDS - (ttm.tm_min * 60 + ttm.tm_sec);
  if (temp2 > t - HOURSECONDS && temp2 <= t) {
    ttm = *(localtime (&temp2));
    if (ttm.tm_sec == 0 && ttm.tm_min == 0) {
      /* fprintf (stderr, "prev_hour:  Too far\n"); */
      return temp2;
    }
  }

  /* Go back farther. */
  /* fprintf (stderr, "prev_hour:  Not far enough; recursing\n"); */
  return prev_hour (temp);
}

/* Given that we are on an hour transition, find the next one.  If a DST
   change skips over the transition, then we skip it too. */
time_t
increment_hour (time_t t) {
  struct tm ttm;
  time_t temp, temp2;

  /* Take care of UTC-based time zones.  These have no DST. */
  if (utc || tadjust)
    return t + HOURSECONDS;

  /* Deal with local time zones. */
  /* Did I mention how bad mktime SUCKS? */

  /* The easy case will work unless we are hosed by DST. */
  temp = t + HOURSECONDS;
  ttm = *(localtime (&temp));
  if (ttm.tm_sec == 0 && ttm.tm_min == 0)
    return temp;

  /* See if we went forward too far. */
  temp2 = temp - (ttm.tm_min * 60 + ttm.tm_sec);
  if (temp2 > t && temp2 <= t + HOURSECONDS) {
    ttm = *(localtime (&temp2));
    if (ttm.tm_sec == 0 && ttm.tm_min == 0) {
      /* fprintf (stderr, "increment_hour:  Too far\n"); */
      return temp2;
    }
  }

  /* Go forward some more. */
  /* fprintf (stderr, "increment_hour:  Not far enough; recursing\n"); */
  return increment_hour (temp);
  /* That will work, even though we are not starting on an hour transition. */
}

/* Find the previous day-transition from a specified time.  Returns a
   reasonable guess even if there is no midnight due to DST adjustment. */
time_t
prev_day (time_t t)
{
  struct tm ttm;
  time_t temp, temp2;
  int today;

  /* Take care of UTC-based time zones. */
  if (utc || tadjust) {
    ttm = *(gmtime (&t));
    ttm.tm_sec = ttm.tm_min = ttm.tm_hour = 0;
    temp = tm2gmt (&ttm) - tadjust;
    if (temp <= t - DAYSECONDS)
      temp += DAYSECONDS;
    if (temp > t)
      temp -= DAYSECONDS;
    assert (temp > t - DAYSECONDS && temp <= t);
    return temp;
  }

  /* Deal with local time zones. */
  /* mktime sucks in every country and every time zone! */

  /* The easy case will work unless we are hosed by DST. */
  ttm = *(localtime (&t));
  today = ttm.tm_mday;
  temp = t - (ttm.tm_hour * HOURSECONDS + ttm.tm_min * 60 + ttm.tm_sec);
  assert (temp > t - DAYSECONDS && temp <= t);
  ttm = *(localtime (&temp));
  if (ttm.tm_hour == 0 && ttm.tm_sec == 0 && ttm.tm_min == 0)
    return temp;

  /* See if we went back too far. */
  if (ttm.tm_mday != today) {
    temp2 = temp + DAYSECONDS - (ttm.tm_hour * HOURSECONDS +
    ttm.tm_min * 60 + ttm.tm_sec);
    /* fprintf (stderr, "prev_day:  Too far\n"); */
    /* This is always our best guess (approach from left) due to DST
       changes being applied exactly at midnight. */
    return temp2;
  }

  /* Go back farther. */
  /* fprintf (stderr, "prev_day:  Not far enough; recursing\n"); */
  return prev_day (temp);
}

/* Given that we are on a day transition, find the next one.  Returns a
   reasonable guess even if there is no midnight due to DST adjustment. */
time_t
increment_day (time_t t)
{
  struct tm ttm;
  time_t temp;
  int today;

  /* Take care of UTC-based time zones. */
  if (utc || tadjust)
    return t + DAYSECONDS;

  /* Deal with local time zones. */
  /* mktime sucks in ways that will get you arrested in the U.S. */

  /* The easy case will work unless we are hosed by DST. */
  ttm = *(localtime (&t));
  today = ttm.tm_mday;
  temp = t + DAYSECONDS;
  ttm = *(localtime (&temp));
  if (ttm.tm_hour == 0 && ttm.tm_sec == 0 && ttm.tm_min == 0)
    return temp;

  /* See if we went forward too far. */
  if (ttm.tm_mday != today) {
    /* fprintf (stderr, "increment_day:  Too far; calling prev_day\n"); */
    return prev_day (temp);
  }

  /* Go forward farther. */
  /* fprintf (stderr, "increment_day:  Not far enough; recursing\n"); */
  return increment_day (temp);
  /* That will work, even though we are not starting on a day transition. */
}

/* Find the Sunday on which to start a calendar. */
time_t
sunday_month (time_t t)
{
  struct tm ttm;
  int almost_done = 0;
  ttm = *(tmtime(t));
  if (ttm.tm_mday == 1)
    almost_done = 1;
  while (ttm.tm_wday || !almost_done) {
    t = prev_day (t-1);
    ttm = *(tmtime(t));
    if (ttm.tm_mday == 1)
      almost_done = 1;
  }
  return t;
}
