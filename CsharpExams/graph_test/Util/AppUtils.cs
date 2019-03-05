using System;
using System.Reflection;
using System.Threading;

namespace graph_test.Utils
{
    public class AppUtils
    {
        #region 获取当前APP名称
        /// <summary>
        /// 获取当前APP名称
        /// </summary>
        /// 
        /// <returns></returns>
        /// 
        #endregion

        private static AppUtils instance;

        private String mAppName;


        private SynchronizationContext mMainLoopHandle = null;

        public static AppUtils GetInstance()
        {
            if (instance == null)
            {
                instance = new AppUtils();
                instance.mMainLoopHandle = SynchronizationContext.Current;
            }
            return instance;
        }

        public String GetAppName()
        {
            if (mAppName == null)
                mAppName = Assembly.GetExecutingAssembly().GetName().Name.ToString();

            return mAppName;
        }

        public SynchronizationContext getMainLoopHandle()
        {
            return mMainLoopHandle;
        }
    }
}
