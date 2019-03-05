using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OptionCommon.Utils
{
    public class GuidHelper
    {
        public static String GetNextGuid()
        {
            return Guid.NewGuid().ToString();
        }
    }
}
