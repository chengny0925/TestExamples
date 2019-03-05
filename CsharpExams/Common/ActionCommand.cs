using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Input;

namespace XXXX.Data.DataField
{
    //
    // 摘要:
    //     Provides an ICommand derived class allowing delegates to be invokved directly
    //     on the view model
    //
    // 类型参数:
    //   T:
    //     The Type of the command parameter
    public class ActionCommand<T> : ICommand where T : class
    {
        private readonly Action<T> _execute;
        private readonly Predicate<T> _canExecute;
        private EventHandler _event;
        //
        // 摘要:
        //     Initializes a new instance of the Abt.Controls.SciChart.ActionCommand`1 class.
        //
        // 参数:
        //   execute:
        //     The execute delegate.
        public ActionCommand(Action<T> execute) : this(execute, null)
        {
        }
        //
        // 摘要:
        //     Initializes a new instance of the Abt.Controls.SciChart.ActionCommand`1 class.
        //
        // 参数:
        //   execute:
        //     The execute delegate.
        //
        //   canExecute:
        //     The can execute predicate.
        public ActionCommand(Action<T> execute, Predicate<T> canExecute)
        {
            if (execute == null)
            {
                throw new ArgumentNullException("execute cannot be null");
            }
            _execute = execute;
            _canExecute = canExecute;
        }

        //
        // 摘要:
        //     Occurs when changes occur that affect whether or not the command should execute.
        public event EventHandler CanExecuteChanged
        {
            add
            {
                EventHandler eventHandler = this._event;
                EventHandler eventHandler2;
                do
                {
                    eventHandler2 = eventHandler;
                    EventHandler value2 = (EventHandler)Delegate.Combine(eventHandler2, value);
                    eventHandler = Interlocked.CompareExchange<EventHandler>(ref this._event, value2, eventHandler2);
                }
                while (eventHandler != eventHandler2);
            }
            remove
            {
                EventHandler eventHandler = this._event;
                EventHandler eventHandler2;
                do
                {
                    eventHandler2 = eventHandler;
                    EventHandler value2 = (EventHandler)Delegate.Remove(eventHandler2, value);
                    eventHandler = Interlocked.CompareExchange<EventHandler>(ref this._event, value2, eventHandler2);
                }
                while (eventHandler != eventHandler2);
            }
        }

        //
        // 摘要:
        //     Defines the method that determines whether the command can execute in its current
        //     state.
        //
        // 参数:
        //   parameter:
        //     Data used by the command. If the command does not require data to be passed,
        //     this object can be set to null.
        //
        // 返回结果:
        //     true if this command can be executed; otherwise, false.
        public bool CanExecute(object parameter)
        {
            if(_canExecute != null)
            {
                return _canExecute((T)parameter);
            }
            return true;
        }
        //
        // 摘要:
        //     Defines the method to be called when the command is invoked.
        //
        // 参数:
        //   parameter:
        //     Data used by the command. If the command does not require data to be passed,
        //     this object can be set to null.
        public void Execute(object parameter)
        {
            T t = parameter as T;
            if (parameter != null && t == null)
            {
                throw new InvalidOperationException(string.Concat(new object[]
                {
                    "Wrong type of parameter being passed in.  Expected [",
                    typeof(T),
                    "]but was [",
                    parameter.GetType(),
                    "]"
                }));
            }
            if (!this.CanExecute(t))
            {
                throw new InvalidOperationException("Should not try to execute command that cannot be executed");
            }
            this._execute(t);
        }
        //
        // 摘要:
        //     Raises the CanExecuteChanged event
        public void RaiseCanExecuteChanged()
        {
            EventHandler eventHandler = this._event;
            if (eventHandler != null)
            {
                eventHandler(this, EventArgs.Empty);
            }
        }
    }
    public class ActionCommand : ActionCommand<object>
    {
        private sealed class ActionHelper
        {
            public Action _execute;

            public void Execute(object obj)
            {
                this._execute();
            }
        }

        private sealed class ActionCanExecuteHelper
        {
            public Action _execute;

            public Func<bool> _canExecute;

            public void Execute(object obj)
            {
                this._execute();
            }

            public bool CanExecute(object obj)
            {
                return this._canExecute();
            }
        }
        public ActionCommand(Action execute) : base(Convert(execute))
        {
            //ActionHelper helper = new ActionHelper();
            //helper._execute = execute;
            //Action<object> execute2 = new Action<object>(helper.Execute);
            //base..ctor(execute2);
        }

        public static Action<object> Convert(Action execute)
        {
            ActionHelper helper = new ActionHelper();
            helper._execute = execute;
            Action<object> execute2 = new Action<object>(helper.Execute);
            return execute2;
        }

        public ActionCommand(Action execute, Func<bool> canExecute) : base(Convert(execute), Convert(canExecute))
        {
            //ActionCanExecuteHelper helper = new ActionCanExecuteHelper();
            //helper._execute = execute;
            //helper._canExecute = canExecute;
            //Action<object> action = new Action<object>(helper.Execute);
            //Predicate<object> canExecute2 = new Predicate<object>(helper.CanExecute);
            //base..ctor(action, canExecute2);
        }

        public static Predicate<object> Convert(Func<bool> canExecute)
        {
            ActionCanExecuteHelper helper = new ActionCanExecuteHelper();
            helper._canExecute = canExecute;
            Predicate<object> canExecute2 = new Predicate<object>(helper.CanExecute);
            return canExecute2;
        }
    }
}
