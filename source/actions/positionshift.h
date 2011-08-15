/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#ifndef POSITIONSHIFT_H
#define POSITIONSHIFT_H

#include <QUndoCommand>
#include <boost/shared_ptr.hpp>

class System;

class PositionShift : public QUndoCommand
{
public:
    enum ShiftType
    {
        SHIFT_FORWARD,
        SHIFT_BACKWARD
    };

    PositionShift(boost::shared_ptr<System> system, quint32 positionIndex, ShiftType type);
    void redo();
    void undo();

protected:
    boost::shared_ptr<System> system;
    const quint32 positionIndex;
    const ShiftType type;
};

#endif // POSITIONSHIFT_H
