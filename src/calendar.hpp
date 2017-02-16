/***************************************************************************
                          calendar.cpp  -  basic calendar transformations
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by G. Duvert
    email                : gilles.duvert@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CALENDAR_HPP_
#define CALENDAR_HPP_
#include "basegdl.hpp" // DLong
double Gregorian2Julian(struct tm *ts);
bool dateToJD(DDouble &jd, DLong day, DLong month, DLong year, DLong hour, DLong minute, DDouble second);
bool j2ymdhms(DDouble jd, DLong &iMonth, DLong &iDay , DLong &iYear ,
                  DLong &iHour , DLong &iMinute, DDouble &Second, DLong &dow, DLong &icap);

#endif
