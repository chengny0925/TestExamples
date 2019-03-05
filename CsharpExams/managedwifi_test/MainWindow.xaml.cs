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

namespace managedwifi_test
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        WifiManager.WifiManager _wifiManager = new WifiManager.WifiManager();
        public MainWindow()
        {
            InitializeComponent();

            this.Loaded += MainWindow_Loaded;
        }

        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            init();
        }

        private void init()
        {
            _wifiManager.ScanSSID();
            wifiList.ItemsSource = _wifiManager.SSids;
            currConnectedWifi.DataContext = _wifiManager.CurrConnectedWifi;
        }

        private void refreshBtn_Click(object sender, RoutedEventArgs e)
        {
            _wifiManager.ScanSSID();
        }
    }
}
