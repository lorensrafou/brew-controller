using CoreBrewServer.Entities;
using CoreBrewServer.Models;

namespace CoreBrewServer.Helpers
{
    public class Brewing {
        private static Brew brewStatus = new Brew();

        public static Brew GetBrewStatus(){
            return brewStatus;
        }

        public static Brew StartBrew(BrewDay BrewDay)
        {
            brewStatus.running = true;

            return brewStatus;
        }
    }
}