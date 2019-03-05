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

namespace graph_test.MarkLine
{
    /// <summary>
    /// MarkLine.xaml 的交互逻辑
    /// </summary>
    public partial class MarkLine : UserControl
    {
        public MarkLine()
        {
            InitializeComponent();

            this.lineMain.PreviewMouseLeftButtonUp += LineMain_PreviewMouseLeftButtonUp;
        }


        private void LineMain_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            this.lineMain.Stroke = new SolidColorBrush(Colors.Yellow);
        }

        #region 端点数据集


        public PointCollection DataPoints
        {
            get { return (PointCollection)GetValue(DataPointsProperty); }
            set { SetValue(DataPointsProperty, value); }
        }

        public Point StartPoint
        {
            get { return (Point)GetValue(StartPointProperty); }
            set { SetValue(StartPointProperty, value); }
        }

        public Point EndPoint
        {
            get { return (Point)GetValue(EndPointProperty); }
            set { SetValue(EndPointProperty, value); }
        }

        // Using a DependencyProperty as the backing store for DataPoints.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty DataPointsProperty =
            DependencyProperty.Register("DataPoints", typeof(PointCollection), typeof(MarkLine), new PropertyMetadata(new PointCollection() { new Point(8, 8), new Point(16, 16) }, DataPointsChanged));

        public static readonly DependencyProperty StartPointProperty =
           DependencyProperty.Register("StartPoint", typeof(Point), typeof(MarkLine), new PropertyMetadata( new Point(8, 8), StartPointChanged));

        public static readonly DependencyProperty EndPointProperty =
          DependencyProperty.Register("EndPoint", typeof(Point), typeof(MarkLine), new PropertyMetadata(new Point(8, 8), EndPointChanged));

        private static void DataPointsChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            MarkLine line = (MarkLine)d;
            if (e.NewValue != e.OldValue)
            {
                //数据集改变时重新绘制线段和设置端点圆形位置
                PointCollection points = (PointCollection)e.NewValue;
                //Canvas.SetTop(line.thmStart, points[0].Y - 8);
                //Canvas.SetLeft(line.thmStart, points[0].X - 8);
                //Canvas.SetTop(line.thmEnd, points[1].Y - 8);
                //Canvas.SetLeft(line.thmEnd, points[1].X - 8);
                line.StartPoint = points[0];
                line.EndPoint = points[1];
            }
        }

        private static void StartPointChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            MarkLine line = (MarkLine)d;
            if (e.NewValue != e.OldValue)
            {
                Point p = (Point)e.NewValue;
                Canvas.SetTop(line.thmStart, p.Y - 8);
                Canvas.SetLeft(line.thmStart, p.X - 8);
            }
        }

        private static void EndPointChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            MarkLine line = (MarkLine)d;
            if (e.NewValue != e.OldValue)
            {
                Point p = (Point)e.NewValue;
                Canvas.SetTop(line.thmEnd, p.Y - 8);
                Canvas.SetLeft(line.thmEnd, p.X - 8);
            }
        }

        public static readonly RoutedEvent MarkPointMovedEvent =
            EventManager.RegisterRoutedEvent("MarkPointMovedEvent", RoutingStrategy.Bubble, typeof(RoutedPropertyChangedEventArgs<Object>), typeof(MarkLine));

        /// <summary>
        /// 处理各种路由事件的方法 
        /// </summary>
        public event RoutedEventHandler MarkPointMoved
        {
            //将路由事件添加路由事件处理程序
            add { AddHandler(MarkPointMovedEvent, value); }
            //从路由事件处理程序中移除路由事件
            remove { RemoveHandler(MarkPointMovedEvent, value); }
        }


        #endregion

        private PointCollection _points;

        private void thmStart_DragDelta(object sender, System.Windows.Controls.Primitives.DragDeltaEventArgs e)
        {
            Thumb thm = (Thumb)sender;
            double y = Canvas.GetTop(thm) + e.VerticalChange;
            double x = Canvas.GetLeft(thm) + e.HorizontalChange;
            Canvas.SetTop(thm, Canvas.GetTop(thm) + e.VerticalChange);
            Canvas.SetLeft(thm, Canvas.GetLeft(thm) + e.HorizontalChange);
            _points = this.DataPoints;
            Point old = _points[0];
            this.DataPoints = new PointCollection()
            {
                new Point(x+thm.ActualWidth/2,y+thm.ActualHeight/2),
                _points[1]
            };

            LinePointMovedRoutedEventArgs args = new LinePointMovedRoutedEventArgs(old, DataPoints[0], MarkPointMovedEvent, this);
            this.RaiseEvent(args);
        }

        private void thmEnd_DragDelta(object sender, System.Windows.Controls.Primitives.DragDeltaEventArgs e)
        {
            Thumb thm = (Thumb)sender;
            double y = Canvas.GetTop(thm) + e.VerticalChange;
            double x = Canvas.GetLeft(thm) + e.HorizontalChange;
            Canvas.SetTop(thm, Canvas.GetTop(thm) + e.VerticalChange);
            Canvas.SetLeft(thm, Canvas.GetLeft(thm) + e.HorizontalChange);
            _points = this.DataPoints;
            Point old = _points[1];
            this.DataPoints = new PointCollection()
            {
                _points[0],
                new Point(x+thm.ActualWidth/2,y+thm.ActualHeight/2)
            };

            LinePointMovedRoutedEventArgs args = new LinePointMovedRoutedEventArgs(old, DataPoints[1], MarkPointMovedEvent, this);
            this.RaiseEvent(args);
        }

        internal void MovePoint(Point oldValue, Point newValue)
        {
            if(DataPoints[0] == oldValue)
            {
                _points = this.DataPoints;
                this.DataPoints = new PointCollection()
                {
                    newValue,
                    _points[1]
                };
            }
            else if(DataPoints[1] == oldValue)
            {
                _points = this.DataPoints;
                this.DataPoints = new PointCollection()
                {
                    _points[0],
                   newValue
                };
            }
            else
            {
                Console.WriteLine("error, should not come here");
            }
        }

    }



    public class LinePointMovedRoutedEventArgs : RoutedEventArgs
    {
        public Point OldValue { get; set; }
        public Point NewValue { get; set; }

        public LinePointMovedRoutedEventArgs(Point oldVal, Point newVal, RoutedEvent routedEvent, object source) : base(routedEvent, source)
        {
            OldValue = oldVal;
            NewValue = newVal;
        }
    }
}
