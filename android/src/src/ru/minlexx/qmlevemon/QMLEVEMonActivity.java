package ru.minlexx.qmlevemon;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
//import android.util.Log;

public class QMLEVEMonActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static NotificationManager m_notificationManager;
    private static Notification.Builder m_builder;
    private static QMLEVEMonActivity m_instance;
    private static final String TAG = "qmlevemon";

    public QMLEVEMonActivity()
    {
        m_instance = this;
    }

    public static void notify(String title, String text)
    {
        if (m_notificationManager == null) {
            m_notificationManager = (NotificationManager)m_instance.getSystemService(Context.NOTIFICATION_SERVICE);
            m_builder = new Notification.Builder(m_instance);
            m_builder.setSmallIcon(R.drawable.icon);
        }

        m_builder.setContentTitle(title);
        m_builder.setContentText(text);
        m_notificationManager.notify(1, m_builder.build());

        //Log.d(TAG, "notify()");
    }
}
