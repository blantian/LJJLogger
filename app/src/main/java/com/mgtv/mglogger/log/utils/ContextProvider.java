/**
 * Copyright 2012 LeTV Technology Co. Ltd., Inc. All rights reserved.
 *
 * @Author : qingxia
 * @Description :
 */

package com.mgtv.mglogger.log.utils;

import android.content.Context;

/**
 * description: This class provide a global application context.
 * <p>
 * author: Created by xianggengping on 2017/10/18.
 * <p>
 * email: gengping@mgtv.com
 */
public final class ContextProvider {
    private static Context sContext = null;

    public static void initIfNotInited(Context context) {
        if (sContext == null) {
            init(context);
        }
    }

    /**
     * This function should be invoked in Application while the
     * application is been
     * created.
     * @param context
     */
    public static void init(Context context) {
        if (context == null) {
            throw new NullPointerException(
                    "Can not use null initialized application context");
        }
        sContext = context;
    }

    /**
     * Get application context.
     * @return
     */
    public static Context getApplicationContext() {
        if (sContext == null) {
            throw new NullPointerException("Global application uninitialized");
        }
        return sContext;
    }

    private ContextProvider() {
    }
}
