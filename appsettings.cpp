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

#include <QWidget>

#include "appsettings.h"

const QString default_vertex_source =
        "varying vec3 v_normal;\n"
        "varying vec4 v_color;\n"
        "varying vec3 v_pos;\n"
        "\n"
        "void main()\n"
        "{\n"
        "\tv_normal = gl_Normal;\n"
        "\tv_color = gl_Color;\n"
        "\tv_pos = gl_Vertex.xyz;\n"
        "\tgl_Position = ftransform();\n"
        "};\n";

const QString default_fragment_source =
        "varying vec3 v_normal;\n"
        "varying vec4 v_color;\n"
        "varying vec3 v_pos;\n"
        "const vec3 light_pos = vec3(10., 10., 10.);\n"
        "const vec3 light_color = vec3(1., 1., 1.);\n"
        "\n"
        "void main()\n"
        "{\n"
        "\tvec3 ambcol = v_color.xyz;\n"
        "\tvec3 light_normal = normalize( light_pos - v_pos );\n"
        "\tfloat d = max(0.0, dot(v_normal, light_normal) );\n"
        "\n"
        "\tvec3 col = ambcol + light_color * d;\n"
        "\n"
        "\tgl_FragColor = vec4(clamp(col, 0.0, 1.0), 1.0);\n"
        "};\n";

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
    defaultValues_.insert("auto_compile", true);

    defaultValues_.insert("RenderSettings/doDepthTest", true);
    defaultValues_.insert("RenderSettings/doCullFace", true);
    defaultValues_.insert("RenderSettings/doFrontFaceCCW", true);
    defaultValues_.insert("RenderSettings/doDrawCoords", true);

    defaultValues_.insert("ShaderAttributes/position",  "a_position");
    defaultValues_.insert("ShaderAttributes/color",     "a_color");
    defaultValues_.insert("ShaderAttributes/normal",    "a_normal");
    defaultValues_.insert("ShaderUniforms/projection",  "u_projection");
    defaultValues_.insert("ShaderUniforms/view",        "u_view");
    defaultValues_.insert("ShaderUniforms/time",        "u_time");
    defaultValues_.insert("ShaderUniforms/aspect",      "u_aspect");

    // install them if not present already
    auto keys = defaultValues_.keys();
    for (int i=0; i<keys.size(); ++i)
    {
        if (!contains(keys[i]))
            setValue(keys[i], defaultValues_[keys[i]]);
    }
}

QStringList AppSettings::getShaderAttributes()
{
    beginGroup("ShaderAttributes");
    QStringList keys = childKeys();
    endGroup();

    QStringList values;
    for (auto &k : keys)
        values << getValue("ShaderAttributes/"+k).toString();

    return values;
}

QStringList AppSettings::getShaderUniforms()
{
    beginGroup("ShaderUniforms");
    QStringList keys = childKeys();
    endGroup();

    QStringList values;
    for (auto &k : keys)
        values << getValue("ShaderUniforms/"+k).toString();

    return values;
}

QVariant AppSettings::getValue(const QString &key) const
{
    // return from saved settings
    if (contains(key))
        return value(key);

    // check if in default settings
    Q_ASSERT_X(defaultValues_.contains(key), "AppSettings::getValue", key.toStdString().c_str());

    // return from default settings
    if (defaultValues_.contains(key))
        return defaultValues_.value(key);

    // NOT FOUND!
    return QVariant();
}

void AppSettings::setLayout(QWidget * w)
{
    if (!w || w->objectName().isEmpty())
        return;

    setValue(w->objectName(), w->saveGeometry());
}

void AppSettings::getLayout(QWidget * w)
{
    if (!w || w->objectName().isEmpty())
        return;

    if (!contains(w->objectName()))
        return;

    QByteArray a = getValue(w->objectName()).toByteArray();
    w->restoreGeometry(a);
}
