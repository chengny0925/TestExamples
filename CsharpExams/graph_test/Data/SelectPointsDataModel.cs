using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace graph_test.Data
{
    public class SelectPointsDataModel
    {
        private PointCollection _selectPoints = new PointCollection();
        public PointCollection SelectPoints
        {
            get { return _selectPoints; }
            set
            {
                if (value == _selectPoints) return;
                _selectPoints = value;
            }
        }

        private PointCollection _allPoints = new PointCollection();

        public PointCollection AllPoints
        {
            get { return _allPoints; }
            set
            {
                if (value == _allPoints) return;
                _allPoints = value;
            }
        }

        public SelectPointsDataModel()
        {
            _allPoints = new PointCollection()
            {
                new Point(20,30),
                new Point(50,80),
                new Point(120,30),
                new Point(120,90),
                new Point(220,30),
                new Point(220,150),
                new Point(320,50)
            };
        }

        public void AddSelectPoint(Point p)
        {
            if(_selectPoints.Count >= 3)
            {
                return;
            }

            if(_selectPoints.Contains(p))
            {
                return;
            }

            SelectPoints.Add(p);

            //CheckSelecPoints();
        }

        public bool CheckSelecPoints()
        {
            if(SelectPoints.Count == 3)
            {
                return true;
            }

            return false;
        }
    }
}
