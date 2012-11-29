#ifndef PLUGINWINDOW_H
#define PLUGINWINDOW_H

#include <QMainWindow>

namespace Ui {
class PluginWindow;
}

class PluginWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit PluginWindow(QWidget *parent = 0);
    ~PluginWindow();
    
private:
    Ui::PluginWindow *ui;
};

#endif // PLUGINWINDOW_H
