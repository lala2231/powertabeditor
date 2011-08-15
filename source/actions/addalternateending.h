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
  
#ifndef ADDALTERNATEENDING_H
#define ADDALTERNATEENDING_H

#include <QUndoCommand>
#include <boost/shared_ptr.hpp>

class Score;
class AlternateEnding;

class AddAlternateEnding : public QUndoCommand
{
public:
    AddAlternateEnding(Score* score, boost::shared_ptr<AlternateEnding> altEnding);

    void redo();
    void undo();

private:
    Score* score;
    boost::shared_ptr<AlternateEnding> altEnding;
};

#endif // ADDALTERNATEENDING_H
