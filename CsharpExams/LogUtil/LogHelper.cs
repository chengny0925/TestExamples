using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using log4net;
using System.IO;

namespace LogUtil
{
    /// <summary>
    /// 日志种类
    /// </summary>
    public enum LogLevel
    {
        /// <summary>
        /// 普通信息
        /// </summary>
        Info,

        /// <summary>
        /// 调试信息
        /// </summary>
        Debug,

        /// <summary>
        /// 警告信息
        /// </summary>
        Warn,

        /// <summary>
        /// 错误信息
        /// </summary>
        Error,

        /// <summary>
        /// 重要信息
        /// </summary>
        Fatal
    }

    /// <summary>
    /// 日志接口
    /// </summary>
    public interface ILog
    {
        #region Method

        /// <summary>
        /// Fatal method
        /// </summary>
        /// <param name="msg">fatal info</param>
        /// <param name="ex">exception</param>
        void Fatal(string msg, System.Exception ex);

        /// <summary>
        /// Fatal method
        /// </summary>
        /// <param name="msg">custom info</param>
        void Fatal(object msg);

        /// <summary>
        /// Error method
        /// </summary>
        /// <param name="msg">error info</param>
        /// <param name="ex">exception</param>
        void Error(string msg, System.Exception ex);

        /// <summary>
        /// Error method
        /// </summary>
        /// <param name="msg">custom info</param>
        void Error(object msg);

        /// <summary>
        /// Warn Method
        /// </summary>
        /// <param name="msg">warn info</param>
        /// <param name="ex">exception</param>
        void Warn(string msg, System.Exception ex);

        /// <summary>
        /// Warn Method
        /// </summary>
        /// <param name="msg">custom info</param>
        void Warn(object msg);

        /// <summary>
        /// Info method
        /// </summary>
        /// <param name="msg">common info</param>
        /// <param name="ex">exception</param>
        void Info(string msg, System.Exception ex);

        /// <summary>
        /// Info method
        /// </summary>
        /// <param name="msg">custom info</param>
        void Info(object msg);

        /// <summary>
        /// Debug method
        /// </summary>
        /// <param name="msg">debug info</param>
        /// <param name="ex">exception</param>
        void Debug(string msg, System.Exception ex);

        /// <summary>
        /// Debug method
        /// </summary>
        /// <param name="msg">custom info</param>
        void Debug(object msg);

        #endregion
    }

    /// <summary>
    /// 日志帮助类
    /// </summary>
    public class LogHelper : ILog
    {
        public static void SetConfig()
        {
            log4net.Config.XmlConfigurator.Configure();
        }

        public static void SetConfig(string configFile)
        {
            log4net.Config.XmlConfigurator.Configure(new FileInfo(configFile));
        }
        /// <summary>
        /// 日志接口变量
        /// </summary>
        private static readonly log4net.ILog _log = LogManager.GetLogger("Logger");

        /// <summary>
        /// 日志单例变量
        /// </summary>
        private static LogHelper _logger;

        /// <summary>
        /// 当前日志对象
        /// </summary>
        public static LogHelper CurrentLogger
        {
            get
            {
                if (_logger == null)
                {
                    _logger = new LogHelper();
                }
                return _logger;
            }
        }

        #region ILog 成员

        /// <summary>
        /// 重要信息
        /// </summary>
        /// <param name="msg">信息</param>
        /// <param name="ex">异常</param>
        public void Fatal(string msg, System.Exception ex)
        {
            if (_log.IsFatalEnabled)
            {
                _log.Fatal(msg, ex);
            }
        }

        /// <summary>
        /// 重要信息
        /// </summary>
        /// <param name="msg">自定义信息</param>
        public void Fatal(object msg)
        {
            if (_log.IsFatalEnabled)
            {
                _log.Fatal(msg);
            }
        }

        /// <summary>
        /// 错误信息
        /// </summary>
        /// <param name="msg">信息</param>
        /// <param name="ex">异常</param>
        public void Error(string msg, System.Exception ex)
        {
            if (_log.IsErrorEnabled)
            {
                _log.Error(msg, ex);
            }
        }

        /// <summary>
        /// 错误信息
        /// </summary>
        /// <param name="msg">自定义信息</param>
        public void Error(object msg)
        {
            if (_log.IsErrorEnabled)
            {
                _log.Error(msg);
            }
        }

        /// <summary>
        /// 警告信息
        /// </summary>
        /// <param name="msg">信息</param>
        /// <param name="ex">异常</param>
        public void Warn(string msg, System.Exception ex)
        {
            if (_log.IsWarnEnabled)
            {
                _log.Warn(msg, ex);
            }
        }

        /// <summary>
        /// 警告信息
        /// </summary>
        /// <param name="msg">自定义信息</param>
        public void Warn(object msg)
        {
            if (_log.IsWarnEnabled)
            {
                _log.Warn(msg);
            }
        }

        /// <summary>
        /// 普通信息
        /// </summary>
        /// <param name="msg">信息</param>
        /// <param name="ex">异常</param>
        public void Info(string msg, System.Exception ex)
        {
            if (_log.IsInfoEnabled)
            {
                _log.Info(msg, ex);
            }
        }

        /// <summary>
        /// 普通信息
        /// </summary>
        /// <param name="msg">自定义信息</param>
        public void Info(object msg)
        {
            if (_log.IsInfoEnabled)
            {
                _log.Info(msg);
            }
        }

        /// <summary>
        /// 调试信息
        /// </summary>
        /// <param name="msg">信息</param>
        /// <param name="ex">异常</param>
        public void Debug(string msg, System.Exception ex)
        {
            if (_log.IsDebugEnabled)
            {
                _log.Debug(msg, ex);
            }
        }

        /// <summary>
        /// 调试信息
        /// </summary>
        /// <param name="msg">信息</param>
        public void Debug(object msg)
        {
            if (_log.IsDebugEnabled)
            {
                _log.Debug(msg);
            }
        }

        #endregion
    }
}

