// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "file_signal_reader.h"
#include <QSettings>


namespace sigviewer {

int FileSignalReader::setEventTypeColors()
{
    // Display each event type in a distinct color
    QSharedPointer<ColorManager> colorPicker = ApplicationContextImpl::getInstance()->color_manager_;

    //set event colors
    srand (time(NULL));     /* initialize random seed: */

    QVector<QColor> eventColorList = {"#0055ff", "#00aa00", "#aa00ff", "#ff0000", "#00557f",
                                 "#5555ff", "#ff55ff", "#00aaff", "#00aa7f", "#ff5500"};

    int colorChoice = 5;    //Set the first event color to be pink

    QSettings settings;
    settings.beginGroup("ColorManager");
    int size = settings.beginReadArray("event");

    for (int type = 0; type < 254; type++)
    {
        QColor color;
        if (type < size)
        {
            settings.setArrayIndex(type);
            color = settings.value("color").toString();
            color.setAlpha(settings.value("alpha").toInt());
        }

        /* if the user has specified color setting for the current event type previously
         * in QSettings, we want to use it.
         * If the color setting is #0000ff (default), we assume the user
         * hasn't specified color for the current event type yet. Below is our algorithm
         * to pick some good colors for event types. */

        if (color.name().compare("#0000ff", Qt::CaseInsensitive) == 0)
        {
            // generate random number first:
            int red = rand() % 41 + (-20);
            int green = rand() % 41 + (-20);
            int blue = rand() % 41 + (-20);

            colorChoice++;
            if (colorChoice == 10)   //we only have 10 basic colors
                colorChoice = 0;

            color = eventColorList[colorChoice];

            red += color.red();
            green += color.green();
            blue += color.blue();

            if (red < 0)
                red = 0;
            if (red > 255)
                red = 255;
            if (green < 0)
                green = 0;
            if (green > 255)
                green = 255;
            if (blue < 0)
                blue = 0;
            if (blue > 255)
                blue = 255;


            color.setRgb(red, green, blue);
            color.setAlpha(120);
        }
        colorPicker->setEventColor(type, color); //QColor(0, 85, 255, 80)
        eventColorList[colorChoice] = color;
    }

    settings.endArray();
    settings.endGroup();

    colorPicker->saveSettings();

    return 0;
}

}
