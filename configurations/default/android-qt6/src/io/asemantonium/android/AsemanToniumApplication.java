/*
    Copyright (C) 2017 AsemanTonium Team
    http://asemantonium.co

    AsemanToniumQtTools is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AsemanToniumQtTools is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

package io.asemantonium.android;

import org.qtproject.qt.android.bindings.QtApplication;

import android.app.Application;
import android.content.Intent;
import android.content.Context;

public class AsemanToniumApplication extends QtApplication
{
    private static Context context;
    private static Application app_instance = null;

    public void onCreate(){
        super.onCreate();
        app_instance = this;
        AsemanToniumApplication.context = getApplicationContext();
    }

    public static Context getAppContext() {
        return AsemanToniumApplication.context;
    }

    public static Context instance() {
        return app_instance;
    }
}
