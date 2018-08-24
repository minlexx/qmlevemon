package ru.minlexx.qmlevemon;
 
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import org.qtproject.qt5.android.bindings.QtService;


public class QMLEVEMonService extends QtService
{
    private static final String TAG = "qmlevemon_service";

    public static void startMyService(Context ctx) {
        Log.d(TAG, "Starting service");
        ctx.startService(new Intent(ctx, QMLEVEMonService.class));
    }
    
    public static void stopMyService(Context ctx) {
        Log.d(TAG, "Stopping service");
        ctx.stopService(new Intent(ctx, QMLEVEMonService.class));
    }
} 
