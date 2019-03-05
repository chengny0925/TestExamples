using graph_test.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace graph_test
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        bool hasDrawLines = false;
        List<MarkLine.MarkLine> markLines = new List<MarkLine.MarkLine>();
        List<MarkPoint.MarkPoint> markPoints = new List<MarkPoint.MarkPoint>();
        SelectPointsDataModel dataModel = new SelectPointsDataModel();

        public MainWindow()
        {
            InitializeComponent();

            Init();
        }

        private void Init()
        {
            foreach (var p in dataModel.AllPoints)
            {
                MarkPoint.MarkPoint point = new MarkPoint.MarkPoint();
                point.PreviewMouseLeftButtonUp += Point_PreviewMouseLeftButtonUp;
                point.Point = p;
                this.graphArea.Children.Add(point);
                markPoints.Add(point);
            }

            foreach(var item in markPoints)
            {
                item.PointMove += Point_PointMove;
            }


            //MarkPoint.MarkPoint point = new MarkPoint.MarkPoint();
            //point.Point = new Point(20,30);
            //this.graphArea.Children.Add(point);

            //MarkPoint.MarkPoint point2 = new MarkPoint.MarkPoint();
            //point2.Point = new Point(200, 230);
            //this.graphArea.Children.Add(point2);

            //MarkPoint.MarkPoint point3 = new MarkPoint.MarkPoint();
            //point3.Point = new Point(250, 30);
            //this.graphArea.Children.Add(point3);
        }

        private void Point_PointMove(object sender, RoutedEventArgs e)
        {
            var args = e as RoutedPropertyChangedEventArgs<Point>;
            Point old = args.OldValue;
            Point newPoint = args.NewValue;
            if(dataModel.AllPoints.Contains(old))
            {
                dataModel.AllPoints.Remove(old);
                dataModel.SelectPoints.Remove(old);
                dataModel.AllPoints.Add(newPoint);
                dataModel.SelectPoints.Add(newPoint);
            }
        }

        private void Point_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (hasDrawLines)
            {
                return;
            }

            MarkPoint.MarkPoint point = sender as MarkPoint.MarkPoint;
            dataModel.AddSelectPoint(point.Point);
            if (dataModel.CheckSelecPoints())
            {
                DrawLines();
            }
        }

        private void DrawLines()
        {
            for(int i = 0; i < dataModel.SelectPoints.Count; i++ )
            {
                for(int j = i+1;j < dataModel.SelectPoints.Count; j++)
                {
                    MarkLine.MarkLine line = new MarkLine.MarkLine();
                    line.DataPoints = new PointCollection()
                    {
                        dataModel.SelectPoints[i],
                        dataModel.SelectPoints[j]
                    };
                    line.MarkPointMoved += Line_MarkPointMoved;
                    this.graphArea.Children.Add(line);
                    markLines.Add(line);
                }
            }

            hasDrawLines = true;
        }

        private void Line_MarkPointMoved(object sender, RoutedEventArgs e)
        {
            MarkLine.MarkLine sourceLine = (MarkLine.MarkLine)sender;
            MarkLine.LinePointMovedRoutedEventArgs args = e as MarkLine.LinePointMovedRoutedEventArgs;
            foreach (var item in markLines)
            {
                if(item == sourceLine)
                {
                    continue;
                }

                if(!item.DataPoints.Contains(args.OldValue))
                {
                    continue;
                }

                item.MovePoint(args.OldValue, args.NewValue);
            }

            foreach(var item in markPoints)
            {
                if(item.Point == args.OldValue)
                {
                    item.MovePoint(args.OldValue, args.NewValue);
                }
            }
        }

    }
}
