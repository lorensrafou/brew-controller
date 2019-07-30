using System;
using System.IO;
using CoreBrewServer.Entities;
using CoreBrewServer.Models;

namespace CoreBrewServer.Helpers
{
    public class Brewing {
        private static Brew brewStatus = new Brew();

        public static Brew GetBrewStatus(){
            brewStatus.temperature = GetTemperature();

            return brewStatus;
        }

        public static Brew StartBrew(BrewDay BrewDay)
        {
            brewStatus.running = true;

            Mash();

            return brewStatus;
        }

        private static float GetTemperature(){
            string sensorData = File.ReadAllText("/sys/bus/w1/devices/28-00000871c1e8/w1_slave");

            float cTemp = Int32.Parse(sensorData.Split("t=",2)[1])/1000;
            float fTemp = (cTemp * 9/5) +32;

            return fTemp;
        }

        public static async void Mash(){

        }
    }
}