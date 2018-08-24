// java file goes in android/src/com/kdab/training/MyService.java
package ru.minlexx.qmlevemon;
 
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import org.qtproject.qt5.android.bindings.QtService;
 
public class QMLEVEMonService extends QtService
{
    public static void startMyService(Context ctx) {
        ctx.startService(new Intent(ctx, QMLEVEMonService.class));
    }
    
    public static void stopMyService(Context ctx) {
        ctx.stopService(new Intent(ctx, QMLEVEMonService.class));
    }
} 
