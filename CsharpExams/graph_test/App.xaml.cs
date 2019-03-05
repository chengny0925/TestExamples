using graph_test.Utils;
using LogUtil;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace graph_test
{
    /// <summary>
    /// App.xaml 的交互逻辑
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            LogHelper.SetConfig("log4net.config");
            base.OnStartup(e);
            AppUtils.GetInstance();

            // appDomain 异常捕获
            AppDomain.CurrentDomain.UnhandledException += CurrentDomainOnUnhandledException;


        }

        private void CurrentDomainOnUnhandledException(object sender, UnhandledExceptionEventArgs unhandledExceptionEventArgs)
        {
            LogHelper.CurrentLogger.Fatal("Fatal: " + unhandledExceptionEventArgs.ExceptionObject);
        }

        protected override void OnExit(ExitEventArgs e)
        {
            Thread workerThread = new Thread(enviromentExit);
            workerThread.Start();
        }

        private void enviromentExit()
        {
            Thread.Sleep(3000);
            Environment.Exit(0);
        }
    }
}
