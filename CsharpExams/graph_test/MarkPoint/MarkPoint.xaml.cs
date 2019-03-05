using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace graph_test.MarkPoint
{
    /// <summary>
    /// MarkPoint.xaml 的交互逻辑
    /// </summary>
    public partial class MarkPoint : UserControl
    {
        public MarkPoint()
        {
            InitializeComponent();
        }


        #region
        public Point Point
        {
            get { return (Point)GetValue(PointProperty); }
            set { SetValue(PointProperty, value); }
        }

        public static readonly DependencyProperty PointProperty =
         DependencyProperty.Register("Point", typeof(Point), typeof(MarkPoint), new PropertyMetadata(new Point(8, 8), PointChanged));

        public string PointDisplay
        {
            get { return (string)GetValue(PointDisplayProperty); }
            set { SetValue(PointDisplayProperty, value); }
        }

        public static readonly DependencyProperty PointDisplayProperty =
         DependencyProperty.Register("PointDisplay", typeof(string), typeof(MarkPoint), new PropertyMetadata("", null));

        private static void PointChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            MarkPoint mp = (MarkPoint)d;
            if (e.NewValue != e.OldValue)
            {
                Point p = (Point)e.NewValue;
                Canvas.SetTop(mp.thm, p.Y - 8);
                Canvas.SetLeft(mp.thm, p.X - 8);
                mp.PointDisplay = string.Format("{0},{1}", p.X, p.Y);
                RoutedPropertyChangedEventArgs<Point> args = new RoutedPropertyChangedEventArgs<Point>((Point)e.OldValue, (Point)e.NewValue, PointMoveEvent);
                mp.RaiseEvent(args);
            }
        }


        public static readonly RoutedEvent PointMoveEvent = 
            EventManager.RegisterRoutedEvent("PointMoveEvent", RoutingStrategy.Bubble, typeof(RoutedPropertyChangedEventArgs<Object>), typeof(MarkPoint));
        /// <summary>
        /// 处理各种路由事件的方法 
        /// </summary>
        public event RoutedEventHandler PointMove
        {
            //将路由事件添加路由事件处理程序
            add { AddHandler(PointMoveEvent, value); }
            //从路由事件处理程序中移除路由事件
            remove { RemoveHandler(PointMoveEvent, value); }
        }

        #endregion


        private void thm_DragDelta(object sender, System.Windows.Controls.Primitives.DragDeltaEventArgs e)
        {
            Thumb thm = (Thumb)sender;
            double y = Canvas.GetTop(thm) + e.VerticalChange;
            double x = Canvas.GetLeft(thm) + e.HorizontalChange;
            Canvas.SetTop(thm, Canvas.GetTop(thm) + e.VerticalChange);
            Canvas.SetLeft(thm, Canvas.GetLeft(thm) + e.HorizontalChange);

            this.Point = new Point(x + thm.ActualWidth / 2, y + thm.ActualHeight / 2);
        }

        internal void MovePoint(Point oldValue, Point newValue)
        {
            if(Point == oldValue)
            {
                this.Point = newValue;
            }
            else
            {
                Console.WriteLine("error");
            }
        }
    }
}
