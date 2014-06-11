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
        "#version 140\n"
        "// vertex attributes\n"
        "in vec3 a_normal;\n"
        "in vec4 a_color;\n"
        "in vec4 a_position;\n"
        "// shader uniforms (application specific)\n"
        "uniform mat4 u_projection;\n"
        "uniform mat4 u_view;\n"
        "// output of vertex shader\n"
        "out vec3 v_normal;\n"
        "out vec4 v_color;\n"
        "out vec3 v_pos;\n"
        "\n"
        "void main()\n"
        "{\n"
        "\t// pass attributes to fragment shader\n"
        "\tv_normal = a_normal;\n"
        "\tv_color = a_color;\n"
        "\tv_pos = a_position.xyz;\n"
        "\t// set final vertex position\n"
        "\tgl_Position = u_projection * u_view * a_position;\n"
        "}\n";

const QString default_fragment_source =
        "#version 140\n"
        "// input from vertex shader\n"
        "in vec3 v_normal;\n"
        "in vec4 v_color;\n"
        "in vec3 v_pos;\n"
        "// shader uniforms (user)\n"
        "uniform vec3 u_light_pos;\n"
        "uniform vec3 u_light_color;\n"
        "uniform float u_shinyness;\n"
        "// output to rasterizer\n"
        "out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "\t// 'ambient color' or base color\n"
        "\tvec3 ambcol = v_color.xyz;\n"
        "\t// normal to light source\n"
        "\tvec3 light_normal = normalize( u_light_pos - v_pos );\n"
        "\t// dot-product of light normal and vertex normal gives linear light influence\n"
        "\tfloat d = max(0.0, dot(v_normal, light_normal) );\n"
        "\t// shaping the linear light influence\n"
        "\tfloat lighting = pow(d, 1.0 + u_shinyness);\n"
        "\t// adding the light to the base color\n"
        "\tvec3 col = ambcol + lighting * u_light_color;\n"
        "\t// typical output of a fragment shader\n"
        "\tcolor = vec4(clamp(col, 0.0, 1.0), 1.0);\n"
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
    defaultValues_.insert("image_path", QString("./"));
    defaultValues_.insert("auto_compile", true);

    defaultValues_.insert("image0", QString(""));
    defaultValues_.insert("image1", QString(""));
    defaultValues_.insert("image2", QString(""));
    defaultValues_.insert("image3", QString(""));

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
