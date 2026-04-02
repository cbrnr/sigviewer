// © SigViewer developers
//
// License: GPL-3.0

#include "application_context.h"

#include <QDebug>

#include "base/exception.h"
#include "gui/main_window_model.h"

namespace sigviewer {

//-----------------------------------------------------------------------------
QSharedPointer<ApplicationContext> ApplicationContext::getInstance(bool cleanup) {
    static QSharedPointer<ApplicationContext> instance(new ApplicationContext);
    if (cleanup) instance.clear();
    if (instance.isNull()) {
        instance = QSharedPointer<ApplicationContext>(new ApplicationContext);
        qDebug() << "ApplicationContext::getInstance() created new instance = "
                 << instance.data();
    }
    return instance;
}

//-------------------------------------------------------------------------
void ApplicationContext::init(std::set<ApplicationMode> activated_modes) {
    qDebug() << "ApplicationContext::init() instance = " << getInstance();

    getInstance()->activated_modes_ = activated_modes;
    getInstance()->setState(APP_STATE_NO_FILE_OPEN);
    if (activated_modes.count(APPLICATION_NON_GUI_MODE) == 0) {
        getInstance()->color_manager_ = QSharedPointer<ColorManager>(new ColorManager);
        getInstance()->main_window_model_ =
            QSharedPointer<MainWindowModel>(new MainWindowModel(getInstance()));
    }
}

//-------------------------------------------------------------------------
void ApplicationContext::cleanup() {
    qDebug() << "ApplicationContext::cleanup()";
    getInstance(true);
}

//-----------------------------------------------------------------------------
ApplicationContext::~ApplicationContext() { qDebug() << "deleting ApplicationContext"; }

//-------------------------------------------------------------------------
bool ApplicationContext::modeActivated(ApplicationMode mode) const {
    return activated_modes_.count(mode) > 0;
}

//-------------------------------------------------------------------------
QSharedPointer<FileContext> ApplicationContext::getCurrentFileContext() {
    return current_file_context_;
}

//-------------------------------------------------------------------------
void ApplicationContext::setCurrentTabContext(QSharedPointer<TabContext> tab_context) {
    if (!current_tab_context_.isNull()) current_tab_context_->disconnect(this);
    current_tab_context_ = tab_context;
    if (!connect(current_tab_context_.data(),
            SIGNAL(selectionStateChanged(TabSelectionState)),
            SIGNAL(currentTabSelectionStateChanged(TabSelectionState))))
        throw(
            Exception(tr("ApplicationContext::setCurrentTabContext failed to connect "
                         "to selectionStateChanged")
                    .toStdString()));
    if (!connect(current_tab_context_.data(),
            SIGNAL(editStateChanged(TabEditState)),
            SIGNAL(currentTabEditStateChanged(TabEditState))))
        throw(
            Exception(tr("ApplicationContext::setCurrentTabContext failed to connect "
                         "to editStateChanged")
                    .toStdString()));
}

//-------------------------------------------------------------------------
QSharedPointer<CommandExecuter> ApplicationContext::getCurrentCommandExecuter() {
    return current_tab_context_;
}

//-------------------------------------------------------------------------
void ApplicationContext::addFileContext(QSharedPointer<FileContext> file_context) {
    current_file_context_ = file_context;
    if (!connect(current_file_context_.data(),
            SIGNAL(stateChanged(FileState)),
            SIGNAL(currentFileStateChanged(FileState))))
        throw(
            Exception(tr("ApplicationContext::addFileContext faild to connect "
                         "stateChanged(FileState)")
                    .toStdString()));
    setState(APP_STATE_FILE_OPEN);
}

//-------------------------------------------------------------------------
void ApplicationContext::removeCurrentFileContext() {
    if (!current_file_context_.isNull()) current_file_context_->disconnect();
    current_file_context_ = QSharedPointer<FileContext>(0);
    setState(APP_STATE_NO_FILE_OPEN);
}

//-----------------------------------------------------------------------------
QSharedPointer<MainWindowModel> ApplicationContext::getMainWindowModel() {
    return main_window_model_;
}

//-------------------------------------------------------------------------
QSharedPointer<ColorManager> ApplicationContext::getEventColorManager() {
    return color_manager_;
}

//-----------------------------------------------------------------------------
void ApplicationContext::setState(ApplicationState state) {
    qDebug() << "ApplicationContext::setState " << state << "; this = " << this;
    state_ = state;
    emit stateChanged(state_);
}

}  // namespace sigviewer
