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

#include "appsettings.h"

const QString default_vertex_source =
        "void main()\n"
        "{\n"
        "   glFragCoord = ftransform();\n"
        "}\n";

const QString default_fragment_source =
        "void main()\n"
        "{\n"
        "   glFragColor = vec4(1.0, 0.7, 0.5, 1.0);\n"
        "}\n";

// the single instance
AppSettings * appSettings;

AppSettings::AppSettings(QObject *parent) :
    QSettings(
        // we prefere ini-format for better control/removal
        // (no headbanging with registry on windows)
        QSettings::IniFormat,
        // unique settings for each user
        QSettings::UserScope,
        // identifier
        "modular-audio-graphics",
        "scheeder",
        parent)
{
    createDefaultValues_();
}


void AppSettings::createDefaultValues_()
{
    defaultValues_.insert("vertex_source", default_vertex_source);
    defaultValues_.insert("fragment_source", default_fragment_source);
    defaultValues_.insert("source_path", QString("./"));
}



QVariant AppSettings::getValue(const QString &key) const
{
    // return from saved settings
    if (contains(key))
        return value(key);

    // check if in default settings
    Q_ASSERT(defaultValues_.contains(key));

    // return from default settings
    if (defaultValues_.contains(key))
        return defaultValues_.value(key);

    // NOT FOUND!
    return QVariant();
}
