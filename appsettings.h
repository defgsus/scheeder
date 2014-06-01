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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>

class AppSettings : public QSettings
{
    Q_OBJECT
public:
    explicit AppSettings(QObject *parent = 0);

    /** Returns a map with default settings */
    const QMap<QString, QVariant>& defaultValues() const
    { return defaultValues_; }

    /** Return a setting for a key.
        If the key is not found in the settings,
        defaultValues() will be queried.
        If it's not found there an empty QVariant will be returned.
        @note asserts in debug-mode for unknown keys!! */
    QVariant getValue(const QString& key) const;

signals:

public slots:

private:

    void createDefaultValues_();

    QMap<QString, QVariant> defaultValues_;
};

/** Single instance */
extern AppSettings * appSettings;

#endif // APPSETTINGS_H
