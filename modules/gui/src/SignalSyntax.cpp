// copyright (c) 2017 Richard Guadagno
// contact: rrguadagno@gmail.com
//
// This file is part of French-Roast
//
//    French-Roast is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    French-Roast is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with French-Roast.  If not, see <http://www.gnu.org/licenses/>.
//

#include <QRegularExpression>
#include "SignalSyntax.h"
#include "CodeFont.h"



void SignalSyntax::highlightBlock(const QString& text)
{
  QTextCharFormat pointFormat;
  pointFormat.setFontWeight(QFont::Normal);
  pointFormat.setForeground(QBrush(QColor(180,182,182)));
  pointFormat.setFont(CodeFont{});

  QTextCharFormat ppointFormat;
  ppointFormat.setFontWeight(QFont::Normal);
  ppointFormat.setForeground(QBrush(QColor(232,117,15)));
  ppointFormat.setFont(CodeFont{});

  
  QRegularExpression pointReg("[A-Za-z,\\.:() <>\\[\\]\\_]*");
  QRegularExpressionMatchIterator itr = pointReg.globalMatch(text);
  while(itr.hasNext()) {
    QRegularExpressionMatch match = itr.next();
    setFormat(match.capturedStart(), match.capturedLength(), pointFormat);
  }

  QRegularExpression ppointReg("::[A-Za-z0-9_]*:");
  itr = ppointReg.globalMatch(text);
  while(itr.hasNext()) {
    QRegularExpressionMatch match = itr.next();
    setFormat(match.capturedStart(), match.capturedLength(), ppointFormat);
  }

  QRegularExpression pointReg2("[:]*");
   itr = pointReg2.globalMatch(text);
  while(itr.hasNext()) {
    QRegularExpressionMatch match = itr.next();
    setFormat(match.capturedStart(), match.capturedLength(), pointFormat);
  }
  
  
}

SignalSyntax::SignalSyntax(QTextDocument* docptr) : QSyntaxHighlighter(docptr)
{
}
