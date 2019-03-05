using managedwifi_test.Common;
using NativeWifi;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace managedwifi_test.WifiManager
{

    public class WIFISSID : BaseViewModel
    {
        private string ssid = "NONE";
        public string SSID
        {
            get { return ssid; }
            set
            {
                if (value == ssid) return;
                ssid = value;
                OnPropertyChanged();
            }
        }

        public string profileName = "";

        private string dot11DefaultAuthAlgorithm = "";
        public string Dot11DefaultAuthAlgorithm
        {
            get { return dot11DefaultAuthAlgorithm; }
            set
            {
                if (value == dot11DefaultAuthAlgorithm) return;
                dot11DefaultAuthAlgorithm = value;
                OnPropertyChanged();
            }
        }

        private string dot11DefaultCipherAlgorithm = "";
        public string Dot11DefaultCipherAlgorithm
        {
            get { return dot11DefaultCipherAlgorithm; }
            set
            {
                if (value == dot11DefaultCipherAlgorithm) return;
                dot11DefaultCipherAlgorithm = value;
                OnPropertyChanged();
            }
        }

        private bool networkConnectable = true;
        public bool NetworkConnectable
        {
            get { return networkConnectable; }
            set
            {
                if (value == networkConnectable) return;
                networkConnectable = value;
                OnPropertyChanged();
            }
        }

        private string wlanNotConnectableReason = "";
        public string WlanNotConnectableReason
        {
            get { return wlanNotConnectableReason; }
            set
            {
                if (value == wlanNotConnectableReason) return;
                wlanNotConnectableReason = value;
                OnPropertyChanged();
            }
        }

        private int wlanSignalQuality = 0;
        public int WlanSignalQuality
        {
            get { return wlanSignalQuality; }
            set
            {
                if (value == wlanSignalQuality) return;
                wlanSignalQuality = value;
                OnPropertyChanged();
            }
        }

        public WlanClient.WlanInterface wlanInterface = null;
    } 


    public class WifiManager
    {
        public ObservableCollection<WIFISSID> SSids = new ObservableCollection<WIFISSID>();

        public WIFISSID CurrConnectedWifi = new WIFISSID();

        static string GetStringForSSID(Wlan.Dot11Ssid ssid)
        {
            return Encoding.UTF8.GetString(ssid.SSID, 0, (int)ssid.SSIDLength);
        }

        /// <summary>  
        /// 枚举所有无线设备接收到的SSID  
        /// </summary>  
        public void ScanSSID()
        {
            SSids.Clear();
            WlanClient client = new WlanClient();
            foreach (WlanClient.WlanInterface wlanIface in client.Interfaces)
            {
                // Lists all networks with WEP security  
                Wlan.WlanAvailableNetwork[] networks = wlanIface.GetAvailableNetworkList(0);
                wlanIface.WlanConnectionNotification += WlanIface_WlanConnectionNotification;
                foreach (Wlan.WlanAvailableNetwork network in networks)
                {
                    WIFISSID targetSSID = new WIFISSID();

                    targetSSID.wlanInterface = wlanIface;
                    targetSSID.WlanSignalQuality = (int)network.wlanSignalQuality;
                    targetSSID.SSID = GetStringForSSID(network.dot11Ssid);
                    if((network.flags & Wlan.WlanAvailableNetworkFlags.HasProfile) == Wlan.WlanAvailableNetworkFlags.HasProfile)
                    {
                        targetSSID.profileName = network.profileName;
                    }
                    else
                    {
                        Console.Write(network.flags + "  ");
                    }

                    if ((network.flags & Wlan.WlanAvailableNetworkFlags.Connected) == Wlan.WlanAvailableNetworkFlags.Connected)
                    {
                        CurrConnectedWifi.SSID = targetSSID.SSID;
                    }


                    if (targetSSID.SSID == targetSSID.profileName)
                    {
                        Console.WriteLine("yes");
                    }
                    else
                    {
                        Console.WriteLine("no");
                    }

                    if((network.flags & Wlan.WlanAvailableNetworkFlags.Connected) == Wlan.WlanAvailableNetworkFlags.HasProfile)
                    {

                    }
                   
                    //targetSSID.SSID = Encoding.Default.GetString(network.dot11Ssid.SSID, 0, (int)network.dot11Ssid.SSIDLength);  
                    targetSSID.Dot11DefaultAuthAlgorithm = network.dot11DefaultAuthAlgorithm.ToString();
                    targetSSID.Dot11DefaultCipherAlgorithm = network.dot11DefaultCipherAlgorithm.ToString();
                    SSids.Add(targetSSID);

                    //if ( network.dot11DefaultCipherAlgorithm == Wlan.Dot11CipherAlgorithm.WEP )  
                    //{  
                    //    Console.WriteLine( "Found WEP network with SSID {0}.", GetStringForSSID(network.dot11Ssid));  
                    //}  
                    //Console.WriteLine("Found network with SSID {0}.", GetStringForSSID(network.dot11Ssid));  
                    //Console.WriteLine("dot11BssType:{0}.", network.dot11BssType.ToString());  
                    //Console.WriteLine("dot11DefaultAuthAlgorithm:{0}.", network.dot11DefaultAuthAlgorithm.ToString());  
                    //Console.WriteLine("dot11DefaultCipherAlgorithm:{0}.", network.dot11DefaultCipherAlgorithm.ToString());  
                    //Console.WriteLine("dot11Ssid:{0}.", network.dot11Ssid.ToString());  

                    //Console.WriteLine("flags:{0}.", network.flags.ToString());  
                    //Console.WriteLine("morePhyTypes:{0}.", network.morePhyTypes.ToString());  
                    //Console.WriteLine("networkConnectable:{0}.", network.networkConnectable.ToString());  
                    //Console.WriteLine("numberOfBssids:{0}.", network.numberOfBssids.ToString());  
                    //Console.WriteLine("profileName:{0}.", network.profileName.ToString());  
                    //Console.WriteLine("wlanNotConnectableReason:{0}.", network.wlanNotConnectableReason.ToString());  
                    //Console.WriteLine("wlanSignalQuality:{0}.", network.wlanSignalQuality.ToString());  
                    //Console.WriteLine("-----------------------------------");  
                    // Console.WriteLine(network.ToString());  
                }
            }
        } // EnumSSID 

        private void WlanIface_WlanConnectionNotification(Wlan.WlanNotificationData notifyData, Wlan.WlanConnectionNotificationData connNotifyData)
        {
            int notificationCode = (int)connNotifyData.wlanReasonCode;
            switch (notificationCode)
            {
                case (int)Wlan.WlanReasonCode.Success:
                    CurrConnectedWifi.SSID = GetStringForSSID(connNotifyData.dot11Ssid);
                    Console.WriteLine("连接无线网络.......");
                    break;
                //case (int)Wlan.WlanNotificationCodeAcm.ConnectionComplete:

                //    break;
                //case (int)Wlan.WlanNotificationCodeAcm.Disconnecting:

                //    Console.WriteLine("正在断开无线网络连接.......");
                //    break;
                //case (int)Wlan.WlanNotificationCodeAcm.Disconnected:
                //    Console.WriteLine("已经断开无线网络连接.......");
                //    break;
                default:
                    break;
            }


        }
    }
}
