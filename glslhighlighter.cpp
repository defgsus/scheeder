/***************************************************************************

Copyright (C) 2014  stefan.berke @ modular-audio-graphics.com

This source is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

****************************************************************************/

#include "glslhighlighter.h"
#include "glslsyntax.h"
#include "appsettings.h"

GlslHighlighter::GlslHighlighter(QTextDocument *parent)
    :   QSyntaxHighlighter(parent)
{
    QTextCharFormat
        keywordFormat,
        reservedKeywordFormat,
        functionFormat,
        variableFormat,
        specificVariableFormat;

    // -- styles for each category --

    // keywords
    keywordFormat.setFontWeight(QFont::Bold);
    keywordFormat.setForeground(QBrush(QColor(220,220,220)));
    // reserved keywords
    reservedKeywordFormat.setFontWeight(QFont::Bold);
    reservedKeywordFormat.setForeground(QBrush(QColor(220,50,50)));
    // built-in functions
    functionFormat.setFontWeight(QFont::Bold);
    functionFormat.setForeground(QBrush(QColor(200,200,220)));
    // built-in variables
    variableFormat.setFontWeight(QFont::Bold);
    variableFormat.setForeground(QBrush(QColor(200,220,200)));
    // application-specific variables
    specificVariableFormat.setFontWeight(QFont::Bold);
    specificVariableFormat.setForeground(QBrush(QColor(200,220,220)));
    // comments
    commentFormat_.setForeground(QBrush(QColor(140,140,140)));

    // temporary
    HighlightingRule rule;
    GlslSyntax syntax;

    // setup keywords

    for (auto &k : syntax.keywords())
    {
        // match whole-word with word boundaries
        rule.pattern = QRegExp( "\\b" + k + "\\b" );
        rule.format = keywordFormat;
        rules_.append(rule);
    }

    // setup reserved keywords

    for (auto &k : syntax.reservedKeywords())
    {
        rule.pattern = QRegExp( "\\b" + k + "\\b" );
        rule.format = reservedKeywordFormat;
        rules_.append(rule);
    }

    // setup functions

    for (auto &k : syntax.functions())
    {
        rule.pattern = QRegExp( "\\b" + k + "\\b" );
        rule.format = functionFormat;
        rules_.append(rule);
    }

    // setup variables

    for (auto &k : syntax.variables())
    {
        rule.pattern = QRegExp( "\\b" + k + "\\b" );
        rule.format = variableFormat;
        rules_.append(rule);
    }

    // setup app-specific variables

    auto vars = appSettings->getShaderAttributes();
    vars += appSettings->getShaderUniforms();
    for (auto &v : vars)
    {
        rule.pattern = QRegExp( "\\b" + v + "\\b" );
        rule.format = specificVariableFormat;
        rules_.append(rule);
    }

    // setup single line comments
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = commentFormat_;
    rules_.append(rule);

    // setup multiline comments
    commentStartExpression_ = QRegExp("/\\*");
    commentEndExpression_ = QRegExp("\\*/");
}


void GlslHighlighter::highlightBlock(const QString &text)
{
    // apply rules
    foreach (const HighlightingRule &rule, rules_)
    {
        QRegExp expression(rule.pattern);
        // find the string
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            // set format
            setFormat(index, length, rule.format);
            // find again
            index = expression.indexIn(text, index + length);
        }
    }


    // --- multiline comments ---

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression_.indexIn(text);

    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression_.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex
                            + commentEndExpression_.matchedLength();
        }
        setFormat(startIndex, commentLength, commentFormat_);
        startIndex = commentStartExpression_.indexIn(text, startIndex + commentLength);
    }
}
