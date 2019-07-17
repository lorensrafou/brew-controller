using System.Collections.Generic;

namespace CoreBrewServer.Models
{
    public class BrewSchedule
    {
        public int Id { get; set; }
        public string Name {get; set;}
        public float MashTemperature { get; set; }
        public int MashDuration { get; set; }
        public int BoilDuration { get; set; }
        public virtual List<BrewDay> BrewDays { get; set; } = new List<BrewDay>();
    }
}