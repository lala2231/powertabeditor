/*
  * Copyright (C) 2014 Cameron White
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

#include "textitemdialog.h"
#include "ui_textitemdialog.h"

#include <QMessageBox>

TextItemDialog::TextItemDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::TextItemDialog)
{
    ui->setupUi(this);
}

TextItemDialog::~TextItemDialog()
{
    delete ui;
}

std::string TextItemDialog::getContents() const
{
    return ui->contentsLineEdit->text().toStdString();
}

void TextItemDialog::accept()
{
    if (ui->contentsLineEdit->text().isEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Text"));
        msgBox.setText(tr("The text cannot be empty."));
        msgBox.exec();
    }
    else
        done(Accepted);
}