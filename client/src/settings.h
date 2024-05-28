#pragma once 
#include "mainwindow.h"


class Settings {
protected:

	void saveSettings(nlohmann::json& configs);
};